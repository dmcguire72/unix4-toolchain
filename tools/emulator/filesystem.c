#include "pdp11.h"
#include "filesystem.h"

#include <stdio.h>
#include <unistd.h>
#include <limits.h>

void init_fdtab(void)
{
    int i;

    for (i = 0; i < 16; i++)
        fdtab[i] = -1;

    fdtab[0] = 0;
    fdtab[1] = 1;
    fdtab[2] = 2;
}

int setup_working_directory(const char *guest_chdir)
{
    if (root[0] != '\0' && chdir(root) < 0) {
        perror("v4linux: cannot enter root directory");
        return 2;
    }

    if (guest_chdir != NULL) {
        char target_cwd[PATH_MAX * 2];

        if (root[0] != '\0') {
            if (guest_chdir[0] == '/') {
                snprintf(target_cwd, sizeof target_cwd,
                         "%s%s", root, guest_chdir);
            } else {
                snprintf(target_cwd, sizeof target_cwd,
                         "%s/%s", root, guest_chdir);
            }
        } else {
            snprintf(target_cwd, sizeof target_cwd,
                     "%s", guest_chdir);
        }

        if (chdir(target_cwd) < 0) {
            perror("v4linux: cannot chdir to requested directory");
            return 2;
        }
    }

    return 0;
}