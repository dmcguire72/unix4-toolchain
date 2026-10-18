#include "pdp11.h"
#include "program.h"
#include "script.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int resolve_root(char *abs_root, size_t size)
{
    (void)size;

    if (root[0] == '\0')
        return 0;

    if (realpath(root, abs_root) == NULL) {
        perror("v4linux: invalid root directory");
        return 2;
    }

    root = abs_root;

    return 0;
}

void resolve_program(const char *prog,
                     char *initial_prog,
                     size_t size)
{
    if (root[0] != '\0' && prog[0] == '/') {
        snprintf(initial_prog, size, "%s%s", root, prog);
    } else if (realpath(prog, initial_prog) == NULL) {
        if (root[0] != '\0') {
            snprintf(initial_prog, size, "%s/%s", root, prog);
        } else {
            snprintf(initial_prog, size, "%s", prog);
        }
    }

    if (access(initial_prog, F_OK) != 0) {
        char test_path[PATH_MAX * 2];

        snprintf(test_path, sizeof test_path, "%s/bin/%s",
                 root, prog);

        if (access(test_path, F_OK) == 0) {
            snprintf(initial_prog, size, "%s", test_path);
        } else {
            snprintf(test_path, sizeof test_path,
                     "%s/usr/bin/%s", root, prog);

            if (access(test_path, F_OK) == 0)
                snprintf(initial_prog, size, "%s", test_path);
        }
    }
}

int load_program(const char *initial_prog, int argc, char **argv)
{
    char shell_path[PATH_MAX * 2];
    char script_path[PATH_MAX * 2];
    char **shell_argv;
    int i;
    int magic;

    if (load_image(initial_prog, argc, argv) == 0)
        return 0;

    magic = has_valid_magic(initial_prog);

    if (magic != 0)
        return -1;

    if (root[0] != '\0') {
        snprintf(shell_path, sizeof shell_path,
                 "%s/bin/sh", root);
    } else {
        snprintf(shell_path, sizeof shell_path, "/bin/sh");
    }

    if (access(shell_path, F_OK) != 0)
        return -1;

    if (prepare_script_path(initial_prog, script_path,
                            sizeof script_path) < 0)
        return -1;

    shell_argv = calloc((size_t)argc + 2, sizeof *shell_argv);
    if (!shell_argv)
        return -1;

    shell_argv[0] = (char *)"/bin/sh";
    shell_argv[1] = script_path;

    for (i = 1; i < argc; i++)
        shell_argv[i + 1] = argv[i];

    if (load_image(shell_path, argc + 1, shell_argv) < 0) {
        free(shell_argv);
        cleanup_script();
        return -1;
    }

    free(shell_argv);
    atexit(cleanup_script);

    return 0;
}