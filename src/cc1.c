/*
 * Copyright 1972, 1973 Bell Telephone Laboratories, Inc.
 */
#include "cc1.h"

#define max_(a, b)	((a) > (b) ? (a) : (b))

static const char maprel[] = { EQUAL, NEQUAL, GREATEQ, GREAT, LESSEQ,
	LESS, GREATP, GREATQP, LESSP, LESSEQP };
static const char notrel[] = { NEQUAL, EQUAL, GREAT, GREATEQ, LESS,
	LESSEQ, GREATQP, GREATP, LESSEQP, LESSP };

static int nreg = 3, isn = 10000, nstack, nfloat;
static const struct table *const tabtab[] = { regtab, efftab, cctab, sptab, 0 };

static const struct table lsptab_obj[1] = { { 0, 0 } };
#define lsptab	lsptab_obj
static int scr = SCR0;

static void branch(int, int, int);
static void label(int), popstk(int);
static int rcexpr(int, const struct table *, int);

struct cfv {
	int fflag, nargs;
};
static int cexpr(int, const struct table *, int, struct cfv *);
static int optim(int);
static int unoptim(int);
static int acommute(int);
static void cbranch(int, int, int, int);
static void pswitch(int, int, int);
static int degree(int), dcalc(int, int);

static int mod16(int a, int b) { return b == 0 ? a : a % b; }
static int div16(int a, int b) { return b == 0 ? (a < 0 ? -1 : 0) : a / b; }

static int
ash16(int v, int n)
{
	n &= 077;
	if (n & 040)
		n -= 64;
	if (n >= 0) {
		while (n-- > 0)
			v = (int16_t)(v * 2);
		return v;
	}
	while (n++ < 0)
		v >>= 1;
	return v;
}

static void putchar1(int c) { cprintf("%c", c); }

static unsigned char seen[OSSIZ + TB + 8];

static int
fixnode(int a, int base, int n)
{
	int idx, pm, k;

	if (a == 0)
		return 0;
	if (a < base || a >= base + 2 * n || ((a - base) & 1)) {
		error("Temp file botch");
		exit(1);
	}
	idx = TB + (a - base) / 2;
	if (seen[idx])
		return idx;
	seen[idx] = 1;
	pm = ptrmask(OP(idx));
	for (k = 0; k < 5; k++)
		if (pm & (1 << k))
			ts[idx + k] = (word)fixnode(ts[idx + k] & 0xffff, base, n);
	return idx;
}

static int
fixtree(int root, int base, int n)
{
	memset(seen, 0, sizeof seen);
	return fixnode(root, base, n);
}

int
main(int argc, char **argv)
{
	int c, i, tree, tab, n, base, eof = 0;
	const struct table *table;
	FILE *asc;

	outf = NULL;
	if (argc < 4) {
		error("Arg count");
		exit(1);
	}
	if ((asc = fopen(argv[1], "rb")) == NULL || (binf = fopen(argv[2], "rb")) == NULL) {
		error("Missing temp file");
		exit(1);
	}
	if ((ascf = fopen(argv[3], "wb")) == NULL) {
		error("Can't create %s", argv[3]);
		exit(1);
	}
	outf = ascf;
	mask7 = 1;
	ts[CZERO] = CON; ts[CZERO + 1] = INT; ts[CZERO + 2] = 0; ts[CZERO + 3] = 0;
	ts[CONE] = CON; ts[CONE + 1] = INT; ts[CONE + 2] = 0; ts[CONE + 3] = 1;
	ts[FCZERO] = SFCON; ts[FCZERO + 1] = DOUBLE; ts[FCZERO + 2] = 0; ts[FCZERO + 3] = 0;

	base = getw16(binf, &eof) & 0xffff;
	while ((c = fgetc(asc)) > 0) {
		if (c != '#') {
			cprintf("%c", c);
			continue;
		}
		n = getw16(binf, &eof);
		tree = getw16(binf, &eof) & 0xffff;
		tab = getw16(binf, &eof);
		line = getw16(binf, &eof);
		if (eof || n < 0 || n > OSSIZ || tab < 0 || tab > 4) {
			error("Temp file botch");
			exit(1);
		}
		for (i = 0; i < n; i++)
			ts[TB + i] = getw16(binf, &eof);
		if (tab == 4) {
			pswitch(TB, TB + n, (int16_t)tree);
			continue;
		}
		tree = fixtree(tree, base, n);
		table = tabtab[tab];
		tree = optim(tree);
		nstack = 0;
		rcexpr(tree, table, 0);
	}
	if (nfloat)
		cprintf(".globl\tfltused\n");
	fflush(ascf);
	exit(nerror != 0);
}

static int
degree(int t)
{
	if (t == 0 || OP(t) == 0)
		return 0;
	if (OP(t) == CON)
		return -3;
	if (OP(t) == AMPER)
		return -2;
	if (OP(t) == AMPERA)
		return -1;
	if ((OPD(OP(t)) & LEAF) != 0) {
		if (TYPE(t) == CHAR)
			return 1;
		return 0;
	}
	return DEGREE(t);
}

static void
pname(int p, int flag)
{
	int i;

loop:
	switch (OP(p)) {
	case SFCON:
	case CON:
		cprintf("$0%o", VALUE(p));
		return;
	case FCON:
		cprintf("L%d", VALUE(p));
		return;
	case NAME:
		if ((i = OFFSET(p)) != 0)
			cprintf("%d.+", i);
		switch (CLASS(p)) {
		case AUTO:
			cprintf("%d.(r5)", NLOC(p));
			return;
		case EXTERN:
			cprintf("_%.8s", NAMEP(p));
			return;
		case STRUCT:
			error("Illegal structure reference");
			cprintf("$0");
			return;
		case REG:
			if (i)
				error("Bad reg. reference");
			cprintf("r%d", NLOC(p));
			return;
		}
		cprintf("L%d", NLOC(p));
		return;
	case AMPER:
		cprintf("$");
		p = TR1(p);
		goto loop;
	case STAR:
		p = TR1(p);
		if (OP(p) == PLUS) {
			if (OP(TR2(p)) == AMPER) {
				pname(TR1(TR2(p)), 0);
				cprintf("(r%d)", NLOC(TR1(p)));
				return;
			}
			cprintf("%d.(r%d)", VALUE(TR2(p)), NLOC(TR1(p)));
			return;
		} else if (OP(p) == INCAFT) {
			cprintf("(r%d)%c", NLOC(TR1(p)), flag ? 0 : '+');
			return;
		} else if (OP(p) == DECBEF) {
			cprintf("%c(r%d)", flag ? 0 : '-', NLOC(TR1(p)));
			return;
		} else if (OP(p) == NAME && CLASS(p) == REG) {
			cprintf("(r%d)", NLOC(p));
			return;
		}
		cprintf("*");
		goto loop;
	}
	error("pname called illegally");
}

static int
dcalc(int p, int nrleft)
{
	int p1;

	if (p == 0)
		return 0;
	switch (OP(p)) {
	case NAME:
	case AMPER:
		return 12;
	case CON:
	case SFCON:
		return VALUE(p) == 0 ? 4 : (VALUE(p) == 1 ? 5 : 8);
	case FCON:
		return 12;
	case STAR:
		p1 = TR1(p);
		if (OP(p1) == NAME || OP(p1) == CON)
			return 12;
		if (OP(p1) == STAR)
			p1 = TR1(p1);
		if (OP(p1) == PLUS && OP(TR1(p1)) == NAME
		 && CLASS(TR1(p1)) == REG && OP(TR2(p1)) == CON)
			return 12;
		if (OP(p1) == PLUS && OP(TR2(p1)) == AMPER && OP(TR1(p1)) == NAME
		 && TYPE(TR2(p1)) == (PTR + CHAR) && CLASS(TR1(p1)) == REG)
			return 12;
		if ((OP(p1) == INCAFT || OP(p1) == DECBEF) &&
		     TYPE(TR1(p1)) != (STRUCT + PTR))
			p1 = TR1(p1);
		if (OP(p1) == NAME && CLASS(p1) == REG)
			return 12;
	}
	return DEGREE(p) <= nrleft ? 20 : 24;
}

static int
xdcalc(int p, int nrleft)
{
	int d = dcalc(p, nrleft);

	if (d < 20 && TYPE(p) == CHAR) {
		if (nrleft >= 1)
			d = 20;
		else
			d = 24;
	}
	return d;
}

static int
notcompat(int p, int st)
{
	int at = TYPE(p);

	if ((at & 07) == STRUCT)
		at &= (int16_t)077770;
	if (st == 0)
		return at > 1 && at <= 07;
	if (st == 1)
		return at > 0 && at <= 07;
	st -= 2;
	if ((at & (int16_t)077740) != 0)
		at = 020;
	if ((at & (int16_t)077770) != 0)
		at = (at & 07) | 020;
	if (st == 2 && at == 3)
		at = 2;
	if (OP(p) == NAME && CLASS(p) == REG && at == INT && st == CHAR)
		return 0;
	return st != at;
}

static void
prins(int op, int c)
{
	const struct instab *insp;
	const char *ip;

	for (insp = instab; insp->op != 0; insp++) {
		if (insp->op == op) {
			ip = c ? insp->str2 : insp->str1;
			if (ip == 0)
				break;
			cprintf("%s", ip);
			return;
		}
	}
	error("No match' for op %d", op);
}

static int
collcon(int p)
{
	int op;

	if (OP(p) == PLUS) {
		op = OP(TR2(p));
		if (op == CON || op == AMPER || op == AMPERA)
			return 1;
	}
	return 0;
}

static int
isfloat(int t)
{
	if ((OPD(OP(t)) & RELAT) != 0)
		t = TR1(t);
	if (TYPE(t) == FLOAT || TYPE(t) == DOUBLE) {
		nfloat = 1;
		return 'f';
	}
	return 0;
}

static int
oddreg(int t, int areg)
{
	int reg = areg;

	if (!isfloat(t))
		switch (OP(t)) {
		case DIVIDE:
		case MOD:
		case ASDIV:
		case ASMOD:
			reg++;
		case TIMES:
		case ASTIMES:
			return reg | 1;
		}
	return reg;
}

static int
arlength(int t)
{
	if (t >= PTR)
		return 2;
	switch (t) {
	case INT:
	case CHAR:
		return 2;
	case FLOAT:
	case DOUBLE:
		return 8;
	}
	return 1024;
}

static int
ispow2(int tree)
{
	int d;

	if (!isfloat(tree) && OP(TR2(tree)) == CON) {
		d = VALUE(TR2(tree));
		if (d > 0 && (d & (d - 1)) == 0)
			return d;
	}
	return 0;
}

static void
pow2(int tree)
{
	int d, i;

	if ((d = ispow2(tree)) != 0) {
		for (i = 0; (d >>= 1) != 0; i++)
			;
		VALUE(TR2(tree)) = i;
		d = OP(tree);
		OP(tree) = d == TIMES ? LSHIFT :
		    (d == DIVIDE ? RSHIFT : (d == ASTIMES ? ASLSH : ASRSH));
	}
}

#define SWLAB(f)	ts[f]
#define SWVAL(f)	ts[(f) + 1]

static int
sort(int fp, int lp)
{
	int cp, intch, t;

	while (fp < (lp -= 2)) {
		intch = 0;
		for (cp = fp; cp < lp; cp += 2) {
			if (SWVAL(cp) == SWVAL(cp + 2)) {
				error("Duplicate case (%d)", SWVAL(cp));
				return 1;
			}
			if (SWVAL(cp) > SWVAL(cp + 2)) {
				intch++;
				t = SWVAL(cp);
				SWVAL(cp) = SWVAL(cp + 2);
				SWVAL(cp + 2) = t;
				t = SWLAB(cp);
				SWLAB(cp) = SWLAB(cp + 2);
				SWLAB(cp + 2) = t;
			}
		}
		if (intch == 0)
			break;
	}
	return 0;
}

static void
pswitch(int afp, int alp, int deflab)
{
	int tlab, ncase, i, j, tabs, worst, best, range;
	int swp, fp, lp;
	int poctab[200];

	fp = afp;
	lp = alp;
	if (fp == lp) {
		cprintf("jbr\tL%d\n", deflab);
		return;
	}
	tlab = isn++;
	(void)tlab;
	if (sort(fp, lp))
		return;
	ncase = (lp - fp) / 2;
	lp -= 2;
	range = (int16_t)(SWVAL(lp) - SWVAL(fp));
	if (range > 0 && range <= 3 * ncase) {
		if (SWVAL(fp))
			cprintf("sub\t$0%o,r0\n", SWVAL(fp));
		cprintf("cmp\tr0,$0%o\n", range);
		cprintf("jhi\tL%d\n", deflab);
		cprintf("asl\tr0\n");
		cprintf("jmp\t*.+4(r0)\n");
		for (i = SWVAL(fp); i <= SWVAL(lp); i++) {
			if (i == SWVAL(fp)) {
				cprintf("L%d\n", SWLAB(fp));
				fp += 2;
			} else
				cprintf("L%d\n", deflab);
		}
		goto esw;
	}
	if (ncase < 8) {
		i = isn++;
		j = isn++;
		cprintf("mov\t$L%d,r1\n", i);
		cprintf("mov\tr0,L%d\n", j);
		cprintf("L%d:cmp\tr0,(r1)+\n", isn);
		cprintf("jne\tL%d\n", isn++);
		cprintf("jmp\t*L%d-L%d(r1)\n", j, i);
		cprintf(".data\nL%d:", i);
		for (; fp <= lp; fp += 2)
			cprintf("%o\n", SWVAL(fp));
		cprintf("L%d:..\n", j);
		for (fp = afp; fp <= lp; fp += 2)
			cprintf("L%d\n", SWLAB(fp));
		cprintf("L%d\n.text\n", deflab);
		goto esw;
	}
	best = 077777;
	tabs = 0;
	for (i = ncase / 4; i <= ncase / 2; i++) {
		for (j = 0; j < i; j++)
			poctab[j] = 0;
		for (swp = fp; swp <= lp; swp += 2)
			poctab[(uint16_t)SWVAL(swp) % i]++;
		worst = 0;
		for (j = 0; j < i; j++)
			if (poctab[j] > worst)
				worst = poctab[j];
		if (i * worst < best) {
			tabs = i;
			best = i * worst;
		}
	}
	cprintf("jsr\tr2,hsw; %o; L%d\n", tabs, isn);
	cprintf("jmp\t*L%d-L%d(r1)\n", isn + tabs + 1, isn + 1);
	cprintf(".data\nL%d:", isn++);
	for (i = 0; i <= tabs; i++)
		cprintf("L%d\n", isn + i);
	for (i = 0; i < tabs; i++) {
		cprintf("L%d:..\n", isn++);
		for (swp = fp; swp <= lp; swp += 2)
			if ((uint16_t)SWVAL(swp) % tabs == (unsigned)i)
				cprintf("%o\n", (uint16_t)SWVAL(swp) / tabs);
	}
	cprintf("L%d:", isn++);
	for (i = 0; i < tabs; i++) {
		cprintf("L%d\n", deflab);
		for (swp = fp; swp <= lp; swp += 2)
			if ((uint16_t)SWVAL(swp) % tabs == (unsigned)i)
				cprintf("L%d\n", SWLAB(swp));
	}
	cprintf(".text\n");
esw:
	cprintf("/esw\n");
}

static void
branch(int lbl, int aop, int c)
{
	int op;

	if ((op = aop) != 0)
		prins(op, c);
	else
		cprintf("jbr");
	cprintf("\tL%d\n", lbl);
}

static void label(int l) { cprintf("L%d:", l); }

static void
popstk(int a)
{
	switch (a) {
	case 0:
		return;
	case 2:
		cprintf("tst\t(sp)+\n");
		return;
	case 4:
		cprintf("cmp\t(sp)+,(sp)+\n");
		return;
	}
	cprintf("add\t$0%o,sp\n", a);
}

static void
cbranch(int tree, int lbl, int cond, int reg)
{
	int l1, opfix, lb = 0;

	if (tree == 0)
		return;
	switch (OP(tree)) {
	case LOGAND:
		if (cond) {
			cbranch(TR1(tree), l1 = isn++, 0, reg);
			cbranch(TR2(tree), lbl, 1, reg);
			label(l1);
		} else {
			cbranch(TR1(tree), lbl, 0, reg);
			cbranch(TR2(tree), lbl, 0, reg);
		}
		return;
	case LOGOR:
		if (cond) {
			cbranch(TR1(tree), lbl, 1, reg);
			cbranch(TR2(tree), lbl, 1, reg);
		} else {
			cbranch(TR1(tree), l1 = isn++, 1, reg);
			cbranch(TR2(tree), lbl, 0, reg);
			label(l1);
		}
		return;
	case EXCLA:
		cbranch(TR1(tree), lbl, !cond, reg);
		return;
	case COMMA:
		rcexpr(TR1(tree), efftab, reg);
		tree = TR2(tree);
		break;
	}
	opfix = 0;
	if ((OPD(OP(tree)) & RELAT) == 0) {
		lb = scr;
		scr += 5;
		ts[lb] = NEQUAL;
		TYPE(lb) = TYPE(tree);
		DEGREE(lb) = DEGREE(tree);
		TR1(lb) = tree;
		TR2(lb) = CZERO;
		if (isfloat(tree))
			TR2(lb) = FCZERO;
		tree = lb;
	}
	if (OP(TR2(tree)) == CON && VALUE(TR2(tree)) == 0)
		opfix = 200;
	rcexpr(tree, cctab, reg);
	if (isfloat(tree))
		cprintf("cfcc\n");
	branch(lbl, OP(tree) + opfix, !cond);
	if (lb)
		scr = lb;
}

static const struct optab *
match(int tree, const struct table *table, int nrleft)
{
	int op, d1, d2 = 0, t1, t2, dope;
	int p1, p2 = 0;
	const struct optab *opt;

	if (tree == 0)
		return 0;
	if (table == lsptab)
		table = sptab;
	op = OP(tree);
	dope = OPD(op);
	if ((dope & LEAF) == 0)
		p1 = TR1(tree);
	else
		p1 = tree;
	t1 = TYPE(p1);
	(void)t1;
	d1 = dcalc(p1, nrleft);
	if ((dope & BINARY) != 0) {
		p2 = TR2(tree);
		t2 = TYPE(p2);
		(void)t2;
		d2 = dcalc(p2, nrleft);
	}
	if (op == 0)
		return 0;
	for (; table->op != op; table++)
		if (table->op == 0)
			return 0;
	for (opt = table->tabp; opt->tabdeg1 != 0; opt++) {
		if (d1 > (opt->tabdeg1 & 077)
		 || (opt->tabdeg1 >= 0100 && (OP(p1) != STAR)))
			continue;
		if (notcompat(p1, opt->tabtyp1))
			continue;
		if ((OPD(op) & BINARY) != 0 && p2 != 0) {
			if (d2 > (opt->tabdeg2 & 077)
			 || ((opt->tabdeg2 >= 0100) && (OP(p2) != STAR)))
				continue;
			if (notcompat(p2, opt->tabtyp2))
				continue;
		}
		return opt;
	}
	return 0;
}

static int rcexpr_(int, const struct table *, int);
static int rdepth;

static int
rcexpr(int tree, const struct table *table, int reg)
{
	int r;

	if (++rdepth > 400) {
		error("Expression too complex");
		exit(1);
	}
	r = rcexpr_(tree, table, reg);
	rdepth--;
	return r;
}

static int
rcexpr_(int tree, const struct table *table, int reg)
{
	int r, modf;
	struct cfv st = { 0, 0 };

	if (tree == 0)
		return 0;
	switch (OP(tree)) {
	case SETREG:
		nreg = TYPE(tree) - 1;
		return 0;

	case CBRANCH:
		cbranch(BTREE(tree), LBL(tree), COND(tree), 0);
		return 0;

	case INIT:
		if (OP(TR1(tree)) == AMPER)
			TR1(tree) = TR1(TR1(tree));
		if (OP(TR1(tree)) != NAME && OP(TR1(tree)) != CON)
			error("Illegal initialization");
		else
			cexpr(tree, regtab, nreg, &st);
		return 0;

	case EXCLA:
		if ((OPD(OP(TR1(tree))) & RELAT) != 0) {
			tree = TR1(tree);
			OP(tree) = notrel[OP(tree) - EQUAL];
		}
		break;

	case RFORCE:
		if ((r = rcexpr(TR1(tree), table, reg)) != 0)
			cprintf("mov%c\tr%d,r0\n", isfloat(TR1(tree)), r);
		return 0;

	case COMMA:
		rcexpr(TR1(tree), efftab, reg);
		tree = TR2(tree);
		break;

	case TIMES:
	case DIVIDE:
	case ASTIMES:
	case ASDIV:
		pow2(tree);
	}
	if ((r = cexpr(tree, table, reg, &st)) >= 0)
		return r;
	if (table != regtab)
		if ((r = cexpr(tree, regtab, reg, &st)) >= 0) {
			modf = isfloat(tree);
			if (table == sptab || table == lsptab) {
				cprintf("mov%c\tr%d,%c(sp)\n", modf, r,
				    table == sptab ? '-' : 0);
				nstack++;
			}
			if (table == cctab)
				cprintf("tst%c\tr%d\n", modf, r);
			return 0;
		}
	error("No match for op %d", OP(tree));
	return 0;
}

static int
comarg(int tree, int *flagp)
{
	if (TYPE(tree) == STRUCT)
		error("Illegal structure");
	if (nstack || isfloat(tree)) {
		rcexpr(tree, sptab, 0);
		return arlength(TYPE(tree));
	}
	(*flagp)++;
	rcexpr(tree, lsptab, 0);
	return 0;
}

static int
chkleaf(int tree, const struct table *table, int reg)
{
	int lb, r;
	struct cfv st = { 0, 0 };

	if (dcalc(tree, nreg - reg) > 12)
		return -1;
	lb = scr;
	scr += 5;
	ts[lb] = LOAD_;
	TYPE(lb) = TYPE(tree);
	DEGREE(lb) = DEGREE(tree);
	TR1(lb) = tree;
	r = cexpr(lb, table, reg, &st);
	scr = lb;
	return r;
}

static int
cexpr(int tree, const struct table *table, int areg, struct cfv *st)
{
	int c, r;
	int p, p1, p2;
	const struct table *ctable;
	const char *string;
	int reg, reg1, rreg, flag = 0;
	const struct optab *opt;

	reg = areg;
	p1 = TR2(tree);
	if ((c = OP(tree)) == CALL) {
		r = 0;
		st->nargs = 0;
		st->fflag = 0;
		if (OP(TR1(tree)) != NAME) {
			st->nargs++;
			nstack++;
		}
		if (OP(p1)) {
			while (OP(p1) == COMMA) {
				r += comarg(TR2(p1), &st->fflag);
				p1 = TR1(p1);
				st->nargs++;
			}
			r += comarg(p1, &st->fflag);
			st->nargs++;
		}
		OP(tree) = CALL + 1;
		DEGREE(tree) = r;
	}
	if (((OPD(c) & RELAT) || c == LOGAND || c == LOGOR) && table != cctab) {
		cbranch(tree, c = isn++, 1, reg);
		rcexpr(CZERO, table, reg);
		branch(isn, 0, 0);
		label(c);
		rcexpr(CONE, table, reg);
		label(isn++);
		return reg;
	}
	if (c == QUEST) {
		if (table == cctab)
			return -1;
		cbranch(TR1(tree), c = isn++, 0, reg);
		flag = nstack;
		rreg = rcexpr(TR1(p1), table, reg);
		nstack = flag;
		branch(r = isn++, 0, 0);
		label(c);
		reg = rcexpr(TR2(p1), table, rreg);
		if (rreg != reg)
			cprintf("mov%c\tr%d,r%d\n", isfloat(tree), reg, rreg);
		reg = rreg;
		label(r);
		goto retrn;
	}
	if (c == AMPER && OP(TR1(tree)) == NAME && CLASS(TR1(tree)) == REG)
		error("Illegal use of register");
	reg = oddreg(tree, reg);
	reg1 = reg + 1;
	if (chkleaf(tree, table, reg) >= 0)
		goto retrn;
	if ((opt = match(tree, table, nreg - reg)) == 0)
		return -1;
	string = opt->tabstring;
	p1 = TR1(tree);
	p2 = 0;
	if (OPD(OP(tree)) & BINARY)
		p2 = TR2(tree);
loop:
	switch (c = *string++) {

	case '\0':
		if (OP(tree) == CALL + 1) {
			popstk(DEGREE(tree));
			reg = 0;
			nstack -= st->nargs;
		}
retrn:
		if (!isfloat(tree))
			if (OP(tree) == DIVIDE || OP(tree) == ASDIV)
				reg--;
		return reg;

	case 'A':
		p = p1;
		goto adr;

	case 'B':
		p = p2;
		goto adr;

	case 'O':
		p = tree;
adr:
		c = 0;
		if (*string == '\'') {
			c++;
			string++;
		}
		pname(p, c);
		goto loop;

	case 'M':
		if ((c = *string) == '\'')
			string++;
		else
			c = 0;
		prins(OP(tree), c);
		goto loop;

	case 'C':
		if ((OPD(OP(tree)) & LEAF) != 0)
			p = tree;
		else
			p = p1;
		goto pbyte;

	case 'P':
		p = tree;
		goto pb1;

	case 'D':
		p = p2;
pbyte:
		if (TYPE(p) == CHAR)
			putchar1('b');
pb1:
		if (isfloat(p))
			putchar1('f');
		goto loop;

	case 'L':
		if (TYPE(p1) == CHAR || TYPE(p2) == CHAR)
			putchar1('b');
		p = tree;
		goto pb1;

	case 'E':
		p = TR1(p1);
		goto constp;

	case 'F':
		p = TR1(p2);
constp:
		cprintf("0%o", p);
		goto loop;

	case 'G':
		p = p1;
		flag = 01;
		goto subtre;

	case 'K':
		p = p2;
		flag = 02;
		goto subtre;

	case 'H':
		p = tree;
		flag = 04;

subtre:
		ctable = regtab;
		c = *string++ - 'A';
		if ((c & 02) != 0)
			ctable = sptab;
		if ((c & 04) != 0) {
			if (OP(p) != INCAFT && OP(p) != DECAFT
			 && match(p, efftab, nreg - reg))
				ctable = efftab;
			else
				ctable = cctab;
		}
		if ((c & 01) != 0) {
			p = TR1(p);
			if (collcon(p) && ctable != sptab) {
				if (OP(TR2(p)) == AMPERA)
					flag |= 010;
				p = TR1(p);
			}
		}
		if (table == lsptab && ctable == sptab)
			ctable = lsptab;
		if (c & 010)
			r = reg1;
		else if ((OPD(OP(p)) & LEAF) || DEGREE(p) < 2)
			r = reg;
		else
			r = areg;
		rreg = rcexpr(p, ctable, r);
		if (flag & 010)
			cprintf("add\tr5,r%d\n", rreg);
		if (c & 010)
			reg1 = rreg;
		else if (rreg != reg && ctable == regtab) {
			if (oddreg(tree, 0) == 0 && ((flag & 04) ||
			      ((flag & 01)
			  && xdcalc(p2, nreg - rreg - 1) <= (opt->tabdeg2 & 077)) ||
			      ((flag & 02)
			  && xdcalc(p1, nreg - rreg - 1) <= (opt->tabdeg1 & 077)))) {
				reg = rreg;
				reg1 = rreg + 1;
			} else
				cprintf("mov%c\tr%d,r%d\n", isfloat(tree), rreg, reg);
		}
		goto loop;

	case 'I':
		r = reg;
		if (*string == '-') {
			string++;
			r--;
		}
		goto preg;

	case 'J':
		r = reg1;
preg:
		if (r > nreg)
			error("Register overflow: simplify expression");
		cprintf("r%d", r);
		goto loop;

	case '-':
		if (*string == '(') {
			nstack++;
			if (table != lsptab)
				putchar1('-');
			goto loop;
		}
		break;

	case ')':
		putchar1(')');
		if (*string == '+')
			nstack--;
		goto loop;

	case '#':
		p = TR1(p1);
		goto nmbr;

	case '"':
		p = TR1(p2);
		goto nmbr;

	case '~':
		p = p1;

nmbr:
		if (collcon(p)) {
			switch (OP(p = TR2(p))) {
			case CON:
				if (VALUE(p))
					cprintf("%d.", VALUE(p));
				break;
			case AMPER:
				pname(TR1(p), 0);
				break;
			case AMPERA:
				p = TR1(p);
				cprintf("%d.", NLOC(p) + OFFSET(p));
				break;
			}
		}
		goto loop;

	case 'V':
		OP(tree) = maprel[OP(tree) - EQUAL];
		goto loop;

	case 'Z':
		cprintf("$0%o", OFFSET(p1) + NLOC(p1));
		goto loop;

	case '^':
		cprintf("0%o", TR2(tree));
		goto loop;

	case 'T':
		if (dcalc(p1, 5) > 12 && !match(p1, cctab, 10))
			cprintf("tst\tr%d\n", reg);
		goto loop;

	case '`':
		if (st->fflag)
			cprintf("*$");
		goto loop;
	}
	putchar1(c);
	goto loop;
}

struct acl {
	int nextl;
	int nextn;
	int nlist[20];
	int llist[21];
};

static void
constfold(int op, int node, int v)
{
	int a = VALUE(node);

	switch (op) {
	case PLUS:
		a += v;
		break;
	case TIMES:
		a *= v;
		break;
	case AND:
		a &= v;
		break;
	case OR:
		a |= v;
		break;
	case EXOR:
		a ^= v;
		break;
	case DIVIDE:
	case MOD:
		if (v == 0)
			error("Divide check");
		else if (op == DIVIDE)
			a = div16(a, v);
		else
			a = mod16(a, v);
		break;
	case RSHIFT:
		a = ash16(a, -v);
		break;
	case LSHIFT:
		a = ash16(a, v);
		break;
	default:
		error("C error: const");
		return;
	}
	VALUE(node) = (word)a;
}

static void
squash(struct acl *l, int p, int maxp)
{
	int np;

	for (np = p; np < maxp; np++)
		l->llist[np] = l->llist[np + 1];
}

static void
distrib(struct acl *list)
{
	int p1, p2, t, ndmaj, ndmin, dividend, divisor, maxnod, mindiv = 0;
	int *LL = list->llist;

loop:
	maxnod = list->nextl;
	ndmaj = 1000;
	dividend = -1;
	divisor = -1;
	for (p1 = 0; p1 <= maxnod; p1++) {
		if (OP(LL[p1]) != TIMES || OP(TR2(LL[p1])) != CON)
			continue;
		ndmin = 0;
		for (p2 = 0; p2 <= maxnod; p2++) {
			if (p1 == p2 || OP(LL[p2]) != TIMES || OP(TR2(LL[p2])) != CON)
				continue;
			if (VALUE(TR2(LL[p1])) == VALUE(TR2(LL[p2]))) {
				TR2(LL[p2]) = TR1(LL[p1]);
				OP(LL[p2]) = PLUS;
				TR1(LL[p1]) = LL[p2];
				LL[p1] = optim(LL[p1]);
				squash(list, p2, maxnod);
				list->nextl--;
				goto loop;
			}
			if (mod16(VALUE(TR2(LL[p2])), VALUE(TR2(LL[p1]))) == 0)
				goto contmaj;
			if (mod16(VALUE(TR2(LL[p1])), VALUE(TR2(LL[p2]))) == 0) {
				ndmin++;
				mindiv = p2;
			}
		}
		if (ndmin > 0 && ndmin < ndmaj) {
			ndmaj = ndmin;
			dividend = p1;
			divisor = mindiv;
		}
contmaj:;
	}
	if (dividend < 0)
		return;
	t = list->nlist[--list->nextn];
	p1 = dividend;
	p2 = divisor;
	OP(t) = PLUS;
	TYPE(t) = TYPE(LL[p1]);
	TR1(t) = LL[p1];
	TR2(t) = TR1(LL[p2]);
	VALUE(TR2(LL[p1])) = (word)div16(VALUE(TR2(LL[p1])), VALUE(TR2(LL[p2])));
	TR1(LL[p2]) = t;
	t = optim(LL[p2]);
	if (p1 < p2) {
		LL[p1] = t;
		squash(list, p2, maxnod);
		list->nextl--;
		goto loop;
	}
	LL[p2] = t;
	squash(list, p1, maxnod);
	list->nextl--;
	goto loop;
}

static void
insert(int op, int tree, struct acl *list)
{
	int d, d1, i, t;

	if (OP(tree) == op) {
ins:
		if (list->nextn >= 20) {
			error("Expression too complex");
			exit(1);
		}
		list->nlist[list->nextn++] = tree;
		insert(op, TR1(tree), list);
		insert(op, TR2(tree), list);
		return;
	}
	tree = optim(tree);
	if (OP(tree) == op)
		goto ins;
	if (!isfloat(tree)) {
		if ((OP(tree) == TIMES || OP(tree) == LSHIFT) && OP(TR2(tree)) == CON
		  && OP(TR1(tree)) == PLUS && OP(TR2(TR1(tree))) == CON) {
			d = VALUE(TR2(tree));
			if (OP(tree) == TIMES)
				VALUE(TR2(tree)) = (word)(VALUE(TR2(tree)) * VALUE(TR2(TR1(tree))));
			else
				VALUE(TR2(tree)) = (word)ash16(VALUE(TR2(TR1(tree))), d);
			VALUE(TR2(TR1(tree))) = (word)d;
			OP(TR1(tree)) = OP(tree);
			OP(tree) = PLUS;
			if (op == PLUS)
				goto ins;
		}
	}
	d = degree(tree);
	for (i = 0; i < list->nextl; i++) {
		if ((d1 = degree(list->llist[i])) < d) {
			t = list->llist[i];
			list->llist[i] = tree;
			tree = t;
			d = d1;
		}
	}
	if (list->nextl >= 20) {
		error("Expression too complex");
		exit(1);
	}
	list->llist[list->nextl++] = tree;
}

static int
acommute(int tree)
{
	struct acl acl;
	int d, i, op, flt, t, t1, t2 = 0;

	acl.nextl = 0;
	acl.nextn = 0;
	op = OP(tree);
	flt = isfloat(tree);
	insert(op, tree, &acl);
	acl.nextl--;
	if (!flt) {
		t2 = acl.nextl;
		for (i = acl.nextl; i > 0 && OP(acl.llist[t2]) == CON
		     && OP(acl.llist[t2 - 1]) == CON; i--) {
			acl.nextl--;
			t2--;
			constfold(op, acl.llist[t2], VALUE(acl.llist[t2 + 1]));
		}
	}
	if (op == PLUS && !flt) {
		if (acl.nextl > 0 && OP(acl.llist[t2]) == CON && VALUE(acl.llist[t2]) == 0) {
			acl.nextl--;
			t2--;
		}
		if (acl.nextl <= 0)
			return acl.llist[t2];
		if (OP(acl.llist[t2]) == CON &&
		   (OP(acl.llist[t2 - 1]) == AMPER || OP(acl.llist[t2 - 1]) == AMPERA)) {
			t2--;
			OFFSET(TR1(acl.llist[t2])) += VALUE(acl.llist[t2 + 1]);
			acl.nextl--;
		}
	} else if (op == TIMES) {
		t1 = acl.llist[acl.nextl];
		if (OP(t1) == CON && VALUE(t1) == 0)
			return t1;
	}
	if (op == PLUS && !flt)
		distrib(&acl);
	t2 = 0;
	tree = acl.llist[t2];
	d = max_(degree(tree), 1);
	if (op == TIMES && !flt)
		d++;
	for (i = 0; i < acl.nextl; i++) {
		t1 = acl.nlist[i];
		t = acl.llist[++t2];
		TR2(t1) = t;
		DEGREE(t1) = d = degree(t) >= d ? d + 1 : d;
		TR1(t1) = tree;
		tree = t1;
	}
	if (OP(tree) == TIMES && ispow2(tree))
		DEGREE(tree) = max_(degree(TR1(tree)), 1);
	return tree;
}

static int
unoptim(int tree)
{
	int subtre;
	uint16_t w[4];

	if (tree == 0)
		return 0;
	if (OP(tree) == CBRANCH) {
		BTREE(tree) = optim(BTREE(tree));
		return tree;
	}
	subtre = TR1(tree) = optim(TR1(tree));
	if (OP(tree) == STAR && (OP(subtre) == AMPER || OP(subtre) == AMPERA))
		return TR1(subtre);
	if (OP(tree) == ITOF && OP(subtre) == CON) {
		pdpwords((double)VALUE(subtre), w, 1);
		if (w[1] == 0 && w[2] == 0 && w[3] == 0) {
			TYPE(subtre) = DOUBLE;
			VALUE(subtre) = (word)w[0];
			OP(subtre) = SFCON;
			return subtre;
		}
	}
	if (OP(subtre) == CON)
		switch (OP(tree)) {
		case NEG:
			VALUE(subtre) = (word)-VALUE(subtre);
			return subtre;
		case COMPL:
			VALUE(subtre) = (word)~VALUE(subtre);
			return subtre;
		}
	DEGREE(tree) = max_(1, degree(subtre));
	return tree;
}

static int
optim(int tree)
{
	int op, dope, d1, d2, t;

	if (tree == 0)
		return 0;
	op = OP(tree);
	if (op == 0)
		return tree;
	dope = OPD(op);
	if ((dope & LEAF) != 0)
		return tree;
	if ((dope & BINARY) == 0)
		return unoptim(tree);
	if ((dope & COMMUTE) != 0) {
acomm:
		d1 = TYPE(tree);
		tree = acommute(tree);
		TYPE(tree) = d1;
		return tree;
	}
	TR1(tree) = optim(TR1(tree));
	TR2(tree) = optim(TR2(tree));
	if ((dope & RELAT) != 0) {
		if (degree(TR1(tree)) < degree(TR2(tree))) {
			t = TR1(tree);
			TR1(tree) = TR2(tree);
			TR2(tree) = t;
			OP(tree) = maprel[op - EQUAL];
		}
		if (TYPE(TR1(tree)) == CHAR && OP(TR2(tree)) == CON
		 && dcalc(TR1(tree), 0) <= 12
		 && VALUE(TR2(tree)) <= 127 && VALUE(TR2(tree)) >= 0)
			TYPE(TR2(tree)) = CHAR;
	}
	d1 = max_(degree(TR1(tree)), 1);
	d2 = max_(degree(TR2(tree)), 0);
	switch (op) {
	case CALL:
		DEGREE(tree) = 10;
		break;

	case QUEST:
	case COLON:
		DEGREE(tree) = max_(d1, d2);
		break;

	case MINUS:
		if (OP(TR2(tree)) == CON) {
			OP(tree) = PLUS;
			VALUE(TR2(tree)) = (word)-VALUE(TR2(tree));
			goto acomm;
		}
		goto def;

	case DIVIDE:
	case ASDIV:
	case ASTIMES:
		if (ispow2(tree) == 0) {
	case MOD:
	case ASMOD:
			d1 += 2;
			d2 += 2;
		}
	case LSHIFT:
	case RSHIFT:
		if (OP(TR1(tree)) == CON && OP(TR2(tree)) == CON) {
			constfold(op, TR1(tree), VALUE(TR2(tree)));
			return TR1(tree);
		}
	def:
	default:
		DEGREE(tree) = d1 == d2 ? ++d1 : max_(d1, d2);
		break;
	}
	return tree;
}
