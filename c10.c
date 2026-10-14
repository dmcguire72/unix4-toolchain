/*

	    	C compiler, part 2

	Copyright 1972 Bell Telephone Laboratories, Inc.

*/

#include "c1.h"

int	nreg	= 4;
int	isn	= 10000;
int	line;
int	nerror;
int	fltmod;
const char *tmpfil;
FILE	*fin, *fout;

static struct tnode ospace[TSIZE];

static void pname(const struct tnode *p);
static int isfloat(const struct tnode *t, const char **s);

static void
botch(void)
{
	fputs("Temp file botch.\n", stderr);
	exit(1);
}

static struct tnode *
nodeat(int w, int n)
{
	if (w == 0)
		return NULL;
	if (w < 0 || w > n)
		botch();
	return &ospace[w - 1];
}

static struct tnode *
readtree(const struct optab **tab)
{
	static const struct optab *const tabtab[] = { regtab, efftab, cctab, sptab };
	int n, root, t, i, j, w;
	struct tnode *p;

	n = getwrd();
	root = getwrd();
	t = getwrd();
	line = getwrd();
	if (n < 0 || n > TSIZE || root < 0 || root > n || t < 0 || t > TAB_SP)
		botch();
	*tab = tabtab[t];
	for (i = 0; i < n; i++) {
		p = &ospace[i];
		memset(p, 0, sizeof *p);
		p->op = getwrd();
		p->type = getwrd();
		p->degree = getwrd();
		switch (p->op) {
		case T_NAME:
			p->sclass = getwrd();
			if (p->sclass == SC_EXTERN) {
				for (j = 0; j < NAMSIZ; j += 2) {
					w = getwrd();
					p->name[j] = w & 0377;
					p->name[j + 1] = (w >> 8) & 0377;
				}
			} else
				p->off = getwrd();
			break;
		case T_CON:
		case T_STRING:
			p->cval = getwrd();
			break;
		default:
			p->p1 = nodeat(getwrd(), n);
			p->p2 = nodeat(getwrd(), n);
		}
	}
	return nodeat(root, n);
}

int
main(int argc, char **argv)
{
	int c;
	const struct optab *table;
	struct tnode *tree;

	if (argc < 4) {
		error("Arg count");
		fprintf(stderr, "usage: %s input tmpfile output\n", argv[0]);
		exit(1);
	}
	if ((fin = fopen(argv[1], "rb")) == NULL) {
		error("Can't find %s", argv[1]);
		exit(1);
	}
	if ((fout = fopen(argv[3], "wb")) == NULL) {
		error("Can't create %s", argv[3]);
		exit(1);
	}
	tmpfil = argv[2];

	while ((c = rdchar()) != 0) {
		if (c == '#') {
			tree = readtree(&table);
			rcexpr(tree, table, 0);
		} else
			wrchar(c);
	}
	flush();
	exit(nerror != 0);
}

const char *
match(struct tnode *tree, const struct optab *table, int nreg)
{
	int op, d1, d2, t1, t2;
	struct tnode *p1, *p2;
	const struct pat *mp;

	d2 = t2 = 0;
	p2 = NULL;
	if (tree == NULL)
		return NULL;
	op = tree->op;
	if (op >= 29)
		p1 = tree->p1;
	else
		p1 = tree;
	if (p1 == NULL)
		return NULL;
	t1 = p1->type;
	d1 = dcalc(p1, nreg);
	if ((opdope[op] & 01) != 0) {
		p2 = tree->p2;
		if (p2 != NULL)
			t2 = p2->type;
		d2 = dcalc(p2, nreg);
	}
	for (; table->op != 0; table++)
		if (table->op == op)
			goto foundop;
	return NULL;
foundop:
	for (mp = table->pats; mp->m1 != 0; mp++) {
		if (d1 > (mp->m1 & 077) || ((mp->m1 >= 0100) && (p1->op != 36)))
			continue;
		if (notcompat(t1, mp->t1))
			continue;
		if ((opdope[op] & 01) != 0 && p2 != NULL) {
			if (d2 > (mp->m2 & 077) || ((mp->m2 >= 0100) && (p2->op != 36)))
				continue;
			if (notcompat(t2, mp->t2))
				continue;
		}
		return mp->code;
	}
	return NULL;
}

void
rcexpr(struct tnode *tree, const struct optab *table, int reg)
{
	if (tree == NULL)
		return;
	if (tree->op >= O_JUMPC) {
		if (tree->op == O_JUMPC)
			jumpc(tree->p1, tree->degree, tree->type);
		else
			cbranch(tree->p1, tree->degree, tree->type, 0);
		return;
	}
	if (cexpr(tree, table, reg))
		return;
	if (table != regtab)
		if (cexpr(tree, regtab, reg)) {
			if (table == sptab)
				oprintf("mov\tr%d,-(sp)\n", reg);
			if (table == cctab)
				oprintf("tst\tr%d\n", reg);
			return;
		}
	error("No match for op %d", tree->op);
}

int
cexpr(struct tnode *tree, const struct optab *table, int reg)
{
	struct tnode *p1, *p2, *p;
	const struct optab *ctable;
	int c, r, k;
	const char *string;

	if ((c = tree->op) == O_CALL) {
		p1 = tree->p1;
		p2 = tree->p2;
		r = 0;
		if (p2) {
			while (p2->op == 9) {
				rcexpr(p2->p2, sptab, 0);
				r += rlength((p = p2->p2)->type);
				p2 = p2->p1;
			}
			rcexpr(p2, sptab, 0);
			r += rlength(p2->type);
		}
		tree->op = O_MCALL;
		tree->degree = r;
	}
	if (c == O_QUEST) {
		if (tree->p2 == NULL || tree->p2->op != T_COLON) {
			error("Illegal conditional");
			return 1;
		}
		c = isn++;
		cbranch(tree->p1, c, 0, reg);
		rcexpr(tree->p2->p1, table, reg);
		r = isn++;
		branch(r, 0, 0);
		label(c);
		rcexpr(tree->p2->p2, table, reg);
		label(r);
		return 1;
	}
	if ((string = match(tree, table, nreg - reg)) == NULL)
		return 0;
	p1 = tree->p1;
	p2 = tree->p2;
	for (;;) {
		switch (c = *string++) {

		case '\0':
			p = tree;
			if (p->op == O_MCALL && p->degree > 0)
				popstk(p->degree);
			return 1;

		case 'A':
			pname(tree->p1);
			continue;

		case 'B':
			pname(tree->p2);
			continue;

		case 'O':
			pname(tree);
			continue;

		case 'M':
			if ((c = *string) == '\'')
				string++;
			else
				c = 0;
			prins(tree->op, c);
			continue;

		case 'C':
			p = tree->p1;
			goto pbyte;

		case 'P':
			p = tree;
			goto pb1;

		case 'D':
			p = tree->p2;
		pbyte:
			if (p->type == 1)
				wrchar('b');
		pb1:
			if (isfloat(p, NULL))
				wrchar('f');
			continue;

		case 'L':
			if (tree->p1->type == 1 || tree->p2->type == 1)
				wrchar('b');
			continue;

		case 'E':
			oprintf("%o", p1->cval);
			continue;

		case 'F':
			oprintf("%o", p2->cval);
			continue;

		case 'G':
			p = p1;
			goto subtre;

		case 'K':
			p = p2;
			goto subtre;

		case 'H':
			p = tree;

		subtre:
			ctable = regtab;
			r = reg;
			k = *string++ - 'A';
			if ((k & 02) != 0)
				ctable = sptab;
			if ((k & 04) != 0)
				ctable = cctab;
			if ((k & 010) != 0)
				r = reg + 1;
			if ((k & 01) != 0)
				if (p->op == 36) {
					p = p->p1;
					if (collcon(p) && ctable != sptab)
						p = p->p1;
				}
			rcexpr(p, ctable, r);
			continue;

		case 'I':
			r = reg;
			goto preg;

		case 'J':
			r = reg + 1;
		preg:
			oprintf("r%d", r);
			continue;

		case '#':
			p = p1->p1;
			goto nmbr;

		case '"':
			p = p2->p1;
			goto nmbr;

		case '~':
			p = tree->p1;

		nmbr:
			if (collcon(p)) {
				c = p->op;
				if ((r = p->p2->cval) != 0)
					oprintf("%o", c == 40 ? r : -r);
			}
			continue;

		case 'N':
			if ((c = isfloat(tree, &string)) == fltmod)
				continue;
			oprintf((fltmod = c) == 2 ? "setf\n" : "setd\n");
			continue;

		case 'Z':
			oprintf("$%o", p1->off);
			continue;
		}
		wrchar(c);
	}
}

static void
pname(const struct tnode *p)
{
loop:
	switch (p->op) {

	case T_CON:
		oprintf("$%o", p->cval);
		return;

	case T_STRING:
		oprintf("$l%d", p->cval);
		return;

	case T_NAME:
		switch (p->sclass) {

		case SC_AUTO:
			oprintf("%o(r5)", p->off);
			return;

		case SC_EXTERN:
			oprintf("%p", p->name);
			return;

		}
		oprintf("L%d", p->off);
		return;

	case O_ADDR:
		wrchar('$');
		p = p->p1;
		goto loop;

	case O_IND:
		wrchar('*');
		p = p->p1;
		goto loop;
	}
	error("pname called illegally");
}

int
dcalc(struct tnode *p, int nreg)
{
	int op;

	if (p == NULL)
		return 0;
	op = p->op;
	switch (op) {

	case T_NAME:
	case T_STRING:
	case 23:
	case 24:
		return 12;

	case T_CON:
		return p->cval == 0 ? 4 : 8;

	case O_ADDR:
		return 12;

	case O_IND:
		if ((op = dcalc(p->p1, nreg)) < 16)
			return 16;
	}
	return p->degree <= nreg ? 20 : 24;
}

int
notcompat(int at, int st)
{
	if (st == 0)
		return at > 1 && at < 16;
	if (st == 1)
		return at > 0 && at < 16;
	st -= 2;
	if (st == 2 && at == 3)
		at = 2;
	return st != at;
}

void
prins(int op, int c)
{
	const struct instab *insp;
	const char *s;

	for (insp = instab; insp->op != 0; insp++)
		if (insp->op == op) {
			if ((s = c != 0 ? insp->s2 : insp->s1) == NULL)
				break;
			oprintf("%s", s);
			return;
		}
	error("No match' for op %d", op);
}

int
collcon(const struct tnode *p)
{
	if (p->op == 40 || p->op == 41)
		if (p->p2->op == T_CON)
			return 1;
	return 0;
}

static int
isfloat(const struct tnode *t, const char **s)
{
	int rt;

	if (s != NULL) {
		rt = **s - '0';
		if (rt == 2 || rt == 4) {
			(*s)++;
			return rt > 2 ? 3 : 2;
		}
	}
	if ((opdope[t->op] & 010) != 0)
		t = t->p1;
	if ((rt = t->type) >= 2 && rt <= 3)
		return rt;
	return 0;
}
