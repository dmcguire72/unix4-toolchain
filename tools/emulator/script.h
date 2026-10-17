#ifndef SCRIPT_H
#define SCRIPT_H

#include <stddef.h>

int has_valid_magic(const char *path);

int prepare_script_path(const char *program,
                        char *guest_path,
                        size_t guest_path_size);

void cleanup_script(void);

#endif