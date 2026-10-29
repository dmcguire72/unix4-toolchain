/**
 * UNIX Fourth Edition tp(V) Extraction / Creation Utility
 */
#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BLK        512
#define ENTSZ      64
#define NAMELEN    32
#define MAXBLOCK   65535
#define MAXSIZE    0xffffffu

#define V4_ALLOC   0100000
#define V4_TYPE    060000
#define V4_SETUID  04000
#define V4_SETGID  02000

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

static int verbose, errors;
static const char *prog = "tp";

static void warn(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
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
static void note(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static int keep_going;
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
static void die(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));
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

static unsigned rd16(const unsigned char *p) { return p[0] | p[1] << 8; }
static void wr16(unsigned char *p, unsigned v) { p[0] = v & 255; p[1] = (v >> 8) & 255; }
static uint32_t le32(const unsigned char *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }

static char *norm_name(const char *p, int quiet)
{
    while (p[0] == '.' && p[1] == '/') { p += 2; while (*p == '/') p++; }
    if (*p == '/') {
        if (!quiet) fprintf(stderr, "%s: removing leading '/' from '%s'\n", prog, p);
        while (*p == '/') p++;
    }
    char *s = strdup(p);
    if (!s) die("out of memory");
    size_t n = strlen(s);
    while (n > 1 && s[n - 1] == '/') s[--n] = 0;
    for (const char *q = s;;) {
        const char *e = strchr(q, '/');
        size_t l = e ? (size_t)(e - q) : strlen(q);
        if (l == 2 && q[0] == '.' && q[1] == '.') {
            if (!quiet) warn("'%s': '..' is not allowed in tp names", p);
            free(s);
            return NULL;
        }
        if (!e) break;
        q = e + 1;
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
    memset(e, 0, ENTSZ);
    memcpy(e, en->name, strnlen(en->name, NAMELEN));
    wr16(e + 32, en->mode);
    e[34] = (unsigned char)en->uid;
    e[35] = (unsigned char)en->gid;
    e[37] = (unsigned char)(en->size >> 16);
    wr16(e + 38, en->size & 0xffff);
    wr16(e + 40, en->mtime >> 16);
    wr16(e + 42, en->mtime & 0xffff);
    wr16(e + 44, en->tapea);
    unsigned sum = 0;
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
    int fd = strcmp(path, "-") ? open(path, O_RDONLY) : 0;
    if (fd < 0) die("%s: %s", path, strerror(errno));
    size_t cap = 1 << 20, n = 0;
    unsigned char *b = malloc(cap);
    if (!b) die("out of memory");
    for (;;) {
        if (n == cap) {
            if (cap > ((size_t)MAXBLOCK + 1) * BLK * 2) die("%s: too large for a tp archive", path);
            cap *= 2;
            if (!(b = realloc(b, cap))) die("out of memory");
        }
        ssize_t r = read(fd, b + n, cap - n);
        if (r < 0) die("%s: %s", path, strerror(errno));
        if (r == 0) break;
        n += (size_t)r;
    }
    if (fd) close(fd);
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
    a->ents = malloc(cap * sizeof *a->ents);
    for (unsigned blk = 1; blk <= last && blk < a->nblocks && blk < first_data; blk++) {
        a->dirblocks = blk;
        for (int i = 0; i < BLK / ENTSZ; i++) {
            const unsigned char *e = a->img + (size_t)blk * BLK + (size_t)i * ENTSZ;
            if (rd16(e) == 0) continue;
            struct ent en;
            if (!unpack_entry(e, &en)) {
                char nm[NAMELEN + 1];
                memcpy(nm, e, NAMELEN); nm[NAMELEN] = 0;
                note("bad checksum in directory entry '%.32s'", nm);
                continue;
            }
            if (!memchr(en.name, 0, NAMELEN)) { note("directory entry name is not terminated"); continue; }
            char *norm = norm_name(en.name, 1);
            if (!norm) { note("refusing unsafe name '%s'", en.name); continue; }
            free(norm);
            if (en.size) {
                if (en.tapea <= a->dirblocks || (size_t)en.tapea * BLK + en.size > a->len) {
                    note("'%s': data (block %u, %u bytes) lies outside the archive", en.name, en.tapea, (unsigned)en.size);
                    continue;
                }
                if (en.tapea < first_data) first_data = en.tapea;
            }
            if (a->nents == cap) { cap *= 2; a->ents = realloc(a->ents, cap * sizeof *a->ents); }
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
    struct tm tm;
    gmtime_r(&t, &tm);
    strftime(ts, sizeof ts, "%Y-%m-%d %H:%M", &tm);
    modestr(en->mode, ms);
    printf("%s %3u/%-3u %8u %s  blk %-5u %s\n", ms, en->uid, en->gid, (unsigned)en->size, ts, en->tapea, en->name);
}

static int open_parent(int root, const char *name, const char **base)
{
    char *copy = strdup(name);
    if (!copy) die("out of memory");
    int fd = root, owned = 0;
    char *p = copy;
    for (char *slash; (slash = strchr(p, '/')); p = slash + 1) {
        *slash = 0;
        if (*p && strcmp(p, ".")) {
            if (mkdirat(fd, p, 0777) && errno != EEXIST) { warn("%s: mkdir: %s", name, strerror(errno)); goto fail; }
            int nfd = openat(fd, p, O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
            if (nfd < 0) { warn("%s: %s (not a plain directory)", name, strerror(errno)); goto fail; }
            if (owned) close(fd);
            fd = nfd;
            owned = 1;
        }
    }
    *base = name + (p - copy);
    free(copy);
    return fd;
fail:
    if (owned) close(fd);
    free(copy);
    return -1;
}

static void extract(const struct archive *a, int nsel, char **sel, const char *dest, int preserve)
{
    int root = open(dest, O_RDONLY | O_DIRECTORY);
    if (root < 0) die("%s: %s", dest, strerror(errno));
    mode_t um = umask(0);
    umask(um);
    for (size_t i = 0; i < a->nents; i++) {
        const struct ent *en = &a->ents[i];
        if (!selected(en->name, nsel, sel)) continue;
        if (verbose) printf("x %s\n", en->name);
        if ((en->mode & V4_TYPE) != 0) { warn("'%s' is not a plain file (mode %o), skipped", en->name, en->mode); continue; }
        const char *base;
        int dfd = open_parent(root, en->name, &base);
        if (dfd < 0) continue;
        mode_t perm = en->mode & 0777;
        if (preserve) {
            if (en->mode & V4_SETUID) perm |= S_ISUID;
            if (en->mode & V4_SETGID) perm |= S_ISGID;
        } else {
            perm &= ~um;
        }
        int fd = openat(dfd, base, O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0600);
        if (fd < 0) {
            warn("%s: %s", en->name, strerror(errno));
        } else {
            const unsigned char *data = a->img + (size_t)en->tapea * BLK;
            size_t left = en->size;
            int bad = 0;
            while (left) {
                ssize_t w = write(fd, data, left);
                if (w < 0) { warn("%s: write: %s", en->name, strerror(errno)); bad = 1; break; }
                data += w; left -= (size_t)w;
            }
            if (!bad) {
                if (preserve && geteuid() == 0 && fchown(fd, en->uid, en->gid)) warn("%s: chown: %s", en->name, strerror(errno));
                if (fchmod(fd, perm)) warn("%s: chmod: %s", en->name, strerror(errno));
                struct timespec ts[2] = {{(time_t)en->mtime, 0}, {(time_t)en->mtime, 0}};
                if (futimens(fd, ts)) warn("%s: utimens: %s", en->name, strerror(errno));
            }
            if (close(fd)) warn("%s: %s", en->name, strerror(errno));
        }
        if (dfd != root) close(dfd);
    }
    close(root);
}

struct list { struct ent *v; size_t n, cap; };
static int uid_warned;
static int force_uid = -1, force_gid = -1;

static void add_file(struct list *l, const char *path, const struct stat *st)
{
    char *name = norm_name(path, 0);
    if (!name) { errors++; return; }
    if (strlen(name) >= NAMELEN) { warn("'%s': name longer than %d characters", name, NAMELEN - 1); free(name); return; }
    for (size_t i = 0; i < l->n; i++)
        if (!strcmp(l->v[i].name, name)) { warn("'%s': duplicate entry", name); free(name); return; }
    if ((uintmax_t)st->st_size > MAXSIZE) { warn("'%s': larger than 16 MB", name); free(name); return; }
    if (l->n == l->cap) { l->cap = l->cap ? l->cap * 2 : 64; l->v = realloc(l->v, l->cap * sizeof *l->v); }
    struct ent *en = &l->v[l->n++];
    memset(en, 0, sizeof *en);
    memcpy(en->name, name, strlen(name));
    en->path = strdup(path);
    en->size = (uint32_t)st->st_size;
    en->mode = V4_ALLOC | (st->st_mode & 0777);
    if (st->st_mode & S_ISUID) en->mode |= V4_SETUID;
    if (st->st_mode & S_ISGID) en->mode |= V4_SETGID;
    uintmax_t uid = force_uid >= 0 ? (uintmax_t)force_uid : st->st_uid;
    uintmax_t gid = force_gid >= 0 ? (uintmax_t)force_gid : st->st_gid;
    if ((uid > 255 || gid > 255) && !uid_warned) {
        fprintf(stderr, "%s: warning: uid/gid above 255 do not fit a V4 byte and are stored as 0\n", prog);
        uid_warned = 1;
    }
    en->uid = uid > 255 ? 0 : (unsigned)uid;
    en->gid = gid > 255 ? 0 : (unsigned)gid;
    en->mtime = st->st_mtime < 0 ? 0 : (uint64_t)st->st_mtime > 0xffffffffu ? 0xffffffffu : (uint32_t)st->st_mtime;
    free(name);
}

static int dcmp(const struct dirent **a, const struct dirent **b) { return strcmp((*a)->d_name, (*b)->d_name); }
static int dirsel(const struct dirent *d) { return strcmp(d->d_name, ".") && strcmp(d->d_name, ".."); }

static void walk(struct list *l, const char *path)
{
    struct stat st;
    if (lstat(path, &st)) { warn("%s: %s", path, strerror(errno)); return; }
    if (S_ISREG(st.st_mode)) { add_file(l, path, &st); return; }
    if (S_ISDIR(st.st_mode)) {
        struct dirent **names;
        int n = scandir(path, &names, dirsel, dcmp);
        if (n < 0) { warn("%s: %s", path, strerror(errno)); return; }
        for (int i = 0; i < n; i++) {
            size_t len = strlen(path) + strlen(names[i]->d_name) + 2;
            char *child = malloc(len);
            if (!strcmp(path, ".")) snprintf(child, len, "%s", names[i]->d_name);
            else snprintf(child, len, "%s%s%s", path, path[strlen(path) - 1] == '/' ? "" : "/", names[i]->d_name);
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
        FILE *b = fopen(bootfile, "rb");
        if (!b) die("%s: %s", bootfile, strerror(errno));
        size_t n = fread(boot, 1, BLK, b);
        if (fgetc(b) != EOF) die("%s: boot block must be at most %d bytes", bootfile, BLK);
        (void)n;
        fclose(b);
    }
    unsigned char *dir = calloc(lay->dirblocks, BLK);
    for (size_t i = 0; i < l->n; i++) pack_entry(dir + i * ENTSZ, &l->v[i]);

    int tostdout = !strcmp(archive, "-");
    FILE *o = tostdout ? stdout : fopen(archive, "wb");
    if (!o) die("%s: %s", archive, strerror(errno));
    write_all(o, boot, BLK, archive);
    write_all(o, dir, (size_t)lay->dirblocks * BLK, archive);
    free(dir);
    size_t written = 1 + lay->dirblocks;
    for (size_t i = 0; i < l->n; i++) {
        struct ent *en = &l->v[i];
        if (verbose) fprintf(stderr, "a %s\n", en->name);
        FILE *f = fopen(en->path, "rb");
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
    if (!tostdout) { fclose(o); unlink(archive); }
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
"  -p      extract: restore set-id bits and (as root) owner\n"
"  -i      keep going after bad directory entries\n"
"  -U/-G   store this uid/gid for every file\n"
"  -v      verbose\n",
    program, program, program, program, program
    );
}

int main(int argc, char **argv)
{
    int mode = 0, preserve = 0, ignore = 0, opt;
    const char *archive = "-", *dir = NULL, *boot = NULL, *bootout = NULL, *listfile = NULL;
    const struct layout *lay = NULL;
    if (argc > 0) prog = argv[0][0] ? strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : argv[0] : "tp";
    static const struct option longopts[] = {{"help", 0, NULL, 'h'}, {0, 0, 0, 0}};
    while ((opt = getopt_long(argc, argv, "cxtkf:C:b:B:T:U:G:vmDpih", longopts, NULL)) != -1) {
        switch (opt) {
        case 'c': case 'x': case 't': case 'k':
            if (mode && mode != opt) die("only one of -c -x -t -k may be given");
            mode = opt; break;
        case 'f': archive = optarg; break;
        case 'C': dir = optarg; break;
        case 'b': boot = optarg; break;
        case 'B': bootout = optarg; break;
        case 'T': listfile = optarg; break;
        case 'U': force_uid = atoi(optarg); break;
        case 'G': force_gid = atoi(optarg); break;
        case 'v': verbose = 1; break;
        case 'm': lay = &MAG; break;
        case 'D': lay = &DEC; break;
        case 'p': preserve = 1; break;
        case 'i': ignore = 1; break;
        case 'h': usage(stdout, argv[0]); return 0;
        default: usage(stderr, argv[0]); return 2;
        }
    }
    if (!mode && !bootout) { usage(stderr, argv[0]); return 2; }
    char **args = argv + optind;
    int nargs = argc - optind;

    if (mode == 'c') {
        if (!strcmp(archive, "-") && isatty(1)) die("refusing to write an archive to a terminal");
        struct list l = {0};
        char *arch_abs = NULL;
        if (dir && strcmp(archive, "-")) {
            char cwd[PATH_MAX];
            if (!getcwd(cwd, sizeof cwd)) die("getcwd: %s", strerror(errno));
            if (archive[0] == '/') arch_abs = strdup(archive);
            else { arch_abs = malloc(strlen(cwd) + strlen(archive) + 2); sprintf(arch_abs, "%s/%s", cwd, archive); }
            if (boot && boot[0] != '/') { char *b = malloc(strlen(cwd) + strlen(boot) + 2); sprintf(b, "%s/%s", cwd, boot); boot = b; }
            if (listfile && strcmp(listfile, "-") && listfile[0] != '/') { char *b = malloc(strlen(cwd) + strlen(listfile) + 2); sprintf(b, "%s/%s", cwd, listfile); listfile = b; }
        }
        if (dir && chdir(dir)) die("%s: %s", dir, strerror(errno));
        if (listfile) {
            FILE *f = strcmp(listfile, "-") ? fopen(listfile, "r") : stdin;
            if (!f) die("%s: %s", listfile, strerror(errno));
            char *line = NULL; size_t lc = 0; ssize_t n;
            while ((n = getline(&line, &lc, f)) > 0) {
                if (line[n - 1] == '\n') line[--n] = 0;
                if (n) walk(&l, line);
            }
            free(line);
            if (f != stdin) fclose(f);
        }
        for (int i = 0; i < nargs; i++) walk(&l, args[i]);
        if (!listfile && nargs == 0) walk(&l, ".");
        if (errors) die("not creating the archive because of the errors above");
        create(arch_abs ? arch_abs : archive, lay ? lay : &MAG, &l, boot);
        return errors ? 1 : 0;
    }

    struct archive a;
    if (mode == 'x' && dir) {
        struct stat st;
        if (stat(dir, &st) && mkdir(dir, 0777)) die("%s: %s", dir, strerror(errno));
    }
    keep_going = ignore;
    open_archive(&a, archive, lay);
    if (bootout) {
        FILE *b = fopen(bootout, "wb");
        if (!b) die("%s: %s", bootout, strerror(errno));
        write_all(b, a.img, BLK, bootout);
        if (fclose(b)) die("%s: write error", bootout);
        if (!mode) return errors ? 1 : 0;
    }
    char **sel = args;
    for (int i = 0; i < nargs; i++) {
        char *nn = norm_name(args[i], 1);
        if (nn) sel[i] = nn;
    }
    switch (mode) {
    case 't':
        for (size_t i = 0; i < a.nents; i++)
            if (selected(a.ents[i].name, nargs, sel)) {
                if (verbose) print_entry(&a.ents[i]);
                else puts(a.ents[i].name);
            }
        break;
    case 'x':
        extract(&a, nargs, sel, dir ? dir : ".", preserve);
        break;
    case 'k': {
        size_t blocks = 0;
        for (size_t i = 0; i < a.nents; i++) blocks += (a.ents[i].size + BLK - 1) / BLK;
        printf("%s: %zu files, %zu data blocks, %u directory blocks scanned: %s\n", archive, a.nents, blocks,
               a.dirblocks, errors ? "PROBLEMS FOUND" : "ok");
        break;
    }
    }
    return errors ? 1 : 0;
}
