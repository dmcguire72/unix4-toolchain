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

    char *guest_exec = NULL;

    if (asprintf(&guest_exec, "/host%s", host_exec) < 0)
        fail("memory allocation failed");

    char *host_cwd = getcwd(NULL, 0);

    if (host_cwd == NULL)
        fail("cannot determine current directory");

    char *guest_cwd = NULL;

    if (asprintf(&guest_cwd, "/host%s", host_cwd) < 0)
        fail("memory allocation failed");

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
        emu_argv[i + 4] = argv[i];

    execv(PDP11, emu_argv);

    fail("cannot execute emulator %s: %s", PDP11, strerror(errno));

    return 0;
}