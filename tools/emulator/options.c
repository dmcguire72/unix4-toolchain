#include "pdp11.h"
#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int host_enabled = 0;

int parse_args(int argc, char **argv, int *arg_index,
               const char **guest_chdir)
{
    int ai = 1;
    const char *env;

    env = getenv("V4LINUX_TRACE_CPU");
    if (env && !strcmp(env, "1"))
        trace |= TR_CPU;

    env = getenv("V4LINUX_TRACE_SYS");
    if (env && !strcmp(env, "1"))
        trace |= TR_SYS;

    env = getenv("V4LINUX_TRACE_FPU");
    if (env && !strcmp(env, "1"))
        trace |= TR_FPU;

    env = getenv("V4LINUX_TRACE_ALL");
    if (env && !strcmp(env, "1"))
        trace |= TR_CPU | TR_FPU | TR_SYS;

    env = getenv("V4LINUX_STOP_AT_EXEC");
    if (env && !strcmp(env, "1"))
        stop_at_exec = 1;

    env = getenv("V4LINUX_TRACE_FILE");
    if (env && *env)
        trace_open(env);

    while (ai < argc && argv[ai][0] == '-') {
        if (!strcmp(argv[ai], "-r") && ai + 1 < argc) {
            root = argv[ai + 1];
            ai += 2;
        } else if (!strcmp(argv[ai], "--chdir") &&
                   ai + 1 < argc) {
            *guest_chdir = argv[ai + 1];
            ai += 2;
        } else if (!strcmp(argv[ai], "--host")) {
            host_enabled = 1;
            ai++;
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
                "usage: v4linux [-r root] [--chdir dir] [--host] [-x] [-s] [-t] [-F] [-T] [-o file]\n"
                "              prog args...\n"
                "  --host  enable host integrations\n"
                "  -s  trace guest system calls (strace style)\n"
                "  -t  trace every instruction     -F  trace FP instructions\n"
                "  -T  all traces                  -o  write trace to file\n");
        return 2;
    }

    *arg_index = ai;
    return 0;
}