#include "pdp11.h"
#include "options.h"

#include <stdio.h>
#include <string.h>

int parse_args(int argc, char **argv, int *arg_index,
               const char **guest_chdir)
{
    int ai = 1;

    while (ai < argc && argv[ai][0] == '-') {
        if (!strcmp(argv[ai], "-r") && ai + 1 < argc) {
            root = argv[ai + 1];
            ai += 2;
        } else if (!strcmp(argv[ai], "--chdir") &&
                   ai + 1 < argc) {
            *guest_chdir = argv[ai + 1];
            ai += 2;
        } else if (!strcmp(argv[ai], "-x")) {
            stop_at_exec = 1;
            ai++;
        } else if (!strcmp(argv[ai], "-t")) {
            trace = 1;
            ai++;
        } else {
            break;
        }
    }

    if (ai >= argc) {
        fprintf(stderr,
                "usage: pdp11 [-r root] [--chdir dir] [-x] prog args...\n");
        return 2;
    }

    *arg_index = ai;
    return 0;
}