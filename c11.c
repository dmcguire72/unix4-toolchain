/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#include "c1.h"

void
jumpc(struct tnode *tree, int lbl, int cond)
{
	int l1, l2;

	if (tree == NULL)
		return;
	switch (tree->op) {

	case O_AND:
		if (cond) {
			cbranch(tree->p1, l1 = isn++, 0, 0);
			cbranch(tree->p2, l1, 0, 0);
			jump(lbl);
			label(l1);
		} else {
			cbranch(tree->p1, l1 = isn++, 0, 0);
			cbranch(tree->p2, l2 = isn++, 1, 0);
			label(l1);
			jump(lbl);
			label(l2);
		}
		return;

	case O_OR:
		if (cond) {
			cbranch(tree->p1, l1 = isn++, 1, 0);
			cbranch(tree->p2, l2 = isn++, 0, 0);
			label(l1);
			jump(lbl);
			label(l2);
		} else {
			cbranch(tree->p1, l1 = isn++, 1, 0);
			cbranch(tree->p2, l1, 1, 0);
			jump(lbl);
			label(l1);
		}
		return;

	case O_NOT:
		jumpc(tree->p1, lbl, !cond);
		return;
	}
	rcexpr(tree, cctab, 0);
	branch(l1 = isn++, tree->op, cond);
	jump(lbl);
	label(l1);
}

void
cbranch(struct tnode *tree, int lbl, int cond, int reg)
{
	int l1;

	if (tree == NULL)
		return;
	switch (tree->op) {

	case O_AND:
		if (cond) {
			cbranch(tree->p1, l1 = isn++, 0, reg);
			cbranch(tree->p2, lbl, 1, reg);
			label(l1);
		} else {
			cbranch(tree->p1, lbl, 0, reg);
			cbranch(tree->p2, lbl, 0, reg);
		}
		return;

	case O_OR:
		if (cond) {
			cbranch(tree->p1, lbl, 1, reg);
			cbranch(tree->p2, lbl, 1, reg);
		} else {
			cbranch(tree->p1, l1 = isn++, 1, reg);
			cbranch(tree->p2, lbl, 0, reg);
			label(l1);
		}
		return;

	case O_NOT:
		cbranch(tree->p1, lbl, !cond, reg);
		return;
	}
	rcexpr(tree, cctab, reg);
	branch(lbl, tree->op, !cond);
}

void
branch(int lbl, int op, int c)
{
	if (op) {
		if ((opdope[op] & 04) == 0)
			op = 61;
		prins(op, c);
	} else
		oprintf("br");
	oprintf("\tl%d\n", lbl);
}

void
popstk(int a)
{
	switch (a) {

	case 0:
		return;

	case 2:
		oprintf("tst\t(sp)+\n");
		return;

	case 4:
		oprintf("cmp\t(sp)+,(sp)+\n");
		return;
	}
	oprintf("add\t$%o,sp\n", a);
}
