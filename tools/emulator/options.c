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
            trace |= TR_CPU;
            ai++;
        } else if (!strcmp(argv[ai], "-s")) {
            trace |= TR_SYS;
            ai++;
        } else if (!strcmp(argv[ai], "-F")) {
            trace |= TR_FPU;
            ai++;
        } else if (!strcmp(argv[ai], "-T")) {
            trace |= TR_CPU | TR_FPU | TR_SYS;
            ai++;
        } else if (!strcmp(argv[ai], "-o") && ai + 1 < argc) {
            trace_open(argv[ai + 1]);
            ai += 2;
        } else {
            break;
        }
    }

    if (ai >= argc) {
        fprintf(stderr,
                "usage: v4linux [-r root] [--chdir dir] [-x] [-s] [-t] [-F] [-T] [-o file]\n"
                "              prog args...\n"
                "  -s  trace guest system calls (strace style)\n"
                "  -t  trace every instruction     -F  trace FP instructions\n"
                "  -T  all traces                  -o  write trace to file\n");
        return 2;
    }

    *arg_index = ai;
    return 0;
}