#define _GNU_SOURCE
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>

#ifndef STATX_ATTR_MOUNT_ROOT
#define STATX_ATTR_MOUNT_ROOT 0x00002000
#endif

static const char *EMULATOR = "/opt/v4/bin/v4linux";
static const char *SYS_ROOT = "/opt/v4/sys";
static const char *GUEST_OVERLAY = "/var/run/v4";
static const char *HOST_MOUNT = "/var/run/v4/host";
static const char *HOST_DIR = "/host";
static const char *HOME_DIR = "/home";

static inline void fail(const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "v4linux-dispatch: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);                                
    va_end(args);

    fputc('\n', stderr);
    exit(126);
}

static inline int path_has_prefix(const char *path, const char *prefix)
{
    size_t len = strlen(prefix);

    return strncmp(path, prefix, len) == 0 &&
           (path[len] == '\0' || path[len] == '/');
}

static inline char *duplicate_or_fail(const char *s)
{
    char *copy = strdup(s);

    if (copy == NULL)
        fail("memory allocation failed");

    return copy;
}

static inline void v4_alias(const char *name, char out[15])
{
	size_t n = strlen(name), blen, elen = 0, plen;
	const char *dot = strrchr(name, '.');
	uint32_t h = 2166136261u;
	size_t i;
	char ext[5] = "";

	if (n <= 14) {
		memcpy(out, name, n + 1);
		return;
	}
	for (i = 0; i < n; i++) {
		h ^= (unsigned char)name[i];
		h *= 16777619u;
	}
	h &= 0xffffff;
	blen = n;
	if (dot && dot != name) {
		elen = strlen(dot);
		if (elen <= 4) {
			memcpy(ext, dot, elen + 1);
			blen = (size_t)(dot - name);
		} else {
			elen = 0;
		}
	}
	plen = 14 - 1 - 6 - elen;
	if (plen > blen) plen = blen;
	for (i = 0; i < plen; i++) {
		unsigned char c = name[i];
		out[i] = (c > ' ' && c < 0177 && c != '/' && c != '~') ? c : '_';
	}
	snprintf(out + plen, 15 - plen, "~%06x%s", (unsigned)h, ext);
}

static inline char *alias_path_components(const char *path)
{
    if (path == NULL)
        return NULL;

    size_t len = strlen(path);
    char *result = malloc(len + 15);
    if (result == NULL)
        fail("memory allocation failed");

    const char *p = path;
    char *r = result;

    while (*p) {
        if (*p == '/') {
            *r++ = *p++;
            continue;
        }

        const char *end = strchr(p, '/');
        if (end == NULL)
            end = p + strlen(p);

        size_t comp_len = end - p;
        if (comp_len <= 14) {
            memcpy(r, p, comp_len);
            r += comp_len;
        } else {
            char *comp = malloc(comp_len + 1);
            if (comp == NULL)
                fail("memory allocation failed");
            memcpy(comp, p, comp_len);
            comp[comp_len] = '\0';

            char aliased[16];
            v4_alias(comp, aliased);
            free(comp);

            size_t alen = strlen(aliased);
            memcpy(r, aliased, alen);
            r += alen;
        }
        p = end;
    }
    *r = '\0';

    return result;
}

static inline char *translate_path_raw(const char *path)
{
    const char *suffix;
    char *translated = NULL;

    if (path == NULL)
        fail("cannot translate a null path");

    if (path[0] != '/')
        return duplicate_or_fail(path);

    if (path_has_prefix(path, HOME_DIR))
        return duplicate_or_fail(path);

    if (path_has_prefix(path, HOST_MOUNT)) {
        suffix = path + strlen(HOST_MOUNT);

        if (asprintf(&translated, "%s%s", HOST_DIR, suffix) < 0)
            fail("memory allocation failed");

        return translated;
    }

    if (path_has_prefix(path, GUEST_OVERLAY)) {
        suffix = path + strlen(GUEST_OVERLAY);

        return duplicate_or_fail(*suffix ? suffix : "/");
    }

    if (path_has_prefix(path, SYS_ROOT)) {
        suffix = path + strlen(SYS_ROOT);

        return duplicate_or_fail(*suffix ? suffix : "/");
    }

    if (strcmp(path, "/") == 0)
        return duplicate_or_fail(HOST_DIR);

    if (asprintf(&translated, "%s%s", HOST_DIR, path) < 0)
        fail("memory allocation failed");

    return translated;
}

static inline char *translate_path(const char *path)
{
    char *raw = translate_path_raw(path);
    char *aliased = alias_path_components(raw);
    free(raw);
    return aliased;
}

static inline char *translate_embedded_path(
    const char *arg,
    const char *path_start)
{
    char *translated_path = translate_path(path_start);
    char *result = NULL;

    size_t prefix_len = (size_t)(path_start - arg);

    if (asprintf(&result, "%.*s%s",
                 (int)prefix_len, arg, translated_path) < 0) {
        free(translated_path);
        fail("memory allocation failed");
    }

    free(translated_path);

    return result;
}

static inline char *translate_argument(const char *arg)
{
    static const char *const path_options[] = {
        "-I",
        "-L",
        "-B",
        "-F",
        "-isystem",
        "-iquote",
        "-include",
        "-imacros",
        "-idirafter",
        "-iprefix",
        "-iwithprefix",
        "-iwithprefixbefore"
    };

    const char *equals;
    const char *guest_prefix;
    const char *system_prefix;
    const char *special_prefix;

    if (arg[0] == '/')
        return translate_path(arg);

    equals = strchr(arg, '=');

    if (equals != NULL && equals[1] == '/')
        return translate_embedded_path(arg, equals + 1);

    for (size_t i = 0;
         i < sizeof(path_options) / sizeof(path_options[0]);
         ++i) {
        size_t len = strlen(path_options[i]);

        if (strncmp(arg, path_options[i], len) == 0) {
            if (arg[len] == '/') {
                return translate_embedded_path(arg, arg + len);
            } else {
                char *aliased_val = alias_path_components(arg + len);
                char *res = NULL;
                if (asprintf(&res, "%.*s%s", (int)len, arg, aliased_val) < 0) {
                    free(aliased_val);
                    fail("memory allocation failed");
                }
                free(aliased_val);
                return res;
            }
        }
    }

    guest_prefix = strstr(arg, GUEST_OVERLAY);
    system_prefix = strstr(arg, SYS_ROOT);

    if (guest_prefix == NULL) {
        special_prefix = system_prefix;
    } else if (system_prefix == NULL) {
        special_prefix = guest_prefix;
    } else {
        special_prefix = guest_prefix < system_prefix
                             ? guest_prefix
                             : system_prefix;
    }

    if (special_prefix != NULL)
        return translate_embedded_path(arg, special_prefix);

    if ((arg[0] != '-' && strchr(arg, ' ') == NULL) || access(arg, F_OK) == 0)
        return alias_path_components(arg);

    return duplicate_or_fail(arg);
}

static inline int is_mountpoint(const char *path)
{
    struct statx sx;

    if (statx(AT_FDCWD, path, AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW | AT_STATX_DONT_SYNC,
              0, &sx) == 0) {
        if (sx.stx_attributes_mask & STATX_ATTR_MOUNT_ROOT)
            return (sx.stx_attributes & STATX_ATTR_MOUNT_ROOT) != 0;
    }

    struct stat st, st_parent;
    char parent[PATH_MAX];

    if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
        return 0;

    if (snprintf(parent, sizeof parent, "%s/..", path) >= (int)sizeof parent)
        return 0;

    if (stat(parent, &st_parent) != 0)
        return 0;

    return (st.st_dev != st_parent.st_dev) ||
           (st.st_dev == st_parent.st_dev && st.st_ino == st_parent.st_ino);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        fail("kernel did not pass an executable path");

    if (access(EMULATOR, X_OK) != 0)
        fail("emulator not installed or not executable: %s", EMULATOR);

    if (!is_mountpoint(GUEST_OVERLAY))
        fail("V4 guest root is not mounted; run v4linux-setup.sh first");

    if (!is_mountpoint(HOST_MOUNT))
        fail("host root is not mounted at %s", HOST_MOUNT);

    char *host_exec = realpath(argv[1], NULL);

    if (host_exec == NULL)
        fail("cannot resolve executable path: %s", argv[1]);

    struct stat st;

    if (stat(host_exec, &st) != 0 || !S_ISREG(st.st_mode))
        fail("not a regular executable file: %s", host_exec);

    const char *exec_source =
        argv[1][0] == '/' ? argv[1] : host_exec;

    char *guest_exec = translate_path(exec_source);

    char *host_cwd = getcwd(NULL, 0);

    if (host_cwd == NULL)
        fail("cannot determine current directory");

    char *guest_cwd = translate_path(host_cwd);

    if (setenv("PWD", guest_cwd, 1) != 0)
        fail("cannot update PWD: %s", strerror(errno));

    char **emu_argv = calloc((size_t)argc + 6, sizeof(*emu_argv));

    if (emu_argv == NULL)
        fail("memory allocation failed");

    emu_argv[0] = (char *)EMULATOR;
    emu_argv[1] = "--host";
    emu_argv[2] = "-r";
    emu_argv[3] = (char *)GUEST_OVERLAY;
    emu_argv[4] = "--chdir";
    emu_argv[5] = guest_cwd;
    emu_argv[6] = guest_exec;

    for (int i = 2; i < argc; ++i)
        emu_argv[i + 5] = translate_argument(argv[i]);

    execv(EMULATOR, emu_argv);

    fail("cannot execute emulator %s: %s", EMULATOR, strerror(errno));

    return 0;
}