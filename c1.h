/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#ifndef C1_H
#define C1_H

#include "cc.h"

struct pat {
	unsigned char m1, t1;
	unsigned char m2, t2;
	const char *code;
};

struct optab {
	int op;
	const struct pat *pats;
};

struct instab {
	int op;
	const char *s1, *s2;
};

extern const struct optab regtab[], efftab[], cctab[], sptab[];

extern const struct instab instab[];

extern int nreg;
extern int isn;
void	rcexpr(struct tnode *tree, const struct optab *table, int reg);
int	cexpr(struct tnode *tree, const struct optab *table, int reg);
const char *match(struct tnode *tree, const struct optab *table, int nreg);
int	dcalc(struct tnode *p, int nreg);
int	notcompat(int at, int st);
void	prins(int op, int c);
int	collcon(const struct tnode *p);

void	jumpc(struct tnode *tree, int lbl, int cond);
void	cbranch(struct tnode *tree, int lbl, int cond, int reg);
void	branch(int lbl, int op, int c);
void	popstk(int a);

#endif
