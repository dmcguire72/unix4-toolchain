#include "pdp11.h"
#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    env = getenv("V4LINUX_TRACE_FUNC");
    if (env && !strcmp(env, "1"))
        trace |= TR_FUNC;

    env = getenv("V4LINUX_TRACE_STACK");
    if (env && !strcmp(env, "1"))
        crash_trace = 1;

    env = getenv("V4LINUX_TRACE_ALL");
    if (env && !strcmp(env, "1")) {
        trace |= TR_ALL;
        crash_trace = 1;
    }

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
        } else if (!strcmp(argv[ai], "-f") ||
                   !strcmp(argv[ai], "--trace-func")) {
            trace |= TR_FUNC;
            ai++;
        } else if (!strcmp(argv[ai], "-b") ||
                   !strcmp(argv[ai], "--backtrace")) {
            crash_trace = 1;
            ai++;
        } else if (!strcmp(argv[ai], "-T")) {
            trace |= TR_ALL;
            crash_trace = 1;
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
                "usage: %s [-r root] [--chdir dir] [--host] [-x] [-s] [-t] [-F] [-f] [-b] [-T] [-o file]\n"
                "              prog args...\n"
                "  --host  enable host integrations\n"
                "  -s  trace guest system calls (strace style)\n"
                "  -t  trace every instruction     -F  trace FP instructions\n"
                "  -f  trace function calls and their args (--trace-func)\n"
                "  -b  stack trace on crash (--backtrace)\n"
                "  -T  all traces, -o  write trace to file\n"
                "env: V4LINUX_TRACE_{CPU,SYS,FPU,FUNC,STACK,ALL}=1, V4LINUX_TRACE_FILE=path\n", argv[0]);
        return 2;
    }

    *arg_index = ai;
    return 0;
}