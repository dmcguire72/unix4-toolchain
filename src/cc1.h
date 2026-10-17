/*
 * Copyright 1972, 1973 Bell Telephone Laboratories, Inc.
 */
#ifndef CC1_H
#define CC1_H
#include "ccw.h"

enum { LOAD_ = 106 };

struct optab {
	unsigned char tabdeg1, tabtyp1, tabdeg2, tabtyp2;
	const char *tabstring;
};
struct table {
	int op;
	const struct optab *tabp;
};
struct instab {
	int op;
	const char *str1, *str2;
};

extern const struct table regtab[], efftab[], cctab[], sptab[];
extern const struct instab instab[];
#endif
