#define _GNU_SOURCE
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

static const char *PDP11 = "/opt/v4/bin/v4linux";
static const char *GUEST_ROOT = "/var/run/pdp11-v4";
static const char *HOST_MOUNT = "/var/run/pdp11-v4/host";
static const char *SYS_ROOT = "/opt/v4/sys";

static void fail(const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "unix4-dispatch: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);                                
    va_end(args);

    fputc('\n', stderr);
    exit(126);
}

static int path_has_prefix(const char *path, const char *prefix)
{
    size_t len = strlen(prefix);

    return strncmp(path, prefix, len) == 0 &&
           (path[len] == '\0' || path[len] == '/');
}

static char *duplicate_or_fail(const char *s)
{
    char *copy = strdup(s);

    if (copy == NULL)
        fail("memory allocation failed");

    return copy;
}

static char *translate_path(const char *path)
{
    const char *suffix;
    char *translated = NULL;

    if (path == NULL)
        fail("cannot translate a null path");

    if (path[0] != '/')
        return duplicate_or_fail(path);

    if (path_has_prefix(path, "/home"))
        return duplicate_or_fail(path);

    if (path_has_prefix(path, HOST_MOUNT)) {
        suffix = path + strlen(HOST_MOUNT);

        if (asprintf(&translated, "/host%s", suffix) < 0)
            fail("memory allocation failed");

        return translated;
    }

    if (path_has_prefix(path, GUEST_ROOT)) {
        suffix = path + strlen(GUEST_ROOT);

        return duplicate_or_fail(*suffix ? suffix : "/");
    }

    if (path_has_prefix(path, SYS_ROOT)) {
        suffix = path + strlen(SYS_ROOT);

        return duplicate_or_fail(*suffix ? suffix : "/");
    }

    if (strcmp(path, "/") == 0)
        return duplicate_or_fail("/host");

    if (asprintf(&translated, "/host%s", path) < 0)
        fail("memory allocation failed");

    return translated;
}

static char *translate_embedded_path(
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

static char *translate_argument(const char *arg)
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

        if (strncmp(arg, path_options[i], len) == 0 &&
            arg[len] == '/') {
            return translate_embedded_path(arg, arg + len);
        }
    }

    guest_prefix = strstr(arg, GUEST_ROOT);
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

    return duplicate_or_fail(arg);
}

static int is_mountpoint(const char *path)
{
    pid_t pid = fork();

    if (pid < 0)
        return 0;

    if (pid == 0) {
        execlp("mountpoint", "mountpoint", "-q", "--", path,
               (char *)NULL);
        _exit(127);
    }

    int status;

    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR)
            continue;
        return 0;
    }

    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        fail("kernel did not pass an executable path");

    if (access(PDP11, X_OK) != 0)
        fail("emulator not installed or not executable: %s", PDP11);

    if (!is_mountpoint(GUEST_ROOT))
        fail("V4 guest root is not mounted; run pdp11-setup.sh first");

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

    char **emu_argv = calloc((size_t)argc + 5, sizeof(*emu_argv));

    if (emu_argv == NULL)
        fail("memory allocation failed");

    emu_argv[0] = (char *)PDP11;
    emu_argv[1] = "-r";
    emu_argv[2] = (char *)GUEST_ROOT;
    emu_argv[3] = "--chdir";
    emu_argv[4] = guest_cwd;
    emu_argv[5] = guest_exec;

    for (int i = 2; i < argc; ++i)
        emu_argv[i + 4] = translate_argument(argv[i]);

    execv(PDP11, emu_argv);

    fail("cannot execute emulator %s: %s", PDP11, strerror(errno));

    return 0;
}