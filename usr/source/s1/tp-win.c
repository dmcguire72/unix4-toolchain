/*
 * UNIX Fourth Edition tp(V) Extraction / Creation Utility
 */
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <wchar.h>

#ifndef PATH_MAX
#define PATH_MAX 32768
#endif

#define BLK        512
#define ENTSZ      64
#define NAMELEN    32
#define MAXBLOCK   65535
#define MAXSIZE    0xffffffu

#define V4_ALLOC   0100000
#define V4_TYPE    060000
#define V4_SETUID  04000
#define V4_SETGID  02000

#define UNIX_EPOCH_DELTA 11644473600ULL
#define FILETIME_TICKS_PER_SECOND 10000000ULL

struct layout {
    const char *name;
    unsigned dirblocks;
    unsigned maxent;
    unsigned tapeblocks;
};
static const struct layout MAG = {"magtape", 62, 496, 0};
static const struct layout DEC = {"DECtape", 24, 192, 578};

struct ent {
    char name[NAMELEN];
    unsigned mode, uid, gid;
    uint32_t size, mtime;
    unsigned tapea;
    char *path;
};

struct tp_stat {
    uint64_t st_size;
    int64_t st_mtime;
    unsigned st_mode;
    unsigned st_uid;
    unsigned st_gid;
    int is_dir;
    int is_file;
    int is_reparse;
};

static int verbose, errors;
static const char *prog = "tp";
static int keep_going;
static int uid_warned;
static int force_uid = -1, force_gid = -1;

static void warn(const char *fmt, ...)
{
    va_list ap;
    fprintf(stderr, "%s: ", prog);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    errors++;
}

static void note(const char *fmt, ...)
{
    va_list ap;
    fprintf(stderr, "%s: ", prog);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    if (!keep_going) errors++;
}

static void die(const char *fmt, ...)
{
    va_list ap;
    fprintf(stderr, "%s: ", prog);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(2);
}

static char *tp_strdup(const char *s)
{
    size_t n;
    char *p;
    if (!s) return NULL;
    n = strlen(s) + 1;
    p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static size_t bounded_strlen(const char *s, size_t maxlen)
{
    size_t n = 0;
    while (n < maxlen && s[n]) n++;
    return n;
}

static void set_errno_from_win32(DWORD e)
{
    switch (e) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
        errno = ENOENT;
        break;
    case ERROR_ACCESS_DENIED:
    case ERROR_SHARING_VIOLATION:
    case ERROR_LOCK_VIOLATION:
        errno = EACCES;
        break;
    case ERROR_ALREADY_EXISTS:
    case ERROR_FILE_EXISTS:
        errno = EEXIST;
        break;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
        errno = ENOMEM;
        break;
    case ERROR_DISK_FULL:
        errno = ENOSPC;
        break;
    case ERROR_FILENAME_EXCED_RANGE:
        errno = ENAMETOOLONG;
        break;
    case ERROR_INVALID_NAME:
    case ERROR_INVALID_PARAMETER:
        errno = EINVAL;
        break;
    default:
        errno = EIO;
        break;
    }
}

static wchar_t *utf8_to_wide(const char *s)
{
    int need;
    wchar_t *out;
    if (!s) return NULL;
    need = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, NULL, 0);
    if (!need) {
        need = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
        if (!need) return NULL;
        out = (wchar_t *)malloc((size_t)need * sizeof(wchar_t));
        if (!out) return NULL;
        if (!MultiByteToWideChar(CP_ACP, 0, s, -1, out, need)) {
            free(out);
            return NULL;
        }
    } else {
        out = (wchar_t *)malloc((size_t)need * sizeof(wchar_t));
        if (!out) return NULL;
        if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, out, need)) {
            free(out);
            return NULL;
        }
    }
    for (wchar_t *p = out; *p; ++p) if (*p == L'/') *p = L'\\';
    return out;
}

static char *wide_to_utf8(const wchar_t *s)
{
    int need;
    char *out;
    if (!s) return NULL;
    need = WideCharToMultiByte(CP_UTF8, 0, s, -1, NULL, 0, NULL, NULL);
    if (!need) return NULL;
    out = (char *)malloc((size_t)need);
    if (!out) return NULL;
    if (!WideCharToMultiByte(CP_UTF8, 0, s, -1, out, need, NULL, NULL)) {
        free(out);
        return NULL;
    }
    return out;
}

static FILE *tp_fopen(const char *path, const char *mode)
{
    wchar_t *wpath = utf8_to_wide(path);
    wchar_t wmode[16];
    size_t i;
    FILE *f;
    if (!wpath) { errno = EINVAL; return NULL; }
    for (i = 0; i + 1 < sizeof(wmode) / sizeof(wmode[0]) && mode[i]; ++i)
        wmode[i] = (wchar_t)(unsigned char)mode[i];
    wmode[i] = 0;
    f = _wfopen(wpath, wmode);
    free(wpath);
    return f;
}

static int tp_open_read_binary(const char *path)
{
    wchar_t *wpath = utf8_to_wide(path);
    int fd;
    if (!wpath) { errno = EINVAL; return -1; }
    fd = _wopen(wpath, _O_RDONLY | _O_BINARY);
    free(wpath);
    return fd;
}

static int tp_unlink(const char *path)
{
    wchar_t *wpath = utf8_to_wide(path);
    BOOL ok;
    if (!wpath) { errno = EINVAL; return -1; }
    ok = DeleteFileW(wpath);
    if (!ok) set_errno_from_win32(GetLastError());
    free(wpath);
    return ok ? 0 : -1;
}

static int tp_chdir(const char *path)
{
    wchar_t *wpath = utf8_to_wide(path);
    int rc;
    if (!wpath) { errno = EINVAL; return -1; }
    rc = _wchdir(wpath);
    free(wpath);
    return rc;
}

static char *tp_getcwd_alloc(void)
{
    DWORD cap = 256;
    wchar_t *wide = NULL;
    char *utf8 = NULL;
    for (;;) {
        DWORD got;
        wchar_t *next = (wchar_t *)realloc(wide, (size_t)cap * sizeof(wchar_t));
        if (!next) { free(wide); errno = ENOMEM; return NULL; }
        wide = next;
        got = GetCurrentDirectoryW(cap, wide);
        if (!got) { set_errno_from_win32(GetLastError()); free(wide); return NULL; }
        if (got < cap) break;
        cap = got + 1;
    }
    utf8 = wide_to_utf8(wide);
    free(wide);
    if (!utf8) errno = ENOMEM;
    return utf8;
}

static int tp_path_is_absolute(const char *p)
{
    if (!p || !*p) return 0;
    if (p[0] == '/' || p[0] == '\\') return 1;
    if (((p[0] >= 'A' && p[0] <= 'Z') || (p[0] >= 'a' && p[0] <= 'z')) &&
        p[1] == ':' && (p[2] == '/' || p[2] == '\\')) return 1;
    return 0;
}

static char *path_join(const char *a, const char *b)
{
    size_t al = strlen(a), bl = strlen(b), start = 0;
    int sep;
    int is_drive_root = al == 3 &&
        (((a[0] >= 'A' && a[0] <= 'Z') || (a[0] >= 'a' && a[0] <= 'z')) &&
         a[1] == ':' && (a[2] == '/' || a[2] == '\\'));
    char *out;
    if (!is_drive_root) {
        while (al > 1 && (a[al - 1] == '/' || a[al - 1] == '\\')) al--;
    }
    sep = al > 0 && a[al - 1] != '/' && a[al - 1] != '\\';
    if (al == 2 && a[1] == ':') sep = 1;
    out = (char *)malloc(al + (size_t)sep + bl + 1);
    if (!out) return NULL;
    memcpy(out, a, al);
    start = al;
    if (sep) out[start++] = '/';
    memcpy(out + start, b, bl + 1);
    return out;
}

static int tp_lstat(const char *path, struct tp_stat *st)
{
    wchar_t *wpath = utf8_to_wide(path);
    WIN32_FILE_ATTRIBUTE_DATA d;
    ULARGE_INTEGER ticks;
    uint64_t unix_ticks;
    if (!wpath) { errno = EINVAL; return -1; }
    if (!GetFileAttributesExW(wpath, GetFileExInfoStandard, &d)) {
        set_errno_from_win32(GetLastError());
        free(wpath);
        return -1;
    }
    free(wpath);
    memset(st, 0, sizeof(*st));
    st->is_reparse = (d.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
    st->is_dir = (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    st->is_file = !st->is_dir && !st->is_reparse;
    st->st_size = ((uint64_t)d.nFileSizeHigh << 32) | d.nFileSizeLow;
    ticks.LowPart = d.ftLastWriteTime.dwLowDateTime;
    ticks.HighPart = d.ftLastWriteTime.dwHighDateTime;
    unix_ticks = ticks.QuadPart;
    if (unix_ticks >= UNIX_EPOCH_DELTA * FILETIME_TICKS_PER_SECOND)
        st->st_mtime = (int64_t)(unix_ticks / FILETIME_TICKS_PER_SECOND - UNIX_EPOCH_DELTA);
    else
        st->st_mtime = -(int64_t)((UNIX_EPOCH_DELTA * FILETIME_TICKS_PER_SECOND - unix_ticks) / FILETIME_TICKS_PER_SECOND);
    st->st_mode = (d.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? 0444u : 0666u;
    st->st_uid = 0;
    st->st_gid = 0;
    return 0;
}

static DWORD tp_get_attributes(const char *path)
{
    wchar_t *wpath = utf8_to_wide(path);
    DWORD attrs;
    if (!wpath) { errno = EINVAL; return INVALID_FILE_ATTRIBUTES; }
    attrs = GetFileAttributesW(wpath);
    if (attrs == INVALID_FILE_ATTRIBUTES) set_errno_from_win32(GetLastError());
    free(wpath);
    return attrs;
}

static int tp_mkdir(const char *path)
{
    wchar_t *wpath = utf8_to_wide(path);
    BOOL ok;
    if (!wpath) { errno = EINVAL; return -1; }
    ok = CreateDirectoryW(wpath, NULL);
    if (!ok) set_errno_from_win32(GetLastError());
    free(wpath);
    return ok ? 0 : -1;
}

static int plain_directory(const char *path)
{
    DWORD attrs = tp_get_attributes(path);
    return attrs != INVALID_FILE_ATTRIBUTES &&
           (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
           (attrs & FILE_ATTRIBUTE_REPARSE_POINT) == 0;
}

static int ascii_iequal_n(const char *a, size_t n, const char *b)
{
    size_t i;
    if (strlen(b) != n) return 0;
    for (i = 0; i < n; ++i) {
        unsigned char x = (unsigned char)a[i], y = (unsigned char)b[i];
        if (x >= 'a' && x <= 'z') x = (unsigned char)(x - 'a' + 'A');
        if (y >= 'a' && y <= 'z') y = (unsigned char)(y - 'a' + 'A');
        if (x != y) return 0;
    }
    return 1;
}

static int safe_windows_name(const char *name)
{
    const char *p = name;
    while (*p) {
        const char *e = strchr(p, '/');
        size_t n = e ? (size_t)(e - p) : strlen(p);
        size_t base_n = 0;
        const char *dot;
        if (n == 0) { p = e ? e + 1 : p + n; if (!e) break; continue; }
        if (n == 1 && p[0] == '.') { p = e ? e + 1 : p + n; if (!e) break; continue; }
        if (n == 2 && p[0] == '.' && p[1] == '.') return 0;
        if (p[n - 1] == '.' || p[n - 1] == ' ') return 0;
        for (size_t i = 0; i < n; ++i) {
            unsigned char c = (unsigned char)p[i];
            if (c < 32 || c == ':' || c == '<' || c == '>' || c == '"' ||
                c == '|' || c == '?' || c == '*') return 0;
        }
        dot = (const char *)memchr(p, '.', n);
        base_n = dot ? (size_t)(dot - p) : n;
        if (ascii_iequal_n(p, base_n, "CON") || ascii_iequal_n(p, base_n, "PRN") ||
            ascii_iequal_n(p, base_n, "AUX") || ascii_iequal_n(p, base_n, "NUL") ||
            ascii_iequal_n(p, base_n, "COM1") || ascii_iequal_n(p, base_n, "COM2") ||
            ascii_iequal_n(p, base_n, "COM3") || ascii_iequal_n(p, base_n, "COM4") ||
            ascii_iequal_n(p, base_n, "COM5") || ascii_iequal_n(p, base_n, "COM6") ||
            ascii_iequal_n(p, base_n, "COM7") || ascii_iequal_n(p, base_n, "COM8") ||
            ascii_iequal_n(p, base_n, "COM9") || ascii_iequal_n(p, base_n, "LPT1") ||
            ascii_iequal_n(p, base_n, "LPT2") || ascii_iequal_n(p, base_n, "LPT3") ||
            ascii_iequal_n(p, base_n, "LPT4") || ascii_iequal_n(p, base_n, "LPT5") ||
            ascii_iequal_n(p, base_n, "LPT6") || ascii_iequal_n(p, base_n, "LPT7") ||
            ascii_iequal_n(p, base_n, "LPT8") || ascii_iequal_n(p, base_n, "LPT9")) return 0;
        if (!e) break;
        p = e + 1;
    }
    return 1;
}

static unsigned rd16(const unsigned char *p) { return (unsigned)p[0] | ((unsigned)p[1] << 8); }
static void wr16(unsigned char *p, unsigned v) { p[0] = (unsigned char)(v & 255); p[1] = (unsigned char)((v >> 8) & 255); }
static uint32_t le32(const unsigned char *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

static char *norm_name(const char *p, int quiet)
{
    char *s, *q;
    size_t n;
    s = tp_strdup(p);
    if (!s) die("out of memory");
    for (q = s; *q; ++q) if (*q == '\\') *q = '/';

    if (((s[0] >= 'A' && s[0] <= 'Z') || (s[0] >= 'a' && s[0] <= 'z')) && s[1] == ':') {
        memmove(s, s + 2, strlen(s + 2) + 1);
    }
    while (s[0] == '.' && s[1] == '/') {
        memmove(s, s + 2, strlen(s + 2) + 1);
        while (*s == '/') memmove(s, s + 1, strlen(s + 1) + 1);
    }
    if (*s == '/') {
        if (!quiet) fprintf(stderr, "%s: removing leading '/' from '%s'\n", prog, s);
        while (*s == '/') memmove(s, s + 1, strlen(s + 1) + 1);
    }
    n = strlen(s);
    while (n > 1 && s[n - 1] == '/') s[--n] = 0;
    for (q = s;;) {
        const char *e = strchr(q, '/');
        size_t len = e ? (size_t)(e - q) : strlen(q);
        if (len == 2 && q[0] == '.' && q[1] == '.') {
            if (!quiet) warn("'%s': '..' is not allowed in tp names", p);
            free(s);
            return NULL;
        }
        if (len && memchr(q, ':', len)) {
            if (!quiet) warn("'%s': ':' is not allowed in Windows tp names", p);
            free(s);
            return NULL;
        }
        if (!e) break;
        q = (char *)e + 1;
    }
    if (!*s) { free(s); return NULL; }
    return s;
}

static int selected(const char *name, int nsel, char **sel)
{
    if (nsel == 0) return 1;
    for (int i = 0; i < nsel; i++) {
        size_t l = strlen(sel[i]);
        if (!strncmp(name, sel[i], l) && (name[l] == 0 || name[l] == '/')) return 1;
    }
    return 0;
}

static void pack_entry(unsigned char *e, const struct ent *en)
{
    unsigned sum = 0;
    memset(e, 0, ENTSZ);
    memcpy(e, en->name, bounded_strlen(en->name, NAMELEN));
    wr16(e + 32, en->mode);
    e[34] = (unsigned char)en->uid;
    e[35] = (unsigned char)en->gid;
    e[37] = (unsigned char)(en->size >> 16);
    wr16(e + 38, en->size & 0xffff);
    wr16(e + 40, en->mtime >> 16);
    wr16(e + 42, en->mtime & 0xffff);
    wr16(e + 44, en->tapea);
    for (int w = 0; w < 31; w++) sum += rd16(e + 2 * w);
    wr16(e + 62, (0x10000 - (sum & 0xffff)) & 0xffff);
}

static int unpack_entry(const unsigned char *e, struct ent *en)
{
    unsigned sum = 0;
    for (int w = 0; w < 32; w++) sum += rd16(e + 2 * w);
    memcpy(en->name, e, 32);
    en->mode = rd16(e + 32);
    en->uid = e[34];
    en->gid = e[35];
    en->size = (uint32_t)e[37] << 16 | rd16(e + 38);
    en->mtime = (uint32_t)rd16(e + 40) << 16 | rd16(e + 42);
    en->tapea = rd16(e + 44);
    en->path = NULL;
    return (sum & 0xffff) == 0;
}

struct archive {
    unsigned char *img;
    size_t len, nblocks;
    struct ent *ents;
    size_t nents;
    unsigned dirblocks;
};

static unsigned char *slurp(const char *path, size_t *len)
{
    int is_stdin = !strcmp(path, "-");
    int fd = is_stdin ? _fileno(stdin) : tp_open_read_binary(path);
    size_t cap = 1u << 20, n = 0;
    unsigned char *b;
    if (fd < 0) die("%s: %s", path, strerror(errno));
    if (is_stdin) _setmode(fd, _O_BINARY);
    b = (unsigned char *)malloc(cap);
    if (!b) die("out of memory");
    for (;;) {
        unsigned int want;
        int r;
        if (n == cap) {
            if (cap > ((size_t)MAXBLOCK + 1) * BLK * 2) die("%s: too large for a tp archive", path);
            cap *= 2;
            {
                unsigned char *nb = (unsigned char *)realloc(b, cap);
                if (!nb) { free(b); die("out of memory"); }
                b = nb;
            }
        }
        want = (unsigned int)((cap - n) > 0x7fffffffU ? 0x7fffffffU : cap - n);
        r = _read(fd, b + n, want);
        if (r < 0) { int e = errno; free(b); if (!is_stdin) _close(fd); errno = e; die("%s: %s", path, strerror(errno)); }
        if (r == 0) break;
        n += (size_t)r;
    }
    if (!is_stdin) _close(fd);
    *len = n;
    return b;
}

static void open_archive(struct archive *a, const char *path, const struct layout *fixed)
{
    memset(a, 0, sizeof *a);
    a->img = slurp(path, &a->len);
    a->nblocks = a->len / BLK;
    if (a->len < 2 * BLK) die("%s: too small to be a tp archive", path);
    if (a->len % BLK) fprintf(stderr, "%s: warning: size is not a multiple of %d bytes\n", prog, BLK);
    if (a->len >= 2 * (BLK + 8) && le32(a->img) == BLK && le32(a->img + BLK + 4) == BLK)
        fprintf(stderr, "%s: warning: this looks like a SIMH .tap image; unwrap the tape records first\n", prog);

    unsigned last = fixed ? fixed->dirblocks : 62;
    unsigned first_data = ~0u;
    size_t cap = 64;
    a->ents = (struct ent *)malloc(cap * sizeof *a->ents);
    if (!a->ents) die("out of memory");
    for (unsigned blk = 1; blk <= last && blk < a->nblocks && blk < first_data; blk++) {
        a->dirblocks = blk;
        for (int i = 0; i < BLK / ENTSZ; i++) {
            const unsigned char *e = a->img + (size_t)blk * BLK + (size_t)i * ENTSZ;
            struct ent en;
            if (rd16(e) == 0) continue;
            if (!unpack_entry(e, &en)) {
                char nm[NAMELEN + 1];
                memcpy(nm, e, NAMELEN); nm[NAMELEN] = 0;
                note("bad checksum in directory entry '%.32s'", nm);
                continue;
            }
            if (!memchr(en.name, 0, NAMELEN)) { note("directory entry name is not terminated"); continue; }
            {
                char *norm;
                for (char *np = en.name; *np; ++np) if (*np == '\\') *np = '/';
                norm = norm_name(en.name, 1);
                if (!norm) { note("refusing unsafe name '%s'", en.name); continue; }
                memset(en.name, 0, sizeof en.name);
                memcpy(en.name, norm, strlen(norm));
                free(norm);
            }
            if (en.size) {
                if (en.tapea <= a->dirblocks || (size_t)en.tapea * BLK + en.size > a->len) {
                    note("'%s': data (block %u, %u bytes) lies outside the archive", en.name, en.tapea, (unsigned)en.size);
                    continue;
                }
                if (en.tapea < first_data) first_data = en.tapea;
            }
            if (a->nents == cap) {
                size_t newcap = cap * 2;
                struct ent *ne = (struct ent *)realloc(a->ents, newcap * sizeof *a->ents);
                if (!ne) die("out of memory");
                a->ents = ne;
                cap = newcap;
            }
            a->ents[a->nents++] = en;
        }
    }
}

static void modestr(unsigned m, char *s)
{
    static const char rwx[] = "rwxrwxrwx";
    strcpy(s, "----------");
    switch (m & V4_TYPE) { case 040000: s[0] = 'd'; break; case 020000: s[0] = 'c'; break; case 060000: s[0] = 'b'; break; }
    for (int i = 0; i < 9; i++) if (m & (0400 >> i)) s[1 + i] = rwx[i];
    if (m & V4_SETUID) s[3] = (m & 0100) ? 's' : 'S';
    if (m & V4_SETGID) s[6] = (m & 010) ? 's' : 'S';
}

static void print_entry(const struct ent *en)
{
    char ms[16], ts[32];
    time_t t = (time_t)en->mtime;
    struct tm *tmp;
    struct tm tm;
    tmp = gmtime(&t);
    if (tmp) tm = *tmp;
    else memset(&tm, 0, sizeof(tm));
    strftime(ts, sizeof ts, "%Y-%m-%d %H:%M", &tm);
    modestr(en->mode, ms);
    printf("%s %3u/%-3u %8u %s  blk %-5u %s\n", ms, en->uid, en->gid, (unsigned)en->size, ts, en->tapea, en->name);
}

static char *full_member_path(const char *dest, const char *name)
{
    char *full;
    if (!safe_windows_name(name)) return NULL;
    full = path_join(dest, name);
    return full;
}

static int ensure_member_parent_dirs(const char *dest, const char *name)
{
    char *current;
    const char *p;
    if (!plain_directory(dest)) {
        warn("%s: extraction root is not a plain directory (or is a reparse point)", dest);
        return -1;
    }
    current = tp_strdup(dest);
    if (!current) die("out of memory");
    p = name;
    while (*p) {
        const char *slash = strchr(p, '/');
        size_t n = slash ? (size_t)(slash - p) : strlen(p);
        if (!slash) break;
        if (n) {
            char *part = (char *)malloc(n + 1);
            char *next;
            DWORD attrs;
            if (!part) { free(current); die("out of memory"); }
            memcpy(part, p, n); part[n] = 0;
            next = path_join(current, part);
            free(part);
            if (!next) { free(current); die("out of memory"); }
            attrs = tp_get_attributes(next);
            if (attrs == INVALID_FILE_ATTRIBUTES) {
                if (errno != ENOENT || tp_mkdir(next) != 0) {
                    if (errno != EEXIST) {
                        warn("%s: mkdir: %s", next, strerror(errno));
                        free(next); free(current); return -1;
                    }
                }
                attrs = tp_get_attributes(next);
            }
            if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY) ||
                (attrs & FILE_ATTRIBUTE_REPARSE_POINT)) {
                warn("%s: refusing non-directory or reparse-point path component", next);
                free(next); free(current); return -1;
            }
            free(current);
            current = next;
        }
        p = slash + 1;
    }
    free(current);
    return 0;
}

static int write_member_file(const char *path, const unsigned char *data, size_t size,
                             uint32_t mtime, unsigned perm, int preserve)
{
    wchar_t *wpath = utf8_to_wide(path);
    HANDLE h;
    size_t left = size;
    int bad = 0;
    if (!wpath) { errno = EINVAL; warn("%s: invalid path encoding", path); return -1; }

    DWORD attrs = GetFileAttributesW(wpath);
    if (attrs != INVALID_FILE_ATTRIBUTES &&
        ((attrs & FILE_ATTRIBUTE_DIRECTORY) || (attrs & FILE_ATTRIBUTE_REPARSE_POINT))) {
        free(wpath);
        warn("%s: refusing to overwrite a directory or reparse point", path);
        return -1;
    }
    h = CreateFileW(wpath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        set_errno_from_win32(GetLastError());
        warn("%s: %s", path, strerror(errno));
        free(wpath);
        return -1;
    }
    {
        BY_HANDLE_FILE_INFORMATION info;
        if (!GetFileInformationByHandle(h, &info) ||
            (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT))) {
            CloseHandle(h);
            free(wpath);
            warn("%s: refusing a directory or reparse point", path);
            return -1;
        }
    }
    while (left) {
        DWORD want = (DWORD)(left > (1u << 20) ? (1u << 20) : left);
        DWORD written = 0;
        if (!WriteFile(h, data, want, &written, NULL) || written == 0) {
            set_errno_from_win32(GetLastError());
            warn("%s: write: %s", path, strerror(errno));
            bad = 1;
            break;
        }
        data += written;
        left -= written;
    }
    if (!bad) {
        ULONGLONG raw = ((ULONGLONG)mtime + UNIX_EPOCH_DELTA) * FILETIME_TICKS_PER_SECOND;
        FILETIME ft;
        ULARGE_INTEGER u;
        u.QuadPart = raw;
        ft.dwLowDateTime = u.LowPart;
        ft.dwHighDateTime = u.HighPart;
        if (!SetFileTime(h, NULL, &ft, &ft)) {
            set_errno_from_win32(GetLastError());
            warn("%s: set time: %s", path, strerror(errno));
        }
    }
    if (!CloseHandle(h) && !bad) {
        set_errno_from_win32(GetLastError());
        warn("%s: close: %s", path, strerror(errno));
        bad = 1;
    }
    free(wpath);

    if (!bad) {
        wchar_t *wfinal = utf8_to_wide(path);
        DWORD attrs = GetFileAttributesW(wfinal);
        if (attrs != INVALID_FILE_ATTRIBUTES) {
            attrs &= ~FILE_ATTRIBUTE_NORMAL;
            if ((perm & 0222u) == 0) attrs |= FILE_ATTRIBUTE_READONLY;
            else attrs &= ~FILE_ATTRIBUTE_READONLY;
            if (attrs == 0) attrs = FILE_ATTRIBUTE_NORMAL;
            if (!SetFileAttributesW(wfinal, attrs)) {
                set_errno_from_win32(GetLastError());
                warn("%s: set attributes: %s", path, strerror(errno));
            }
        }
        free(wfinal);
    }
    (void)preserve;
    return bad ? -1 : 0;
}

static void extract(const struct archive *a, int nsel, char **sel, const char *dest, int preserve)
{
    int old_umask = _umask(0);
    _umask(old_umask);
    if (!plain_directory(dest)) die("%s: extraction root is not a plain directory (or is a reparse point)", dest);
    if (preserve) {
        fprintf(stderr, "%s: warning: Windows cannot restore Unix uid/gid, set-id, or execute permission bits\n", prog);
    }
    for (size_t i = 0; i < a->nents; i++) {
        const struct ent *en = &a->ents[i];
        char *full;
        unsigned perm = en->mode & 0777u;
        if (!preserve) perm &= ~(unsigned)old_umask;
        if (!selected(en->name, nsel, sel)) continue;
        if (verbose) printf("x %s\n", en->name);
        if ((en->mode & V4_TYPE) != 0) { warn("'%s' is not a plain file (mode %o), skipped", en->name, en->mode); continue; }
        if (!safe_windows_name(en->name)) {
            warn("'%s' cannot be represented safely as a Windows filename; skipped", en->name);
            continue;
        }
        if (ensure_member_parent_dirs(dest, en->name) != 0) continue;
        full = full_member_path(dest, en->name);
        if (!full) { warn("'%s' cannot be represented safely as a Windows filename; skipped", en->name); continue; }
        write_member_file(full, a->img + (size_t)en->tapea * BLK, en->size, en->mtime, perm, preserve);
        free(full);
    }
}

struct list { struct ent *v; size_t n, cap; };

static void add_file(struct list *l, const char *path, const struct tp_stat *st)
{
    char *name = norm_name(path, 0);
    uint64_t uid, gid;
    if (!name) { errors++; return; }
    if (strlen(name) >= NAMELEN) { warn("'%s': name longer than %d characters", name, NAMELEN - 1); free(name); return; }
    for (size_t i = 0; i < l->n; i++)
        if (!strcmp(l->v[i].name, name)) { warn("'%s': duplicate entry", name); free(name); return; }
    if (st->st_size > MAXSIZE) { warn("'%s': larger than 16 MB", name); free(name); return; }
    if (l->n == l->cap) {
        size_t newcap = l->cap ? l->cap * 2 : 64;
        struct ent *nv = (struct ent *)realloc(l->v, newcap * sizeof *l->v);
        if (!nv) { free(name); die("out of memory"); }
        l->v = nv;
        l->cap = newcap;
    }
    struct ent *en = &l->v[l->n++];
    memset(en, 0, sizeof *en);
    memcpy(en->name, name, strlen(name));
    en->path = tp_strdup(path);
    if (!en->path) die("out of memory");
    en->size = (uint32_t)st->st_size;
    en->mode = V4_ALLOC | (st->st_mode & 0777u);
    if (st->st_mode & 04000u) en->mode |= V4_SETUID;
    if (st->st_mode & 02000u) en->mode |= V4_SETGID;
    uid = force_uid >= 0 ? (uint64_t)force_uid : st->st_uid;
    gid = force_gid >= 0 ? (uint64_t)force_gid : st->st_gid;
    if ((uid > 255 || gid > 255) && !uid_warned) {
        fprintf(stderr, "%s: warning: uid/gid above 255 do not fit a V4 byte and are stored as 0\n", prog);
        uid_warned = 1;
    }
    en->uid = uid > 255 ? 0 : (unsigned)uid;
    en->gid = gid > 255 ? 0 : (unsigned)gid;
    en->mtime = st->st_mtime < 0 ? 0 : (uint64_t)st->st_mtime > 0xffffffffu ? 0xffffffffu : (uint32_t)st->st_mtime;
    free(name);
}

static int name_compare(const void *a, const void *b)
{
    const char *const *x = (const char *const *)a;
    const char *const *y = (const char *const *)b;
    return strcmp(*x, *y);
}

static void walk(struct list *l, const char *path)
{
    struct tp_stat st;
    if (tp_lstat(path, &st)) { warn("%s: %s", path, strerror(errno)); return; }
    if (st.is_reparse) {
        fprintf(stderr, "%s: skipping '%s' (reparse point / symbolic link)\n", prog, path);
        return;
    }
    if (st.is_file) { add_file(l, path, &st); return; }
    if (st.is_dir) {
        char *pattern = path_join(path, "*");
        wchar_t *wpattern;
        WIN32_FIND_DATAW fd;
        HANDLE h;
        char **names = NULL;
        size_t count = 0, cap = 0;
        if (!pattern) die("out of memory");
        wpattern = utf8_to_wide(pattern);
        free(pattern);
        if (!wpattern) { warn("%s: invalid path encoding", path); return; }
        h = FindFirstFileW(wpattern, &fd);
        free(wpattern);
        if (h == INVALID_HANDLE_VALUE) { set_errno_from_win32(GetLastError()); warn("%s: %s", path, strerror(errno)); return; }
        do {
            char *nm;
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;
            nm = wide_to_utf8(fd.cFileName);
            if (!nm) { FindClose(h); for (size_t i = 0; i < count; ++i) free(names[i]); free(names); die("out of memory"); }
            if (count == cap) {
                size_t nc = cap ? cap * 2 : 32;
                char **nn = (char **)realloc(names, nc * sizeof *names);
                if (!nn) { free(nm); FindClose(h); for (size_t i = 0; i < count; ++i) free(names[i]); free(names); die("out of memory"); }
                names = nn; cap = nc;
            }
            names[count++] = nm;
        } while (FindNextFileW(h, &fd));
        FindClose(h);
        if (count > 1) qsort(names, count, sizeof *names, name_compare);
        for (size_t i = 0; i < count; i++) {
            char *child = path_join(path, names[i]);
            if (!child) { for (size_t j = i; j < count; ++j) free(names[j]); free(names); die("out of memory"); }
            walk(l, child);
            free(child);
            free(names[i]);
        }
        free(names);
        return;
    }
    fprintf(stderr, "%s: skipping '%s' (not a regular file or directory)\n", prog, path);
}

static void write_all(FILE *o, const void *p, size_t n, const char *what)
{
    if (n && fwrite(p, 1, n, o) != n) die("%s: write error", what);
}

static void create(const char *archive, const struct layout *lay, struct list *l, const char *bootfile)
{
    if (l->n > lay->maxent) die("%zu files do not fit: a %s directory holds %u entries", l->n, lay->name, lay->maxent);
    unsigned next = lay->dirblocks + 1;
    for (size_t i = 0; i < l->n; i++) {
        struct ent *en = &l->v[i];
        en->tapea = next;
        unsigned nb = (en->size + BLK - 1) / BLK;
        if ((size_t)next + nb > (lay->tapeblocks ? lay->tapeblocks : MAXBLOCK + 1))
            die("'%s' does not fit on a %s tape (%u blocks)", en->name, lay->name, lay->tapeblocks ? lay->tapeblocks : MAXBLOCK + 1);
        next += nb;
    }
    unsigned char boot[BLK] = {0};
    if (bootfile) {
        FILE *b = tp_fopen(bootfile, "rb");
        size_t n;
        if (!b) die("%s: %s", bootfile, strerror(errno));
        n = fread(boot, 1, BLK, b);
        if (ferror(b)) die("%s: read error", bootfile);
        if (n == BLK && fgetc(b) != EOF) die("%s: boot block must be at most %d bytes", bootfile, BLK);
        if (fclose(b)) die("%s: close error", bootfile);
    }
    unsigned char *dir = (unsigned char *)calloc(lay->dirblocks, BLK);
    if (!dir) die("out of memory");
    for (size_t i = 0; i < l->n; i++) pack_entry(dir + i * ENTSZ, &l->v[i]);

    int tostdout = !strcmp(archive, "-");
    FILE *o = tostdout ? stdout : tp_fopen(archive, "wb");
    if (!o) die("%s: %s", archive, strerror(errno));
    if (tostdout) _setmode(_fileno(stdout), _O_BINARY);
    write_all(o, boot, BLK, archive);
    write_all(o, dir, (size_t)lay->dirblocks * BLK, archive);
    free(dir);
    size_t written = 1 + lay->dirblocks;
    for (size_t i = 0; i < l->n; i++) {
        struct ent *en = &l->v[i];
        FILE *f;
        if (verbose) fprintf(stderr, "a %s\n", en->name);
        f = tp_fopen(en->path, "rb");
        if (!f) { warn("%s: %s", en->path, strerror(errno)); goto abort; }
        unsigned char buf[BLK];
        size_t left = en->size;
        while (left) {
            size_t want = left < BLK ? left : BLK;
            size_t got = fread(buf, 1, want, f);
            if (got != want) { warn("%s: file changed while it was being archived", en->path); fclose(f); goto abort; }
            if (got < BLK) memset(buf + got, 0, BLK - got);
            write_all(o, buf, BLK, archive);
            written++;
            left -= got;
        }
        fclose(f);
    }
    if (lay->tapeblocks) {
        unsigned char zero[BLK] = {0};
        for (; written < lay->tapeblocks; written++) write_all(o, zero, BLK, archive);
    }
    if (fflush(o) || (!tostdout && fclose(o))) die("%s: write error", archive);
    if (verbose) fprintf(stderr, "%s: %zu files, %zu blocks, %s layout\n", archive, l->n, written, lay->name);
    return;
abort:
    if (!tostdout) { fclose(o); tp_unlink(archive); }
    exit(2);
}

static void usage(FILE *o, const char *program)
{
    fprintf(o,
"usage: %s -c [-vmD] [-f archive] [-C dir] [-b bootfile] [-T list] [-U uid] [-G gid] [file ...]\n"
"       %s -x [-vpi] [-f archive] [-C dir] [name ...]\n"
"       %s -t [-v]   [-f archive] [name ...]\n"
"       %s -k        [-f archive]                    (verify checksums and extents)\n"
"       %s -B bootfile -f archive                    (save the boot block)\n"
"\n"
"  -c create   -x extract   -t list   -k check      -f archive ('-' = stdin/stdout)\n"
"  -m magtape layout (62 directory blocks, 496 files; default)\n"
"  -D DECtape layout (24 directory blocks, 192 files, 578-block tape)\n"
"  -C dir  change to dir first      -T list  read file names from list ('-' = stdin)\n"
"  -b file boot block to store      -B file  write the archive's boot block to file\n"
"  -p      extract: preserve supported metadata (Windows cannot restore Unix owners/set-id/execute bits)\n"
"  -i      keep going after bad directory entries\n"
"  -U/-G   store this uid/gid for every file\n"
"  -v      verbose       --help  show this help\n",
        program, program, program, program, program);
}

static int tp_main(int argc, char **argv)
{
    int mode = 0, preserve = 0, ignore = 0;
    const char *archive = "-", *dir = NULL, *boot = NULL, *bootout = NULL, *listfile = NULL;
    const struct layout *lay = NULL;
    char **args;
    int nargs = 0, end_options = 0;

    if (argc > 0 && argv[0][0]) {
        const char *a = strrchr(argv[0], '/');
        const char *b = strrchr(argv[0], '\\');
        const char *base = a && b ? (a > b ? a : b) : (a ? a : b);
        prog = base ? base + 1 : argv[0];
    }
    args = (char **)calloc((size_t)(argc ? argc : 1), sizeof(char *));
    if (!args) die("out of memory");

    for (int i = 1; i < argc; ++i) {
        char *arg = argv[i];
        if (end_options) { args[nargs++] = arg; continue; }
        if (!strcmp(arg, "--")) { end_options = 1; continue; }
        if (!strcmp(arg, "--help")) { usage(stdout, argv[0]); free(args); return 0; }
        if (arg[0] != '-' || arg[1] == 0 || !strcmp(arg, "-")) { args[nargs++] = arg; continue; }
        for (size_t j = 1; arg[j]; ++j) {
            int opt = (unsigned char)arg[j];
            const char *value = NULL;
            switch (opt) {
            case 'c': case 'x': case 't': case 'k':
                if (mode && mode != opt) die("only one of -c -x -t -k may be given");
                mode = opt;
                break;
            case 'f': case 'C': case 'b': case 'B': case 'T': case 'U': case 'G':
                if (arg[j + 1]) { value = arg + j + 1; j = strlen(arg) - 1; }
                else if (i + 1 < argc) value = argv[++i];
                else die("option -%c requires an argument", opt);
                switch (opt) {
                case 'f': archive = value; break;
                case 'C': dir = value; break;
                case 'b': boot = value; break;
                case 'B': bootout = value; break;
                case 'T': listfile = value; break;
                case 'U': force_uid = atoi(value); break;
                case 'G': force_gid = atoi(value); break;
                }
                break;
            case 'v': verbose = 1; break;
            case 'm': lay = &MAG; break;
            case 'D': lay = &DEC; break;
            case 'p': preserve = 1; break;
            case 'i': ignore = 1; break;
            case 'h': usage(stdout, argv[0]); free(args); return 0;
            default: usage(stderr, argv[0]); free(args); return 2;
            }
            if (opt == 'f' || opt == 'C' || opt == 'b' || opt == 'B' || opt == 'T' || opt == 'U' || opt == 'G') break;
        }
    }
    if (!mode && !bootout) { usage(stderr, argv[0]); free(args); return 2; }

    if (mode == 'c') {
        if (!strcmp(archive, "-") && _isatty(_fileno(stdout))) die("refusing to write an archive to a terminal");
        struct list l = {0};
        char *arch_abs = NULL;
        char *cwd = NULL;
        if (dir && strcmp(archive, "-")) {
            cwd = tp_getcwd_alloc();
            if (!cwd) die("getcwd: %s", strerror(errno));
            if (tp_path_is_absolute(archive)) arch_abs = tp_strdup(archive);
            else arch_abs = path_join(cwd, archive);
            if (!arch_abs) die("out of memory");
            if (boot && !tp_path_is_absolute(boot)) {
                char *b = path_join(cwd, boot);
                if (!b) die("out of memory");
                boot = b;
            }
            if (listfile && strcmp(listfile, "-") && !tp_path_is_absolute(listfile)) {
                char *b = path_join(cwd, listfile);
                if (!b) die("out of memory");
                listfile = b;
            }
        }
        if (dir && tp_chdir(dir)) die("%s: %s", dir, strerror(errno));
        if (listfile) {
            FILE *f = !strcmp(listfile, "-") ? stdin : tp_fopen(listfile, "r");
            char line[8192];
            if (!f) die("%s: %s", listfile, strerror(errno));
            while (fgets(line, sizeof line, f)) {
                size_t n = strlen(line);
                if (n && line[n - 1] == '\n') line[--n] = 0;
                if (n && line[n - 1] == '\r') line[--n] = 0;
                if (n) walk(&l, line);
                if (n == sizeof(line) - 1 && line[n - 1] != '\n') {
                    int ch;
                    while ((ch = fgetc(f)) != EOF && ch != '\n') { }
                    warn("file list contains a path longer than %zu characters", sizeof(line) - 2);
                }
            }
            if (ferror(f)) die("%s: read error", listfile);
            if (f != stdin) fclose(f);
        }
        for (int i = 0; i < nargs; i++) walk(&l, args[i]);
        if (!listfile && nargs == 0) walk(&l, ".");
        if (errors) die("not creating the archive because of the errors above");
        create(arch_abs ? arch_abs : archive, lay ? lay : &MAG, &l, boot);
        free(cwd);
        free(arch_abs);
        for (size_t i = 0; i < l.n; ++i) { free(l.v[i].path); }
        free(l.v);
        free(args);
        return errors ? 1 : 0;
    }

    struct archive a;
    if (mode == 'x' && dir) {
        struct tp_stat st;
        if (tp_lstat(dir, &st)) {
            if (errno != ENOENT || tp_mkdir(dir) != 0) die("%s: %s", dir, strerror(errno));
        } else if (!st.is_dir || st.is_reparse) {
            die("%s: extraction destination is not a plain directory", dir);
        }
    }
    keep_going = ignore;
    open_archive(&a, archive, lay);
    if (bootout) {
        FILE *b = tp_fopen(bootout, "wb");
        if (!b) die("%s: %s", bootout, strerror(errno));
        write_all(b, a.img, BLK, bootout);
        if (fclose(b)) die("%s: write error", bootout);
        if (!mode) { free(a.ents); free(a.img); free(args); return errors ? 1 : 0; }
    }
    for (int i = 0; i < nargs; i++) {
        char *nn = norm_name(args[i], 1);
        if (nn) args[i] = nn;
    }
    switch (mode) {
    case 't':
        for (size_t i = 0; i < a.nents; i++)
            if (selected(a.ents[i].name, nargs, args)) {
                if (verbose) print_entry(&a.ents[i]);
                else puts(a.ents[i].name);
            }
        break;
    case 'x':
        extract(&a, nargs, args, dir ? dir : ".", preserve);
        break;
    case 'k': {
        size_t blocks = 0;
        for (size_t i = 0; i < a.nents; i++) blocks += (a.ents[i].size + BLK - 1) / BLK;
        printf("%s: %zu files, %zu data blocks, %u directory blocks scanned: %s\n", archive, a.nents, blocks,
               a.dirblocks, errors ? "PROBLEMS FOUND" : "ok");
        break;
    }
    }
    free(a.ents);
    free(a.img);
    free(args);
    return errors ? 1 : 0;
}

int wmain(int argc, wchar_t **wargv)
{
    char **argv = (char **)calloc((size_t)(argc ? argc : 1), sizeof(char *));
    int rc;
    if (!argv) {
        fputs("tp: out of memory\n", stderr);
        return 2;
    }
    for (int i = 0; i < argc; ++i) {
        argv[i] = wide_to_utf8(wargv[i]);
        if (!argv[i]) {
            for (int j = 0; j < i; ++j) free(argv[j]);
            free(argv);
            fputs("tp: cannot convert command line to UTF-8\n", stderr);
            return 2;
        }
    }
    rc = tp_main(argc, argv);
    for (int i = 0; i < argc; ++i) free(argv[i]);
    free(argv);
    return rc;
}
