/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#include "c0.h"

void
jumpc(struct tnode *tree, int lbl, int cond)
{
	rcexpr(block(easystmt() + O_JUMPC, cond, lbl, tree, NULL), TAB_CC);
}

static int
nidx(const struct tnode *p)
{
	return p == NULL ? 0 : (int)(p - ospace) + 1;
}

void
rcexpr(struct tnode *tree, int table)
{
	int i, j;
	const struct tnode *p;

	wrchar('#');
	putwrd(space);
	putwrd(nidx(tree));
	putwrd(table);
	putwrd(line);
	for (i = 0; i < space; i++) {
		p = &ospace[i];
		putwrd(p->op);
		putwrd(p->type);
		putwrd(p->degree);
		switch (p->op) {
		case T_NAME:
			putwrd(p->sclass);
			if (p->sclass == SC_EXTERN)
				for (j = 0; j < NAMSIZ; j += 2)
					putwrd((uint8_t)p->name[j] |
					    (uint8_t)p->name[j + 1] << 8);
			else
				putwrd(p->off);
			break;
		case T_CON:
		case T_STRING:
			putwrd(p->cval);
			break;
		default:
			putwrd(nidx(p->p1));
			putwrd(nidx(p->p2));
		}
	}
}

void
retseq(void)
{
	oprintf("jmp\tretrn\n");
}

void
slabel(void)
{
	oprintf(".data; l%d: 1f; .text; 1:\n", csym->off);
}

void
setstk(int a)
{
	int ts;

	ts = a - stack;
	stack = a;
	switch (ts) {

	case 0:
		return;

	case -2:
		oprintf("tst\t-(sp)\n");
		return;

	case -4:
		oprintf("cmp\t-(sp),-(sp)\n");
		return;
	}
	oprintf("add\t$%o,sp\n", ts);
}

void
defvec(int a)
{
	(void)a;
	oprintf("mov\tsp,r0\nmov\tr0,-(sp)\n");
	stack -= 2;
}

void
defstat(struct symbol *s)
{
	int len;

	len = length(s->type);
	if (s->dim)
		oprintf(".data; l%d:1f; .bss; 1:.=.+%o; .even; .text\n", s->off,
		    s->dim * len);
	else
		oprintf(".bss; l%d:.=.+%o; .even; .text\n", s->off, len);
}
