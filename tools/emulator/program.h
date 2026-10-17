#ifndef PROGRAM_H
#define PROGRAM_H

#include <stddef.h>

int resolve_root(char *abs_root, size_t size);

void resolve_program(const char *prog,
                     char *initial_prog,
                     size_t size);

int load_program(const char *initial_prog,
                 int argc,
                 char **argv);

#endif