#include "pdp11.h"

#include <stdio.h>
#include <limits.h>

#include "options.h"
#include "filesystem.h"
#include "program.h"

static void run_program(void)
{
    for (;;)
        step();
}

int main(int argc, char **argv)
{
    int arg_index;
    const char *guest_chdir = NULL;
    static char abs_root[PATH_MAX];
    char initial_prog[PATH_MAX * 2];

    init_fdtab();

    if (parse_args(argc, argv, &arg_index, &guest_chdir) != 0)
        return 2;

    if (resolve_root(abs_root, sizeof abs_root) != 0)
        return 2;

    resolve_program(argv[arg_index], initial_prog,
                    sizeof initial_prog);

    if (setup_working_directory(guest_chdir) != 0)
        return 2;

    if (load_program(initial_prog, argc - arg_index,
                     argv + arg_index) != 0) {
        fprintf(stderr, "pdp11: cannot load %s\n", initial_prog);
        return 2;
    }

    run_program();

    return 0;
}