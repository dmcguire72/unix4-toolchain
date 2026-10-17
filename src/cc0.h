/*
 * Copyright 1972, 1973 Bell Telephone Laboratories, Inc.
 */
#ifndef CC0_H
#define CC0_H
#include "ccw.h"

#define HSHSIZ	200
#define CMSIZ	40
#define SWSIZ	200
#define NCPW	2
#define DIMSIZ	100

enum { GOTO = 10, RETURN, IF, WHILE, ELSE, SWITCH, CASE, BREAK, CONTIN, DO, DEFAULT, FOR };
enum { INSERT = 119, PERIOD, SQUOTE, DQUOTE, LETTER, DIGIT, NEWLN, SPACE, UNKN };

extern int opdope[];
extern const unsigned char cvtab[], ctab[128];
#endif
