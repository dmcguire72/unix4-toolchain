/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#include "c0.h"

struct tnode ospace[TSIZE];
int	space;

void
build(int op)
{
	struct tnode *p1, *p2, *pt;
	int t1, d1, t2, d2, t, d, dope, lr, cvn;

	p2 = NULL;
	t2 = d2 = 0;
	if (op == 4) {
		build(40);
		op = 36;
	}
	dope = opdope[op];
	if ((dope & D_BINARY) != 0) {
		p2 = *--cp;
		if (p2 != NULL) {
			t2 = p2->type;
			d2 = p2->degree;
		}
	}
	p1 = *--cp;
	t1 = p1->type;
	d1 = p1->degree;
	switch (op) {

	case 9:
		*cp++ = block(9, 0, 0, p1, p2);
		return;

	case 90:
		if (p2->op != 8)
			error("Illegal conditional");
		goto goon;

	case O_CALL:
		*cp++ = block(O_CALL, t1, 24, p1, p2);
		return;

	case O_IND:
		if ((t1 -= 16) < 0) {
			error("Illegal indirection");
			t1 += 16;
		}
		if (p1->op != T_NAME && d1 == 0)
			d1 = 1;
		*cp++ = block(O_IND, t1, d1, p1, NULL);
		return;

	case O_ADDR:
		if (p1->op == O_IND) {
			*cp++ = p1->p1;
			return;
		}
		if (p1->op == T_NAME) {
			*cp++ = block(p1->sclass == SC_AUTO ? O_ADDRA : O_ADDR,
			    t1 + 16, 1, p1, NULL);
			return;
		}
		error("Illegal lvalue");
	}
goon:
	if ((dope & D_LVAL) != 0)
		chklval(p1);
	if ((dope & D_WORDL) != 0)
		chkw(p1);
	if ((dope & D_WORDR) != 0)
		chkw(p2);
	if ((dope & D_BINARY) != 0) {
		cvn = cvtab[9 * lintyp(t1) + lintyp(t2)];
		if ((dope & D_ASSIGN) != 0) {
			t = t1;
			lr = 1;
			cvn &= 07;
		} else {
			t = (cvn & 0100) != 0 ? t2 : t1;
			lr = cvn & 0200;
			cvn = (cvn >> 3) & 07;
		}
		if (cvn) {
			if (cvn == 07) {
				error("Illegal conversion");
				goto nocv;
			}
			cvn += (dope & D_ASSIGN) != 0 ? 83 : 93;
			if (lr) {
				t2 = t;
				d2 = (p2 = convert(p2, t, d2, cvn))->degree;
			} else {
				t1 = t;
				d1 = (p1 = convert(p1, t, d1, cvn))->degree;
			}
nocv:;
		}
		if (d2 > d1 && (dope & D_COMM) != 0) {
			if ((dope & D_REL) != 0)
				op = maprel[op - 60];
			d = d1;
			d1 = d2;
			d2 = d;
			pt = p1;
			p1 = p2;
			p2 = pt;
			d = t1;
			t1 = t2;
			t2 = d;
		}
		if (d1 == d2)
			d = d1 + 1;
		else
			d = max(d1, d2);
		if ((dope & D_REL) != 0)
			t = 0;
		*cp++ = block(op, t, d, p1, p2);
		return;
	}
	*cp++ = block(op, t1, d1 == 0 ? 1 : d1, p1, NULL);
}

struct tnode *
convert(struct tnode *p, int t, int d, int cvn)
{
	int c;

	if (p->op == T_CON) {
		c = p->cval;
		switch (cvn) {

		case 99:
			c = (int16_t)(c * 2);

		case 98:
			c = (int16_t)(c * 2);

		case 97:
			c = (int16_t)(c * 2);
			p->cval = c;
			return p;
		}
	}
	return block(cvn, t, max(1, d), p, NULL);
}

void
chkw(struct tnode *p)
{
	int t;

	if ((t = p->type) > 1 && t < 16)
		error("Integer operand required");
}

int
lintyp(int t)
{
	return t < 16 ? t : (t < 32 ? t - 12 : 8);
}

struct tnode *
block(int op, int t, int d, struct tnode *p1, struct tnode *p2)
{
	struct tnode *p;

	if (space >= TSIZE) {
		error("Expression overflow");
		exit(1);
	}
	p = &ospace[space++];
	memset(p, 0, sizeof *p);
	p->op = op;
	p->type = t;
	p->degree = d;
	p->p1 = p1;
	p->p2 = p2;
	return p;
}

struct tnode *
conblock(int op, int t, int val)
{
	struct tnode *p;

	p = block(op, t, 0, NULL, NULL);
	p->cval = val;
	return p;
}

struct tnode *
nameblock(struct symbol *s)
{
	struct tnode *p;

	p = block(T_NAME, s->type, 0, NULL, NULL);
	p->sclass = s->sclass;
	if (s->sclass == SC_EXTERN)
		memcpy(p->name, s->name, NAMSIZ);
	else
		p->off = s->off;
	return p;
}

void
chklval(struct tnode *p)
{
	if (p->op != T_NAME)
		if (p->op != O_IND)
			error("Lvalue required");
}

int
max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}
