/*
 * Unix Fourth Edition -- ccw.h
 * 
 * Original:
 * Copyright 1972, 1973 Bell Telephone Laboratories, Inc.
 * 
 * Modernized Port:
 * Copyright 2026 Daniel McGuire
 */
#ifndef CCW_H
#define CCW_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int16_t word;

#define OSSIZ 500
#define TB	  10

#define CZERO	(OSSIZ + TB)
#define CONE	(CZERO + 4)
#define FCZERO	(CONE + 4)
#define SCR0	(FCZERO + 4)
#define TSLEN	(SCR0 + 5 * 64)
extern word ts[TSLEN + 8];
static inline word *
tsptr(int i)
{
	return &ts[((unsigned)i < (unsigned)TSLEN) ? i : TSLEN];
}
#define TSX(i)	(*tsptr(i))
extern int mask7;
void pdpwords(double v, uint16_t w[4], int dbl);

#define OP(p)	TSX((p) + 0)
#define TYPE(p)	TSX((p) + 1)
#define DIMP(p)	TSX((p) + 2)
#define DEGREE(p)	TSX((p) + 2)
#define TR1(p)	TSX((p) + 3)
#define TR2(p)	TSX((p) + 4)
#define CLASS(p)	TSX((p) + 3)
#define OFFSET(p)	TSX((p) + 4)
#define NLOC(p)	TSX((p) + 5)
#define VALUE(p)	TSX((p) + 3)
#define BTREE(p)	TSX((p) + 1)
#define LBL(p)	TSX((p) + 2)
#define COND(p)	TSX((p) + 3)
#define SSP(p)		((int)((uint16_t)TSX((p) + 2) & 0377))
#define LENP(p)		((int)(((uint16_t)TSX((p) + 2) >> 8) & 0377))
#define NAMEP(p)	((char *)&TSX((p) + 5))

#define ncps	8

enum {
	EOFC = 0, SEMI, LBRACE, RBRACE, LBRACK, RBRACK, LPARN, RPARN, COLON, COMMA,
	KEYW = 19, NAME, CON, STRING, FCON, SFCON,
	SIZEOF = 29, INCBEF, DECBEF, INCAFT, DECAFT, EXCLA, AMPER, STAR, NEG, COMPL,
	DOT, PLUS, MINUS, TIMES, DIVIDE, MOD, RSHIFT, LSHIFT, AND, OR, EXOR,
	ARROW, ITOF, FTOI, LOGAND, LOGOR,
	EQUAL = 60, NEQUAL, LESSEQ, LESS, GREATEQ, GREAT, LESSP, LESSEQP, GREATP,
	GREATQP,
	ASPLUS = 70, ASMINUS, ASTIMES, ASDIV, ASMOD, ASRSH, ASLSH, ASSAND, ASOR,
	ASXOR, ASSIGN,
	QUEST = 90,
	CALL = 100, MCALL, JUMP, CBRANCH, INIT, SETREG, LOAD,
	RFORCE = 110, BRANCH, LABEL
};
#define AMPERA	29

enum { INT = 0, CHAR, FLOAT, DOUBLE, STRUCT, RSTRUCT, PTR = 010, FUNC = 020, ARRAY = 030 };

enum { KEYWC = 1, MOS = 4, AUTO, EXTERN, STATIC, REG, STRTAG, ARG, ARG1 };

#define BINARY	01
#define LVALUE	02
#define RELAT	04
#define ASSGOP	010
#define LWORD	020
#define RWORD	040
#define COMMUTE	0100
#define RASSOC	0200
#define LEAF	0400

enum { TAB_REG = 0, TAB_EFF, TAB_CC, TAB_SP };

extern int opdope[];
#define NOPDOPE	111

static inline int
OPD(int op)
{
	return (unsigned)op < NOPDOPE ? opdope[op] : 0;
}

void cprintf(const char *fmt, ...);
extern FILE *outf;
extern FILE *ascf;
extern FILE *binf;
extern int line, nerror;
void error(const char *fmt, ...);
int nodeinfo(const word *w, int *pm);
int ptrmask(int op);
#define TREEBASE_ADDR	0x4000
void putw16(int w, FILE *f);
int getw16(FILE *f, int *eof);

#endif
