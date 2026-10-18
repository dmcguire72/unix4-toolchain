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
        return;
    }

    char resolved[PATH_MAX];
    if (realpath(prog, resolved) != NULL) {
        snprintf(initial_prog, size, "%s", resolved);
        return;
    }

    if (root[0] != '\0') {
        char test_path[PATH_MAX * 2];
        snprintf(test_path, sizeof test_path, "%s/bin/%s", root, prog);
        if (access(test_path, F_OK) == 0) {
            snprintf(initial_prog, size, "%s", test_path);
            return;
        }
        snprintf(test_path, sizeof test_path, "%s/usr/bin/%s", root, prog);
        if (access(test_path, F_OK) == 0) {
            snprintf(initial_prog, size, "%s", test_path);
            return;
        }
    }

    snprintf(initial_prog, size, "%s", prog);
}

int load_program(const char *initial_prog, int argc, char **argv)
{
    int magic;

    if (load_image(initial_prog, argc, argv) == 0)
        return 0;

    if (load_errno == 2) {
        return -2; 
    }

    magic = has_valid_magic(initial_prog);
    if (magic == 0)
        return -2;
    else if (magic == -1)
        return -1; 

    return -1;
}