/*
 * Copyright 1972, 1973 Bell Telephone Laboratories, Inc.
 */
#include "cc0.h"
#include <math.h>

#define HB	8
static word ht[HB + HSHSIZ * 8];
#define HCLASS(p)	ht[(p) + 0]
#define HTYPE(p)	ht[(p) + 1]
#define HDIMP(p)	ht[(p) + 2]
#define HOFFSET(p)	ht[(p) + 3]
#define HNAME(p)	((char *)&ht[(p) + 4])
#define HSSP(p)		((int)((uint16_t)ht[(p) + 2] & 0377))
#define HLENP(p)	((int)(((uint16_t)ht[(p) + 2] >> 8) & 0377))
#define SETSSP(a, v)	(a) = (word)(((uint16_t)(a) & ~0377) | ((v) & 0377))
#define SETLENP(a, v)	(a) = (word)(((uint16_t)(a) & 0377) | (((v) & 0377) << 8))
#define HENT(i)		(HB + 8 * (i))

static int isn = 1, peeksym = -1, peekc, eof, dimp;
static int space, osleft, treebase = TB;
static unsigned char nsz[OSSIZ + TB + 8];
static int lastnode;
static word cmst[CMSIZ];
static int cp;
static int contlab, brklab, retlab, deflab, autolen, mosflg, initflg, inhdr;
static int defsym, funcsym, xdflg, proflg, csym, strflg, regvar, nchstr;
static int paraml, parame;
static word cval;
static double fcval;
static int hshused;
static word dimtab[DIMSIZ];
static int swtab[2 * SWSIZ];
static int swp;
static char symbuf[ncps];

static FILE *fin;

static int
getchar0(void)
{
	int c = fgetc(fin);

	return c == EOF ? 0 : c;
}

#undef getchar
#define getchar()	getchar0()
static void putchar0(int c) { fputc(c, ascf); }

static int
cls(int c)
{
	return (c >= 0 && c < 128) ? ctab[c] : UNKN;
}

static int symbol(void);
static int tree(void);
static void extdef(void), blkend(void), errflush(int);
static int blkhed(void);
static int declist(int);
static int strdec(int *, int);
static void statement(int);
static int declare(int, int, int, int);
static int decl1(int, int, int, int);
static void rcexpr(int, int);
static void build(int);
static int block(int, int, int, int, int, int, int);
static int fold(int, int, int);
static int pop(void);

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

static void
pblock(int w)
{
	if (lastnode >= 0)
		nsz[lastnode]++;
	ts[space++] = w;
	if (--osleft <= 0) {
		error("Expression overflow");
		exit(1);
	}
}

static int
block(int n, int op, int t, int d, int p1, int p2, int p3)
{
	int a[6], i, p = space;

	a[0] = op; a[1] = t; a[2] = d; a[3] = p1; a[4] = p2; a[5] = p3;
	lastnode = -1;
	for (i = 0; i < n + 3; i++)
		pblock(i < 6 ? a[i] : 0);
	nsz[p] = n + 3;
	lastnode = p;
	return p;
}

static int
decref(int t)
{
	if ((t & ~07) == 0) {
		error("Illegal indirection");
		return t;
	}
	return (int16_t)(((t >> 2) & ~07) | (t & 07));
}

static int
incref(int t)
{
	return (int16_t)(((t << 2) & ~034) | (t & 07) | PTR);
}

static int
lengthx(int t, int ssp, int lenp)
{
	int n = 1;

	while ((t & 030) == ARRAY) {
		t = decref(t);
		n = dimtab[ssp];
	}
	if ((t & ~07) == FUNC)
		return 0;
	if (t >= PTR)
		return 2 * n;
	switch (t & 07) {
	case INT:
		return 2 * n;
	case CHAR:
		return n;
	case FLOAT:
		return 4 * n;
	case DOUBLE:
		return 8 * n;
	case STRUCT:
		return n * dimtab[lenp];
	case RSTRUCT:
		error("Bad structure");
		return 0;
	}
	error("Compiler error (length)");
	return 0;
}

#define lengtht(p)	lengthx(TYPE(p), SSP(p), LENP(p))
#define lengthh(p)	lengthx(HTYPE(p), HSSP(p), HLENP(p))

static int rl(int l) { return (l & 01) ? l + 1 : l; }

static int
plength(int p)
{
	int t, l;

	if (((t = TYPE(p)) & ~07) == 0)	/* is not reference */
		return 1;
	TYPE(p) = decref(t);
	l = lengtht(p);
	TYPE(p) = t;
	return l;
}

static int
subseq(int c, int a, int b)
{
	if (!peekc)
		peekc = getchar();
	if (peekc != c)
		return a;
	peekc = 0;
	return b;
}

static int
mapch(int ac)
{
	int a, c, n;
	static int mpeek;

	c = ac;
	if (mpeek) {
		a = mpeek;
		mpeek = 0;
	} else
		a = getchar();
loop:
	if (a == c)
		return -1;
	switch (a) {
	case '\n':
	case '\0':
		error("Nonterminated string");
		peekc = a;
		return -1;
	case '\\':
		switch (a = getchar()) {
		case 't':
			return '\t';
		case 'n':
			return '\n';
		case 'b':
			return '\b';
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			n = 0;
			while (n < 0100 && '0' <= a && a <= '7') {
				n <<= 3;
				n += a - '0';
				a = getchar();
			}
			mpeek = a;
			return n;
		case 'r':
			return '\r';
		case '\n':
			if (!inhdr)
				line++;
			inhdr = 0;
			a = getchar();
			goto loop;
		}
	}
	return a;
}

static int
getstr(void)
{
	int c;
	const char *t = ".text", *d = ".data";

	nchstr = 1;
	cval = isn++;
	cprintf("%s\nL%d:.byte ", strflg ? t : d, cval);
	while ((c = mapch('"')) >= 0) {
		cprintf("0%o,", c);
		nchstr++;
	}
	cprintf("0\n.even\n%s\n", strflg ? d : t);
	return STRING;
}

static int
getcc(void)
{
	int c, cc = 0;
	uint16_t v = 0;

	while ((c = mapch('\'')) >= 0)
		if (cc++ < NCPW) /* chars have low byte first*/
			v |= (c & 0377) << (8 * (cc - 1));
	cval = (word)v;
	if (cc > NCPW)
		error("Long character constant");
	return CON;
}

static int
getnum(int base)
{
	int nfract = 0, totdig = 0, decpt = 0, c, d;
	double fr0 = 0;

	cval = 0;
	for (;;) {
		for (;;) {
			if (peekc) {
				c = peekc;
				peekc = 0;
			} else
				c = getchar();
			d = c - '0';
			if ((unsigned)d > 9)
				break;
			totdig++;
			cval = (word)(cval * base + d);
			fr0 = fr0 * 10 + d;
			nfract++;
		}
		if (decpt == 0) {
			nfract = 0;
			if (c == '.') {
				decpt = 1;
				continue;
			}
		}
		break;
	}
	if (totdig != 0 && c == 'e') {
		int neg = 0;

		cval = 0;
		decpt = 1;
		base = 10;
		c = getchar();
		if (c == '+')
			;
		else if (c == '-')
			neg = 1;
		else
			peekc = c;
		for (;;) {
			if (peekc) {
				c = peekc;
				peekc = 0;
			} else
				c = getchar();
			d = c - '0';
			if ((unsigned)d > 9)
				break;
			totdig++;
			cval = (word)(cval * base + d);
		}
		if (neg)
			cval = (word)-cval;
		nfract -= cval;
	}
	peekc = c;
	if (totdig == 0)
		return DOT;
	if (decpt == 0)
		return CON;
	{
		double fr2 = 1;
		int n = nfract, k;
		uint16_t w[4];

		if (n != 0) {
			k = n < 0 ? -n : n;
			while (k-- > 0)
				fr2 *= 10;
		}
		if (n > 0)
			fr0 /= fr2;
		else
			fr0 *= fr2;
		fcval = fr0;
		pdpwords(fr0, w, 1);
		if (w[1] == 0 && w[2] == 0 && w[3] == 0) {
			cval = (word)w[0];
			return SFCON;
		}
		return FCON;
	}
}

static int
lookup(void)
{
	int ihash = 0, rp, i, k;
	const char *np;

	for (i = 0; i < ncps; i++)
		ihash += symbuf[i] & 0177;
	rp = HENT(ihash % HSHSIZ);
	while (*(np = HNAME(rp)) != '\0') {
		for (i = 0; i < ncps; i++)
			if ((np[i] & 0177) != symbuf[i])
				goto no;
		return rp;
no:
		rp += 8;
		if (rp >= HENT(HSHSIZ))
			rp = HENT(0);
	}
	if (++hshused >= HSHSIZ) {
		error("Symbol table overflow");
		exit(1);
	}
	HCLASS(rp) = 0;
	HTYPE(rp) = 0;
	HOFFSET(rp) = 0;
	HDIMP(rp) = 0;
	for (k = 0; k < ncps; k++)
		HNAME(rp)[k] = symbuf[k];
	if (xdflg)
		HNAME(rp)[0] |= 0200;
	return rp;
}

static int
symbol(void)
{
	int c;
	char *sp;

	if (peeksym >= 0) {
		c = peeksym;
		peeksym = -1;
		if (c == NAME)
			mosflg = 0;
		return c;
	}
	if (peekc) {
		c = peekc;
		peekc = 0;
	} else if (eof)
		return EOFC;
	else
		c = getchar();
loop:
	switch (cls(c)) {
	case INSERT:
		inhdr = 1;
		c = getchar();
		goto loop;
	case NEWLN:
		if (!inhdr)
			line++;
		inhdr = 0;
	case SPACE:
		c = getchar();
		goto loop;
	case EOFC:
		eof++;
		return 0;
	case PLUS:
		return subseq(c, PLUS, INCBEF);
	case MINUS: {
		int a = subseq('>', MINUS, ARROW);

		return subseq(c, a, DECBEF);
	}
	case ASSIGN:
		if (subseq(' ', 0, 1))
			return ASSIGN;
		c = symbol();
		if (c >= PLUS && c <= EXOR) {
			if (peekc == 0)
				peekc = getchar();
			if (cls(peekc) != SPACE && (c == MINUS || c == AND || c == TIMES)) {
				error("Warning: assignment understood");
				nerror--;
			}
			return c + 30;
		}
		if (c == ASSIGN)
			return EQUAL;
		peeksym = c;
		return ASSIGN;
	case LESS:
		if (subseq(c, 0, 1))
			return LSHIFT;
		return subseq('=', LESS, LESSEQ);
	case GREAT:
		if (subseq(c, 0, 1))
			return RSHIFT;
		return subseq('=', GREAT, GREATEQ);
	case EXCLA:
		return subseq('=', EXCLA, NEQUAL);
	case DIVIDE:
		if (subseq('*', 1, 0))
			return DIVIDE;
com:
		c = getchar();
com1:
		switch (c) {
		case '\0':
			eof++;
			error("Nonterminated comment");
			return 0;
		case '\n':
			if (!inhdr)
				line++;
			inhdr = 0;
			goto com;
		case 001:
			inhdr++;
			goto com;
		default:
			goto com;
		case '*':
			c = getchar();
			if (c != '/')
				goto com1;
		}
		c = getchar();
		goto loop;
	case PERIOD:
	case DIGIT:
		peekc = c;
		if ((c = getnum(c == '0' ? 8 : 10)) == FCON)
			cval = isn++;
		return c;
	case DQUOTE:
		return getstr();
	case SQUOTE:
		return getcc();
	case LETTER:
		sp = symbuf;
		if (mosflg) {
			*sp++ = '.';
			mosflg = 0;
		}
		while (cls(c) == LETTER || cls(c) == DIGIT) {
			if (sp < symbuf + ncps)
				*sp++ = c;
			c = getchar();
		}
		while (sp < symbuf + ncps)
			*sp++ = '\0';
		peekc = c;
		csym = lookup();
		if (HCLASS(csym) == KEYWC) {
			if (HTYPE(csym) == SIZEOF)
				return SIZEOF;
			cval = HTYPE(csym);
			return KEYW;
		}
		return NAME;
	case AND:
		return subseq('&', AND, LOGAND);
	case OR:
		return subseq('|', OR, LOGOR);
	case UNKN:
		error("Unknown character");
		c = getchar();
		goto loop;
	}
	return cls(c);
}

static int
nextchar(void)
{
	while (cls(peekc) == SPACE)
		peekc = getchar();
	return peekc;
}

static int
pop(void)
{
	if (cp <= 0) {
		error("Expression syntax");
		return 0;
	}
	return cmst[--cp];
}

static int
lintyp(int t)
{
	if (t < PTR)
		return t;
	if ((t & 0170) == 0150)
		return 9;
	if ((t & 037) == t)
		return (t & 07) + 5;
	return 10;
}

static void
chkw(int p)
{
	int t;

	if ((t = TYPE(p)) > CHAR && t < PTR)
		error("Integer operand required");
}

static void
chklval(int p)
{
	if (OP(p) != NAME && OP(p) != STAR)
		error("Lvalue required");
}

static void
setype(int p, int t, int dimptr)
{
	TYPE(p) = t;
	if (dimptr != -1)
		DIMP(p) = dimptr;
	switch (OP(p)) {
	case AMPERA:
	case AMPER:
		setype(TR1(p), decref(t), dimptr);
		return;
	case STAR:
		setype(TR1(p), incref(t), dimptr);
		return;
	case PLUS:
	case MINUS:
		setype(TR1(p), t, dimptr);
	}
}

static int
chkfun(int p)
{
	int t;

	if (((t = TYPE(p)) & 030) == FUNC)
		return block(1, AMPER, incref(t), DIMP(p), p, 0, 0);
	return p;
}

static int
disarray(int p)
{
	int t;

	if (((t = TYPE(p)) & 030) != ARRAY || (OP(p) == NAME && CLASS(p) == MOS))
		return p;
	SETSSP(DIMP(p), SSP(p) + 1);
	cmst[cp++] = p;
	setype(p, decref(t), -1);
	build(AMPER);
	return pop();
}

static int
convert(int p, int t, int cvn, int len)
{
	int n;

	switch (cvn) {
	case 1:
		n = TIMES;
		goto md;
	case 2:
		n = DIVIDE;
md:
		return block(2, n, t, 0, p, block(1, CON, 0, 0, len, 0, 0), 0);
	case 3:
		n = ITOF;
		goto di;
	case 4:
		n = FTOI;
di:
		return block(1, n, t, 0, p, 0, 0);
	}
	error("C error-- convert");
	return p;
}

static int
fold(int op, int p1, int p2)
{
	int v1, v2;

	if (OP(p1) != CON || (p2 != 0 && OP(p2) != CON))
		return 0;
	v1 = VALUE(p1);
	v2 = p2 ? VALUE(p2) : 0;
	switch (op) {
	case PLUS:
		v1 += v2;
		break;
	case MINUS:
		v1 -= v2;
		break;
	case TIMES:
		v1 *= v2;
		break;
	case DIVIDE:
		v1 = v2 == 0 ? (v1 < 0 ? -1 : 0) : v1 / v2;
		break;
	case MOD:
		v1 = v2 == 0 ? v1 : v1 % v2;
		break;
	case AND:
		v1 &= v2;
		break;
	case OR:
		v1 |= v2;
		break;
	case EXOR:
		v1 ^= v2;
		break;
	case NEG:
		v1 = -v1;
		break;
	case COMPL:
		v1 = ~v1;
		break;
	case LSHIFT:
		v1 = ash16(v1, v2);
		break;
	case RSHIFT:
		v1 = ash16(v1, -v2);
		break;
	default:
		return 0;
	}
	VALUE(p1) = v1;
	cmst[cp++] = p1;
	return 1;
}

static void
build(int op)
{
	int t1, t2 = 0, t, dope, leftc, cvn, pcvn;
	int p1, p2 = 0;

	if (op == LBRACK) {
		build(PLUS);
		op = STAR;
	}
	dope = OPD(op);
	if ((dope & BINARY) != 0) {
		p2 = chkfun(disarray(pop()));
		t2 = TYPE(p2);
	}
	p1 = pop();
	if (op == SIZEOF) {
		t1 = lengtht(p1);
		OP(p1) = CON;
		TYPE(p1) = INT;
		DIMP(p1) = 0;
		VALUE(p1) = t1;
		cmst[cp++] = p1;
		return;
	}
	if (op != AMPER) {
		p1 = disarray(p1);
		if (op != CALL)
			p1 = chkfun(p1);
	}
	t1 = TYPE(p1);
	pcvn = 0;
	switch (op) {
	case 0:
		cmst[cp++] = p1;
		return;
	case COMMA:
	case LOGAND:
	case LOGOR:
		cmst[cp++] = block(2, op, 0, 0, p1, p2, 0);
		return;
	case QUEST:
		if (OP(p2) != COLON)
			error("Illegal conditional");
		t = t2;
		goto nocv;
	case CALL:
		if ((t1 & 030) != FUNC)
			error("Call of non-function");
		cmst[cp++] = block(2, CALL, decref(t1), DIMP(p1), p1, p2, 0);
		return;
	case STAR:
		if (OP(p1) == AMPER || OP(p1) == AMPERA) {
			cmst[cp++] = TR1(p1);
			return;
		}
		if ((t1 & 030) == FUNC)
			error("Illegal indirection");
		cmst[cp++] = block(1, STAR, decref(t1), DIMP(p1), p1, 0, 0);
		return;
	case AMPER:
		if (OP(p1) == STAR) {
			DIMP(TR1(p1)) = DIMP(p1);
			TYPE(TR1(p1)) = incref(t1);
			cmst[cp++] = TR1(p1);
			return;
		}
		if (OP(p1) == NAME) {
			if (CLASS(p1) == AUTO)
				op = AMPERA;
			cmst[cp++] = block(1, op, incref(t1), DIMP(p1), p1, 0, 0);
			return;
		}
		error("Illegal lvalue");
		break;
	case INCBEF:
	case DECBEF:
	case INCAFT:
	case DECAFT:
		chklval(p1);
		cmst[cp++] = block(2, op, t1, DIMP(p1), p1, plength(p1), 0);
		return;
	case ARROW:
		cmst[cp++] = p1;
		chkw(p1);
		TYPE(p1) = PTR + STRUCT;
		build(STAR);
		p1 = pop();
	case DOT:
		if (OP(p2) != NAME || CLASS(p2) != MOS)
			error("Illegal structure ref");
		cmst[cp++] = p1;
		t = t2;
		if ((t & 030) == ARRAY) {
			t = decref(t);
			SETSSP(DIMP(p2), SSP(p2) + 1);
		}
		setype(p1, t, DIMP(p2));
		build(AMPER);
		cmst[cp++] = block(1, CON, 7, 0, NLOC(p2), 0, 0);
		build(PLUS);
		if ((t2 & 030) != ARRAY)
			build(STAR);
		return;
	}
	if ((dope & LVALUE) != 0)
		chklval(p1);
	if ((dope & LWORD) != 0)
		chkw(p1);
	if ((dope & RWORD) != 0)
		chkw(p2);
	if ((dope & BINARY) == 0) {
		if (!fold(op, p1, 0))
			cmst[cp++] = block(1, op, t1, DIMP(p1), p1, 0, 0);
		return;
	}
	if (t2 == 7) {
		t = t1;
		TYPE(p2) = 0;
		t2 = 0;
		goto nocv;
	}
	cvn = cvtab[11 * lintyp(t1) + lintyp(t2)];
	leftc = cvn & 0100;
	t = leftc ? t2 : t1;
	if (op == ASSIGN && t1 != STRUCT && t2 != STRUCT) {
		t = t1;
		if (leftc || cvn != 3)
			goto nocv;
	}
	if ((cvn &= 077) != 0) {
		if (op == COLON && t1 >= PTR && t1 == t2)
			goto nocv;
		if (cvn == 077) {
			if ((dope & RELAT) == 0 || t1 < PTR || t2 < PTR)
illcv:
				error("Illegal conversion");
			goto nocv;
		}
		if (cvn == 2) {
			t = 0;
			if ((dope & RELAT) != 0) {
				cvn = 0;
				goto nocv;
			}
			if (op != MINUS)
				goto illcv;
			pcvn = cvn;
			cvn = 0;
			goto nocv;
		}
		if (leftc) {
			if ((dope & ASSGOP) != 0) {
				if (cvn == 3) {
					leftc = 0;
					cvn = 4;
					t = t1;
					goto rcvt;
				} else
					goto illcv;
			}
			p1 = convert(p1, t, cvn, plength(p2));
		} else {
rcvt:
			p2 = convert(p2, t, cvn, plength(p1));
		}
nocv:;
	}
	if ((dope & RELAT) != 0) {
		if (op > NEQUAL && (t1 >= PTR || t2 >= PTR))
			op += 4;
		t = 0;
	}
	if (fold(op, p1, p2))
		return;
	cmst[cp++] = block(2, op, t, DIMP(DIMP(p1) == 0 ? p2 : p1), p1, p2, 0);
	if (pcvn) {
		p1 = pop();
		cmst[cp++] = convert(p1, 0, pcvn, plength(TR1(p1)));
	}
}

static void
rcexpr(int tr, int table)
{
	int c, sp, i, n, pm, k;
	word buf[OSSIZ + 8];

	if (tr == 0)
		return;
	putchar0('#');
	c = space - treebase;
#define ADDR(i)	((i) == 0 ? 0 : TREEBASE_ADDR + 2 * ((i) - TB))
	for (i = 0; i < c; ) {
		n = nsz[treebase + i];
		if (n == 0)
			n = 1;
		pm = ptrmask(ts[treebase + i]);
		for (k = 0; k < n && i + k < c; k++) {
			word w = ts[treebase + i + k];

			buf[i + k] = (pm & (1 << k)) ? (word)ADDR(w) : w;
		}
		i += n;
	}
	putw16(c, binf);
	putw16(ADDR(tr), binf);
	putw16(table, binf);
	putw16(line, binf);
	for (sp = 0; sp < c; sp++)
		putw16(buf[sp], binf);
}

static void branch(int lab) { cprintf("jbr\tL%d\n", lab); }
static void label(int l) { cprintf("L%d:", l); }
static void cbranch(int tr, int lbl, int cond) { rcexpr(block(1, CBRANCH, tr, lbl, cond, 0, 0), TAB_CC); }

static int
tree(void)
{
#define SEOF	200
#define SSIZE	20
	int opst[SSIZE + 1], prst[SSIZE + 1];
	int op, pp, andflg, o, cs, p, ps, os, np;

	osleft = OSSIZ;
	space = treebase;
	op = 0;
	pp = 0;
	cp = 0;
	opst[op] = SEOF;
	prst[pp] = 06;
	andflg = 0;

advanc:
	switch (o = symbol()) {
	case NAME:
		cs = csym;
		if (HCLASS(cs) == 0 && HTYPE(cs) == 0) {
			if (nextchar() == '(') {
				HCLASS(cs) = EXTERN;
				HTYPE(cs) = FUNC;
			} else if (initflg)
				HCLASS(cs) = EXTERN;
			else {
				HTYPE(cs) = ARRAY;
				if (HOFFSET(cs) == 0)
					HOFFSET(cs) = isn++;
			}
		}
		cmst[cp++] = block(2, NAME, HTYPE(cs), HDIMP(cs), HCLASS(cs), 0, 0);
		if (HCLASS(cs) == EXTERN) {
			for (o = 0; o < 4; o++) {
				pblock(ht[cs + 4 + o]);
				if ((ht[cs + 4 + o] & ~0177) == 0)
					break;
			}
		} else
			pblock(HOFFSET(cs));
		goto tand;

	case FCON:
		if (!initflg) {
			uint16_t w[4];

			pdpwords(fcval, w, 1);
			cprintf(".data\nL%d:0%o;0%o;0%o;0%o\n.text\n", cval, w[0], w[1], w[2], w[3]);
		}
	case CON:
	case SFCON:
		cmst[cp++] = block(1, o, (o == CON ? INT : DOUBLE), 0, cval, 0, 0);
		goto tand;

	case STRING:
		cmst[cp++] = block(3, NAME, ARRAY + CHAR, 0, STATIC, 0, cval);

tand:
		if (cp >= CMSIZ) {
			error("Expression overflow");
			exit(1);
		}
		if (andflg)
			goto syntax;
		andflg = 1;
		goto advanc;

	case INCBEF:
	case DECBEF:
		if (andflg)
			o += 2;
		goto oponst;

	case COMPL:
	case EXCLA:
	case SIZEOF:
		if (andflg)
			goto syntax;
		goto oponst;

	case MINUS:
		if (!andflg) {
			if ((peeksym = symbol()) == FCON) {
				fcval = -fcval;
				goto advanc;
			}
			if (peeksym == SFCON) {
				fcval = -fcval;
				cval ^= (word)0100000;
				goto advanc;
			}
			o = NEG;
		}
		andflg = 0;
		goto oponst;

	case AND:
	case TIMES:
		if (andflg)
			andflg = 0;
		else if (o == AND)
			o = AMPER;
		else
			o = STAR;
		goto oponst;

	case LPARN:
		if (andflg) {
			o = symbol();
			if (o == RPARN)
				o = MCALL;
			else {
				peeksym = o;
				o = CALL;
				andflg = 0;
			}
		}
		goto oponst;

	case RBRACK:
	case RPARN:
		if (!andflg)
			goto syntax;
		goto oponst;

	case DOT:
	case ARROW:
		mosflg++;
		break;
	}
	if (!andflg)
		goto syntax;
	andflg = 0;

oponst:
	p = (OPD(o) >> 9) & 077;
	if ((o == COMMA || o == COLON) && initflg)
		p = 05;
opon1:
	ps = prst[pp];
	if (p > ps || (p == ps && (OPD(o) & RASSOC) != 0)) {
		switch (o) {
		case INCAFT:
		case DECAFT:
			p = 37;
			break;
		case LPARN:
		case LBRACK:
		case CALL:
			p = 04;
		}
		if (op >= SSIZE) {
			error("expression overflow");
			exit(1);
		}
		opst[++op] = o;
		prst[++pp] = p;
		goto advanc;
	}
	--pp;
	switch (os = opst[op--]) {
	case SEOF:
		peeksym = o;
		build(0);
		return pop();

	case CALL:
		if (o != RPARN)
			goto syntax;
		build(os);
		goto advanc;

	case MCALL:
		cmst[cp++] = block(0, 0, 0, 0, 0, 0, 0);
		os = CALL;
		goto fbuild;

	case LPARN:
		if (o != RPARN)
			goto syntax;
		goto advanc;

	case LBRACK:
		if (o != RBRACK)
			goto syntax;
		build(LBRACK);
		goto advanc;
	}
fbuild:
	build(os);
	goto opon1;

syntax:
	error("Expression syntax");
	errflush(o);
	(void)np;
	return 0;
}

static int
conexp(void)
{
	int t;

	initflg++;
	if ((t = tree()) != 0)
		if (OP(t) != CON)
			error("Constant required");
	initflg--;
	return VALUE(t);
}

static void
decsyn(int o)
{
	error("Declaration syntax");
	errflush(o);
}

static void
redec(void)
{
	error("%.8s redeclared", HNAME(defsym));
}

static int
getype(void)
{
	int o, type, ds = defsym;

	switch (o = symbol()) {
	case TIMES:
		return (int16_t)(getype() * 4 | PTR);

	case LPARN:
		type = getype();
		if ((o = symbol()) != RPARN)
			goto syntax;
		goto getf;

	case NAME:
		defsym = ds = csym;
		type = 0;
		SETSSP(HDIMP(ds), dimp);
getf:
		switch (o = symbol()) {
		case LPARN:
			if (xdflg) {
				xdflg = 0;
				ds = defsym;
				declare(ARG1, 0, 0, 0);
				defsym = ds;
				xdflg++;
			} else if ((o = symbol()) != RPARN)
				goto syntax;
			type = type << 2 | FUNC;
			goto getf;

		case LBRACK:
			if ((o = symbol()) != RBRACK) {
				int i;

				peeksym = o;
				cval = conexp();
				for (i = HSSP(ds); i < dimp; i++)
					dimtab[i] = (word)(dimtab[i] * cval);
				dimtab[dimp++] = cval;
				if ((o = symbol()) != RBRACK)
					goto syntax;
			} else
				dimtab[dimp++] = 1;
			type = type << 2 | ARRAY;
			goto getf;
		}
		peeksym = o;
		return type;
	}
syntax:
	decsyn(o);
	return -1;
}

static void
chkdim(void)
{
	if (dimp >= DIMSIZ) {
		error("Dimension/struct table overflow");
		exit(1);
	}
}

static int
decl1(int askw, int tkw, int offset, int elsize)
{
	int t1, chkoff, type, skw, dsym;

	skw = askw;
	chkoff = 0;
	mosflg = skw == MOS;
	if ((peeksym = symbol()) == SEMI || peeksym == RPARN)
		return 0;
	if ((t1 = getype()) < 0)
		goto syntax;
	type = 0;
	do
		type = type << 2 | (t1 & 030);
	while (((t1 >>= 2) & 030) != 0);
	type |= tkw;
	dsym = defsym;
	if (!(HCLASS(dsym) == 0
	   || (skw == ARG && HCLASS(dsym) == ARG1)
	   || (skw == EXTERN && HCLASS(dsym) == EXTERN && HTYPE(dsym) == type))) {
		if (skw == MOS && HCLASS(dsym) == MOS && HTYPE(dsym) == type)
			chkoff = 1;
		else {
			redec();
			goto syntax;
		}
	}
	HTYPE(dsym) = type;
	if (skw)
		HCLASS(dsym) = skw;
	if (skw == ARG1) {
		if (paraml == 0)
			paraml = dsym;
		else
			HOFFSET(parame) = dsym;
		parame = dsym;
	}
	if (elsize && ((type & 07) == RSTRUCT || (type & 07) == STRUCT)) {
		SETLENP(HDIMP(dsym), dimp);
		chkdim();
		dimtab[dimp++] = elsize;
	}
	elsize = 0;
	if (skw == MOS) {
		elsize = lengthh(dsym);
		if ((offset & 1) != 0 && elsize != 1) {
			offset++;
			elsize++;
		}
		if (chkoff && HOFFSET(dsym) != offset)
			redec();
		HOFFSET(dsym) = offset;
	}
	if ((HTYPE(dsym) & 030) == FUNC) {
		if (HCLASS(dsym) != EXTERN && HCLASS(dsym) != AUTO)
			error("Bad function");
		HCLASS(dsym) = EXTERN;
	}
	if (HCLASS(dsym) == AUTO) {
		autolen += rl(lengthh(dsym));
		HOFFSET(dsym) = -autolen;
	} else if (HCLASS(dsym) == STATIC) {
		HOFFSET(dsym) = isn;
		cprintf(".bss\nL%d:.=.+%d.\n.text\n", isn++, rl(lengthh(dsym)));
	} else if (HCLASS(dsym) == REG) {
		if (((type & 07) > CHAR && (type & 030) == 0)
		 || (type & 030) > PTR || regvar < 3)
			error("Bad register %o", type);
		HOFFSET(dsym) = --regvar;
	}
syntax:
	return elsize;
}

static int
declare(int askw, int tkw, int offset, int elsize)
{
	int o, skw;

	skw = askw;
	do {
		offset += decl1(skw, tkw, offset, elsize);
		if (xdflg && skw != MOS)
			return 0;
	} while ((o = symbol()) == COMMA);
	if (o == SEMI || (o == RPARN && skw == ARG1))
		return offset;
	decsyn(o);
	return 0;
}

static int
strdec(int *tkwp, int mosf)
{
	int elsize, o, ssym, ds;

	mosflg = 1;
	ssym = 0;
	if ((o = symbol()) == NAME) {
		ssym = csym;
		if (HCLASS(ssym) == 0) {
			HCLASS(ssym) = STRTAG;
			SETLENP(HDIMP(ssym), dimp);
			chkdim();
			dimtab[dimp++] = 0;
		}
		if (HCLASS(ssym) != STRTAG)
			redec();
		mosflg = mosf;
		o = symbol();
	}
	mosflg = 0;
	if (o != LBRACE) {
		if (ssym == 0) {
syntax:
			decsyn(o);
			return 0;
		}
		if (HCLASS(ssym) != STRTAG)
			error("Bad structure name");
		if ((elsize = dimtab[HLENP(ssym)]) == 0) {
			*tkwp = RSTRUCT;
			elsize = ssym;
		}
		peeksym = o;
	} else {
		ds = defsym;
		mosflg = 0;
		elsize = declist(MOS);
		if (elsize & 01)
			elsize++;
		defsym = ds;
		if ((o = symbol()) != RBRACE)
			goto syntax;
		if (ssym) {
			if (dimtab[HLENP(ssym)])
				error("%.8s redeclared", HNAME(ssym));
			dimtab[HLENP(ssym)] = elsize;
		}
	}
	return elsize;
}

static int
declist(int skwd)
{
	int o, elsize, ndec, offset = 0, tkw, skw;

loop:
	ndec = 0;
	tkw = -1;
	skw = skwd;
	elsize = 0;
	while ((o = symbol()) == KEYW) {
		switch (cval) {
		case AUTO:
		case STATIC:
		case EXTERN:
		case REG:
			if (skw)
				error("Conflict in storage class");
			skw = cval;
			ndec++;
			if (tkw < 0)
				continue;
			goto list;

		case STRUCT:
			o = cval;
			elsize = strdec(&o, skw == MOS);
			cval = o;
		case INT:
		case CHAR:
		case FLOAT:
		case DOUBLE:
			ndec++;
			if (tkw >= 0)
				error("Type clash");
			tkw = cval;
			if (skw == 0)
				continue;
			goto list;

		default:
			goto brk1;
		}
	}
brk1:
	peeksym = o;
	if (ndec == 0)
		return offset;
list:
	if (tkw < 0)
		tkw = INT;
	if (skw == 0)
		skw = AUTO;
	offset = declare(skw, tkw, offset, elsize);
	goto loop;
}

static void
errflush(int ao)
{
	int o = ao;

	while (o > RBRACE)
		o = symbol();
	peeksym = o;
}

static int
blkhed(void)
{
	int pl, cs;

	autolen = 6;
	declist(0);
	pl = 4;
	while (paraml) {
		HOFFSET(parame) = 0;
		cs = paraml;
		paraml = HOFFSET(paraml);
		if (HTYPE(cs) == FLOAT)
			HTYPE(cs) = DOUBLE;
		HOFFSET(cs) = pl;
		HCLASS(cs) = AUTO;
		if ((HTYPE(cs) & 030) == ARRAY) {
			HTYPE(cs) -= 020;
			SETSSP(HDIMP(cs), HSSP(cs) + 1);
		}
		pl += rl(lengthh(cs));
	}
	for (cs = HENT(0); cs < HENT(HSHSIZ); cs += 8) {
		if (HNAME(cs)[0] == '\0')
			continue;
		if (HCLASS(cs) > KEYWC && (HTYPE(cs) & 07) == RSTRUCT) {
			SETLENP(HDIMP(cs), HLENP(dimtab[HLENP(cs)]));
			HTYPE(cs) = (HTYPE(cs) & ~07) | STRUCT;
		}
		if (HCLASS(cs) == STRTAG && dimtab[HLENP(cs)] == 0)
			error("Undefined structure: %.8s", HNAME(cs));
		if (HCLASS(cs) == ARG)
			error("Not an argument: %.8s", HNAME(cs));
	}
	osleft = OSSIZ;
	space = treebase;
	rcexpr(block(1, SETREG, regvar, 0, 0, 0, 0), TAB_REG);
	return autolen;
}

static void
blkend(void)
{
	int cs;

	for (cs = HENT(0); cs < HENT(HSHSIZ); cs += 8) {
		if (HNAME(cs)[0]) {
			if (HCLASS(cs) == 0)
				error("%.8s undefined", HNAME(cs));
			if ((HNAME(cs)[0] & 0200) == 0) {
				HNAME(cs)[0] = '\0';
				hshused--;
			}
		}
	}
}

static int
pexpr(void)
{
	int o, t;

	if ((o = symbol()) != LPARN)
		goto syntax;
	t = tree();
	if ((o = symbol()) != RPARN)
		goto syntax;
	return t;
syntax:
	error("Statement syntax");
	errflush(o);
	return 0;
}

static void
pswitch(void)
{
	int sswp, swlab, swb, dl, wswp, e;

	swb = sswp = swp;
	if (swp == 0)
		swb = swp = 1;
	branch(swlab = isn++);
	dl = deflab;
	deflab = 0;
	statement(0);
	branch(brklab);
	label(swlab);
	putchar0('#');
	label(brklab);
	if (!deflab) {
		deflab = isn++;
		label(deflab);
	}
	wswp = swp;
	putw16(2 * (wswp - swb), binf);
	putw16(deflab, binf);
	putw16(4, binf);
	putw16(line, binf);
	for (e = swb; e < wswp; e++) {
		putw16(swtab[2 * (e - 1)], binf);
		putw16(swtab[2 * (e - 1) + 1], binf);
	}
	deflab = dl;
	swp = sswp;
}

static int
simplegoto(void)
{
	int csp;

	if ((peeksym = symbol()) == NAME && nextchar() == ';') {
		csp = csym;
		if (HCLASS(csp) == 0 && HTYPE(csp) == 0) {
			HTYPE(csp) = ARRAY;
			if (HOFFSET(csp) == 0)
				HOFFSET(csp) = isn++;
		}
		if ((HCLASS(csp) == 0 || HCLASS(csp) == STATIC) && HTYPE(csp) == ARRAY) {
			peeksym = -1;
			return HOFFSET(csp);
		}
	}
	return 0;
}

static void
dogoto(void)
{
	int np;

	cmst[cp++] = tree();
	build(STAR);
	np = pop();
	chkw(np);
	rcexpr(block(1, JUMP, 0, 0, np, 0, 0), TAB_REG);
}

static void
doret(void)
{
	if (nextchar() != ';')
		rcexpr(block(1, RFORCE, 0, 0, tree(), 0, 0), TAB_REG);
	branch(retlab);
}

static void
chconbrk(int l)
{
	if (l == 0)
		error("Break/continue error");
}

#define FORSPS	150

static int
forstmt(void)
{
	int l, savxpr[FORSPS];
	unsigned char savsz[FORSPS];
	int st, ss, o, i;

	if ((o = symbol()) != LPARN)
		return o;
	if ((o = symbol()) != SEMI) {
		peeksym = o;
		rcexpr(tree(), TAB_EFF);
		if ((o = symbol()) != SEMI)
			return o;
	}
	label(contlab);
	if ((o = symbol()) != SEMI) {
		peeksym = o;
		rcexpr(block(1, CBRANCH, tree(), brklab, 0, 0, 0), TAB_CC);
		if ((o = symbol()) != SEMI)
			return o;
	}
	if ((peeksym = symbol()) == RPARN) {
		peeksym = -1;
		statement(0);
		branch(contlab);
		return 0;
	}
	l = contlab;
	contlab = isn++;
	st = tree();
	if ((o = symbol()) != RPARN)
		return o;
	ss = space;
	if (space - treebase > FORSPS) {
		error("Expression too large");
		space = treebase + FORSPS;
	}
	for (i = 0; treebase + i < space; i++) {
		savxpr[i] = ts[treebase + i];
		savsz[i] = nsz[treebase + i];
	}
	statement(0);
	space = ss;
	for (i = 0; treebase + i < space && i < FORSPS; i++) {
		ts[treebase + i] = savxpr[i];
		nsz[treebase + i] = savsz[i];
	}
	label(contlab);
	rcexpr(st, TAB_EFF);
	branch(l);
	return 0;
}

static void
statement(int d)
{
	int o, o1, o2, o3, np;

stmt:
	switch (o = symbol()) {
	case EOFC:
		error("Unexpected EOF");
	case SEMI:
	case RBRACE:
		return;

	case LBRACE:
		if (d) {
			int n = blkhed() - 4;

			if (proflg)
				cprintf("jsr\tr5,mrsave;0f;%o\n.bss;0:.=.+2\n.text\n", n);
			else
				cprintf("jsr\tr5,rsave; %o\n", n);
		}
		while (!eof) {
			if ((o = symbol()) == RBRACE)
				return;
			peeksym = o;
			statement(0);
		}
		error("Missing '}'");
		return;

	case KEYW:
		switch (cval) {
		case GOTO:
			if ((o1 = simplegoto()) != 0)
				branch(o1);
			else
				dogoto();
			goto semi;

		case RETURN:
			doret();
			goto semi;

		case IF:
			np = pexpr();
			o2 = 0;
			if ((o1 = symbol()) == KEYW) switch (cval) {
			case GOTO:
				if ((o2 = simplegoto()) != 0)
					goto simpif;
				cbranch(np, o2 = isn++, 0);
				dogoto();
				label(o2);
				goto hardif;

			case RETURN:
				if (nextchar() == ';') {
					o2 = retlab;
					goto simpif;
				}
				cbranch(np, o1 = isn++, 0);
				doret();
				label(o1);
				o2++;
				goto hardif;

			case BREAK:
				o2 = brklab;
				goto simpif;

			case CONTIN:
				o2 = contlab;
simpif:
				chconbrk(o2);
				cbranch(np, o2, 1);
hardif:
				if ((o = symbol()) != SEMI)
					goto syntax;
				if ((o1 = symbol()) == KEYW && cval == ELSE)
					goto stmt;
				peeksym = o1;
				return;
			}
			peeksym = o1;
			cbranch(np, o1 = isn++, 0);
			statement(0);
			if ((o = symbol()) == KEYW && cval == ELSE) {
				o2 = isn++;
				branch(o2);
				label(o1);
				statement(0);
				label(o2);
				return;
			}
			peeksym = o;
			label(o1);
			return;

		case WHILE:
			o1 = contlab;
			o2 = brklab;
			label(contlab = isn++);
			brklab = isn++;
			cbranch(pexpr(), brklab, 0);
			statement(0);
			branch(contlab);
			label(brklab);
			contlab = o1;
			brklab = o2;
			return;

		case BREAK:
			chconbrk(brklab);
			branch(brklab);
			goto semi;

		case CONTIN:
			chconbrk(contlab);
			branch(contlab);
			goto semi;

		case DO:
			o1 = contlab;
			o2 = brklab;
			contlab = isn++;
			brklab = isn++;
			label(o3 = isn++);
			statement(0);
			label(contlab);
			contlab = o1;
			if ((o = symbol()) == KEYW && cval == WHILE) {
				cbranch(tree(), o3, 1);
				label(brklab);
				brklab = o2;
				goto semi;
			}
			goto syntax;

		case CASE:
			o1 = conexp();
			if ((o = symbol()) != COLON)
				goto syntax;
			if (swp == 0) {
				error("Case not in switch");
				goto stmt;
			}
			if (swp - 1 >= SWSIZ) {
				error("Switch table overflow");
			} else {
				swtab[2 * (swp - 1)] = isn;
				swtab[2 * (swp - 1) + 1] = o1;
				swp++;
				label(isn++);
			}
			goto stmt;

		case SWITCH:
			o1 = brklab;
			brklab = isn++;
			np = pexpr();
			chkw(np);
			rcexpr(block(1, RFORCE, 0, 0, np, 0, 0), TAB_REG);
			pswitch();
			brklab = o1;
			return;

		case DEFAULT:
			if (swp == 0)
				error("Default not in switch");
			if ((o = symbol()) != COLON)
				goto syntax;
			label(deflab = isn++);
			goto stmt;

		case FOR:
			o1 = contlab;
			o2 = brklab;
			contlab = isn++;
			brklab = isn++;
			if ((o = forstmt()) != 0)
				goto syntax;
			label(brklab);
			contlab = o1;
			brklab = o2;
			return;
		}
		error("Unknown keyword");
		goto syntax;

	case NAME:
		if (nextchar() == ':') {
			peekc = 0;
			o1 = csym;
			if (HCLASS(o1) > 0) {
				error("Redefinition");
				goto stmt;
			}
			HCLASS(o1) = STATIC;
			HTYPE(o1) = ARRAY;
			if (HOFFSET(o1) == 0)
				HOFFSET(o1) = isn++;
			label(HOFFSET(o1));
			if ((peeksym = symbol()) == RBRACE)
				return;
			goto stmt;
		}
	}

	peeksym = o;
	rcexpr(tree(), TAB_EFF);

semi:
	if ((o = symbol()) == SEMI)
		return;
syntax:
	error("Statement syntax");
	errflush(o);
	goto stmt;
}

static void
bxdec(void)
{
	error("Inconsistent external initialization");
}

static int
cinit1(const char *cs, int type, int width, int ninit)
{
	int s, sop;
	uint16_t w[4];

	if ((peeksym = symbol()) == STRING && type == ARRAY + CHAR) {
		peeksym = -1;
		if (ninit)
			bxdec();
		cprintf(".text\n_%.8s=L%d\n", cs, cval);
		return (nchstr + 1) & ~01;
	}
	if (peeksym == RBRACE)
		return ninit;
	initflg++;
	s = tree();
	initflg = 0;
	sop = s ? OP(s) : -1;
	switch (width) {
	case 1:
		cprintf(".byte ");
		if (sop != CON)
			bxdec();
	case 2:
		if (sop == CON) {
			cprintf("%d.\n", VALUE(s));
			break;
		}
		rcexpr(block(1, INIT, 0, 0, s, 0, 0), TAB_REG);
		break;
	case 4:
		pdpwords(fcval, w, 0);
		cprintf("0%o;0%o\n", w[0], w[1]);
		goto flt;
	case 8:
		pdpwords(fcval, w, 1);
		cprintf("0%o;0%o;0%o;0%o\n", w[0], w[1], w[2], w[3]);
flt:
		if (sop == FCON || sop == SFCON)
			break;
	default:
		bxdec();
	}
	return ++ninit;
}

static void
cinit(const char *cs, int type, int nel, int awidth)
{
	int o, ninit, width;

	width = awidth;
	if ((peeksym = symbol()) == COMMA || peeksym == SEMI) {
		cprintf(".comm\t_%.8s,0%o\n", cs, (nel * width + 1) & ~01);
		return;
	}
	ninit = 0;
	cprintf(".data\n_%.8s=.\n", cs);
	if ((o = symbol()) == LBRACE) {
		do
			ninit = cinit1(cs, type, width, ninit);
		while ((o = symbol()) == COMMA);
		if (o != RBRACE)
			peeksym = o;
	} else {
		peeksym = o;
		ninit = cinit1(cs, type, width, 0);
	}
	if (ninit < nel)
		cprintf(".=.+%d.\n", (nel - ninit) * width);
	else
		nel = ninit;
	if (nel > 1 && (type & 030) != ARRAY && (type & 07) != STRUCT)
		error("Too many initializers");
	if (((nel & width) & 01) != 0)
		cprintf(".even\n");
}

static void
cfunc(const char *cs)
{
	int savdimp;

	strflg = 0;
	savdimp = dimp;
	cprintf(".text\n_%.8s:\n", cs);
	declist(ARG);
	regvar = 5;
	retlab = isn++;
	if ((peeksym = symbol()) != LBRACE)
		error("Compound statement required");
	statement(1);
	cprintf("L%d:jmp\trretrn\n", retlab);
	dimp = savdimp;
}

static void
extdef(void)
{
	int o, width, type, elsize = 0, nel, ds;
	char *cs;

	if (((o = symbol()) == EOFC) || o == SEMI)
		return;
	type = 0;
	xdflg++;
xxx:
	if (o == KEYW) {
		if (cval == EXTERN) {
			o = symbol();
			goto xxx;
		}
		if ((type = cval) > STRUCT)
			goto syntax;
		elsize = 0;
		if (type == STRUCT) {
			elsize = strdec(&type, 0);
			if ((peeksym = symbol()) != KEYW)
				blkhed();
		}
	} else {
		if (o != NAME)
			goto syntax;
		peeksym = o;
	}
	do {
		defsym = 0;
		strflg = 1;
		decl1(EXTERN, type & 07, 0, elsize);
		if ((ds = defsym) == 0)
			return;
		cs = HNAME(ds);
		funcsym = ds;
		cs[0] |= 0200;
		cprintf(".globl\t_%.8s\n", cs);
		xdflg = 0;
		type = HTYPE(ds);
		if ((type & 030) == FUNC && ((peeksym = symbol()) == LBRACE
		    || peeksym == KEYW)) {
			cfunc(cs);
			return;
		}
		nel = 1;
		while ((HTYPE(ds) & 030) == ARRAY) {
			nel = dimtab[HSSP(ds)];
			HTYPE(ds) = decref(HTYPE(ds));
		}
		width = lengthh(ds);
		if (HTYPE(ds) == STRUCT) {
			nel *= width / 2;
			width = 2;
		}
		HTYPE(ds) = type;
		cinit(cs, type, nel, width);
	} while ((o = symbol()) == COMMA);
	if (o == SEMI)
		return;
syntax:
	error("External definition syntax");
	errflush(o);
	statement(0);
}

static const struct { const char *name; int val; } kwtab[] = {
	{ "int", INT }, { "char", CHAR }, { "float", FLOAT }, { "double", DOUBLE },
	{ "struct", STRUCT }, { "auto", AUTO }, { "extern", EXTERN },
	{ "static", STATIC }, { "register", REG }, { "goto", GOTO },
	{ "return", RETURN }, { "if", IF }, { "while", WHILE }, { "else", ELSE },
	{ "switch", SWITCH }, { "case", CASE }, { "break", BREAK },
	{ "continue", CONTIN }, { "do", DO }, { "default", DEFAULT },
	{ "for", FOR }, { "sizeof", SIZEOF }, { NULL, 0 }
};

int
main(int argc, char **argv)
{
	int i, k, rp;

	outf = NULL;
	line = 1;
	if (argc < 4) {
		error("Arg count");
		exit(1);
	}
	if ((fin = fopen(argv[1], "rb")) == NULL) {
		error("Can't find %s", argv[1]);
		exit(1);
	}
	if ((ascf = fopen(argv[2], "wb")) == NULL ||
	    (binf = fopen(argv[3], "wb")) == NULL) {
		error("Can't create temp");
		exit(1);
	}
	outf = ascf;
	if (argc > 4)
		proflg++;
	xdflg++;
	for (i = 0; kwtab[i].name != NULL; i++) {
		const char *np = kwtab[i].name;

		for (k = 0; k < ncps; k++) {
			symbuf[k] = *np;
			if (*np != '\0')
				np++;
		}
		rp = lookup();
		HCLASS(rp) = KEYWC;
		HTYPE(rp) = kwtab[i].val;
	}
	xdflg = 0;
	treebase = TB;
	putw16(TREEBASE_ADDR, binf);
	while (!eof) {
		extdef();
		blkend();
	}
	fflush(ascf);
	fflush(binf);
	exit(nerror != 0);
}
