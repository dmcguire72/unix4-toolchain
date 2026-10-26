/*
 * Unix Fourth Edition -- astab.c
 * 
 * Original assembly:
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 * 
 * C Port:
 * Copyright 2026 Daniel McGuire
 */
#ifndef AS_H
#define AS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NBSYM	179

struct bsym {
	const char *name;
	uint16_t type;
	uint16_t value;
};
extern const struct bsym bsymtab[NBSYM];

#define TOK_BUILTIN	01000
#define TOK_USER	04000

#define ASEXIT_OK	0
#define ASEXIT_ERR	1

const char *as_tmpdir(void);

#endif
