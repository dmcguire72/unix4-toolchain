/*
 * Unix Fourth Edition -- as2.c -- as2
 * 
 * Original assembly:
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 * 
 * C Port:
 * Copyright 2026 Daniel McGuire
 */
#include "as.h"
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#define BRLEN	1024
#define EOT     004

#define OP_BR	0400
#define OP_JMP	0100

struct ent {
	uint16_t type;
	uint16_t value;
};

static struct ent *E;
static int nuser, nfb, fbbase, endtable;

#define dot		(E[0].value)
#define dotrel		(E[0].type)
#define dotdot		(E[1].value)
#define TYPEB(i)	((int8_t)(E[i].type & 0377))
#define SYMTOK(i)	(0200 + (i))
#define SYMIDX(t)	((t) - 0200)

static int r0, r1, r2, r3, r4;

static int xsymbol = -1;
static int savop;
static int numval;
static int swapf, rlimit;
static int passno;
static int istext;
static int outmod = 0777;
static int defund;
static int line;
static int maxtyp;
static int brtabp, brdelt;
static unsigned char brtab[BRLEN / 8];
static int savdot[3];
static int txtsiz, datsiz, bsssiz, symsiz, stksiz, exorig;
static int datbase, bssbase;

enum { S_TXT, S_DAT, S_BSS, S_TREL, S_DREL, S_BREL, S_SYM };
static long seek[7] = { 020 };
static long *tseekp = &seek[S_TXT], *rseekp = &seek[S_TREL];

static int curfb[20];
#define nxtfb(n)	curfb[10 + (n)]

static struct { int val, type, xsym; } adrbuf[2];
static int nadr;

static char (*names)[8];

static const char *a_tmp[4];
static int txtfil, fbfil, symf, fin, fout;
static int ibufc, ibufp, ibufw;
static unsigned char inbuf[512];
static int tmp4opened;
static char argb[22];

static void aexit(void);

static void
filerr(const char *n)
{
	if (write(2, n, strlen(n)) < 0) {}
	if (write(2, "?\n", 2) < 0) {}
	aexit();
}

static int
ofile(const char *n)
{
	int fd = open(n, O_RDONLY);

	if (fd < 0)
		filerr(n);
	return fd;
}

static void
aexit(void)
{
	int i;
	mode_t um;

	for (i = 0; i < 4; i++)
		if (a_tmp[i] != NULL)
			unlink(a_tmp[i]);
	um = umask(0);
	umask(um);
	chmod("a.out", outmod == 0777 ? (0777 & ~um) : (0666 & ~um));
	exit(outmod == 0777 ? ASEXIT_OK : ASEXIT_ERR);
}

static void
sintr(int sig)
{
	(void)sig;
	aexit();
}

struct obuf {
	long pos;
	long base;
	int n;
	unsigned char buf[512];
};
static struct obuf txtp, relp;

static void
oflush(struct obuf *o)
{
	if (o->n > 0) {
		if (lseek(fout, o->base, SEEK_SET) < 0 ||
		    write(fout, o->buf, o->n) != o->n) {
			perror("as2: a.out");
			outmod = 0666;
			aexit();
		}
		o->n = 0;
	}
}

static void
oset(struct obuf *o, long off)
{
	o->pos = off;
	o->n = 0;
}

static void
putc2(struct obuf *o, int c)
{
	if (o->n == 0)
		o->base = o->pos;
	else if (o->base + o->n != o->pos || o->n == 512)
		oflush(o), o->base = o->pos;
	o->buf[o->n++] = c;
	o->pos++;
}

static void
putw2(struct obuf *o, int w)
{
	putc2(o, w & 0377);
	putc2(o, (w >> 8) & 0377);
}

static int vflag;

static int
getw1(void)
{
	for (;;) {
		if (--ibufc > 0) {
			ibufw = inbuf[ibufp] | inbuf[ibufp + 1] << 8;
			ibufp += 2;
			return (int16_t)ibufw;
		}
		{
			ssize_t n = read(fin, inbuf, 512);

			if (n >= 0) {
				ibufc = n / 2;
				if (ibufc != 0) {
					ibufw = inbuf[0] | inbuf[1] << 8;
					ibufp = 2;
					return (int16_t)ibufw;
				}
			}
		}
		if (istext) {
			close(fin);
			if (tmp4opened)
				filerr(a_tmp[3]);
			tmp4opened = 1;
			fin = ofile(a_tmp[3]);
			continue;
		}
		vflag = 1;
		return 4;
	}
}

static int
getword(void)
{
	if (savop != 0) {
		int t = savop;

		savop = 0;
		return t;
	}
	return getw1();
}

static void
readop(void)
{
	if (savop != 0) {
		r4 = savop;
		savop = 0;
		return;
	}
	r4 = getw1();
	if ((uint16_t)r4 < 0200)
		return;
	if ((uint16_t)r4 < 04000)
		r4 = SYMTOK((r4 - 01000) / 4);
	else
		r4 = SYMTOK(NBSYM + ((uint16_t)r4 - 04000) / 4);
}

static void
fbadv(int n)
{
	int p = curfb[10 + n];

	curfb[n] = p;
	if (p < 0)
		p = fbbase;
	else
		p++;
	while ((E[p].type >> 8) != 2 * n) {
		if ((int16_t)E[p].type < 0)
			break;
		p++;
	}
	curfb[10 + n] = p;
}

static void
setup(void)
{
	int i;

	for (i = 0; i < 20; i++)
		curfb[i] = -1;
	fin = txtfil;
	ibufc = 0;
	for (i = 0; i < 10; i++)
		fbadv(i);
}

static void
error(int code)
{
	char buf[8];
	char *p;
	int i, l;

	outmod = 0666;
	for (p = argb; *p; p++) {
		if (write(2, p, 1) < 0) {}
		*p = '\0';
	}
	l = (uint16_t)line;
	buf[0] = code;
	buf[1] = ' ';
	for (i = 5; i >= 2; i--) {
		buf[i] = l % 10 + '0';
		l /= 10;
	}
	buf[6] = '\n';
	if (write(2, buf, 7) < 0) {}
}

static void errora(void) { error('a'); }
static void errore(void) { error('e'); }

static void outb(void);

static void
outw(void)
{
	int sp, rel;
	uint16_t u;

	if (dotrel == 4) {
		error('x');
		return;
	}
	if (dot & 1) {
		error('o');
		r3 = 0;
		outb();
		return;
	}
	dot += 2;
	if (passno == 0)
		return;
	u = (uint16_t)r3;
	sp = u >> 15;
	u = (uint16_t)(u << 1);
	r3 = (int16_t)u >> 1;
	if (r3 == 040) {
		outmod = 0666;
		r3 = (int16_t)((xsymbol - NBSYM) * 4);
		r3 = (int16_t)(r3 * 2);
		r3 |= 4;
	} else {
		r3 &= ~040;
		if ((uint16_t)r3 >= 5)
			r3 = 1;
		if ((uint16_t)r3 >= 2 && (uint16_t)r3 <= 4) {
			if (!sp)
				r2 = (int16_t)(r2 + dotdot);
		} else if (sp)
			r2 = (int16_t)(r2 - dotdot);
		if (--r3 < 0)
			r3 = 0;
	}
	r3 = (int16_t)(r3 * 2);
	rel = r3 | sp;
	putw2(&txtp, r2);
	*tseekp += 2;
	putw2(&relp, rel);
	*rseekp += 2;
}

static void
outb(void)
{
	if (dotrel == 4) {
		error('x');
		return;
	}
	if ((uint16_t)r3 > 1)
		error('r');
	if (passno != 0) {
		putc2(&txtp, r2);
		if (!(dot & 1)) {
			putw2(&relp, 0);
			*rseekp += 2;
		}
		(*tseekp)++;
	}
	dot++;
}

#define X	(-2)
#define M	(-1)

static const signed char reltp2[6][6] = {
	{ 0, 0, 0, 0, 0, 0 },
	{ 0, M, 2, 3, 4, 040 },
	{ 0, 2, X, X, X, X },
	{ 0, 3, X, X, X, X },
	{ 0, 4, X, X, X, X },
	{ 0, 040, X, X, X, X },
};
static const signed char reltm2[6][6] = {
	{ 0, 0, 0, 0, 0, 0 },
	{ 0, M, 2, 3, 4, 040 },
	{ 0, X, 1, X, X, X },
	{ 0, X, X, 1, X, X },
	{ 0, X, X, X, 1, X },
	{ 0, X, X, X, X, X },
};
static const signed char relte2[6][6] = {
	{ 0, 0, 0, 0, 0, 0 },
	{ 0, M, X, X, X, X },
	{ 0, X, X, X, X, X },
	{ 0, X, X, X, X, X },
	{ 0, X, X, X, X, X },
	{ 0, X, X, X, X, X },
};

static int
maprel(int t)
{
	if (t == 040)
		return 5;
	t &= 037;
	if ((uint16_t)t > (uint16_t)maxtyp)
		maxtyp = t;
	if (t > 5)
		t = 1;
	return t;
}

static void
combin(const signed char (*tab)[6])
{
	int g, lo, hi, row, col, v;

	if (passno == 0) {
		g = (r0 | r3) & 040;
		lo = r0 & 037;
		hi = r3 & 037;
		if (lo > hi) {
			int t = lo;

			lo = hi;
			hi = t;
		}
		if (lo == 0)
			r3 = 0;
		else {
			r3 = hi;
			if (tab == reltm2 && lo == hi)
				r3 = 1;
		}
		r3 |= g;
		return;
	}
	maxtyp = 0;
	row = maprel(r0);
	col = maprel(r3);
	v = tab[row][col];
	if (v >= 0) {
		r3 = v;
		return;
	}
	if (v != M)
		error('r');
	r3 = maxtyp;
}

static int
dvd(int dividend, int divisor, int *quo, int *rem)
{
	long q;

	if ((int16_t)divisor == 0)
		return 0;
	q = (long)(int32_t)(uint32_t)(uint16_t)dividend / (int16_t)divisor;
	if (q > 32767 || q < -32768)
		return 0;
	*quo = q;
	*rem = (long)(uint16_t)dividend % (int16_t)divisor;
	return 1;
}

static int16_t
ashift(int v, int n)
{
	n &= 077;
	if (n & 040)
		n -= 64;
	if (n > 0) {
		while (n-- > 0)
			v = (int16_t)(v * 2);
		return v;
	}
	while (n++ < 0)
		v >>= 1;
	return v;
}

static void expres1(void);

static void
expres(void)
{
	xsymbol = -1;
	expres1();
}

static void
expres1(void)
{
	int pend = '+';
	int q, rm;

	r2 = 0;
	r3 = 1;
	goto first;

advanc:
	readop();
first:
	r0 = r4;
	if (r4 < 0 || r4 > 0177) {
		int i = SYMIDX(r4);

		r0 = TYPEB(i);
		if (r0 == 0 && passno != 0)
			error('u');
		if (r0 == 040) {
			xsymbol = i;
			r1 = 0;
			goto oprand;
		}
		r1 = E[i].value;
		goto oprand;
	}
	if (r4 >= 0141) {
		int p = curfb[r4 - 0141];

		r1 = E[p].value;
		r0 = (int8_t)(E[p].type & 0377);
		goto oprand;
	}
	switch (r4) {
	case '+': case '-': case '*': case '/': case '&': case 037:
	case 035: case 036: case '%': case '^': case '!':
		if (pend != '+')
			errore();
		pend = r4;
		goto advanc;
	case '[': {
		int s2 = r2, s3 = r3;

		readop();
		expres1();
		if (r4 != ']')
			error(']');
		r0 = r3;
		r1 = r2;
		r3 = s3;
		r2 = s2;
		goto oprand;
	}
	case 1:
		r1 = getword();
		r0 = 1;
		goto oprand;
	case 2:
		r1 = numval;
		r0 = 1;
		goto oprand;
	}

	r1 = 0x4000;
	return;

oprand:
	switch (pend) {
	case '+':
		combin(reltp2);
		r2 = (int16_t)(r2 + r1);
		break;
	case '-':
		combin(reltm2);
		r2 = (int16_t)(r2 - r1);
		break;
	case '*':
		combin(relte2);
		r1 = (int16_t)(r1 * r2);
		r2 = r1;
		break;
	case '/': {
		int sv3;

		combin(relte2);
		sv3 = r3;
		if (dvd(r2, r1, &q, &rm))
			r2 = (int16_t)q;
		else
			r2 = 0;
		r3 = sv3;
		break;
	}
	case 037:
		combin(relte2);
		r2 = (int16_t)(r2 | r1);
		break;
	case '&':
		combin(relte2);
		r1 = (int16_t)~r1;
		r2 = (int16_t)(r2 & ~r1);
		break;
	case 035:
		combin(relte2);
		r2 = ashift(r2, r1);
		break;
	case 036:
		if ((int16_t)(-r1) == 0) {
			r1 = 0;
			combin(relte2);
			r2 = ashift(r2, r1);
			break;
		}
		r1 = (int16_t)((int16_t)(-r1) + 1);
		r2 = (int16_t)((uint16_t)r2 >> 1);
		combin(relte2);
		r2 = ashift(r2, r1);
		break;
	case '%': {
		int sv3;

		combin(relte2);
		sv3 = r3;
		if (dvd(r2, r1, &q, &rm))
			r2 = (int16_t)rm;
		r3 = sv3;
		break;
	}
	case '!':
		combin(relte2);
		r1 = (int16_t)~r1;
		r2 = (int16_t)(r2 + r1);
		break;
	case '^':
		r3 = r0;
		break;
	}
	pend = '+';
	goto advanc;
}

static void
checkreg(void)
{
	if ((uint16_t)r2 > 7 || ((uint16_t)r1 > 1 && (uint16_t)r3 < 5)) {
		errora();
		r2 = 0;
		r3 = 0;
	}
}

static void
checkrp(void)
{
	if (r4 != ')') {
		error(')');
		return;
	}
	readop();
}

static void
pushadr(int v, int t, int x)
{
	if (nadr < 2) {
		adrbuf[nadr].val = v;
		adrbuf[nadr].type = t;
		adrbuf[nadr].xsym = x;
		nadr++;
	}
}

static void
addres(void)
{
	int ind = 0;

again:
	switch (r4) {
	case '(':
		readop();
		expres();
		checkrp();
		checkreg();
		if (r4 == '+') {
			readop();
			r2 |= 020;
			r2 |= ind;
			return;
		}
		if (ind == 0) {
			r2 |= 010;
			return;
		}
		r2 |= 070;
		pushadr(0, 0, xsymbol);
		return;
	case '-':
		readop();
		if (r4 == '(') {
			readop();
			expres();
			checkrp();
			checkreg();
			r2 |= ind;
			r2 |= 040;
			return;
		}
		savop = r4;
		r4 = '-';
		goto getx;
	case '$':
		readop();
		expres();
		pushadr(r2, r3, xsymbol);
		r2 = ind;
		r2 |= 027;
		return;
	case '*':
		if (ind != 0)
			error('*');
		ind = 010;
		readop();
		goto again;
	}
getx:
	expres();
	if (r4 == '(') {
		readop();
		pushadr(r2, r3, xsymbol);
		expres();
		checkreg();
		checkrp();
		r2 |= 060;
		r2 |= ind;
		return;
	}
	if (r3 == 024) {
		checkreg();
		r2 |= ind;
		return;
	}
	r3 = (r3 & 0xffff) | 0100000;
	r3 = (int16_t)r3;
	r2 = (int16_t)(r2 - dot);
	r2 = (int16_t)(r2 - 4);
	if (nadr != 0)
		r2 = (int16_t)(r2 - 2);
	pushadr(r2, r3, xsymbol);
	r2 = 067;
	r2 |= ind;
}

static int
setbr(int target)
{
	int n = brtabp, res = 0, d;

	if (n >= BRLEN)
		return 2;
	brtabp++;
	d = (int16_t)(target - dot);
	if (d > 0)
		d = (int16_t)(d - brdelt);
	if (d < -254 || d > 256) {
		brtab[n >> 3] |= 1 << (n & 7);
		res = 2;
	}
	return res;
}

static int
getbr(void)
{
	int n = brtabp;

	if (n >= BRLEN)
		return 1;
	brtabp++;
	return (brtab[n >> 3] >> (n & 7)) & 1;
}

static void
xpr(void)
{
	expres();
	outw();
}

static void
opeof(void)
{
	int cnt = 16, c;
	char *p = argb;

	line = 1;
	for (;;) {
		c = getword();
		r4 = c;
		if (c < 0)
			break;
		*p++ = c;
		if (--cnt > 0)
			continue;
		p--;
	}
	*p++ = '\n';
	*p++ = '\0';
}

static void
dobranch(int op)
{
	int d;

	r2 = (int16_t)(r2 - dot);
	d = r2;
	if (d < -254 || d > 256) {
		error('b');
		r2 = 0;
	} else if ((r2 & 1) || r3 != (int16_t)dotrel) {
		error('b');
		r2 = 0;
	} else {
		r2 = (int16_t)(r2 >> 1);
		r2--;
		r2 &= 0377;
	}
	r2 |= op;
	r3 = 0;
	outw();
}

static void
op2b(int op, int src)
{
	int i;

	addres();
	if (swapf) {
		int t = src;

		src = r2;
		r2 = t;
	}
	src = (int16_t)((((src & 0377) << 8) | ((src >> 8) & 0377)));
	src >>= 2;
	if ((uint16_t)src >= (uint16_t)rlimit)
		error('x');
	r2 |= src;
	r2 |= op;
	r3 = 0;
	outw();
	for (i = 0; i < nadr; i++) {
		r2 = adrbuf[i].val;
		r3 = adrbuf[i].type;
		xsymbol = adrbuf[i].xsym;
		outw();
	}
}

static void
op2a(int op)
{
	int src = r2;

	readop();
	op2b(op, src);
}

static void
opl13(int op)
{
	addres();
	op2a(op);
}

static void
string_tok(void)
{
	int c;

	for (;;) {
		c = getword();
		r4 = c;
		r3 = 1;
		r2 = c;
		if (c < 0)
			break;
		r2 = c & 0377;
		outb();
	}
	r4 = getword();
}

static void
opline(void)
{
	int t, op;

	if (r4 >= 0 && r4 <= 0177) {
		if (r4 == 5) {
			opeof();
			return;
		}
		if (r4 == '<') {
			string_tok();
			return;
		}
		xpr();
		return;
	}
	t = TYPEB(SYMIDX(r4));
	if (t == 024 || t == 033 || t == 034 || t < 5 || t > 036) {
		xpr();
		return;
	}
	op = E[SYMIDX(r4)].value;
	readop();
	nadr = 0;
	swapf = 0;
	rlimit = -1;

	switch (t) {
	case 5:
		rlimit = 0400;
		opl13(op);
		return;
	case 6:
		expres();
		if (passno == 0) {
			r2 |= op;
			r3 = 0;
			outw();
			return;
		}
		dobranch(op);
		return;
	case 7:
		expres();
		checkreg();
		op2a(op);
		return;
	case 8:
		expres();
		checkreg();
		r2 |= op;
		outw();
		return;
	case 9:
		expres();
		if ((uint16_t)r2 >= 64 || r3 > 1)
			errora();
		r2 |= op;
		outw();
		return;
	case 10:
		rlimit = 0400;
		addres();
		if ((uint16_t)r2 >= 4)
			swapf++;
		else
			op = 0174000;
		op2a(op);
		return;
	case 11:
		opl13(op);
		return;
	case 12:
		swapf++;
		rlimit = 0400;
		opl13(op);
		return;
	case 13:
		op2b(op, 0);
		return;
	case 14:
		for (;;) {
			expres();
			outb();
			if (r4 != ',')
				return;
			readop();
		}
	case 15:
		string_tok();
		return;
	case 16:
		if (dot & 1) {
			if (dotrel == 4)
				dot++;
			else {
				r2 = 0;
				r3 = 0;
				outb();
			}
		}
		return;
	case 17:
		expres();
		return;
	case 18:
		return;
	case 19:
		while (r4 >= 0200) {
			E[SYMIDX(r4)].type |= 040;
			readop();
			if (r4 != ',')
				return;
			readop();
		}
		return;
	case 21: case 22: case 23: {
		int k = t - 21;

		savdot[dotrel - 2] = dot;
		if (passno != 0) {
			oflush(&txtp);
			oflush(&relp);
			tseekp = &seek[S_TXT + k];
			oset(&txtp, *tseekp);
			rseekp = &seek[S_TREL + k];
			oset(&relp, *rseekp);
		}
		dot = savdot[k];
		dotrel = k + 2;
		return;
	}
	case 24:
		swapf++;
		rlimit = 01000;
		opl13(op);
		return;
	case 25: {
		expres();
		checkreg();
		op |= (int16_t)(((r2 << 8) | ((r2 >> 8) & 0377)) >> 2) & 0xffff;
		readop();
		expres();
		if (passno == 0) {
			r2 |= op;
			r3 = 0;
			outw();
			return;
		}
		r2 = (int16_t)(dot - r2);
		{
			int d = r2;

			if (d < -2 || d > 0175) {
				error('b');
				r2 = 0;
			} else {
				r2 += 4;
				if ((r2 & 1) || r3 != (int16_t)dotrel) {
					error('b');
					r2 = 0;
				} else {
					r2 >>= 1;
					r2--;
					r2 &= 0377;
				}
			}
		}
		r2 |= op;
		r3 = 0;
		outw();
		return;
	}
	case 26:
		if (r4 >= 0200) {
			int s = SYMIDX(r4);

			readop();
			readop();
			expres();
			if ((E[s].type & 037) == 0) {
				E[s].type |= 040;
				E[s].value = r2;
			}
		}
		return;
	case 29:
	case 30:
		expres();
		if (passno == 0) {
			int ex = setbr(r2);

			if (ex != 0 && op != OP_BR)
				ex += 2;
			dot += ex;
			dot += 2;
			return;
		}
		if (getbr() == 0) {
			dobranch(op);
			return;
		} else {
			int sv2 = r2, sv3 = r3;

			if (op != OP_BR) {
				r2 = 0402 ^ op;
				r3 = 1;
				outw();
			}
			r3 = 1;
			r2 = OP_JMP + 037;
			outw();
			r3 = sv3;
			r2 = sv2;
			outw();
		}
		return;
	}
	xpr();
}

static int *sizep[3] = { &txtsiz, &datsiz, &bsssiz };

static void
dotmax(void)
{
	if (passno == 0) {
		int *sp = sizep[dotrel - 2];

		if ((uint16_t)dot > (uint16_t)*sp)
			*sp = dot;
	}
}

static void
assem(void)
{
	int first;

	for (;;) {
		readop();
		if (r4 == 5 || r4 == '<')
			goto doop;
		if (r4 == '\n' || r4 == ';' || r4 == EOT)
			goto ealoop;
		first = r4;
		if (first == 1) {
			first = 2;
			r4 = getword();
			numval = r4;
		}
		readop();
		if (r4 == '=') {
			readop();
			expres();
			r1 = first;
			if (first == SYMTOK(0)) {
				r3 &= ~040;
				if (r3 != (int16_t)dotrel) {
					error('.');
					goto ealoop;
				}
				if (r3 == 4) {
					dot = r2;
					goto dotmax_;
				}
				r2 = (int16_t)(r2 - dot);
				if (r2 < 0) {
					error('.');
					goto ealoop;
				}
				{
					int cnt = r2;

					while (--cnt >= 0) {
						r2 = 0;
						r3 = 1;
						outb();
					}
				}
				goto dotmax_;
			}
			if (r3 == 040)
				error('r');
			if (first >= 0200) {
				int i = SYMIDX(first);

				E[i].type &= ~037;
				r3 &= 037;
				if (r3 == 0)
					r2 = 0;
				E[i].type |= r3;
				E[i].value = r2;
			}
			goto ealoop;
		}
		if (r4 == ':') {
			if (first >= 0200) {
				int i = SYMIDX(first);

				if (passno == 0) {
					int t = E[i].type & 037;

					if (t != 0 && !(t >= 033 && t <= 034))
						error('m');
					E[i].type = (E[i].type & ~037) | dotrel;
					brdelt = (int16_t)(E[i].value - dot);
					E[i].value = dot;
				} else if (dot != E[i].value)
					error('p');
				continue;
			}
			if (first == 2) {
				int n = numval, p;

				fbadv(n);
				p = curfb[n];
				E[p].type = (E[p].type & ~0377) | (dotrel & 0377);
				brdelt = (int16_t)(E[p].value - dot);
				E[p].value = dot;
				continue;
			}
			error('x');
			continue;
		}
		savop = r4;
		r4 = first;
doop:
		opline();
dotmax_:
		dotmax();
ealoop:
		if (r4 == '\n')
			line++;
		else if (r4 == EOT)
			return;
	}
}

static void
go(void)
{
	int cap = 256, i, w, t, v;
	struct ent *e;
	uint16_t nm[4];

	E = malloc((NBSYM + cap + 2) * sizeof *E);
	names = malloc(cap * 8);
	if (E == NULL || names == NULL) {
		fprintf(stderr, "as2: out of memory\n");
		aexit();
	}
	for (i = 0; i < NBSYM; i++) {
		E[i].type = bsymtab[i].type;
		E[i].value = bsymtab[i].value;
	}

	fin = symf;
	ibufc = 0;
	for (;;) {
		vflag = 0;
		w = getword();
		if (vflag)
			break;
		nm[0] = w;
		symsiz += 12;
		nm[1] = getword();
		nm[2] = getword();
		nm[3] = getword();
		t = (uint16_t)getword();
		if (nuser >= cap) {
			cap *= 2;
			E = realloc(E, (NBSYM + cap + 2) * sizeof *E);
			names = realloc(names, cap * 8);
			if (E == NULL || names == NULL) {
				fprintf(stderr, "as2: out of memory\n");
				aexit();
			}
		}
		for (i = 0; i < 4; i++) {
			names[nuser][2 * i] = nm[i] & 0377;
			names[nuser][2 * i + 1] = nm[i] >> 8;
		}
		e = &E[NBSYM + nuser++];
		if ((t & 037) >= 2 && (t & 037) <= 3) {
			e->type = t + 031;
			e->value = getword();
		} else {
			e->type = 0;
			e->value = 0;
			(void)getword();
		}
	}

	fbbase = NBSYM + nuser;
	nfb = 0;
	fin = fbfil;
	ibufc = 0;
	for (;;) {
		vflag = 0;
		w = getword();
		if (vflag)
			break;
		v = getword();
		if (nfb >= 0 && fbbase + nfb + 2 >= NBSYM + cap + 2) {
			cap *= 2;
			E = realloc(E, (NBSYM + cap + 2) * sizeof *E);
			if (E == NULL) {
				fprintf(stderr, "as2: out of memory\n");
				aexit();
			}
		}
		E[fbbase + nfb].type = (uint16_t)w + 031;
		E[fbbase + nfb].value = v;
		nfb++;
	}
	endtable = fbbase + nfb;
	E[endtable].type = 0100000;
	E[endtable].value = 0;

	setup();
	istext = 1;
	passno = 0;
	assem();
	if (outmod != 0777)
		aexit();

	dot = 0;
	dotrel = 2;
	dotdot = 0;
	brtabp = 0;
	close(fin);
	tmp4opened = 0;
	txtfil = ofile(a_tmp[0]);
	setup();
	passno++;
	bsssiz = (bsssiz + 1) & ~1;
	txtsiz = (txtsiz + 1) & ~1;
	datsiz = (datsiz + 1) & ~1;
	datbase = txtsiz;
	savdot[1] = txtsiz;
	bssbase = txtsiz + datsiz;
	savdot[2] = bssbase;
	seek[S_SYM] = 2 * bssbase + 020;
	seek[S_DREL] = seek[S_SYM] - datsiz;
	seek[S_TREL] = seek[S_DREL] - txtsiz;
	seek[S_DAT] = seek[S_TREL] - datsiz;
	for (i = 0; i < endtable; i++) {
		int idx = NBSYM + i;

		if (idx >= endtable)
			break;
		if ((E[idx].type & 0377) == 0)
			E[idx].type |= defund;
		t = E[idx].type & 037;
		if (t >= 5)
			continue;
		if (t == 4)
			E[idx].value += bssbase;
		else if (t == 3)
			E[idx].value += datbase;
	}
	oset(&txtp, 0);
	oset(&relp, seek[S_TREL]);
	putw2(&txtp, 0407);
	putw2(&txtp, txtsiz);
	putw2(&txtp, datsiz);
	putw2(&txtp, bsssiz);
	putw2(&txtp, symsiz);
	putw2(&txtp, stksiz);
	putw2(&txtp, exorig);
	putw2(&txtp, 0);
	assem();

	istext = 0;
	oflush(&txtp);
	oflush(&relp);

	oset(&txtp, seek[S_SYM]);
	for (i = 0; i < nuser; i++) {
		int k;

		for (k = 0; k < 8; k += 2)
			putw2(&txtp, (unsigned char)names[i][k] | (unsigned char)names[i][k + 1] << 8);
		putw2(&txtp, E[NBSYM + i].type);
		putw2(&txtp, E[NBSYM + i].value);
	}
	oflush(&txtp);
	aexit();
}

int
main(int argc, char **argv)
{
	int i;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, sintr);
	if (argc < 5) {
		fprintf(stderr, "usage: %s tmp1 tmp2 tmp3 tmp4 [-g]\n", argv[0]);
		return ASEXIT_ERR;
	}
	if (argc >= 6)
		defund = 040;
	for (i = 0; i < 4; i++)
		a_tmp[i] = argv[i + 1];
	txtfil = ofile(a_tmp[0]);
	fbfil = ofile(a_tmp[1]);
	symf = ofile(a_tmp[2]);
	fin = symf;
	if ((fout = open("a.out", O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
		filerr("a.out");
	go();
	return 0;
}
