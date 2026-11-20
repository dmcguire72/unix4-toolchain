#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <langinfo.h>
#include <limits.h>
#include <locale.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifdef V4_HAVE_NCURSES
#include <ncurses.h>
#endif
#ifdef V4_HAVE_CURL
#include <curl/curl.h>
#endif
#ifdef V4_HAVE_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif
#ifdef V4_HAVE_ZLIB
#include <zlib.h>
#endif

#define STOCK_URL   "https://danielm.pages.dev/useful/unix_v4.tar.xz"
#define SYMBOLS_URL "https://danielm.pages.dev/useful/unix_v4_debug.tar.xz"
#define STOCK_MIRROR_URL   "https://dmcguire.pages.dev/useful/unix_v4.tar.xz"
#define SYMBOLS_MIRROR_URL "https://dmcguire.pages.dev/useful/unix_v4_debug.tar.xz"

#define V4_DEST       "/opt/v4/sys"
#define PDP11_DEST    "/opt/v4/bin"
#define MAN_DEST      "/opt/v4/man"
#define MAN_LINK_ROOT "/usr/local/share/man"
#define MAN_MANIFEST  MAN_DEST "/uninstall.conf"
#define MAGIC_DEST    "/etc/magic"
#define MAGIC_BEGIN   "# BEGIN v4linux managed v4dump.magic"
#define MAGIC_END     "# END v4linux managed v4dump.magic"
#define DISPATCH_DEST "/usr/local/libexec/v4linux-dispatch"
#define GUEST_ROOT    "/var/run/v4"
#define HOST_MOUNT    GUEST_ROOT "/host"
#define HOME_MOUNT    GUEST_ROOT "/home"
#define TMP_MOUNT     GUEST_ROOT "/tmp"
#define STATE_DIR     "/var/run/v4-overlay"
#define HOST_STAGING  "/var/run/v4-host-staging"
#define BINFMT_DIR    "/proc/sys/fs/binfmt_misc"
#define BINFMT_MARKER "/run/v4-binfmt-mounted-by-v4linux"
#define RUNTIME_SCRIPT PDP11_DEST "/v4linux-runtime"
#define SERVICE_FILE  "/etc/systemd/system/v4linux.service"
#define SERVICE_NAME  "v4linux.service"
#define LOG_FILE      "/var/log/v4linux-setup.log"

static const char *const BINFMT_NAMES[] = {"v4linux-aout-0407", "v4linux-aout-0410"};
static const char *const REQUIRED_TOOLS[] = {"as", "as2", "cc0", "cc1", "cc11", "ar",
                                             "ld", "tp", "strip", "v4dis", "dumpinfo"};
#define NTOOLS ((int)(sizeof REQUIRED_TOOLS / sizeof *REQUIRED_TOOLS))

static const char RUNTIME_TEMPLATE[] =
#include "v4linux-runtime.inc"
    ;

static const char SERVICE_TEMPLATE[] =
    "[Unit]\n"
    "Description=PDP-11 Unix v4 Persistence\n"
    "After=local-fs.target\n"
    "\n"
    "[Service]\n"
    "Type=oneshot\n"
    "RemainAfterExit=yes\n"
    "ExecStart=@RUNTIME_SCRIPT@\n"
    "ExecStop=/bin/umount -R @GUEST_ROOT@\n"
    "\n"
    "[Install]\n"
    "WantedBy=multi-user.target\n";

enum { E_OK = 0, E_FAIL = 1, E_CANCEL = 2 };

static char g_err[2048];
static volatile sig_atomic_t g_cancel;

static int fail(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static int fail(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(g_err, sizeof g_err, fmt, ap);
    va_end(ap);
    return E_FAIL;
}

#define TRY(x) do { int rc_ = (x); if (rc_) return rc_; } while (0)

static int tick(void) { return g_cancel ? E_CANCEL : E_OK; }

static void on_sigint(int sig) { (void)sig; g_cancel = 1; }

static void *xmalloc(size_t n) {
    void *p = malloc(n ? n : 1);
    if (!p) { fputs("v4linux-setup: out of memory\n", stderr); exit(1); }
    return p;
}

static void *xrealloc(void *p, size_t n) {
    p = realloc(p, n ? n : 1);
    if (!p) { fputs("v4linux-setup: out of memory\n", stderr); exit(1); }
    return p;
}

static char *xstrdup(const char *s) { return strcpy(xmalloc(strlen(s) + 1), s); }

static char *xasprintf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static char *xasprintf(const char *fmt, ...) {
    va_list ap;
    char *out;
    va_start(ap, fmt);
    if (vasprintf(&out, fmt, ap) < 0) { fputs("v4linux-setup: out of memory\n", stderr); exit(1); }
    va_end(ap);
    return out;
}

static double now_mono(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

static void sleep_ms(int ms) {
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000000L};
    nanosleep(&ts, NULL);
}

typedef struct { char **v; size_t n, cap; } SL;

static void sl_push(SL *l, char *s) {
    if (l->n == l->cap) { l->cap = l->cap ? l->cap * 2 : 16; l->v = xrealloc(l->v, l->cap * sizeof *l->v); }
    l->v[l->n++] = s;
}

static void sl_free(SL *l) {
    for (size_t i = 0; i < l->n; i++) free(l->v[i]);
    free(l->v);
    memset(l, 0, sizeof *l);
}

static int cmp_str(const void *a, const void *b) { return strcmp(*(char *const *)a, *(char *const *)b); }

typedef struct { char *d; size_t n, cap; } Buf;

static void buf_add(Buf *b, const char *s, size_t n) {
    if (b->n + n + 1 > b->cap) {
        b->cap = (b->n + n + 1) * 2;
        b->d = xrealloc(b->d, b->cap);
    }
    memcpy(b->d + b->n, s, n);
    b->n += n;
    b->d[b->n] = 0;
}

static const char *buf_str(Buf *b) { if (!b->d) buf_add(b, "", 0); return b->d; }

static bool starts_with(const char *s, const char *p) { return strncmp(s, p, strlen(p)) == 0; }

static bool lexists(const char *p) { struct stat st; return lstat(p, &st) == 0; }
static bool exists(const char *p) { struct stat st; return stat(p, &st) == 0; }
static bool isdir(const char *p) { struct stat st; return stat(p, &st) == 0 && S_ISDIR(st.st_mode); }
static bool isfile(const char *p) { struct stat st; return stat(p, &st) == 0 && S_ISREG(st.st_mode); }
static bool islink(const char *p) { struct stat st; return lstat(p, &st) == 0 && S_ISLNK(st.st_mode); }
static bool islnk_dir(const char *p) { struct stat st; return lstat(p, &st) == 0 && S_ISDIR(st.st_mode); }

static char *path_join(const char *a, const char *b) {
    if (b[0] == '/' || !a[0]) return xstrdup(b);
    size_t n = strlen(a);
    return xasprintf("%s%s%s", a, (n && a[n - 1] == '/') ? "" : "/", b);
}

static char *path_dirname(const char *p) {
    const char *slash = strrchr(p, '/');
    if (!slash) return xstrdup("");
    size_t n = (size_t)(slash - p) + 1;
    char *out = xmalloc(n + 1);
    memcpy(out, p, n);
    out[n] = 0;
    size_t k = n;
    while (k > 1 && out[k - 1] == '/') out[--k] = 0;
    return out;
}

static const char *path_basename(const char *p) {
    const char *slash = strrchr(p, '/');
    return slash ? slash + 1 : p;
}

static char *path_normpath(const char *path) {
    if (!path[0]) return xstrdup(".");
    bool abs_ = path[0] == '/';
    int lead = abs_ ? ((path[1] == '/' && path[2] != '/') ? 2 : 1) : 0;
    SL out = {0};
    char *copy = xstrdup(path), *save = NULL;
    for (char *tok = strtok_r(copy, "/", &save); tok; tok = strtok_r(NULL, "/", &save)) {
        if (!*tok || !strcmp(tok, ".")) continue;
        if (strcmp(tok, "..") ||
            (!abs_ && out.n == 0) || (out.n && !strcmp(out.v[out.n - 1], ".."))) {
            sl_push(&out, xstrdup(tok));
        } else if (out.n) {
            free(out.v[--out.n]);
        }
    }
    Buf b = {0};
    for (int i = 0; i < lead; i++) buf_add(&b, "/", 1);
    for (size_t i = 0; i < out.n; i++) {
        if (i) buf_add(&b, "/", 1);
        buf_add(&b, out.v[i], strlen(out.v[i]));
    }
    char *res = xstrdup(b.n ? buf_str(&b) : ".");
    free(b.d);
    sl_free(&out);
    free(copy);
    return res;
}

static char *path_abspath(const char *p) {
    if (p[0] == '/') return path_normpath(p);
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof cwd)) strcpy(cwd, "/");
    char *joined = path_join(cwd, p), *res = path_normpath(joined);
    free(joined);
    return res;
}

static char *path_realpath(const char *p) {
    char buf[PATH_MAX];
    if (realpath(p, buf)) return xstrdup(buf);
    char *abs_ = path_abspath(p);
    if (!strcmp(abs_, "/")) return abs_;
    char *dir = path_dirname(abs_);
    char *rdir = path_realpath(dir);
    char *res = path_join(rdir, path_basename(abs_));
    free(abs_); free(dir); free(rdir);
    return res;
}

static char *path_relpath(const char *path) {
    char *a = path_abspath(path), *c = path_abspath(".");
    SL as = {0}, cs = {0};
    char *sa = NULL, *sc = NULL;
    for (char *t = strtok_r(a, "/", &sa); t; t = strtok_r(NULL, "/", &sa)) sl_push(&as, xstrdup(t));
    for (char *t = strtok_r(c, "/", &sc); t; t = strtok_r(NULL, "/", &sc)) sl_push(&cs, xstrdup(t));
    size_t i = 0;
    while (i < as.n && i < cs.n && !strcmp(as.v[i], cs.v[i])) i++;
    Buf b = {0};
    for (size_t k = i; k < cs.n; k++) buf_add(&b, b.n ? "/.." : "..", b.n ? 3 : 2);
    for (size_t k = i; k < as.n; k++) {
        if (b.n) buf_add(&b, "/", 1);
        buf_add(&b, as.v[k], strlen(as.v[k]));
    }
    char *res = xstrdup(b.n ? buf_str(&b) : ".");
    free(b.d); sl_free(&as); sl_free(&cs); free(a); free(c);
    return res;
}

static int mkdir_p(const char *path, mode_t mode) {
    if (!path[0] || isdir(path)) return 0;
    char *tmp = xstrdup(path);
    for (char *p = tmp + 1; *p; p++) {
        if (*p != '/') continue;
        *p = 0;
        if (mkdir(tmp, 0777) < 0 && errno != EEXIST) { int e = errno; free(tmp); errno = e; return -1; }
        *p = '/';
    }
    int rc = mkdir(tmp, mode);
    int e = errno;
    free(tmp);
    if (rc < 0 && e != EEXIST) { errno = e; return -1; }
    if (rc < 0 && !isdir(path)) { errno = EEXIST; return -1; }
    return 0;
}

static int rm_cb(const char *p, const struct stat *st, int flag, struct FTW *f) {
    (void)st; (void)flag; (void)f;
    remove(p);
    return 0;
}

static void rm_rf(const char *path) {
    if (islnk_dir(path)) nftw(path, rm_cb, 32, FTW_DEPTH | FTW_PHYS);
    else if (lexists(path)) unlink(path);
}

static char *read_file(const char *path, size_t *len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    Buf b = {0};
    char tmp[65536];
    size_t n;
    while ((n = fread(tmp, 1, sizeof tmp, f)) > 0) buf_add(&b, tmp, n);
    bool err = ferror(f);
    fclose(f);
    if (err) { free(b.d); return NULL; }
    *len = b.n;
    return (char *)buf_str(&b);
}

static bool which(const char *cmd) {
    const char *path = getenv("PATH");
    if (!path) path = "/usr/local/bin:/usr/bin:/bin";
    char *copy = xstrdup(path), *save = NULL;
    bool found = false;
    for (char *d = strtok_r(copy, ":", &save); d && !found; d = strtok_r(NULL, ":", &save)) {
        char *p = path_join(d, cmd);
        struct stat st;
        found = stat(p, &st) == 0 && S_ISREG(st.st_mode) && access(p, X_OK) == 0;
        free(p);
    }
    free(copy);
    return found;
}

static char *str_replace_all(char *s, const char *key, const char *val) {
    Buf b = {0};
    size_t kl = strlen(key);
    for (const char *p = s; *p;) {
        if (!strncmp(p, key, kl)) { buf_add(&b, val, strlen(val)); p += kl; }
        else { buf_add(&b, p, 1); p++; }
    }
    char *res = xstrdup(buf_str(&b));
    free(b.d);
    free(s);
    return res;
}

static char *fill(const char *tmpl) {
    static const char *const kv[][2] = {
        {"@V4_DEST@", V4_DEST}, {"@GUEST_ROOT@", GUEST_ROOT}, {"@HOST_MOUNT@", HOST_MOUNT},
        {"@HOME_MOUNT@", HOME_MOUNT}, {"@TMP_MOUNT@", TMP_MOUNT}, {"@STATE_DIR@", STATE_DIR},
        {"@HOST_STAGING@", HOST_STAGING}, {"@BINFMT_DIR@", BINFMT_DIR},
        {"@BINFMT_MARKER@", BINFMT_MARKER}, {"@DISPATCH_DEST@", DISPATCH_DEST},
        {"@RUNTIME_SCRIPT@", RUNTIME_SCRIPT}};
    char *s = xstrdup(tmpl);
    for (size_t i = 0; i < sizeof kv / sizeof *kv; i++) s = str_replace_all(s, kv[i][0], kv[i][1]);
    return s;
}

static char *link_name(const char *tool) {
    if (!strcmp(tool, "cc11")) return xstrdup("v4-cc");
    if (!strcmp(tool, "v4dis")) return xstrdup("v4-disasm");
    if (!strcmp(tool, "dumpinfo")) return xstrdup("v4-dumpinfo");
    return xasprintf("v4-%s", tool);
}

static bool is_url(const char *t) {
    return starts_with(t, "http://") || starts_with(t, "https://") ||
           starts_with(t, "ftp://") || starts_with(t, "file://");
}

static void unescape_mountinfo(const char *in, char *out) {
    size_t n = strlen(in), j = 0;
    for (size_t i = 0; i < n;) {
        if (in[i] == '\\' && i + 3 < n && isdigit((unsigned char)in[i + 1]) &&
            isdigit((unsigned char)in[i + 2]) && isdigit((unsigned char)in[i + 3])) {
            out[j++] = (char)strtol((char[]){in[i + 1], in[i + 2], in[i + 3], 0}, NULL, 8);
            i += 4;
        } else {
            out[j++] = in[i++];
        }
    }
    out[j] = 0;
}

static bool is_mountpoint(const char *path) {
    char *real = path_realpath(path);
    bool found = false;
    FILE *f = fopen("/proc/self/mountinfo", "r");
    if (f) {
        char *line = NULL;
        size_t cap = 0;
        while (!found && getline(&line, &cap, f) > 0) {
            char *save = NULL, *tok = strtok_r(line, " \t\r\n", &save);
            for (int i = 0; tok && i < 4; i++) tok = strtok_r(NULL, " \t\r\n", &save);
            if (!tok) continue;
            char *dec = xmalloc(strlen(tok) + 1);
            unescape_mountinfo(tok, dec);
            found = !strcmp(dec, real);
            free(dec);
        }
        free(line);
        fclose(f);
    }
    free(real);
    return found;
}

static bool fs_supported(const char *name) {
    FILE *f = fopen("/proc/filesystems", "r");
    if (!f) return false;
    char *line = NULL;
    size_t cap = 0;
    bool found = false;
    while (!found && getline(&line, &cap, f) > 0) {
        char *last = NULL, *save = NULL;
        for (char *t = strtok_r(line, " \t\r\n", &save); t; t = strtok_r(NULL, " \t\r\n", &save)) last = t;
        found = last && !strcmp(last, name);
    }
    free(line);
    fclose(f);
    return found;
}

static bool valid_man_rel(const char *rel) {
    const char *slash = strchr(rel, '/');
    if (!slash) return false;
    const char *name = slash + 1;
    return starts_with(rel, "man") && *name && !strchr(name, '/') && strcmp(name, ".") && strcmp(name, "..");
}

static void human_bytes(double n, char *out, size_t sz) {
    static const char *const units[] = {"B", "KB", "MB", "GB"};
    for (int i = 0; i < 4; i++) {
        if (n < 1024 || i == 3) {
            if (i == 0) snprintf(out, sz, "%.0f B", n);
            else snprintf(out, sz, "%.1f %s", n, units[i]);
            return;
        }
        n /= 1024;
    }
}

static void human_time(double s, char *out, size_t sz) {
    if (s < 0) s = 0;
    if (s < 1.0) { snprintf(out, sz, "%.2fms", s * 1000); return; }
    if (s < 2.0) { snprintf(out, sz, "%.2fs", s); return; }
    int secs = (int)s, m = secs / 60, sec = secs % 60, h = m / 60;
    m %= 60;
    if (h) snprintf(out, sz, "%d:%02d:%02d", h, m, sec);
    else snprintf(out, sz, "%d:%02d", m, sec);
}

enum { PENDING, RUNNING, DONE, WARN, FAILED, SKIPPED };
#define IS_FINISHED(s) ((s) == DONE || (s) == WARN || (s) == SKIPPED)
#define LOGMAX 500

typedef struct {
    const char *key;
    char *title;
    double weight;
    int status;
    double frac;
    char detail[512], note[512];
    double t0, t1;
} Step;

typedef struct { char *k, *v; } KV;

static struct {
    pthread_mutex_t lock;
    const char *title;
    char *subtitle;
    Step steps[16];
    int nsteps, cur, failed;
    char *logbuf[LOGMAX];
    int log_head, log_n;
    FILE *logfile;
    double t0, t1;
    volatile int cancelled;
    const char *result;
    char error[2048];
    KV summary[16];
    int nsummary;
    char *footer[4];
    int nfooter;
    char *warnings[32];
    int nwarn;
} M = {.lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP, .cur = -1, .failed = -1};

enum { UI_PLAIN, UI_CURSES };

static struct {
    int mode;
    bool verbose, tty, uni, line_open;
    double last_draw;
} UI;

#define LOCK()   pthread_mutex_lock(&M.lock)
#define UNLOCK() pthread_mutex_unlock(&M.lock)

static bool terminal_is_utf8(void) {
    const char *term = getenv("TERM");
    if (term && !strcmp(term, "linux")) return false;
    const char *cs = nl_langinfo(CODESET);
    if (!cs) return false;
    char norm[32];
    size_t j = 0;
    for (const char *p = cs; *p && j + 1 < sizeof norm; p++)
        if (*p != '-') norm[j++] = (char)toupper((unsigned char)*p);
    norm[j] = 0;
    return !strcmp(norm, "UTF8");
}

static void term_size(int *cols, int *lines) {
    int c = 0, l = 0;
    const char *e;
    if ((e = getenv("COLUMNS")) && atoi(e) > 0) c = atoi(e);
    if ((e = getenv("LINES")) && atoi(e) > 0) l = atoi(e);
    struct winsize ws;
    if ((!c || !l) && ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (!c && ws.ws_col > 0) c = ws.ws_col;
        if (!l && ws.ws_row > 0) l = ws.ws_row;
    }
    *cols = c ? c : 80;
    *lines = l ? l : 24;
}

static int term_width(void) {
    int c, l;
    term_size(&c, &l);
    return c - 1 < 40 ? 40 : c - 1;
}

static size_t u8_len(const char *s) {
    size_t n = 0;
    for (; *s; s++) if (((unsigned char)*s & 0xC0) != 0x80) n++;
    return n;
}

static size_t u8_off(const char *s, size_t cps) {
    size_t i = 0, n = 0;
    while (s[i]) {
        if (((unsigned char)s[i] & 0xC0) != 0x80) {
            if (n == cps) break;
            n++;
        }
        i++;
    }
    return i;
}

__attribute__((unused)) static char *u8_clip(const char *text, size_t width) {
    if (u8_len(text) <= width) return xstrdup(text);
    size_t keep = width > 0 ? width - 1 : 0;
    return xasprintf("%.*s%s", (int)u8_off(text, keep), text, UI.uni ? "\xe2\x80\xa6" : "~");
}

static char *u8_ljust(const char *s, long width) {
    long n = (long)u8_len(s);
    return xasprintf("%s%*s", s, n < width ? (int)(width - n) : 0, "");
}

static long round_half_even(double x) {
    long i = (long)x;
    double d = x - (double)i;
    if (d < 0.5) return i;
    if (d > 0.5) return i + 1;
    return i % 2 == 0 ? i : i + 1;
}

static char *ascii_bar(double frac, long width, const char *full, const char *empty) {
    if (frac < 0) frac = 0;
    if (frac > 1) frac = 1;
    long filled = round_half_even(frac * (double)width);
    Buf b = {0};
    for (long i = 0; i < width; i++) {
        const char *g = i < filled ? full : empty;
        buf_add(&b, g, strlen(g));
    }
    char *res = xstrdup(buf_str(&b));
    free(b.d);
    return res;
}

static void wrap_text(const char *text, size_t width, SL *out) {
    if (width < 1) width = 1;
    char *copy = xstrdup(text), *save = NULL;
    Buf line = {0};
    size_t cols = 0;
    for (char *w = strtok_r(copy, " \t\r\n", &save); w; w = strtok_r(NULL, " \t\r\n", &save)) {
        size_t n = u8_len(w);
        if (n > width) {
            if (cols) { sl_push(out, xstrdup(buf_str(&line))); line.n = 0; cols = 0; }
            while (n > width) {
                size_t off = u8_off(w, width);
                sl_push(out, xasprintf("%.*s", (int)off, w));
                w += off;
                n -= width;
            }
        }
        if (cols && cols + 1 + n > width) { sl_push(out, xstrdup(buf_str(&line))); line.n = 0; cols = 0; }
        if (cols) { buf_add(&line, " ", 1); cols++; }
        buf_add(&line, w, strlen(w));
        cols += n;
    }
    if (cols) sl_push(out, xstrdup(buf_str(&line)));
    free(line.d);
    free(copy);
}

static double step_elapsed(const Step *s) {
    if (!s->t0) return 0;
    return (s->t1 ? s->t1 : now_mono()) - s->t0;
}

static double model_elapsed(void) { return (M.t1 ? M.t1 : now_mono()) - M.t0; }

__attribute__((unused)) static double model_overall(void) {
    LOCK();
    double total = 0, done = 0;
    for (int i = 0; i < M.nsteps; i++) {
        total += M.steps[i].weight;
        if (IS_FINISHED(M.steps[i].status)) done += M.steps[i].weight;
    }
    if (total == 0) total = 1;
    if (M.cur >= 0 && M.steps[M.cur].frac >= 0) {
        double f = M.steps[M.cur].frac;
        done += M.steps[M.cur].weight * (f > 1 ? 1 : f);
    }
    UNLOCK();
    return done / total > 1 ? 1 : done / total;
}

static void ui_close_line(void) {
    if (UI.line_open) { putchar('\n'); UI.line_open = false; }
    fflush(stdout);
}

static void step_prefix(int idx, char *out, size_t sz) {
    snprintf(out, sz, "[%d/%d] %s", idx + 1, M.nsteps, M.steps[idx].title);
}

static void plain_draw(int idx, bool force) {
    double now = now_mono();
    if (!force && now - UI.last_draw < 0.1) return;
    UI.last_draw = now;
    Step *s = &M.steps[idx];
    char head[256], line[1280];
    step_prefix(idx, head, sizeof head);
    if (s->frac < 0) {
        snprintf(line, sizeof line, "%s  %s", head, s->detail);
    } else {
        char *bar = ascii_bar(s->frac, 20, "#", "-");
        snprintf(line, sizeof line, "%s  [%s] %3d%%  %s", head, bar, (int)(s->frac * 100), s->detail);
        free(bar);
    }
    fputs("\r\033[K", stdout);
    size_t off = u8_off(line, (size_t)term_width());
    fwrite(line, 1, off, stdout);
    fflush(stdout);
    UI.line_open = true;
}

static void plain_begin(int idx) {
    ui_close_line();
    char head[256];
    step_prefix(idx, head, sizeof head);
    if (UI.tty && !UI.verbose) {
        plain_draw(idx, true);
    } else if (UI.verbose) {
        printf("%s ...\n", head);
        fflush(stdout);
    } else {
        printf("%s ... ", head);
        fflush(stdout);
        UI.line_open = true;
    }
}

static void plain_update(int idx) {
    if (UI.tty && !UI.verbose) plain_draw(idx, false);
}

static void plain_end(int idx) {
    Step *s = &M.steps[idx];
    const char *tag = s->status == DONE ? "done" : s->status == WARN ? "done with warnings" : "FAILED";
    char note[600] = "", tm[32], head[256], line[1500];
    if (s->note[0]) snprintf(note, sizeof note, " (%s)", s->note);
    human_time(step_elapsed(s), tm, sizeof tm);
    step_prefix(idx, head, sizeof head);
    if (UI.tty && !UI.verbose) {
        snprintf(line, sizeof line, "%s ... %s%s [%s]", head, tag, note, tm);
        fputs("\r\033[K", stdout);
        fwrite(line, 1, u8_off(line, (size_t)term_width()), stdout);
        putchar('\n');
    } else if (UI.verbose) {
        printf("%s ... %s%s [%s]\n", head, tag, note, tm);
    } else {
        printf("%s%s [%s]\n", tag, note, tm);
    }
    fflush(stdout);
    UI.line_open = false;
}

static void plain_skip(int idx) {
    ui_close_line();
    Step *s = &M.steps[idx];
    printf("[%d/%d] %s ... skipped%s%s%s\n", idx + 1, M.nsteps, s->title,
           s->note[0] ? " (" : "", s->note, s->note[0] ? ")" : "");
    fflush(stdout);
}

static void plain_log(const char *text) {
    if (!UI.verbose || !strncmp(text, "==", 2)) return;
    ui_close_line();
    printf("    %s\n", text);
    fflush(stdout);
}

static void add_log_locked(const char *text) {
    char *copy = xstrdup(text);
    if (M.log_n == LOGMAX) {
        free(M.logbuf[M.log_head]);
        M.logbuf[M.log_head] = copy;
        M.log_head = (M.log_head + 1) % LOGMAX;
    } else {
        M.logbuf[(M.log_head + M.log_n++) % LOGMAX] = copy;
    }
    if (M.logfile) {
        char stamp[16];
        time_t t = time(NULL);
        struct tm tm;
        localtime_r(&t, &tm);
        strftime(stamp, sizeof stamp, "%H:%M:%S", &tm);
        if (fprintf(M.logfile, "%s %s\n", stamp, text) < 0 || fflush(M.logfile) != 0) {
            fclose(M.logfile);
            M.logfile = NULL;
        }
    }
}

static void m_log(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void m_log(const char *fmt, ...) {
    char text[4096];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(text, sizeof text, fmt, ap);
    va_end(ap);
    LOCK();
    add_log_locked(text);
    UNLOCK();
    if (UI.mode == UI_PLAIN) plain_log(text);
}

static void m_begin(int idx) {
    char text[600];
    LOCK();
    Step *s = &M.steps[idx];
    s->status = RUNNING;
    s->t0 = now_mono();
    s->frac = -1;
    s->detail[0] = 0;
    M.cur = idx;
    snprintf(text, sizeof text, "== %s", s->title);
    add_log_locked(text);
    UNLOCK();
    if (UI.mode == UI_PLAIN) { plain_log(text); plain_begin(idx); }
}

static void m_update(double frac, const char *detail_fmt, ...) __attribute__((format(printf, 2, 3)));
static void m_update(double frac, const char *detail_fmt, ...) {
    LOCK();
    int idx = M.cur;
    if (idx < 0) { UNLOCK(); return; }
    va_list ap;
    va_start(ap, detail_fmt);
    vsnprintf(M.steps[idx].detail, sizeof M.steps[idx].detail, detail_fmt, ap);
    va_end(ap);
    M.steps[idx].frac = frac;
    UNLOCK();
    if (UI.mode == UI_PLAIN) plain_update(idx);
}

static void m_end(int status, const char *note) {
    char text[600] = "";
    LOCK();
    int idx = M.cur;
    if (idx < 0) { UNLOCK(); return; }
    Step *s = &M.steps[idx];
    s->status = status;
    snprintf(s->note, sizeof s->note, "%s", note ? note : "");
    s->t1 = now_mono();
    if (status == FAILED) M.failed = idx;
    if (IS_FINISHED(status)) s->frac = 1.0;
    M.cur = -1;
    if (s->note[0]) { snprintf(text, sizeof text, "   %s", s->note); add_log_locked(text); }
    UNLOCK();
    if (UI.mode == UI_PLAIN) { if (text[0]) plain_log(text); plain_end(idx); }
}

static void m_skip(int idx, const char *note) {
    char text[700];
    LOCK();
    Step *s = &M.steps[idx];
    s->status = SKIPPED;
    snprintf(s->note, sizeof s->note, "%s", note ? note : "");
    snprintf(text, sizeof text, "== %s (skipped%s%s)", s->title, s->note[0] ? ": " : "", s->note);
    add_log_locked(text);
    UNLOCK();
    if (UI.mode == UI_PLAIN) plain_skip(idx);
}

static void m_warn(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void m_warn(const char *fmt, ...) {
    char text[1024], line[1100];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(text, sizeof text, fmt, ap);
    va_end(ap);
    LOCK();
    if (M.nwarn < 32) M.warnings[M.nwarn++] = xstrdup(text);
    snprintf(line, sizeof line, "warning: %s", text);
    add_log_locked(line);
    UNLOCK();
    if (UI.mode == UI_PLAIN) plain_log(line);
}

static void m_finish(const char *result, const char *error) {
    LOCK();
    M.result = result;
    M.t1 = now_mono();
    snprintf(M.error, sizeof M.error, "%s", error ? error : "");
    UNLOCK();
    if (UI.mode == UI_PLAIN) ui_close_line();
}

static void m_summary(const char *k, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
static void m_summary(const char *k, const char *fmt, ...) {
    if (M.nsummary >= 16) return;
    va_list ap;
    va_start(ap, fmt);
    char *v;
    if (vasprintf(&v, fmt, ap) < 0) exit(1);
    va_end(ap);
    M.summary[M.nsummary].k = xstrdup(k);
    M.summary[M.nsummary++].v = v;
}

static int step_index(const char *key) {
    for (int i = 0; i < M.nsteps; i++)
        if (!strcmp(M.steps[i].key, key)) return i;
    return -1;
}

static void add_step(const char *key, const char *title, double weight) {
    M.steps[M.nsteps] = (Step){.key = key, .title = (char *)title, .weight = weight, .status = PENDING, .frac = -1};
    M.nsteps++;
}

static void format_result(void) {
    const char *ok = UI.uni ? "\xe2\x9c\x93" : "OK";
    const char *bad = UI.uni ? "\xe2\x9c\x97" : "FAILED";
    char tm[32];
    human_time(model_elapsed(), tm, sizeof tm);
    if (!strcmp(M.result, "ok")) {
        printf("%s Installation complete in %s\n", ok, tm);
        if (M.nsummary) {
            size_t width = 0;
            for (int i = 0; i < M.nsummary; i++)
                if (u8_len(M.summary[i].k) > width) width = u8_len(M.summary[i].k);
            putchar('\n');
            for (int i = 0; i < M.nsummary; i++) {
                char *k = u8_ljust(M.summary[i].k, (long)width);
                printf("  %s  %s\n", k, M.summary[i].v);
                free(k);
            }
        }
    } else if (!strcmp(M.result, "cancelled")) {
        puts("Installation cancelled.");
    } else {
        if (M.failed >= 0) printf("%s Installation failed while running '%s'\n", bad, M.steps[M.failed].title);
        else printf("%s Installation failed\n", bad);
        if (M.error[0]) {
            SL lines = {0};
            wrap_text(M.error, 76, &lines);
            putchar('\n');
            for (size_t i = 0; i < lines.n; i++) printf("  %s\n", lines.v[i]);
            sl_free(&lines);
        }
    }
    if (M.nwarn) {
        putchar('\n');
        for (int i = 0; i < M.nwarn; i++) printf("  ! %s\n", M.warnings[i]);
    }
    if (M.nfooter) {
        putchar('\n');
        for (int i = 0; i < M.nfooter; i++) puts(M.footer[i]);
    }
}

static int plain_menu(const char *title, const char *subtitle, const char *const *labels,
                      const char *const *descs, int n, int def) {
    printf("\n%s\n", title);
    if (subtitle && *subtitle) printf("  %s\n", subtitle);
    for (int i = 0; i < n; i++) {
        printf("  %d - %s", i + 1, labels[i]);
        if (descs && descs[i] && *descs[i]) printf("  (%s)", descs[i]);
        putchar('\n');
    }
    for (int attempt = 0; attempt < 3; attempt++) {
        printf("Selection [1-%d, default %d]: ", n, def + 1);
        fflush(stdout);
        char *line = NULL;
        size_t cap = 0;
        ssize_t got = getline(&line, &cap, stdin);
        if (got < 0 || g_cancel) { free(line); putchar('\n'); return -1; }
        char *p = line;
        while (isspace((unsigned char)*p)) p++;
        size_t len = strlen(p);
        while (len && isspace((unsigned char)p[len - 1])) p[--len] = 0;
        if (!*p) { free(line); return def; }
        bool digits = true;
        for (char *q = p; *q; q++) if (!isdigit((unsigned char)*q)) digits = false;
        if (digits && strlen(p) < 6 && atoi(p) >= 1 && atoi(p) <= n) { int r = atoi(p) - 1; free(line); return r; }
        free(line);
        puts("  Not a valid choice.");
    }
    return -1;
}

#ifdef V4_HAVE_NCURSES

#define CU_MIN_ROWS 12
#define CU_MIN_COLS 50

static struct {
    bool active;
    chtype ok, bad, warn, accent;
} CU;

typedef struct {
    const char *full, *empty, *rule, *ok, *bad, *warn, *run, *pend, *skip, *sel;
    const char *spin[10];
    int nspin;
} Glyphs;

static Glyphs cu_glyphs(void) {
    if (UI.uni)
        return (Glyphs){"\xe2\x96\x88", "\xe2\x96\x91", "\xe2\x94\x80", "\xe2\x9c\x94", "\xe2\x9c\x98", "!",
                        "\xe2\x96\xb6", "\xe2\x97\x8b", "\xe2\x80\x93", "\xe2\x80\xba",
                        {"\xe2\xa0\x8b", "\xe2\xa0\x99", "\xe2\xa0\xb9", "\xe2\xa0\xb8", "\xe2\xa0\xbc",
                         "\xe2\xa0\xb4", "\xe2\xa0\xa6", "\xe2\xa0\xa7", "\xe2\xa0\x87", "\xe2\xa0\x8f"}, 10};
    return (Glyphs){"#", "-", "-", "+", "x", "!", ">", ".", "-", ">", {"|", "/", "-", "\\"}, 4};
}

static bool cu_init(void) {
    setenv("ESCDELAY", "25", 0);
    SCREEN *scr = newterm(NULL, stdout, stdin);
    if (!scr) return false;
    set_term(scr);
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
#ifdef V4_NCURSESW
    UI.uni = terminal_is_utf8();
#else
    UI.uni = false;
#endif
    CU.ok = CU.bad = CU.warn = CU.accent = 0;
    if (has_colors()) {
        start_color();
        short bg = COLOR_BLACK;
        if (use_default_colors() != ERR) bg = -1;
        init_pair(1, COLOR_GREEN, bg);
        init_pair(2, COLOR_RED, bg);
        init_pair(3, COLOR_YELLOW, bg);
        init_pair(4, COLOR_CYAN, bg);
        CU.ok = COLOR_PAIR(1);
        CU.bad = COLOR_PAIR(2);
        CU.warn = COLOR_PAIR(3);
        CU.accent = COLOR_PAIR(4);
    }
    CU.active = true;
    return true;
}

static void cu_end(void) {
    if (!CU.active) return;
    CU.active = false;
    keypad(stdscr, FALSE);
    nocbreak();
    echo();
    endwin();
}

static void cu_put(int y, int x, const char *text, chtype attr) {
    int h, w;
    getmaxyx(stdscr, h, w);
    if (y < 0 || y >= h || x >= w - 1 || !text || !*text) return;
    size_t off = u8_off(text, (size_t)(w - x - 1));
    attrset(attr);
    mvaddnstr(y, x, text, (int)off);
    attrset(A_NORMAL);
}

static chtype cu_status_attr(int status) {
    switch (status) {
    case DONE: return CU.ok;
    case WARN: return CU.warn;
    case FAILED: return CU.bad;
    case RUNNING: return CU.accent;
    case SKIPPED:
    case PENDING: return A_DIM;
    }
    return 0;
}

typedef struct { char *text; chtype attr; } Row;
typedef struct { Row *v; size_t n, cap; } Rows;

static void rows_add(Rows *r, char *text, chtype attr) {
    if (r->n == r->cap) { r->cap = r->cap ? r->cap * 2 : 32; r->v = xrealloc(r->v, r->cap * sizeof *r->v); }
    r->v[r->n++] = (Row){text, attr};
}

static void rows_free(Rows *r) {
    for (size_t i = 0; i < r->n; i++) free(r->v[i].text);
    free(r->v);
    memset(r, 0, sizeof *r);
}

static void cu_draw(bool final) {
    Glyphs g = cu_glyphs();
    erase();
    int h, w;
    getmaxyx(stdscr, h, w);
    if (h < CU_MIN_ROWS || w < CU_MIN_COLS) {
        char tot[32];
        cu_put(0, 0, "Terminal too small, enlarge it to see progress.", 0);
        snprintf(tot, sizeof tot, "Total: %d%%", (int)(model_overall() * 100));
        cu_put(1, 0, tot, 0);
        refresh();
        return;
    }

    LOCK();
    const char *result = M.result;
    const char *status;
    chtype attr;
    if (result && !strcmp(result, "ok")) { status = "installation complete"; attr = CU.ok | A_BOLD; }
    else if (result && !strcmp(result, "failed")) { status = "installation failed"; attr = CU.bad | A_BOLD; }
    else if (result && !strcmp(result, "cancelled")) { status = "cancelled"; attr = CU.warn | A_BOLD; }
    else if (M.cancelled) { status = "cancelling..."; attr = CU.warn | A_BOLD; }
    else { status = "installing..."; attr = CU.accent | A_BOLD; }
    bool r_ok = result && !strcmp(result, "ok"), r_failed = result && !strcmp(result, "failed"),
         r_cancelled = result && !strcmp(result, "cancelled");

    cu_put(0, 2, M.title, A_BOLD);
    cu_put(0, w - 2 - (int)strlen(status), status, attr);
    int y = 1;
    if (M.subtitle && h >= 16) {
        char *c = u8_clip(M.subtitle, (size_t)(w - 6));
        cu_put(1, 2, c, A_DIM);
        free(c);
        y = 2;
    }
    y += 1;

    double frac = model_overall();
    char tail[64], tm[32];
    human_time(model_elapsed(), tm, sizeof tm);
    snprintf(tail, sizeof tail, " %3d%%  %s", (int)(frac * 100), tm);
    int bar_w = w - 6 - (int)strlen(tail) - 10;
    if (bar_w < 10) bar_w = 10;
    cu_put(y, 2, "Total ", A_DIM);
    chtype bar_attr = r_failed ? CU.bad : r_ok ? CU.ok : CU.accent;
    char *bar = ascii_bar(frac, bar_w, g.full, g.empty);
    cu_put(y, 10, bar, bar_attr);
    free(bar);
    cu_put(y, 10 + bar_w, tail, 0);
    y += 2;

    if (!r_ok) {
        int reserve = 0;
        if (r_failed || r_cancelled) {
            SL wl = {0};
            wrap_text(M.error[0] ? M.error : "Cancelled.", (size_t)(w - 8 < 20 ? 20 : w - 8), &wl);
            reserve = (wl.n < 3 ? (int)wl.n : 3) + 2;
            sl_free(&wl);
        }
        int limit = h - 3 - reserve;
        if (limit < y + 2) limit = y + 2;
        int room = limit - y < 1 ? 1 : limit - y;
        int focus = -1;
        for (int i = 0; i < M.nsteps && focus < 0; i++)
            if (M.steps[i].status == RUNNING || M.steps[i].status == FAILED) focus = i;
        if (focus < 0) {
            for (int i = 0; i < M.nsteps && focus < 0; i++) if (M.steps[i].status == PENDING) focus = i;
            if (focus < 0) focus = M.nsteps - 1;
        }
        int first = 0;
        while (first < focus) {
            int used = (focus - first + 1) + (M.steps[focus].status == RUNNING ? 1 : 0);
            if (used <= room) break;
            first++;
        }
        for (int i = first; i < M.nsteps; i++) {
            if (y >= limit) break;
            Step *st = &M.steps[i];
            const char *glyph = st->status == DONE ? g.ok : st->status == WARN ? g.warn :
                                st->status == FAILED ? g.bad : st->status == SKIPPED ? g.skip :
                                st->status == PENDING ? g.pend : g.run;
            if (st->status == RUNNING && !result) glyph = g.spin[(long)(now_mono() * 10) % g.nspin];
            chtype sa = cu_status_attr(st->status);
            char right[32] = "";
            if ((IS_FINISHED(st->status) || st->status == FAILED) && st->t0) human_time(step_elapsed(st), right, sizeof right);
            char *full = (st->note[0] && st->status != RUNNING) ? xasprintf("%s  %s", st->title, st->note) : xstrdup(st->title);
            char *title = u8_clip(full, (size_t)(w - 14 - (int)strlen(right) < 0 ? 0 : w - 14 - (int)strlen(right)));
            char *gl = xasprintf("%s ", glyph);
            cu_put(y, 2, gl, sa);
            cu_put(y, 4, title, st->status == RUNNING ? A_BOLD : (st->status == PENDING || st->status == SKIPPED) ? A_DIM : 0);
            cu_put(y, w - 2 - (int)strlen(right), right, A_DIM);
            free(full); free(title); free(gl);
            y++;
            if (st->status == RUNNING && y < limit) {
                int inner = w - 56 < 24 ? w - 56 : 24;
                if (inner < 8) inner = 8;
                char *line_bar, pct[16] = "";
                if (st->frac < 0) {
                    int pos = (int)((long)(now_mono() * 12) % (inner > 1 ? inner : 1));
                    Buf b = {0};
                    bool *cells = xmalloc((size_t)inner * sizeof *cells);
                    for (int k = 0; k < inner; k++) cells[k] = false;
                    for (int k = 0; k < 4; k++) cells[(pos + k) % inner] = true;
                    for (int k = 0; k < inner; k++) buf_add(&b, cells[k] ? g.full : g.empty, strlen(cells[k] ? g.full : g.empty));
                    line_bar = xstrdup(buf_str(&b));
                    free(b.d); free(cells);
                } else {
                    line_bar = ascii_bar(st->frac, inner, g.full, g.empty);
                    snprintf(pct, sizeof pct, "%3d%%  ", (int)(st->frac * 100));
                }
                cu_put(y, 4, line_bar, CU.accent);
                free(line_bar);
                int x = 4 + inner + 2;
                char *d = xasprintf("%s%s", pct, st->detail);
                char *dc = u8_clip(d, (size_t)(w - x - 2 < 0 ? 0 : w - x - 2));
                cu_put(y, x, dc, 0);
                free(d); free(dc);
                y++;
            }
        }
        y += 1;
    }

    int rows = h - 2 - y, avail = rows - 1;
    Rows head = {0}, body = {0};
    const char *title = "Log";
    if (r_ok) {
        title = "Summary";
        size_t width = 0;
        for (int i = 0; i < M.nsummary; i++) if (u8_len(M.summary[i].k) > width) width = u8_len(M.summary[i].k);
        for (int i = 0; i < M.nsummary; i++) {
            char *k = u8_ljust(M.summary[i].k, (long)width);
            rows_add(&head, xasprintf("%s  %s", k, M.summary[i].v), 0);
            free(k);
        }
        for (int i = 0; i < M.nwarn; i++) rows_add(&head, xasprintf("%s %s", g.warn, M.warnings[i]), CU.warn);
        if (M.nfooter) {
            rows_add(&head, xstrdup(""), 0);
            for (int i = 0; i < M.nfooter; i++) rows_add(&head, xstrdup(M.footer[i]), A_DIM);
        }
    } else if (r_failed || r_cancelled) {
        title = r_failed ? "Error" : "Cancelled";
        const char *text = M.error[0] ? M.error :
            r_cancelled ? "Nothing was left mounted. Files already copied are harmless; run the installer again to finish." : "";
        chtype color = r_failed ? CU.bad : CU.warn;
        SL wl = {0};
        wrap_text(text, (size_t)(w - 8 < 20 ? 20 : w - 8), &wl);
        for (size_t i = 0; i < wl.n; i++) rows_add(&head, xstrdup(wl.v[i]), color);
        sl_free(&wl);
        rows_add(&head, xstrdup(""), 0);
    }
    if (!r_ok) {
        int want = avail - (int)head.n;
        if (want > M.log_n) want = M.log_n;
        for (int i = M.log_n - (want > 0 ? want : 0); i < M.log_n && want > 0; i++)
            rows_add(&body, u8_clip(M.logbuf[(M.log_head + i) % LOGMAX], (size_t)(w - 8 < 0 ? 0 : w - 8)), A_DIM);
    }
    UNLOCK();

    if (rows >= 3) {
        Buf rule = {0};
        for (int i = 0; i < 2; i++) buf_add(&rule, g.rule, strlen(g.rule));
        buf_add(&rule, " ", 1);
        buf_add(&rule, title, strlen(title));
        buf_add(&rule, " ", 1);
        for (int i = 0; i < w - 10 - (int)strlen(title); i++) buf_add(&rule, g.rule, strlen(g.rule));
        cu_put(y, 2, buf_str(&rule), A_DIM);
        free(rule.d);
        y++;
        size_t nhead = head.n < (size_t)avail ? head.n : (size_t)(avail > 0 ? avail : 0);
        for (size_t i = 0; i < nhead; i++) cu_put(y++, 4, head.v[i].text, head.v[i].attr);
        if (avail > (int)nhead) {
            size_t room = (size_t)avail - nhead, start = body.n > room ? body.n - room : 0;
            for (size_t i = start; i < body.n; i++) cu_put(y++, 4, body.v[i].text, body.v[i].attr);
        }
    }
    rows_free(&head);
    rows_free(&body);

    cu_put(h - 1, 2, final ? "Press any key to exit" : "Ctrl-C to cancel", A_DIM);
    refresh();
}

static int cu_menu(const char *title, const char *subtitle, const char *const *labels,
                   const char *const *descs, int n, int def) {
    Glyphs g = cu_glyphs();
    int sel = def;
    timeout(-1);
    clear();
    for (;;) {
        erase();
        int h, w;
        getmaxyx(stdscr, h, w);
        int y = 1;
        cu_put(y, 2, title, A_BOLD | CU.accent);
        y += 2;
        if (subtitle && *subtitle) {
            SL wl = {0};
            wrap_text(subtitle, (size_t)(w - 6 < 20 ? 20 : w - 6), &wl);
            for (size_t i = 0; i < wl.n; i++) cu_put(y++, 2, wl.v[i], 0);
            sl_free(&wl);
            y++;
        }
        for (int i = 0; i < n; i++) {
            chtype a = i == sel ? (A_REVERSE | A_BOLD) : 0;
            char *raw = xasprintf(" %s %d. %s", i == sel ? g.sel : " ", i + 1, labels[i]);
            char *padded = u8_ljust(raw, w - 6);
            char *clipped = u8_clip(padded, (size_t)(w - 5 < 0 ? 0 : w - 5));
            cu_put(y++, 2, clipped, a);
            free(raw); free(padded); free(clipped);
            if (descs && descs[i] && *descs[i]) {
                SL wl = {0};
                wrap_text(descs[i], (size_t)(w - 12 < 20 ? 20 : w - 12), &wl);
                for (size_t k = 0; k < wl.n; k++) cu_put(y++, 8, wl.v[k], A_DIM);
                sl_free(&wl);
            }
        }
        cu_put(h - 2, 2, UI.uni ? "\xe2\x86\x91/\xe2\x86\x93 select   Enter confirm   q cancel"
                                : "Up/Down select   Enter confirm   q cancel", A_DIM);
        refresh();
        int ch = getch();
        if (g_cancel) return -1;
        if (ch == KEY_UP || ch == 'k') sel = (sel + n - 1) % n;
        else if (ch == KEY_DOWN || ch == 'j') sel = (sel + 1) % n;
        else if (ch >= '1' && ch <= '9' && ch - '1' < n) sel = ch - '1';
        else if (ch == KEY_ENTER || ch == 10 || ch == 13) return sel;
        else if (ch == 'q' || ch == 'Q' || ch == 27 || ch == 3) return -1;
    }
}

static volatile int g_worker_done;
static void execute(void);

static void *worker_main(void *arg) {
    (void)arg;
    execute();
    __atomic_store_n(&g_worker_done, 1, __ATOMIC_RELEASE);
    return NULL;
}

static void cu_run(void) {
    pthread_t th;
    clear();
    if (pthread_create(&th, NULL, worker_main, NULL) != 0) {
        m_finish("failed", "cannot start the installer thread");
        return;
    }
    timeout(100);
    while (!__atomic_load_n(&g_worker_done, __ATOMIC_ACQUIRE)) {
        cu_draw(false);
        getch();
        if (g_cancel) M.cancelled = 1;
    }
    pthread_join(th, NULL);
    if (!M.result) m_finish("failed", "installer stopped unexpectedly");
    timeout(-1);
    for (;;) {
        cu_draw(true);
        int ch = getch();
        if (ch != KEY_RESIZE) break;
        clear();
    }
}

static bool cu_usable(bool plain) {
    if (plain || !isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) return false;
    const char *term = getenv("TERM");
    if (!term || !*term || !strcmp(term, "dumb")) return false;
    int cols, lines;
    term_size(&cols, &lines);
    return lines >= CU_MIN_ROWS && cols >= CU_MIN_COLS;
}

#else

static bool cu_init(void) { return false; }
static void cu_end(void) {}
static bool cu_usable(bool plain) { (void)plain; return false; }
static void cu_run(void) {}
static int cu_menu(const char *t, const char *s, const char *const *l, const char *const *d, int n, int def) {
    (void)t; (void)s; (void)l; (void)d; (void)n; (void)def;
    return -1;
}
static void execute(void);

#endif

static int menu(const char *title, const char *subtitle, const char *const *labels,
                const char *const *descs, int n, int def) {
    if (UI.mode == UI_CURSES) return cu_menu(title, subtitle, labels, descs, n, def);
    return plain_menu(title, subtitle, labels, descs, n, def);
}

typedef struct { pid_t pid; int fd; } Proc;
enum { PM_BOTH, PM_STDOUT, PM_STDERR };

static int proc_start(char *const argv[], int mode, Proc *p) {
    int out[2], ctl[2];
    if (pipe2(out, O_CLOEXEC) < 0) return -1;
    if (pipe2(ctl, O_CLOEXEC) < 0) { int e = errno; close(out[0]); close(out[1]); errno = e; return -1; }
    fflush(NULL);
    pid_t pid = fork();
    if (pid < 0) { int e = errno; close(out[0]); close(out[1]); close(ctl[0]); close(ctl[1]); errno = e; return -1; }
    if (pid == 0) {
        int devnull = open("/dev/null", O_RDWR);
        dup2(devnull, STDIN_FILENO);
        dup2(mode == PM_STDERR ? devnull : out[1], STDOUT_FILENO);
        dup2(mode == PM_STDOUT ? devnull : out[1], STDERR_FILENO);
        execvp(argv[0], argv);
        int e = errno;
        write(ctl[1], &e, sizeof e);
        _exit(127);
    }
    close(out[1]);
    close(ctl[1]);
    int child_errno = 0;
    ssize_t got;
    do got = read(ctl[0], &child_errno, sizeof child_errno); while (got < 0 && errno == EINTR);
    close(ctl[0]);
    if (got == (ssize_t)sizeof child_errno) {
        close(out[0]);
        waitpid(pid, NULL, 0);
        errno = child_errno;
        return -1;
    }
    fcntl(out[0], F_SETFL, fcntl(out[0], F_GETFL) | O_NONBLOCK);
    p->pid = pid;
    p->fd = out[0];
    return 0;
}

static bool proc_drain(Proc *p, Buf *b) {
    char tmp[16384];
    for (;;) {
        ssize_t n = read(p->fd, tmp, sizeof tmp);
        if (n > 0) { buf_add(b, tmp, (size_t)n); continue; }
        if (n == 0) return false;
        if (errno == EINTR) continue;
        return errno == EAGAIN || errno == EWOULDBLOCK;
    }
}

static int proc_wait(Proc *p) {
    int status = 0;
    close(p->fd);
    while (waitpid(p->pid, &status, 0) < 0 && errno == EINTR) { }
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) return -WTERMSIG(status);
    return -1;
}

static void proc_kill(Proc *p) {
    kill(p->pid, SIGTERM);
    proc_wait(p);
}

static int proc_collect(Proc *p, Buf *out, double timeout, int *status) {
    double t0 = now_mono();
    for (;;) {
        struct pollfd pfd = {p->fd, POLLIN, 0};
        poll(&pfd, 1, 100);
        bool open_ = proc_drain(p, out);
        if (g_cancel) { proc_kill(p); return E_CANCEL; }
        if (!open_) break;
        if (timeout > 0 && now_mono() - t0 > timeout) { proc_kill(p); *status = -1000; return E_OK; }
    }
    *status = proc_wait(p);
    return E_OK;
}

static char *last_line(const char *s) {
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) n--;
    size_t start = n;
    while (start && s[start - 1] != '\n') start--;
    return strndup(s + start, n - start);
}

static void strip_inplace(char *s) {
    size_t n = strlen(s), a = 0;
    while (n && isspace((unsigned char)s[n - 1])) s[--n] = 0;
    while (s[a] && isspace((unsigned char)s[a])) a++;
    if (a) memmove(s, s + a, strlen(s + a) + 1);
}

typedef struct {
    const char *bin_dir;
    const char *root;
    const char *variant;
    const char *cache_dir;
    const char *destdir;
    bool redownload, no_redownload, yes, plain, verbose, dry_run;
} Args;

enum { KIND_LOCAL, KIND_CACHE, KIND_URL };

typedef struct {
    char *bin_dir;
    int kind;
    char *local_root, *url, *mirror_url, *variant, *cache_dir;
    bool download, replace_active;
} Plan;

typedef struct { char *page, *rel; } ManPage;

static struct {
    Args a;
    Plan plan;
    char *destdir;
    char *here;
    bool dry, is_root, systemd, activating, undo_deactivate;
    SL temp;
    char *v4_source, *archive, *bin_src, *magic_src, *uninstall_script, *uninstall_boot, *man_src;
    ManPage *man_pages;
    size_t nman;
    int man_count;
} I;

typedef struct { int status; char note[256]; } Out;

static char *D(const char *path) {
    return I.destdir[0] ? xasprintf("%s%s", I.destdir, path) : xstrdup(path);
}

static int sh(bool check, double timeout, int *rc_out, char **out_str, const char *cmd, ...) {
    char *argv[16];
    int argc = 0;
    va_list ap;
    va_start(ap, cmd);
    for (const char *a = cmd; a && argc < 15; a = va_arg(ap, const char *)) argv[argc++] = (char *)a;
    va_end(ap);
    argv[argc] = NULL;

    Buf line = {0};
    for (int i = 0; i < argc; i++) {
        bool plain = argv[i][0] && strspn(argv[i], "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@%+=:,./-_") == strlen(argv[i]);
        if (i) buf_add(&line, " ", 1);
        if (plain) buf_add(&line, argv[i], strlen(argv[i]));
        else {
            buf_add(&line, "'", 1);
            for (const char *c = argv[i]; *c; c++) {
                if (*c == '\'') buf_add(&line, "'\"'\"'", 5);
                else buf_add(&line, c, 1);
            }
            buf_add(&line, "'", 1);
        }
    }
    m_log("$ %s", buf_str(&line));
    free(line.d);

    if (rc_out) *rc_out = 0;
    if (out_str) *out_str = xstrdup("");
    if (I.dry) return E_OK;

    Proc p;
    if (proc_start(argv, PM_BOTH, &p) < 0) {
        if (errno == ENOENT) return fail("required command not found: %s", argv[0]);
        return fail("cannot run %s: %s", argv[0], strerror(errno));
    }
    Buf buf = {0};
    int status = 0;
    int rc = proc_collect(&p, &buf, timeout, &status);
    if (rc) { free(buf.d); return rc; }
    if (status == -1000) { free(buf.d); return fail("'%s' timed out after %.0fs", argv[0], timeout); }

    char *text = xstrdup(buf_str(&buf));
    free(buf.d);
    strip_inplace(text);
    char *copy = xstrdup(text), *save = NULL;
    for (char *l = strtok_r(copy, "\n", &save); l; l = strtok_r(NULL, "\n", &save)) {
        size_t n = strlen(l);
        if (n && l[n - 1] == '\r') l[n - 1] = 0;
        m_log("  %s", l);
    }
    free(copy);
    if (check && status) {
        char *last = text[0] ? last_line(text) : xstrdup("no output");
        int r = fail("'%s%s%s' failed (exit %d): %s", argv[0], argc > 1 ? " " : "", argc > 1 ? argv[1] : "", status, last);
        free(last); free(text);
        return r;
    }
    if (rc_out) *rc_out = status;
    if (out_str) { free(*out_str); *out_str = text; } else free(text);
    return E_OK;
}

static int make_plan(const Args *args, bool interactive, const char *destdir, Plan **out) {
    Plan *plan = calloc(1, sizeof *plan);
    plan->bin_dir = xstrdup(args->bin_dir);
    plan->kind = KIND_LOCAL;
    *out = NULL;

    if (args->root) {
        if (is_url(args->root)) {
            plan->kind = KIND_URL;
            plan->url = xstrdup(args->root);
            plan->download = true;
        } else {
            plan->local_root = path_abspath(args->root);
        }
    } else {
        const char *variant = args->variant;
        if (!variant) {
            if (interactive) {
                const char *labels[] = {"Stock", "With symbols", "Cancel"};
                const char *descs[] = {"The original, unmodified Fourth Edition image.",
                                       "Same system rebuilt with symbols: readable stack traces and accurate time hooks.", ""};
                int c = menu("Emulator -- Filesystem",
                             "The emulator needs a Unix Fourth Edition system image. It will be downloaded now.",
                             labels, descs, 3, 0);
                if (c < 0 || c == 2) { free(plan); return E_OK; }
                variant = c == 0 ? "stock" : "symbols";
            } else if (args->yes) {
                variant = "stock";
            } else {
                return fail("no Unix v4 root specified. Pass a directory or URL as the second argument, "
                            "or use --variant stock|symbols (or --yes for stock).");
            }
        }
        plan->kind = KIND_CACHE;
        plan->variant = xstrdup(variant);
        plan->url = xstrdup(!strcmp(variant, "stock") ? STOCK_URL : SYMBOLS_URL);
        plan->mirror_url = xstrdup(!strcmp(variant, "stock") ? STOCK_MIRROR_URL : SYMBOLS_MIRROR_URL);
        char *base = path_abspath(args->cache_dir);
        plan->cache_dir = path_join(base, variant);
        free(base);
        plan->download = true;

        bool nonempty = false;
        DIR *d = isdir(plan->cache_dir) ? opendir(plan->cache_dir) : NULL;
        if (d) {
            struct dirent *e;
            while ((e = readdir(d)))
                if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) { nonempty = true; break; }
            closedir(d);
        }
        if (nonempty) {
            if (!args->redownload && (args->no_redownload || args->yes || !interactive)) {
                plan->download = false;
            } else {
                char *rel = path_relpath(plan->cache_dir);
                char *shown = starts_with(rel, "..") ? plan->cache_dir : rel;
                char *title = xasprintf("emulator -- '%s' already exists", shown);
                const char *labels[] = {"Use the existing copy", "Download it again", "Cancel"};
                int c = menu(title, "A copy of this Unix v4 image was downloaded earlier.", labels, NULL, 3, 0);
                free(title); free(rel);
                if (c < 0 || c == 2) { free(plan); return E_OK; }
                plan->download = c == 1;
            }
            if (!plan->download) plan->local_root = xstrdup(plan->cache_dir);
        }
    }

    if (!destdir[0] && !args->dry_run && (is_mountpoint(GUEST_ROOT) || is_mountpoint(HOST_STAGING))) {
        if (args->yes) {
            plan->replace_active = true;
        } else if (interactive) {
            const char *labels[] = {"Replace the running installation", "Cancel"};
            int c = menu("v4linux is already installed",
                         GUEST_ROOT " is mounted. Replacing it stops the service and unmounts the guest "
                         "environment first. Guest changes stored in the overlay are kept.",
                         labels, NULL, 2, 0);
            if (c != 0) { free(plan); return E_OK; }
            plan->replace_active = true;
        } else {
            return fail(GUEST_ROOT " is already mounted. Re-run with --yes to replace it, "
                                   "or run 'sudo v4-uninstall' first.");
        }
    }
    *out = plan;
    return E_OK;
}

static char *plan_label(const Plan *p) {
    if (p->kind == KIND_CACHE) return xasprintf("Unix v4 (%s)", p->variant);
    if (p->kind == KIND_URL) return xstrdup("Unix v4 (custom URL)");
    return xstrdup("Unix v4 (local directory)");
}

static void chown_like(const char *path, const struct stat *st, bool follow) {
    if (!I.is_root) return;
    int rc = follow ? chown(path, st->st_uid, st->st_gid) : lchown(path, st->st_uid, st->st_gid);
    if (rc < 0) m_log("  chown %s: %s", path, strerror(errno));
}

static int write_file(const char *path, const char *data, size_t len, mode_t mode) {
    char *dir = path_dirname(path);
    if (mkdir_p(dir, 0777) < 0) { int r = fail("cannot create %s: %s", dir, strerror(errno)); free(dir); return r; }
    char *tmp = xasprintf("%s/.v4tmp.XXXXXX", dir);
    free(dir);
    int fd = mkstemp(tmp);
    if (fd < 0) { int r = fail("cannot create temporary file for %s: %s", path, strerror(errno)); free(tmp); return r; }
    size_t off = 0;
    int err = 0;
    while (off < len) {
        ssize_t n = write(fd, data + off, len - off);
        if (n < 0) { if (errno == EINTR) continue; err = errno; break; }
        off += (size_t)n;
    }
    if (!err && close(fd) < 0) err = errno; else if (err) close(fd);
    if (!err && chmod(tmp, mode) < 0) err = errno;
    if (!err && rename(tmp, path) < 0) err = errno;
    if (err) { unlink(tmp); free(tmp); return fail("cannot write %s: %s", path, strerror(err)); }
    free(tmp);
    return E_OK;
}

static int install_file(const char *src, const char *dst, mode_t mode) {
    size_t len;
    char *data = read_file(src, &len);
    if (!data) return fail("cannot read %s: %s", src, strerror(errno));
    int rc = write_file(dst, data, len, mode);
    free(data);
    return rc;
}

static int make_symlink(const char *target, const char *link) {
    char *dir = path_dirname(link);
    int rc = mkdir_p(dir, 0777);
    free(dir);
    if (rc < 0) return fail("cannot create directory for %s: %s", link, strerror(errno));
    if (lexists(link) && unlink(link) < 0) return fail("cannot replace %s: %s", link, strerror(errno));
    if (symlink(target, link) < 0) return fail("cannot create symlink %s: %s", link, strerror(errno));
    return E_OK;
}

static int copy_regular(const char *src, const char *dst) {
    int in = open(src, O_RDONLY);
    if (in < 0) return fail("cannot open %s: %s", src, strerror(errno));
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (out < 0) { int e = errno; close(in); return fail("cannot create %s: %s", dst, strerror(e)); }
    char *buf = xmalloc(1 << 16);
    int rc = E_OK;
    for (;;) {
        ssize_t n = read(in, buf, 1 << 16);
        if (n < 0) { if (errno == EINTR) continue; rc = fail("cannot read %s: %s", src, strerror(errno)); break; }
        if (n == 0) break;
        for (ssize_t off = 0; off < n;) {
            ssize_t w = write(out, buf + off, (size_t)(n - off));
            if (w < 0) { if (errno == EINTR) continue; rc = fail("cannot write %s: %s", dst, strerror(errno)); goto done; }
            off += w;
        }
    }
done:
    free(buf);
    close(in);
    if (close(out) < 0 && !rc) rc = fail("cannot write %s: %s", dst, strerror(errno));
    return rc;
}

typedef struct { char *path; struct stat st; } Item;

static int cmp_item(const void *a, const void *b) { return strcmp(((const Item *)a)->path, ((const Item *)b)->path); }

static int copy_tree(const char *src, const char *dst, bool progress, int *count) {
    Item *items = NULL;
    size_t nitems = 0, cap = 0;
    SL stack = {0};
    sl_push(&stack, xstrdup(src));
    int rc = E_OK;
    while (stack.n && !rc) {
        char *d = stack.v[--stack.n];
        DIR *dir = opendir(d);
        if (!dir) { rc = fail("cannot read directory %s: %s", d, strerror(errno)); free(d); break; }
        struct dirent *e;
        SL names = {0};
        while ((e = readdir(dir)))
            if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) sl_push(&names, xstrdup(e->d_name));
        closedir(dir);
        for (size_t i = 0; i < names.n && !rc; i++) {
            if (nitems == cap) { cap = cap ? cap * 2 : 1024; items = xrealloc(items, cap * sizeof *items); }
            items[nitems].path = path_join(d, names.v[i]);
            if (lstat(items[nitems].path, &items[nitems].st) < 0) {
                rc = fail("cannot stat %s: %s", items[nitems].path, strerror(errno));
                free(items[nitems].path);
                break;
            }
            if (S_ISDIR(items[nitems].st.st_mode)) sl_push(&stack, xstrdup(items[nitems].path));
            nitems++;
        }
        sl_free(&names);
        free(d);
    }
    sl_free(&stack);
    if (!rc) qsort(items, nitems, sizeof *items, cmp_item);
    if (!rc && mkdir_p(dst, 0777) < 0) rc = fail("cannot create %s: %s", dst, strerror(errno));

    size_t srclen = strlen(src);
    Item *dirs = xmalloc((nitems + 1) * sizeof *dirs);
    char **dirout = xmalloc((nitems + 1) * sizeof *dirout);
    size_t ndirs = 0;
    double last = 0;
    for (size_t i = 0; i < nitems && !rc; i++) {
        if ((rc = tick())) break;
        const char *path = items[i].path;
        const struct stat *st = &items[i].st;
        const char *rel = path + srclen;
        while (*rel == '/') rel++;
        char *out = path_join(dst, rel);
        mode_t mode = st->st_mode;
        if (S_ISDIR(mode)) {
            struct stat os;
            if (lstat(out, &os) == 0 && !S_ISDIR(os.st_mode)) unlink(out);
            if (mkdir_p(out, 0777) < 0) rc = fail("cannot create %s: %s", out, strerror(errno));
            dirs[ndirs] = items[i];
            dirout[ndirs++] = out;
            out = NULL;
        } else {
            struct stat os;
            if (lstat(out, &os) == 0) {
                if (S_ISDIR(os.st_mode)) rc = fail("cannot replace directory with file: %s", out);
                else if (unlink(out) < 0) rc = fail("cannot replace %s: %s", out, strerror(errno));
            }
            if (!rc) {
                if (S_ISLNK(mode)) {
                    char target[PATH_MAX];
                    ssize_t n = readlink(path, target, sizeof target - 1);
                    if (n < 0) rc = fail("cannot read link %s: %s", path, strerror(errno));
                    else {
                        target[n] = 0;
                        if (symlink(target, out) < 0) rc = fail("cannot create symlink %s: %s", out, strerror(errno));
                        else chown_like(out, st, false);
                    }
                } else if (S_ISREG(mode)) {
                    rc = copy_regular(path, out);
                    if (!rc) {
                        chown_like(out, st, true);
                        chmod(out, st->st_mode & 07777);
                        struct timespec ts[2] = {st->st_atim, st->st_mtim};
                        utimensat(AT_FDCWD, out, ts, 0);
                    }
                } else if (S_ISCHR(mode) || S_ISBLK(mode)) {
                    if (mknod(out, mode, st->st_rdev) == 0) chown_like(out, st, true);
                    else if (errno == EPERM || errno == EACCES) m_log("  skipped device node (need root): %s", rel);
                    else rc = fail("cannot create device node %s: %s", out, strerror(errno));
                } else if (S_ISFIFO(mode)) {
                    if (mkfifo(out, mode & 07777) < 0) rc = fail("cannot create fifo %s: %s", out, strerror(errno));
                } else {
                    m_log("  skipped special file: %s", rel);
                }
            }
            free(out);
        }
        double now = now_mono();
        if (progress && (now - last > 0.05 || i + 1 == nitems)) {
            last = now;
            m_update((double)(i + 1) / (double)nitems, "%zu/%zu files", i + 1, nitems);
        }
    }
    if (!rc) {
        for (size_t i = ndirs; i-- > 0;) {
            chown_like(dirout[i], &dirs[i].st, true);
            chmod(dirout[i], dirs[i].st.st_mode & 07777);
            struct timespec ts[2] = {dirs[i].st.st_atim, dirs[i].st.st_mtim};
            utimensat(AT_FDCWD, dirout[i], ts, 0);
        }
    }
    for (size_t i = 0; i < ndirs; i++) free(dirout[i]);
    free(dirout);
    free(dirs);
    for (size_t i = 0; i < nitems; i++) free(items[i].path);
    free(items);
    if (!rc && count) *count = (int)nitems;
    return rc;
}

static int not_nested(const char *src, const char *dst, const char *label) {
    if (!strcmp(src, dst)) return E_OK;
    char *s = xasprintf("%s/", src), *d = xasprintf("%s/", dst);
    int rc = E_OK;
    if (starts_with(s, d)) rc = fail("%s source is inside the destination: %s", label, src);
    else if (starts_with(d, s)) rc = fail("%s destination is inside the source: %s", label, dst);
    free(s); free(d);
    return rc;
}

static int need_root(void) {
    if (I.activating && !I.is_root) return fail("this installer must run as root. Try: sudo ./build install");
    return E_OK;
}

static int check_v4_root(const char *path, char **root_out) {
    if (!isdir(path)) return fail("Unix v4 root is not a directory: %s", path);
    char *root = xstrdup(path);
    char *bindir = path_join(path, "bin");
    if (!isdir(bindir)) {
        DIR *d = opendir(path);
        SL names = {0};
        struct dirent *e;
        while (d && (e = readdir(d)))
            if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) sl_push(&names, xstrdup(e->d_name));
        if (d) closedir(d);
        qsort(names.v, names.n, sizeof *names.v, cmp_str);
        for (size_t i = 0; i < names.n; i++) {
            char *cand = path_join(path, names.v[i]), *cb = path_join(cand, "bin");
            bool hit = isdir(cb);
            free(cb);
            if (hit) { free(root); root = cand; break; }
            free(cand);
        }
        sl_free(&names);
    }
    free(bindir);
    bindir = path_join(root, "bin");
    bool ok = isdir(bindir);
    free(bindir);
    if (!ok) { int r = fail("%s does not look like a Unix v4 root (no bin/ directory)", path); free(root); return r; }
    *root_out = root;
    return E_OK;
}

static int collect_man_pages(void) {
    SL secs = {0};
    DIR *d = isdir(I.man_src) ? opendir(I.man_src) : NULL;
    struct dirent *e;
    while (d && (e = readdir(d)))
        if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) sl_push(&secs, xstrdup(e->d_name));
    if (d) closedir(d);
    qsort(secs.v, secs.n, sizeof *secs.v, cmp_str);

    SL seen = {0};
    size_t cap = 0;
    int rc = E_OK;
    for (size_t i = 0; i < secs.n && !rc; i++) {
        const char *section = secs.v[i];
        char *sdir = path_join(I.man_src, section);
        if (!starts_with(section, "man") || (!isdir(sdir) && !islink(sdir))) { free(sdir); continue; }
        if (islink(sdir)) { rc = fail("refusing symlinked man source directory: %s", sdir); free(sdir); break; }
        SL names = {0};
        DIR *sd = opendir(sdir);
        while (sd && (e = readdir(sd)))
            if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) sl_push(&names, xstrdup(e->d_name));
        if (sd) closedir(sd);
        qsort(names.v, names.n, sizeof *names.v, cmp_str);
        for (size_t k = 0; k < names.n && !rc; k++) {
            char *page = path_join(sdir, names.v[k]);
            if (!isfile(page)) { free(page); continue; }
            size_t nl = strlen(names.v[k]);
            char *rel = (nl >= 3 && !strcmp(names.v[k] + nl - 3, ".gz")) ? xasprintf("%s/%s", section, names.v[k])
                                                                          : xasprintf("%s/%s.gz", section, names.v[k]);
            if (!valid_man_rel(rel)) rc = fail("invalid man page path: %s", rel);
            else {
                bool dup = false;
                for (size_t s = 0; s < seen.n; s++) if (!strcmp(seen.v[s], rel)) { dup = true; break; }
                if (dup) rc = fail("multiple source man pages map to %s", rel);
            }
            if (rc) { free(page); free(rel); break; }
            sl_push(&seen, xstrdup(rel));
            if (I.nman == cap) { cap = cap ? cap * 2 : 32; I.man_pages = xrealloc(I.man_pages, cap * sizeof *I.man_pages); }
            I.man_pages[I.nman++] = (ManPage){page, rel};
        }
        sl_free(&names);
        free(sdir);
    }
    sl_free(&secs);
    sl_free(&seen);
    if (!rc && !I.nman) rc = fail("no man pages found under %s/man*/*", I.man_src);
    return rc;
}

static int s_preflight(Out *o) {
    (void)o;
    TRY(need_root());
    I.bin_src = path_realpath(I.plan.bin_dir);
    if (!isdir(I.bin_src)) return fail("bin directory not found: %s (run './build' first)", I.plan.bin_dir);
    static const char *const exes[] = {"v4linux", "v4linux-dispatch"};
    for (int i = 0; i < 2; i++) {
        char *p = path_join(I.bin_src, exes[i]);
        bool ok = isfile(p) && access(p, X_OK) == 0;
        if (!ok) { int r = fail("expected executable %s (run './build' first)", p); free(p); return r; }
        free(p);
    }
    Buf missing = {0};
    for (int i = 0; i < NTOOLS; i++) {
        char *p = path_join(I.bin_src, REQUIRED_TOOLS[i]);
        if (!isfile(p)) {
            if (missing.n) buf_add(&missing, ", ", 2);
            buf_add(&missing, REQUIRED_TOOLS[i], strlen(REQUIRED_TOOLS[i]));
        }
        free(p);
    }
    if (missing.n) { int r = fail("missing from %s: %s (run './build' first)", I.bin_src, buf_str(&missing)); free(missing.d); return r; }
    free(missing.d);

    char *dumpdir = path_join(I.here, "dumpinfo");
    I.magic_src = path_join(dumpdir, "v4dump.magic");
    free(dumpdir);
    struct stat st;
    if (lstat(I.magic_src, &st) < 0 || !S_ISREG(st.st_mode))
        return fail("expected regular magic definition file: %s", I.magic_src);
    I.uninstall_script = path_join(I.here, "v4linux-uninstall.sh");
    I.uninstall_boot = path_join(I.here, "v4linux-uninstall-boot.sh");
    if (!isfile(I.uninstall_script)) return fail("cannot find %s", I.uninstall_script);
    if (!isfile(I.uninstall_boot)) return fail("cannot find %s", I.uninstall_boot);

    char *parent = path_dirname(I.bin_src), *usr = path_join(parent, "usr");
    I.man_src = path_join(usr, "man");
    free(parent); free(usr);
    TRY(collect_man_pages());

    if (I.plan.local_root) {
        TRY(check_v4_root(I.plan.local_root, &I.v4_source));
        m_log("Unix v4 root: %s", I.v4_source);
    }

    if (I.activating) {
        if (!which("mount") || !which("umount")) return fail("'mount' and 'umount' (util-linux) are required");
        if (!I.plan.replace_active && (is_mountpoint(GUEST_ROOT) || is_mountpoint(HOST_STAGING)))
            return fail(GUEST_ROOT " is already mounted; run 'sudo v4-uninstall' first.");
        if (!fs_supported("overlay")) {
            TRY(sh(false, 0, NULL, NULL, "modprobe", "overlay", NULL));
            if (!fs_supported("overlay")) return fail("the kernel has no OverlayFS support (modprobe overlay failed)");
        }
        if (!isdir(BINFMT_DIR) && !is_mountpoint(BINFMT_DIR)) {
            TRY(sh(false, 0, NULL, NULL, "modprobe", "binfmt_misc", NULL));
            if (!fs_supported("binfmt_misc")) return fail("the kernel has no binfmt_misc support");
        }
        I.systemd = isdir("/run/systemd/system") && which("systemctl");
        if (!I.systemd) m_warn("systemd not detected: the guest mounts will not survive a reboot");
    }
    return E_OK;
}

static int deactivate(void) {
    if (I.destdir[0] || I.dry) return E_OK;
    if (exists(SERVICE_FILE) && which("systemctl"))
        TRY(sh(false, 0, NULL, NULL, "systemctl", "disable", "--now", SERVICE_NAME, NULL));
    static const struct { const char *mp; bool recursive; } mounts[] = {
        {HOST_MOUNT, true}, {HOME_MOUNT, true}, {TMP_MOUNT, true}, {GUEST_ROOT, false}, {HOST_STAGING, true}};
    for (size_t i = 0; i < sizeof mounts / sizeof *mounts; i++) {
        if (!is_mountpoint(mounts[i].mp)) continue;
        if (mounts[i].recursive) TRY(sh(false, 0, NULL, NULL, "umount", "-R", mounts[i].mp, NULL));
        else TRY(sh(false, 0, NULL, NULL, "umount", mounts[i].mp, NULL));
    }
    for (size_t i = 0; i < sizeof BINFMT_NAMES / sizeof *BINFMT_NAMES; i++) {
        char *entry = path_join(BINFMT_DIR, BINFMT_NAMES[i]);
        if (exists(entry)) {
            FILE *f = fopen(entry, "w");
            if (f) { fputs("-1\n", f); fclose(f); }
        }
        free(entry);
    }
    if (isdir(HOST_STAGING)) rmdir(HOST_STAGING);
    return E_OK;
}

static int s_stop_old(Out *o) {
    if (I.dry) { strcpy(o->note, "dry run"); return E_OK; }
    TRY(deactivate());
    if (is_mountpoint(GUEST_ROOT))
        return fail("could not unmount " GUEST_ROOT "; close any shells or programs using it and try again.");
    strcpy(o->note, "stopped");
    return E_OK;
}

typedef struct { double t0, last_t, last_done, rate; } DlProg;

static void dl_start(DlProg *p) { memset(p, 0, sizeof *p); p->t0 = p->last_t = now_mono(); }

static void dl_progress(DlProg *p, double done, double total) {
    double now = now_mono();
    if (now - p->last_t < 0.25) return;
    double inst = (done - p->last_done) / (now - p->last_t);
    p->rate = p->rate == 0 ? inst : 0.7 * p->rate + 0.3 * inst;
    p->last_t = now;
    p->last_done = done;
    char a[32], b[32], r[32], eta[32], detail[256];
    human_bytes(done, a, sizeof a);
    snprintf(detail, sizeof detail, "%s", a);
    if (total > 0) {
        human_bytes(total, b, sizeof b);
        snprintf(detail + strlen(detail), sizeof detail - strlen(detail), " / %s", b);
    }
    if (p->rate > 0) {
        human_bytes(p->rate, r, sizeof r);
        snprintf(detail + strlen(detail), sizeof detail - strlen(detail), "  %s/s", r);
        if (total > 0) {
            human_time((total - done) / p->rate, eta, sizeof eta);
            snprintf(detail + strlen(detail), sizeof detail - strlen(detail), "  ETA %s", eta);
        }
    }
    m_update(total > 0 ? done / total : -1, "%s", detail);
}

static int check_downloaded(const char *path, double total, DlProg *prog, double *size_out,
                            char *errbuf, size_t errsz) {
    struct stat st;
    double done = stat(path, &st) == 0 ? (double)st.st_size : 0;
    if (total > 0 && done != total) {
        snprintf(errbuf, errsz, "incomplete download (%.0f of %.0f bytes)", done, total);
        return E_FAIL;
    }
    if (done == 0) { snprintf(errbuf, errsz, "server returned an empty file"); return E_FAIL; }
    m_log("downloaded %.0f bytes in %.1fs", done, now_mono() - prog->t0);
    *size_out = done;
    return E_OK;
}

#ifdef V4_HAVE_CURL

typedef struct { FILE *fp; DlProg prog; } CurlCtx;

static size_t curl_write_cb(char *p, size_t size, size_t nmemb, void *ud) {
    return fwrite(p, size, nmemb, ((CurlCtx *)ud)->fp);
}

static int curl_xfer_cb(void *ud, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    (void)ultotal; (void)ulnow;
    if (g_cancel) return 1;
    dl_progress(&((CurlCtx *)ud)->prog, (double)dlnow, (double)dltotal);
    return 0;
}

static int fetch(const char *url, const char *path, double *size_out, char *errbuf, size_t errsz) {
    static bool global_init;
    if (!global_init) { curl_global_init(CURL_GLOBAL_DEFAULT); global_init = true; }
    CURL *h = curl_easy_init();
    if (!h) return fail("cannot initialise libcurl");
    CurlCtx ctx;
    dl_start(&ctx.prog);
    ctx.fp = fopen(path, "wb");
    if (!ctx.fp) { int e = errno; curl_easy_cleanup(h); return fail("cannot write %s: %s", path, strerror(e)); }
    char ebuf[CURL_ERROR_SIZE] = "";
    curl_easy_setopt(h, CURLOPT_URL, url);
    curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(h, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(h, CURLOPT_USERAGENT, "v4linux-setup");
    curl_easy_setopt(h, CURLOPT_CONNECTTIMEOUT, 30L);
    curl_easy_setopt(h, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt(h, CURLOPT_LOW_SPEED_TIME, 30L);
    curl_easy_setopt(h, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(h, CURLOPT_ERRORBUFFER, ebuf);
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, &ctx);
    curl_easy_setopt(h, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(h, CURLOPT_XFERINFOFUNCTION, curl_xfer_cb);
    curl_easy_setopt(h, CURLOPT_XFERINFODATA, &ctx);
    CURLcode cc = curl_easy_perform(h);
    curl_off_t cl = -1;
    curl_easy_getinfo(h, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
    int werr = fclose(ctx.fp);
    curl_easy_cleanup(h);
    if (cc == CURLE_ABORTED_BY_CALLBACK && g_cancel) return E_CANCEL;
    if (cc != CURLE_OK) {
        snprintf(errbuf, errsz, "%s", ebuf[0] ? ebuf : curl_easy_strerror(cc));
        return E_FAIL;
    }
    if (werr != 0) return fail("cannot write %s: %s", path, strerror(errno));
    return check_downloaded(path, cl > 0 ? (double)cl : 0, &ctx.prog, size_out, errbuf, errsz);
}

#define DOWNLOAD_BACKEND "libcurl"

#else

static double probe_length(const char *url) {
    if (!which("curl")) return 0;
    Proc p;
    char *argv[] = {"curl", "-sIL", "--connect-timeout", "15", "--max-time", "30", "-A", "v4linux-setup", (char *)url, NULL};
    if (proc_start(argv, PM_STDOUT, &p) < 0) return 0;
    Buf b = {0};
    int status;
    if (proc_collect(&p, &b, 40, &status) || status != 0) { free(b.d); return 0; }
    double len = 0;
    char *save = NULL;
    for (char *l = strtok_r((char *)buf_str(&b), "\n", &save); l; l = strtok_r(NULL, "\n", &save))
        if (!strncasecmp(l, "content-length:", 15)) len = atof(l + 15);
    free(b.d);
    return len;
}

static int fetch(const char *url, const char *path, double *size_out, char *errbuf, size_t errsz) {
    double total = probe_length(url);
    Proc p;
    char *argv_curl[] = {"curl", "-fsSL", "--connect-timeout", "30", "-A", "v4linux-setup", "-o", (char *)path, (char *)url, NULL};
    char *argv_wget[] = {"wget", "-q", "--timeout=30", "--tries=1", "-U", "v4linux-setup", "-O", (char *)path, (char *)url, NULL};
    char **argv = which("curl") ? argv_curl : which("wget") ? argv_wget : NULL;
    if (!argv) return fail("neither libcurl, curl nor wget is available; one of them is needed to download");
    if (proc_start(argv, PM_STDERR, &p) < 0) return fail("cannot run %s: %s", argv[0], strerror(errno));

    DlProg prog;
    dl_start(&prog);
    Buf err = {0};
    bool open_ = true;
    int rc = E_OK;
    while (open_) {
        struct pollfd pfd = {p.fd, POLLIN, 0};
        poll(&pfd, 1, 50);
        open_ = proc_drain(&p, &err);
        if ((rc = tick())) { proc_kill(&p); free(err.d); return rc; }
        struct stat st;
        double done = stat(path, &st) == 0 ? (double)st.st_size : 0;
        dl_progress(&prog, done, total);
    }
    int status = proc_wait(&p);
    if (status != 0) {
        char *msg = last_line(buf_str(&err));
        snprintf(errbuf, errsz, "%s", msg[0] ? msg : "download command failed");
        free(msg); free(err.d);
        return E_FAIL;
    }
    free(err.d);
    return check_downloaded(path, total, &prog, size_out, errbuf, errsz);
}

#define DOWNLOAD_BACKEND "external curl/wget (libcurl was not found at build time)"

#endif

static int s_download(Out *o) {
    m_log("download backend: %s", DOWNLOAD_BACKEND);
    const char *urls[2];
    int nurls = 0;
    urls[nurls++] = I.plan.url;
    if (I.plan.mirror_url) urls[nurls++] = I.plan.mirror_url;

    if (I.dry) {
        for (int i = 0; i < nurls; i++) m_log("would download %s%s", urls[i], i ? " (mirror fallback)" : "");
        strcpy(o->note, "dry run");
        return E_OK;
    }
    const char *tmpdir = getenv("TMPDIR");
    I.archive = xasprintf("%s/v4_dl.XXXXXX.tar.xz", tmpdir && *tmpdir ? tmpdir : "/tmp");
    int fd = mkstemps(I.archive, 7);
    if (fd < 0) return fail("cannot create a temporary file: %s", strerror(errno));
    close(fd);
    sl_push(&I.temp, xstrdup(I.archive));

    char last[512] = "";
    for (int u = 0; u < nurls; u++) {
        bool is_last = u == nurls - 1;
        int max_attempts = is_last ? 3 : 2;
        if (u > 0) {
            m_log("primary source unavailable; trying mirror: %s", urls[u]);
            m_update(-1, "trying mirror");
        }
        for (int attempt = 1; attempt <= max_attempts; attempt++) {
            double size = 0;
            char err[512] = "";
            int rc = fetch(urls[u], I.archive, &size, err, sizeof err);
            if (rc == E_OK) {
                char hb[32];
                human_bytes(size, hb, sizeof hb);
                snprintf(o->note, sizeof o->note, "%s%s", hb, u ? ", from mirror" : "");
                return E_OK;
            }
            if (rc == E_CANCEL) return rc;
            if (!err[0]) return rc;
            snprintf(last, sizeof last, "%s", err);
            m_log("download attempt %d from %s failed: %s", attempt, urls[u], err);
            if (attempt < max_attempts) {
                m_update(-1, "retrying in %ds (%s)", 1 << attempt, err);
                for (int i = 0; i < (1 << attempt) * 10; i++) { TRY(tick()); sleep_ms(100); }
            }
        }
    }
    return fail("download failed from %d source%s: %s", nurls, nurls == 1 ? "" : "s", last);
}

static int check_member(const char *name, const char *kind, const char *link) {
    if (name[0] == '/') return fail("unsafe path in archive: %s", name);
    char *copy = xstrdup(name), *save = NULL;
    for (char *t = strtok_r(copy, "/", &save); t; t = strtok_r(NULL, "/", &save))
        if (!strcmp(t, "..")) { free(copy); return fail("unsafe path in archive: %s", name); }
    free(copy);
    if (link) {
        char *base = (kind[0] == 'l') ? path_dirname(name) : xstrdup("");
        char *joined = path_join(base, link), *res = path_normpath(joined);
        bool bad = link[0] == '/' || !strcmp(res, "..") || starts_with(res, "../");
        free(base); free(joined); free(res);
        if (bad) return fail("unsafe link in archive: %s -> %s", name, link);
    }
    return E_OK;
}

static int tar_list(const char *archive, bool verbose, SL *lines) {
    char *argv_v[] = {"tar", "-tvf", (char *)archive, NULL};
    char *argv_n[] = {"tar", "-tf", (char *)archive, NULL};
    char **argv = verbose ? argv_v : argv_n;
    Proc p;
    if (proc_start(argv, PM_STDOUT, &p) < 0)
        return fail("required command not found: tar (%s)", strerror(errno));
    Buf b = {0};
    int status;
    int rc = proc_collect(&p, &b, 0, &status);
    if (rc) { free(b.d); return rc; }
    if (status != 0) {
        free(b.d);
        Buf e = {0};
        if (proc_start(argv, PM_STDERR, &p) == 0) proc_collect(&p, &e, 0, &status);
        char *msg = last_line(buf_str(&e));
        rc = fail("cannot read the downloaded archive: %s", msg[0] ? msg : "tar failed");
        free(msg); free(e.d);
        return rc;
    }
    char *save = NULL;
    for (char *l = strtok_r((char *)buf_str(&b), "\n", &save); l; l = strtok_r(NULL, "\n", &save)) sl_push(lines, xstrdup(l));
    free(b.d);
    return E_OK;
}

static int check_links(const char *root, const char *rel) {
    char *dir = *rel ? path_join(root, rel) : xstrdup(root);
    DIR *d = opendir(dir);
    free(dir);
    if (!d) return E_OK;
    struct dirent *e;
    int rc = E_OK;
    SL subdirs = {0};
    while ((e = readdir(d)) && !rc) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
        char *r = *rel ? path_join(rel, e->d_name) : xstrdup(e->d_name);
        char *full = path_join(root, r);
        struct stat st;
        if (lstat(full, &st) == 0) {
            if (S_ISLNK(st.st_mode)) {
                char target[PATH_MAX];
                ssize_t n = readlink(full, target, sizeof target - 1);
                if (n >= 0) { target[n] = 0; rc = check_member(r, "l", target); }
            } else if (S_ISDIR(st.st_mode)) {
                sl_push(&subdirs, xstrdup(r));
            }
        }
        free(r); free(full);
    }
    closedir(d);
    for (size_t i = 0; i < subdirs.n && !rc; i++) rc = check_links(root, subdirs.v[i]);
    sl_free(&subdirs);
    return rc;
}

static int extract_exec(const char *archive, const char *target, size_t *count_out) {
    SL names = {0}, verbose = {0};
    int rc = tar_list(archive, false, &names);
    for (size_t i = 0; !rc && i < names.n; i++) rc = check_member(names.v[i], "", NULL);
    if (!rc) rc = tar_list(archive, true, &verbose);
    for (size_t i = 0; !rc && i < verbose.n; i++) {
        const char *l = verbose.v[i];
        if (l[0] != 'l' && l[0] != 'h') continue;
        const char *p = l;
        for (int f = 0; f < 5 && *p; f++) { while (*p == ' ') p++; while (*p && *p != ' ') p++; }
        while (*p == ' ') p++;
        const char *sep = l[0] == 'l' ? strstr(p, " -> ") : strstr(p, " link to ");
        if (!sep) continue;
        char *name = strndup(p, (size_t)(sep - p));
        const char *link = sep + (l[0] == 'l' ? 4 : 9);
        rc = check_member(name, l[0] == 'l' ? "l" : "h", link);
        free(name);
    }
    size_t total = names.n;
    sl_free(&names);
    sl_free(&verbose);
    if (rc) return rc;

    Proc p;
    char *argv[] = {"tar", "-xpvf", (char *)archive, "-C", (char *)target, NULL};
    if (proc_start(argv, PM_BOTH, &p) < 0) return fail("required command not found: tar (%s)", strerror(errno));
    Buf pending = {0};
    char lasterr[512] = "";
    size_t count = 0;
    bool open_ = true;
    while (open_) {
        struct pollfd pfd = {p.fd, POLLIN, 0};
        poll(&pfd, 1, 100);
        open_ = proc_drain(&p, &pending);
        if ((rc = tick())) { proc_kill(&p); free(pending.d); return rc; }
        char *start = pending.d ? pending.d : (char *)"", *nl;
        size_t consumed = 0;
        while ((nl = memchr(start + consumed, '\n', pending.n - consumed))) {
            *nl = 0;
            const char *name = start + consumed;
            if (*name) {
                count++;
                snprintf(lasterr, sizeof lasterr, "%s", name);
                double f = total ? (double)count / (double)total : -1;
                m_update(f > 1 ? 1.0 : f, "%zu entries  %.40s", count, name);
            }
            consumed += (size_t)(nl - name) + 1;
        }
        if (consumed) {
            memmove(pending.d, pending.d + consumed, pending.n - consumed);
            pending.n -= consumed;
            pending.d[pending.n] = 0;
        }
    }
    free(pending.d);
    int status = proc_wait(&p);
    if (status != 0) return fail("extraction failed: %s", lasterr[0] ? lasterr : "tar exited with an error");
    TRY(check_links(target, ""));
    *count_out = count;
    return E_OK;
}

#ifdef V4_HAVE_LIBARCHIVE

static int extract_libarchive(const char *archive, const char *target, size_t *count_out, bool *unreadable) {
    *unreadable = false;
    struct stat st;
    double size = (stat(archive, &st) == 0 && st.st_size > 0) ? (double)st.st_size : 1;
    struct archive *a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_tar(a);
    if (archive_read_open_filename(a, archive, 1 << 16) != ARCHIVE_OK) {
        *unreadable = true;
        fail("cannot read the downloaded archive: %s", archive_error_string(a));
        archive_read_free(a);
        return E_FAIL;
    }
    struct archive *w = archive_write_disk_new();
    int flags = ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_SECURE_NODOTDOT;
    if (I.is_root) flags |= ARCHIVE_EXTRACT_OWNER;
    archive_write_disk_set_options(w, flags);
    archive_write_disk_set_standard_lookup(w);

    int rc = E_OK;
    size_t count = 0;
    for (;;) {
        if ((rc = tick())) break;
        struct archive_entry *e;
        int r = archive_read_next_header(a, &e);
        if (r == ARCHIVE_EOF) break;
        if (r < ARCHIVE_WARN) {
            if (count == 0) *unreadable = true;
            rc = fail("cannot read the downloaded archive: %s", archive_error_string(a));
            break;
        }
        const char *name = archive_entry_pathname(e), *sym = archive_entry_symlink(e), *hard = archive_entry_hardlink(e);
        if (!name) { rc = fail("unsafe path in archive: (unnamed entry)"); break; }
        char *nm = xstrdup(name);
        rc = check_member(nm, sym ? "l" : hard ? "h" : "", sym ? sym : hard);
        if (rc) { free(nm); break; }
        count++;
        double done = (double)archive_filter_bytes(a, -1) / size;
        m_update(done > 1 ? 1.0 : done, "%zu entries  %.40s", count, nm);

        char *full = path_join(target, nm);
        archive_entry_set_pathname(e, full);
        free(full);
        if (hard) {
            char *hl = path_join(target, hard);
            archive_entry_set_hardlink(e, hl);
            free(hl);
        }
        free(nm);

        r = archive_write_header(w, e);
        if (r < ARCHIVE_WARN) { rc = fail("extraction failed: %s", archive_error_string(w)); break; }
        if (archive_entry_size(e) > 0 || archive_entry_size_is_set(e) == 0) {
            const void *buf;
            size_t len;
            la_int64_t off;
            while ((r = archive_read_data_block(a, &buf, &len, &off)) == ARCHIVE_OK) {
                if ((rc = tick())) break;
                if (archive_write_data_block(w, buf, len, off) < ARCHIVE_WARN) {
                    rc = fail("extraction failed: %s", archive_error_string(w));
                    break;
                }
            }
            if (rc) break;
            if (r != ARCHIVE_EOF) { rc = fail("extraction failed: %s", archive_error_string(a)); break; }
        }
        if (archive_write_finish_entry(w) < ARCHIVE_WARN) { rc = fail("extraction failed: %s", archive_error_string(w)); break; }
    }
    archive_write_close(w);
    archive_write_free(w);
    archive_read_free(a);
    if (!rc) *count_out = count;
    return rc;
}

#define EXTRACT_BACKEND "libarchive"
#else
#define EXTRACT_BACKEND "external tar (libarchive was not found at build time)"
#endif

static int s_extract(Out *o) {
    if (I.dry) { strcpy(o->note, "dry run"); return E_OK; }
    char *target;
    if (I.plan.kind == KIND_CACHE) {
        target = xasprintf("%s.partial", I.plan.cache_dir);
        char *dir = path_dirname(target);
        if (mkdir_p(dir, 0777) < 0) { int r = fail("cannot create %s: %s", dir, strerror(errno)); free(dir); free(target); return r; }
        free(dir);
    } else {
        const char *tmpdir = getenv("TMPDIR");
        target = xasprintf("%s/v4_extracted.XXXXXX", tmpdir && *tmpdir ? tmpdir : "/tmp");
        if (!mkdtemp(target)) { int r = fail("cannot create a temporary directory: %s", strerror(errno)); free(target); return r; }
    }
    if (lexists(target)) rm_rf(target);
    if (mkdir_p(target, 0777) < 0) return fail("cannot create %s: %s", target, strerror(errno));
    sl_push(&I.temp, xstrdup(target));

    size_t count = 0;
    int rc;
    m_log("extract backend: %s", EXTRACT_BACKEND);
#ifdef V4_HAVE_LIBARCHIVE
    bool unreadable = false;
    rc = extract_libarchive(I.archive, target, &count, &unreadable);
    if (rc == E_FAIL && unreadable) {
        m_log("libarchive cannot read this archive (%s); trying tar", g_err);
        rm_rf(target);
        mkdir_p(target, 0777);
        rc = extract_exec(I.archive, target, &count);
    }
#else
    rc = extract_exec(I.archive, target, &count);
#endif
    if (rc) return rc;

    unlink(I.archive);
    for (size_t i = 0; i < I.temp.n; i++)
        if (!strcmp(I.temp.v[i], I.archive)) { free(I.temp.v[i]); memmove(&I.temp.v[i], &I.temp.v[i + 1], (I.temp.n - i - 1) * sizeof *I.temp.v); I.temp.n--; break; }

    char *final = target;
    if (I.plan.kind == KIND_CACHE) {
        if (lexists(I.plan.cache_dir)) rm_rf(I.plan.cache_dir);
        if (rename(target, I.plan.cache_dir) < 0) return fail("cannot move %s to %s: %s", target, I.plan.cache_dir, strerror(errno));
        for (size_t i = 0; i < I.temp.n; i++)
            if (!strcmp(I.temp.v[i], target)) { free(I.temp.v[i]); memmove(&I.temp.v[i], &I.temp.v[i + 1], (I.temp.n - i - 1) * sizeof *I.temp.v); I.temp.n--; break; }
        final = I.plan.cache_dir;
    }
    TRY(check_v4_root(final, &I.v4_source));
    m_log("Unix v4 root: %s", I.v4_source);
    snprintf(o->note, sizeof o->note, "%zu entries", count);
    return E_OK;
}

static int s_root(Out *o) {
    if (I.dry) {
        m_log("would copy %s -> %s", I.v4_source ? I.v4_source : "(downloaded root)", V4_DEST);
        strcpy(o->note, "dry run");
        return E_OK;
    }
    char *dst = D(V4_DEST);
    if (mkdir_p(dst, 0755) < 0) { int r = fail("cannot create %s: %s", dst, strerror(errno)); free(dst); return r; }
    char *rs = path_realpath(I.v4_source), *rd = path_realpath(dst);
    int rc = E_OK;
    if (!strcmp(rs, rd)) {
        strcpy(o->note, "already in place");
    } else {
        rc = not_nested(rs, rd, "Unix v4 root");
        int n = 0;
        if (!rc) rc = copy_tree(I.v4_source, dst, true, &n);
        if (!rc) snprintf(o->note, sizeof o->note, "%d files", n);
    }
    free(rs); free(rd); free(dst);
    return rc;
}

static int s_bin(Out *o) {
    if (I.dry) {
        m_log("would copy %s -> %s, install dispatcher and symlinks", I.bin_src, PDP11_DEST);
        strcpy(o->note, "dry run");
        return E_OK;
    }
    char *dst = D(PDP11_DEST);
    int rc = E_OK;
    if (mkdir_p(dst, 0755) < 0) { rc = fail("cannot create %s: %s", dst, strerror(errno)); goto out; }
    m_update(-1, "copying binaries");
    char *rd = path_realpath(dst);
    if (strcmp(I.bin_src, rd)) {
        rc = not_nested(I.bin_src, rd, "v4linux bin");
        if (!rc) rc = copy_tree(I.bin_src, dst, false, NULL);
    }
    free(rd);
    if (rc) goto out;
    char *emu = path_join(dst, "v4linux");
    bool exec_ok = access(emu, X_OK) == 0;
    free(emu);
    if (!exec_ok) { rc = fail("installed emulator is not executable: " PDP11_DEST "/v4linux"); goto out; }

    char *src, *dest;
    src = path_join(I.bin_src, "v4linux-dispatch"); dest = D(DISPATCH_DEST);
    rc = install_file(src, dest, 0755); free(src); free(dest);
    if (rc) goto out;
    dest = path_join(dst, "uninstall-script");
    rc = install_file(I.uninstall_script, dest, 0755); free(dest);
    if (rc) goto out;
    dest = path_join(dst, "uninstall");
    rc = install_file(I.uninstall_boot, dest, 0755); free(dest);
    if (rc) goto out;
    dest = D("/usr/bin/v4-uninstall");
    rc = make_symlink(PDP11_DEST "/uninstall", dest); free(dest);
    for (int i = 0; i < NTOOLS && !rc; i++) {
        char *ln = link_name(REQUIRED_TOOLS[i]), *rel = xasprintf("/usr/bin/%s", ln), *lp = D(rel), *tg = xasprintf(PDP11_DEST "/%s", REQUIRED_TOOLS[i]);
        rc = make_symlink(tg, lp);
        free(ln); free(rel); free(lp); free(tg);
    }
    if (!rc) snprintf(o->note, sizeof o->note, "%d commands linked", NTOOLS);
out:
    free(dst);
    return rc;
}

#ifdef V4_HAVE_ZLIB

static int gzip_file(const char *path, Buf *out) {
    size_t len;
    char *data = read_file(path, &len);
    if (!data) return fail("cannot read %s: %s", path, strerror(errno));
    z_stream zs;
    memset(&zs, 0, sizeof zs);
    if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        free(data);
        return fail("cannot initialise zlib");
    }
    uLong bound = deflateBound(&zs, (uLong)len) + 32;
    unsigned char *dst = xmalloc(bound);
    zs.next_in = (Bytef *)data;
    zs.avail_in = (uInt)len;
    zs.next_out = dst;
    zs.avail_out = (uInt)bound;
    int r = deflate(&zs, Z_FINISH);
    if (r == Z_STREAM_END) buf_add(out, (const char *)dst, zs.total_out);
    deflateEnd(&zs);
    free(dst);
    free(data);
    return r == Z_STREAM_END ? E_OK : fail("zlib failed to compress %s", path);
}

#define GZIP_BACKEND "zlib"

#else

static int gzip_file(const char *path, Buf *out) {
    Proc p;
    char *argv[] = {"gzip", "-9nc", (char *)path, NULL};
    if (proc_start(argv, PM_STDOUT, &p) < 0) return fail("required command not found: gzip");
    int status;
    int rc = proc_collect(&p, out, 0, &status);
    if (rc) return rc;
    if (status != 0) return fail("gzip failed on %s (exit %d)", path, status);
    return E_OK;
}

#define GZIP_BACKEND "external gzip (zlib was not found at build time)"

#endif

static int s_man(Out *o) {
    m_log("gzip backend: %s", GZIP_BACKEND);
    if (I.dry) {
        m_log("would install %zu man pages", I.nman);
        strcpy(o->note, "dry run");
        return E_OK;
    }
    char *dest_root = D(MAN_DEST), *link_root = D(MAN_LINK_ROOT), *manifest = D(MAN_MANIFEST);
    int rc = E_OK;
    FILE *mf = NULL;
    if (mkdir_p(dest_root, 0755) < 0 || mkdir_p(link_root, 0755) < 0) { rc = fail("cannot create man directories: %s", strerror(errno)); goto out; }
    if (islink(manifest)) { rc = fail("refusing symlinked installation manifest: %s", manifest); goto out; }
    if (lexists(manifest) && !isfile(manifest)) { rc = fail("man installation manifest is not a regular file: %s", manifest); goto out; }

    if (isfile(manifest)) {
        size_t len;
        char *data = read_file(manifest, &len);
        if (!data) { rc = fail("cannot read %s: %s", manifest, strerror(errno)); goto out; }
        for (size_t pos = 0; pos < len;) {
            size_t n = strnlen(data + pos, len - pos);
            if (n) {
                char *rel = strndup(data + pos, n);
                if (!valid_man_rel(rel)) { rc = fail("invalid path in %s", manifest); free(rel); free(data); goto out; }
                char *link = path_join(link_root, rel), *want = xasprintf(MAN_DEST "/%s", rel);
                char target[PATH_MAX];
                ssize_t t = islink(link) ? readlink(link, target, sizeof target - 1) : -1;
                if (t >= 0) { target[t] = 0; if (!strcmp(target, want)) unlink(link); }
                free(link); free(want); free(rel);
            }
            pos += n + 1;
        }
        free(data);
    }

    mf = fopen(manifest, "wb");
    if (!mf) { rc = fail("cannot write %s: %s", manifest, strerror(errno)); goto out; }
    chmod(manifest, 0644);
    for (size_t i = 0; i < I.nman && !rc; i++) {
        if ((rc = tick())) break;
        const char *page = I.man_pages[i].page, *rel = I.man_pages[i].rel;
        char *section = strndup(rel, (size_t)(strchr(rel, '/') - rel));
        char *dest_dir = path_join(dest_root, section), *link_dir = path_join(link_root, section);
        if (mkdir_p(dest_dir, 0755) < 0 || mkdir_p(link_dir, 0755) < 0) rc = fail("cannot create man section directory %s: %s", section, strerror(errno));
        free(section); free(dest_dir); free(link_dir);
        if (rc) break;

        char *dest_file = path_join(dest_root, rel);
        size_t pl = strlen(page);
        if (pl >= 3 && !strcmp(page + pl - 3, ".gz")) {
            size_t len;
            char *data = read_file(page, &len);
            if (!data) rc = fail("cannot read %s: %s", page, strerror(errno));
            else { rc = write_file(dest_file, data, len, 0644); free(data); }
        } else {
            Buf gz = {0};
            rc = gzip_file(page, &gz);
            if (!rc) rc = write_file(dest_file, buf_str(&gz), gz.n, 0644);
            free(gz.d);
        }
        free(dest_file);
        if (rc) break;

        char *link = path_join(link_root, rel), *target = xasprintf(MAN_DEST "/%s", rel);
        if (islink(link)) {
            char cur[PATH_MAX];
            ssize_t n = readlink(link, cur, sizeof cur - 1);
            cur[n < 0 ? 0 : n] = 0;
            if (strcmp(cur, target)) rc = fail("man page link exists with a different target: %s", link);
        } else if (lexists(link)) {
            rc = fail("refusing to replace existing non-symlink man page: %s", link);
        } else if (symlink(target, link) < 0) {
            rc = fail("cannot create symlink %s: %s", link, strerror(errno));
        }
        free(link); free(target);
        if (rc) break;
        fwrite(rel, 1, strlen(rel) + 1, mf);
        m_update((double)(i + 1) / (double)I.nman, "%zu/%zu pages", i + 1, I.nman);
    }
    if (!rc) {
        I.man_count = (int)I.nman;
        snprintf(o->note, sizeof o->note, "%zu pages", I.nman);
    }
out:
    if (mf) fclose(mf);
    free(dest_root); free(link_root); free(manifest);
    return rc;
}

static int s_magic(Out *o) {
    if (I.dry) {
        m_log("would add managed block to %s", MAGIC_DEST);
        strcpy(o->note, "dry run");
        return E_OK;
    }
    char *dst = D(MAGIC_DEST);
    int rc = E_OK;
    SL kept = {0};
    char *block = NULL, *text = NULL;
    uid_t uid = 0;
    gid_t gid = 0;
    mode_t mode = 0644;
    if (islink(dst)) { rc = fail("refusing symlinked magic database: %s", dst); goto out; }
    if (lexists(dst) && !isfile(dst)) { rc = fail("magic database is not a regular file: %s", dst); goto out; }
    if (exists(dst)) {
        struct stat st;
        stat(dst, &st);
        uid = st.st_uid; gid = st.st_gid; mode = st.st_mode & 07777;
        FILE *f = fopen(dst, "rb");
        if (!f) { rc = fail("cannot read %s: %s", dst, strerror(errno)); goto out; }
        bool inside = false, found_begin = false, found_end = false, bad = false;
        char *line = NULL;
        size_t cap = 0;
        ssize_t n;
        while ((n = getline(&line, &cap, f)) > 0) {
            char *bare = xstrdup(line);
            size_t bl = strlen(bare);
            while (bl && (bare[bl - 1] == '\n' || bare[bl - 1] == '\r')) bare[--bl] = 0;
            if (!strcmp(bare, MAGIC_BEGIN)) { bad |= inside || found_begin; inside = found_begin = true; }
            else if (!strcmp(bare, MAGIC_END)) { bad |= !inside || found_end; inside = false; found_end = true; }
            else if (!inside) sl_push(&kept, xstrdup(line));
            free(bare);
        }
        free(line);
        fclose(f);
        if (inside || found_begin != found_end || bad) { rc = fail("malformed v4linux magic block in %s; refusing to modify it", dst); goto out; }
    }
    while (kept.n) {
        const char *l = kept.v[kept.n - 1];
        bool blank = true;
        for (; *l; l++) if (!isspace((unsigned char)*l)) blank = false;
        if (!blank) break;
        free(kept.v[--kept.n]);
    }
    Buf base = {0};
    for (size_t i = 0; i < kept.n; i++) buf_add(&base, kept.v[i], strlen(kept.v[i]));
    if (base.n && base.d[base.n - 1] != '\n') buf_add(&base, "\n", 1);
    size_t blen;
    block = read_file(I.magic_src, &blen);
    if (!block) { free(base.d); rc = fail("cannot read %s: %s", I.magic_src, strerror(errno)); goto out; }
    Buf all = {0};
    if (base.n) { buf_add(&all, base.d, base.n); buf_add(&all, "\n", 1); }
    buf_add(&all, MAGIC_BEGIN "\n", sizeof MAGIC_BEGIN);
    buf_add(&all, block, blen);
    buf_add(&all, "\n" MAGIC_END "\n", sizeof MAGIC_END + 1);
    free(base.d);

    char *dir = path_dirname(dst);
    if (mkdir_p(dir, 0777) < 0) { rc = fail("cannot create %s: %s", dir, strerror(errno)); free(dir); free(all.d); goto out; }
    char *tmp = xasprintf("%s/.magic.v4linux.XXXXXX", dir);
    free(dir);
    int fd = mkstemp(tmp);
    if (fd < 0) { rc = fail("cannot create temporary file for %s: %s", dst, strerror(errno)); free(tmp); free(all.d); goto out; }
    int err = 0;
    for (size_t off = 0; off < all.n && !err;) {
        ssize_t w = write(fd, all.d + off, all.n - off);
        if (w < 0) { if (errno != EINTR) err = errno; } else off += (size_t)w;
    }
    if (close(fd) < 0 && !err) err = errno;
    if (!err && I.is_root && chown(tmp, uid, gid) < 0) err = errno;
    if (!err && chmod(tmp, mode) < 0) err = errno;
    if (!err && rename(tmp, dst) < 0) err = errno;
    if (err) { unlink(tmp); rc = fail("cannot update %s: %s", dst, strerror(err)); }
    else strcpy(o->note, "updated");
    free(tmp); free(all.d);
out:
    sl_free(&kept);
    free(block); free(text); free(dst);
    return rc;
}

static int s_service(Out *o) {
    if (I.dry) {
        m_log("would write runtime script and systemd unit");
        strcpy(o->note, "dry run");
        return E_OK;
    }
    char *script = fill(RUNTIME_TEMPLATE), *unit = fill(SERVICE_TEMPLATE);
    char *sp = D(RUNTIME_SCRIPT), *up = D(SERVICE_FILE);
    int rc = write_file(sp, script, strlen(script), 0755);
    if (!rc) rc = write_file(up, unit, strlen(unit), 0644);
    free(script); free(unit); free(sp); free(up);
    if (!rc) strcpy(o->note, "runtime script + unit");
    return rc;
}

static int s_activate(Out *o) {
    I.undo_deactivate = true;
    if (I.systemd) {
        m_update(-1, "systemd");
        TRY(sh(true, 0, NULL, NULL, "systemctl", "daemon-reload", NULL));
        int rc = 0;
        TRY(sh(false, 0, &rc, NULL, "systemctl", "enable", "--now", SERVICE_NAME, NULL));
        if (rc != 0) {
            char *journal = NULL;
            TRY(sh(false, 0, NULL, &journal, "journalctl", "-u", SERVICE_NAME, "-n", "15", "--no-pager", NULL));
            char *tail = journal[0] ? last_line(journal) : xstrdup("see 'journalctl -u v4linux'");
            int r = fail("%s failed to start: %s", SERVICE_NAME, tail);
            free(tail); free(journal);
            return r;
        }
        strcpy(o->note, "service enabled and started");
        return E_OK;
    }
    char *script = D(RUNTIME_SCRIPT);
    int rc = sh(true, 0, NULL, NULL, script, NULL);
    free(script);
    if (!rc) strcpy(o->note, "mounted (no systemd)");
    return rc;
}

static int s_verify(Out *o) {
    SL problems = {0};
    for (int i = 0; i < NTOOLS; i++) {
        char *ln = link_name(REQUIRED_TOOLS[i]), *link = xasprintf("/usr/bin/%s", ln);
        if (access(link, X_OK) != 0) sl_push(&problems, xasprintf("%s is not executable", link));
        free(ln); free(link);
    }
    const char *mps[] = {GUEST_ROOT, HOST_MOUNT, HOME_MOUNT, TMP_MOUNT};
    for (int i = 0; i < 4; i++)
        if (!is_mountpoint(mps[i])) sl_push(&problems, xasprintf("%s is not mounted", mps[i]));
    for (size_t i = 0; i < sizeof BINFMT_NAMES / sizeof *BINFMT_NAMES; i++) {
        char *entry = path_join(BINFMT_DIR, BINFMT_NAMES[i]);
        FILE *f = fopen(entry, "r");
        if (!f) sl_push(&problems, xasprintf("binfmt handler %s is not registered", BINFMT_NAMES[i]));
        else {
            char first[16] = "";
            if (!fgets(first, sizeof first, f)) first[0] = 0;
            if (!starts_with(first, "enabled")) sl_push(&problems, xasprintf("binfmt handler %s is disabled", BINFMT_NAMES[i]));
            fclose(f);
        }
        free(entry);
    }
    if (!problems.n) {
        m_update(-1, "running a guest program");
        int rc = 0;
        char *out = NULL;
        int r = sh(false, 15, &rc, &out, GUEST_ROOT "/bin/echo", "v4-ok", NULL);
        if (r == E_CANCEL) { sl_free(&problems); return r; }
        if (r) sl_push(&problems, xasprintf("smoke test could not run: %s", g_err));
        else if (!strstr(out, "v4-ok")) sl_push(&problems, xasprintf("smoke test failed: " GUEST_ROOT "/bin/echo v4-ok exited %d", rc));
        free(out);
    }
    if (problems.n) {
        for (size_t i = 0; i < problems.n; i++) m_warn("%s", problems.v[i]);
        o->status = WARN;
        snprintf(o->note, sizeof o->note, "%zu problem(s)", problems.n);
    } else {
        strcpy(o->note, "all checks passed");
    }
    sl_free(&problems);
    return E_OK;
}

typedef int (*StepFn)(Out *);

static int run_step(const char *key, StepFn fn) {
    int idx = step_index(key);
    int rc = tick();
    if (rc) return rc;
    m_begin(idx);
    Out o = {DONE, ""};
    rc = fn(&o);
    if (rc == E_CANCEL) m_end(WARN, "cancelled");
    else if (rc) m_end(FAILED, "");
    else m_end(o.status, o.note);
    return rc;
}

static void rollback(void) {
    if (I.undo_deactivate) {
        I.undo_deactivate = false;
        m_log("rolling back: unmount guest environment");
        deactivate();
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < I.temp.n; i++) rm_rf(I.temp.v[i]);
    sl_free(&I.temp);
}

static void build_summary(void) {
    if (I.dry) { m_summary("Mode", "dry run - nothing was changed"); return; }
    m_summary("Emulator", "%s/v4linux", PDP11_DEST);
    m_summary("Unix v4 root", "%s  (%s)", V4_DEST, I.plan.variant ? I.plan.variant : "custom");
    m_summary("Toolchain", "v4-cc  v4-as  v4-ld  v4-ar  v4-strip  v4-disasm  v4-dumpinfo");
    m_summary("Man pages", "%d pages", I.man_count);
    if (I.activating) {
        m_summary("Guest root", "%s   (/host = host /, /home = host /home, /tmp = tmpfs)", GUEST_ROOT);
        m_summary("Persistence", "%s", I.systemd ? SERVICE_NAME " enabled" : "none (no systemd)");
    } else {
        m_summary("Staged in", "%s", I.destdir[0] ? I.destdir : "(dry run)");
    }
    if (M.logfile) m_summary("Log", "%s", LOG_FILE);
    M.footer[M.nfooter++] = xstrdup("Try it:    /opt/v4/sys/bin/sh or v4-cc example/hello.c");
    M.footer[M.nfooter++] = xstrdup("Remove it: sudo v4-uninstall [--purge-all]");
}

static void execute(void) {
    static const struct { const char *key; StepFn fn; } head[] = {{"preflight", s_preflight}};
    static const struct { const char *key; StepFn fn; } mid[] = {{"root", s_root}, {"bin", s_bin}, {"man", s_man},
                                                                 {"magic", s_magic}, {"service", s_service}};
    int rc = run_step(head[0].key, head[0].fn);
    if (!rc && I.plan.replace_active) rc = run_step("stop_old", s_stop_old);
    if (!rc && I.plan.download) {
        rc = run_step("download", s_download);
        if (!rc) rc = run_step("extract", s_extract);
    }
    for (size_t i = 0; i < sizeof mid / sizeof *mid && !rc; i++) rc = run_step(mid[i].key, mid[i].fn);
    const char *why = I.dry ? "dry run" : "staged install (--destdir)";
    if (!rc) {
        if (I.activating) rc = run_step("activate", s_activate);
        else m_skip(step_index("activate"), why);
    }
    if (!rc) {
        if (I.activating) rc = run_step("verify", s_verify);
        else m_skip(step_index("verify"), why);
    }
    if (!rc) {
        I.undo_deactivate = false;
        build_summary();
        m_finish("ok", NULL);
    } else if (rc == E_CANCEL) {
        rollback();
        m_finish("cancelled", NULL);
    } else {
        rollback();
        m_finish("failed", g_err);
    }
    cleanup();
}

static void build_steps(void) {
    add_step("preflight", "Check system and inputs", 1);
    if (I.plan.replace_active) add_step("stop_old", "Stop running guest environment", 1);
    if (I.plan.download) {
        char *label = plan_label(&I.plan);
        add_step("download", xasprintf("Download %s", label), 8);
        add_step("extract", "Extract archive", 5);
        free(label);
    }
    add_step("root", "Install Unix v4 root", 4);
    add_step("bin", "Install emulator and toolchain", 2);
    add_step("man", "Install manual pages", 1);
    add_step("magic", "Register v4dump file type", 1);
    add_step("service", "Write service files", 1);
    add_step("activate", "Mount guest and register binfmt", 3);
    add_step("verify", "Verify installation", 1);
}

static void usage(FILE *f) {
    fputs("usage: v4linux-setup [-h] [--variant {stock,symbols}] [--cache-dir DIR]\n"
          "                    [--redownload | --no-redownload] [-y] [--plain] [-v]\n"
          "                    [--dry-run] [--destdir DIR]\n"
          "                    BIN_DIR [ROOT]\n", f);
}

static void help(void) {
    usage(stdout);
    fputs("\nInstall the v4linux emulator, PDP-11 toolchain and a Unix v4 root.\n\n"
          "positional arguments:\n"
          "  BIN_DIR               bin directory produced by './build' (contains v4linux)\n"
          "  ROOT                  extracted Unix v4 root directory, or a URL to a .tar.xz of one\n\n"
          "options:\n"
          "  -h, --help            show this help message and exit\n"
          "  --variant {stock,symbols}\n"
          "                        download this Unix v4 image without asking\n"
          "  --cache-dir DIR       where downloaded images are kept (default: ./.v4)\n"
          "  --redownload          download again even if a copy exists\n"
          "  --no-redownload       reuse an existing download\n"
          "  -y, --yes             never prompt: stock image, reuse existing download, replace\n"
          "                        active install\n"
          "  --plain               plain text output instead of the full-screen UI\n"
          "                        (-v/--verbose implies --plain)\n"
          "  -v, --verbose         show every command and its output\n"
          "  --dry-run             check inputs and show the steps; change nothing\n"
          "  --destdir DIR         stage files under DIR for packaging (no mounts, service or\n"
          "                        binfmt changes)\n\n"
          "With no ROOT and no --variant, an interactive menu offers to download Unix v4.\n"
          "Helper files are looked up in $V4_TOOLS_DIR, ../tools or . relative to this program.\n", stdout);
}

static int arg_error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static int arg_error(const char *fmt, ...) {
    usage(stderr);
    fputs("v4linux-setup: error: ", stderr);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    return 2;
}

static int parse_args(int argc, char **argv, Args *a) {
    enum { O_VARIANT = 256, O_CACHE, O_REDL, O_NOREDL, O_PLAIN, O_DRY, O_DESTDIR };
    static const struct option longopts[] = {
        {"help", no_argument, 0, 'h'},          {"variant", required_argument, 0, O_VARIANT},
        {"cache-dir", required_argument, 0, O_CACHE}, {"redownload", no_argument, 0, O_REDL},
        {"no-redownload", no_argument, 0, O_NOREDL},  {"yes", no_argument, 0, 'y'},
        {"plain", no_argument, 0, O_PLAIN},     {"verbose", no_argument, 0, 'v'},
        {"dry-run", no_argument, 0, O_DRY},     {"destdir", required_argument, 0, O_DESTDIR},
        {0, 0, 0, 0}};
    memset(a, 0, sizeof *a);
    a->cache_dir = ".v4";
    opterr = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hyv", longopts, NULL)) != -1) {
        switch (c) {
        case 'h': help(); exit(0);
        case 'y': a->yes = true; break;
        case 'v': a->verbose = true; break;
        case O_PLAIN: a->plain = true; break;
        case O_DRY: a->dry_run = true; break;
        case O_DESTDIR: a->destdir = optarg; break;
        case O_CACHE: a->cache_dir = optarg; break;
        case O_REDL:
            if (a->no_redownload) return arg_error("argument --redownload: not allowed with argument --no-redownload");
            a->redownload = true; break;
        case O_NOREDL:
            if (a->redownload) return arg_error("argument --no-redownload: not allowed with argument --redownload");
            a->no_redownload = true; break;
        case O_VARIANT:
            if (strcmp(optarg, "stock") && strcmp(optarg, "symbols"))
                return arg_error("argument --variant: invalid choice: '%s' (choose from 'stock', 'symbols')", optarg);
            a->variant = optarg; break;
        default:
            if (optopt) return arg_error("unrecognized or incomplete option: -%c", optopt);
            return arg_error("unrecognized or incomplete option: %s", argv[optind - 1]);
        }
    }
    int npos = argc - optind;
    if (npos < 1) return arg_error("the following arguments are required: BIN_DIR");
    if (npos > 2) return arg_error("unrecognized arguments: %s", argv[optind + 2]);
    a->bin_dir = argv[optind];
    if (npos == 2) a->root = argv[optind + 1];
    return 0;
}

static char *find_tools_dir(void) {
    const char *env = getenv("V4_TOOLS_DIR");
    char exe[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", exe, sizeof exe - 1);
    SL cand = {0};
    if (env && *env) sl_push(&cand, xstrdup(env));
    if (n > 0) {
        exe[n] = 0;
        char *dir = path_dirname(exe);
        sl_push(&cand, path_join(dir, "../tools"));
        sl_push(&cand, xstrdup(dir));
        free(dir);
    }
    sl_push(&cand, xstrdup("tools"));
    char *found = NULL;
    for (size_t i = 0; i < cand.n && !found; i++) {
        char *probe = path_join(cand.v[i], "v4linux-uninstall.sh");
        if (isfile(probe)) found = path_realpath(cand.v[i]);
        free(probe);
    }
    if (!found) found = xstrdup(cand.n ? cand.v[0] : ".");
    sl_free(&cand);
    return found;
}

static void open_log(bool dry, bool staged) {
    if (dry || staged) return;
    char *dir = path_dirname(LOG_FILE);
    mkdir_p(dir, 0755);
    free(dir);
    M.logfile = fopen(LOG_FILE, "a");
    if (M.logfile) {
        char stamp[32];
        time_t t = time(NULL);
        struct tm tm;
        localtime_r(&t, &tm);
        strftime(stamp, sizeof stamp, "%Y-%m-%d %H:%M:%S", &tm);
        fprintf(M.logfile, "\n==== v4linux-setup %s ====\n", stamp);
        fflush(M.logfile);
    }
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    Args args;
    int rc = parse_args(argc, argv, &args);
    if (rc) return rc;

    struct sigaction sa = {0};
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);

    I.a = args;
    I.destdir = args.destdir ? path_realpath(args.destdir) : xstrdup("");
    I.dry = args.dry_run;
    I.is_root = geteuid() == 0;
    I.here = find_tools_dir();
    I.activating = !(I.destdir[0] || I.dry);

    bool tty = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
    bool interactive = tty && !args.yes;
    const char *term = getenv("TERM");
    UI.verbose = args.verbose;
    UI.tty = isatty(STDOUT_FILENO) && !(term && !strcmp(term, "dumb"));
    UI.uni = terminal_is_utf8();
    UI.mode = UI_PLAIN;
    if (cu_usable(args.plain || args.verbose) && cu_init()) UI.mode = UI_CURSES;

    M.title = "v4linux - installer";
    Plan *plan = NULL;
    if (make_plan(&args, interactive, I.destdir, &plan) != E_OK) {
        cu_end();
        fprintf(stderr, "v4linux-setup: %s\n", g_err);
        return 1;
    }
    if (!plan) {
        cu_end();
        puts("Nothing installed.");
        return 0;
    }
    I.plan = *plan;
    free(plan);

    char *parent = path_dirname(path_abspath(args.bin_dir));
    M.subtitle = xasprintf(UI.uni ? "%s  \xe2\x86\x92  /opt/v4%s%s%s" : "%s  ->  /opt/v4%s%s%s", parent,
                           I.destdir[0] ? "  (staged in " : "", I.destdir, I.destdir[0] ? ")" : "");
    free(parent);

    M.t0 = now_mono();
    build_steps();
    open_log(args.dry_run, I.destdir[0] != 0);
    if (UI.mode == UI_CURSES) cu_run();
    else execute();
    cu_end();
    if (UI.mode == UI_PLAIN) ui_close_line();

    format_result();
    if (M.logfile && strcmp(M.result, "ok") != 0) printf("\n  Full log: %s\n", LOG_FILE);
    return !strcmp(M.result, "ok") ? 0 : !strcmp(M.result, "cancelled") ? 130 : 1;
}
