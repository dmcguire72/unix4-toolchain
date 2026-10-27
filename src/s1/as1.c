/*
 * Unix Fourth Edition -- as1.c -- as (1)
 * 
 * Original assembly:
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 * 
 * C Port:
 * Copyright 2026 Daniel McGuire
 */
#include "as.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

enum {
	T_UNDEF = 0, T_ABS = 1, T_TEXT = 2, T_DATA = 3, T_BSS = 4,
	T_FLOP_FREG = 5, T_BRANCH = 6, T_JSR = 7, T_RTS = 8, T_SYS = 9,
	T_MOVF = 10, T_DOUBLE = 11, T_FLOP_FSRC = 12, T_SINGLE = 13,
	T_BYTE = 14, T_STRING = 15, T_EVEN = 16, T_IF = 17, T_ENDIF = 18,
	T_GLOBL = 19, T_REG = 20, T_TEXTSEG = 21, T_DATASEG = 22,
	T_BSSSEG = 23, T_MUL = 24, T_SOB = 25, T_COMM = 26,
	T_JBR = 29, T_JCC = 30,
	GLOBAL = 040, TYPEMASK = 037
};

#define EOT	004

struct usym {
	char name[8];
	uint16_t type;
	uint16_t value;
};

static struct usym *S;
static int nS, capS;
static int *hash;
static int hashcap;

#define SYMTOK(i)	(0200 + (i))
#define SYM(t)		(S[(t) - 0200])
#define dot		(S[0].value)
#define dotrel		(S[0].type)

static int r0, r1, r2, r3, r4;

static int ch;
static unsigned char inbuf[512];
static int inbfcnt, inbfp;
static int fin;
static int nargs;
static char **argv0, **curarg;
static int ifflg;
static int line;
static int savop;
static int numval;
static int opfound;
static int errflg;
static int fileflg;
static int curfb[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
static int curfbr[10];
static int savdot[3];
static int unglob = 1;

static char tmpname[4][512];
static int pof;
static int fbfil;
static uint16_t outbuf[256];
static int obufp;
static int nblocks = 126;

static char symbol[8];

static const signed char chartab[128] = {
#include "../tables/chartab.inc"
};

static const char *tmpdir_cached;
const char *
as_tmpdir(void)
{
	if (tmpdir_cached == NULL) {
		const char *t = getenv("TMPDIR");

		tmpdir_cached = (t != NULL && *t != '\0') ? t : "/tmp";
	}
	return tmpdir_cached;
}

static void
aexit(void)
{
	int i;

	for (i = 0; i < 4; i++)
		if (tmpname[i][0] != '\0')
			unlink(tmpname[i]);
	exit(errflg ? ASEXIT_ERR : ASEXIT_OK);
}

static void
sintr(int sig)
{
	(void)sig;
	aexit();
}

static void
error(int code)
{
	char buf[8];
	int i, l;

	errflg++;
	if (curarg != NULL && *curarg != NULL) {
		const char *n = *curarg;

		*curarg = NULL;
		write(2, n, strlen(n));
		write(2, "\n\0", 2);
	}
	l = line;
	buf[0] = code;
	buf[1] = ' ';
	for (i = 5; i >= 2; i--) {
		buf[i] = l % 10 + '0';
		l /= 10;
	}
	buf[6] = '\n';
	write(2, buf, 7);
}

static int
fcreat(int k)
{
	char *n = tmpname[k];
	int fd;

	for (;;) {
		fd = open(n, O_RDWR | O_CREAT | O_EXCL, 0444);
		if (fd >= 0)
			return fd;
		if (errno != EEXIST || n[strlen(n) - 1] >= 'z') {
			fprintf(stderr, "%s?\n", n);
			n[0] = '\0';
			aexit();
		}
		n[strlen(n) - 1]++;
	}
}

static void
wr(int fd, const void *p, size_t n)
{
	if (write(fd, p, n) != (ssize_t)n) {
		perror("as: write");
		errflg++;
		aexit();
	}
}

static void
putw1(int w)
{
	if (ifflg != 0 && w != '\n')
		return;
	outbuf[obufp++] = (uint16_t)w;
	if (obufp >= 256) {
		obufp = 0;
		wr(pof, outbuf, 512);
		if (--nblocks == 0) {
			close(pof);
			tmpname[3][strlen(tmpname[3]) - 1] = 'a';
			pof = fcreat(3);
		}
	}
}

static int
rch(void)
{
	for (;;) {
		if (ch != 0) {
			r0 = ch;
			ch = 0;
			return r0;
		}
		if (--inbfcnt >= 0) {
			r0 = inbuf[inbfp++] & 0177;
			if (r0 == 0)
				continue;
			return r0;
		}
		if (fin != 0) {
			ssize_t n = read(fin, inbuf, 512);

			if (n > 0) {
				inbfcnt = n;
				inbfp = 0;
				continue;
			}
			close(fin);
			fin = 0;
		}
		if (--nargs <= 0) {
			r0 = EOT;
			return r0;
		}
		if (ifflg != 0) {
			error('i');
			aexit();
		}
		curarg++;
		fileflg++;
		if ((fin = open(*curarg, O_RDONLY)) < 0) {
			fin = 0;
			fprintf(stderr, "%s?\n", *curarg);
			errflg++;
			aexit();
		}

		if (fin == 0) {
			fin = dup(0);
			close(0);
		}
		line = 1;
		putw1(5);
		for (char *p = *curarg; *p != '\0'; p++)
			putw1((unsigned char)*p);
		putw1(-1);
		inbfcnt = 0;
	}
}

static int
rnch(void)
{
	int v;

	rch();
	v = chartab[r0];
	if (v > 0)
		return v;
	ch = r0;
	return 0;
}

static unsigned
hashsym(const char *n)
{
	unsigned h = 0;
	int i;

	for (i = 0; i < 8; i++)
		h = h * 31 + (unsigned char)n[i];
	return h;
}

static void
rehash(int newcap)
{
	int i, j;

	free(hash);
	hash = malloc(newcap * sizeof *hash);
	if (hash == NULL) {
		fprintf(stderr, "as: out of memory\n");
		aexit();
	}
	hashcap = newcap;
	for (i = 0; i < hashcap; i++)
		hash[i] = -1;
	for (i = 0; i < nS; i++) {
		j = hashsym(S[i].name) & (hashcap - 1);
		while (hash[j] >= 0)
			j = (j + 1) & (hashcap - 1);
		hash[j] = i;
	}
}

static void
symsetup(void)
{
	int i;

	capS = 1024;
	S = malloc(capS * sizeof *S);
	if (S == NULL) {
		fprintf(stderr, "as: out of memory\n");
		exit(ASEXIT_ERR);
	}
	for (i = 0; i < NBSYM; i++) {
		memset(S[i].name, 0, 8);
		strncpy(S[i].name, bsymtab[i].name, 8);
		S[i].type = bsymtab[i].type;
		S[i].value = bsymtab[i].value;
	}
	nS = NBSYM;
	rehash(2048);
}

static int
lookupsym(int *created)
{
	int j = hashsym(symbol) & (hashcap - 1);

	while (hash[j] >= 0) {
		if (memcmp(S[hash[j]].name, symbol, 8) == 0) {
			*created = 0;
			return hash[j];
		}
		j = (j + 1) & (hashcap - 1);
	}
	if (nS - NBSYM >= (0177777 - TOK_USER) / 4) {
		fprintf(stderr, "as: too many symbols\n");
		errflg++;
		aexit();
	}
	if (nS >= capS) {
		capS *= 2;
		S = realloc(S, capS * sizeof *S);
		if (S == NULL) {
			fprintf(stderr, "as: out of memory\n");
			aexit();
		}
	}
	memcpy(S[nS].name, symbol, 8);
	S[nS].type = 0;
	S[nS].value = 0;
	hash[j] = nS;
	*created = 1;
	nS++;
	if (nS * 2 > hashcap)
		rehash(hashcap * 2);
	return nS - 1;
}

static void
rname(void)
{
	int i, idx, created;

	for (i = 0; i < 8; i++)
		symbol[i] = rnch();
	while (rnch() != 0)
		;
	idx = lookupsym(&created);
	(void)created;
	if (idx < NBSYM)
		putw1(TOK_BUILTIN + 4 * idx);
	else
		putw1(TOK_USER + 4 * (idx - NBSYM));
	r4 = SYMTOK(idx);
	r1 = 0;
}

static int
fbcheck(int n)
{
	if ((unsigned)n > 9) {
		error('f');
		return 0;
	}
	return n;
}

static int
number(void)
{
	int d = 0, o = 0;

	for (;;) {
		rch();
		if (r0 < '0' || r0 > '9')
			break;
		d = (int16_t)(d * 10 + r0 - '0');
		o = (int16_t)((o << 3) + r0 - '0');
	}
	r1 = o;
	if (r0 == 'b' || r0 == 'f') {
		int c = r0;
		int n = fbcheck(d);

		r0 = n + 0141;
		if (c != 'b')
			r0 += 10;
		r4 = r0;
		return 1;
	}
	if (r0 == '.') {
		r1 = d;
		r0 = 0;
	}
	ch = r0;
	r0 = r1;
	return 0;
}

static const unsigned char schar[] = {
	'n', 012, 't', 011, 'e', 004, '0', 000, 'r', 015, 'a', 006, 'p', 033,
	0, 0xff
};

static void
rsch(void)
{
	const unsigned char *p;

	rch();
	if (r0 == EOT || r0 == '\n') {
		error('<');
		aexit();
	}
	r1 = 0;
	if (r0 == '\\') {
		rch();
		for (p = schar; ; p += 2) {
			if (p[0] == r0) {
				r0 = p[1];
				r1 = 0;
				return;
			}
			if (p[1] >= 0x80)
				return;
		}
	}
	if (r0 == '>')
		r1 = 1;
}

static void
squote(void)
{
	rsch();
	numval = r0;
	putw1(1);
	putw1(numval);
	r4 = 1;
}

static void
dquote(void)
{
	int c1;

	rsch();
	c1 = r0;
	rsch();
	r0 = (int16_t)(((r0 & 0377) << 8) | (c1 & 0377));
	numval = r0;
	putw1(1);
	putw1(numval);
	r4 = 1;
}

static void
string(void)
{
	putw1('<');
	numval = 0;
	for (;;) {
		rsch();
		if (r1 != 0)
			break;
		putw1(r0 | 0400);
		numval++;
	}
	putw1(-1);
	r4 = '<';
}

static const unsigned char esctab[] = {
	'/', '/', '<', 035, '>', 036, '%', 037, 0, 0
};

static int
rd8(void)
{
	for (;;) {
		int v, k;

		rch();
		r4 = r0;
		v = chartab[r0];
		if (v > 0) {
			ch = r0;
			if (v >= '0' && v <= '9') {
				if (number() == 0) {
					numval = r0;
					putw1(1);
					putw1(numval);
					r4 = 1;
					return 0;
				}
				return 1;
			}
			rname();
			return 0;
		}
		k = 12 + v / 2;
		switch (k) {
		case 1:
			r4 = 037;
			return 1;
		case 2:
			rch();
			for (const unsigned char *p = esctab; ; p += 2) {
				if (p[0] == r0) {
					r4 = p[1];
					break;
				}
				if (p[1] == 0)
					break;
			}
			return 1;
		case 3:
			continue;
		case 4:
		case 11:
			return 1;
		case 5:
			dquote();
			return 0;
		case 6:
			error('g');
			continue;
		case 7:
			squote();
			return 0;
		case 9:
			for (;;) {
				rch();
				r4 = r0;
				if (r0 == EOT || r0 == '\n')
					return 1;
			}
		case 12:
			string();
			return 0;
		}
		return 1;
	}
}

static void
readop(void)
{
	if (savop != 0) {
		r4 = savop;
		savop = 0;
		return;
	}
	if (rd8())
		putw1(r4);
}

static void
errore(void)
{
	error('e');
}

static void
errora(void)
{
	error('a');
}

static int16_t
combin_type(int sub)
{
	int g = (r0 | r3) & GLOBAL;
	int t1 = r0 & TYPEMASK, t2 = r3 & TYPEMASK, lo, hi, res;

	lo = t1 < t2 ? t1 : t2;
	hi = t1 < t2 ? t2 : t1;
	if (lo == 0)
		res = 0;
	else if (sub && lo == hi)
		res = 1;
	else
		res = hi;
	return res | g;
}

static void
dvd16(int dividend, int divisor, int *quo, int *rem)
{
	long q;

	*quo = 0;
	*rem = dividend;
	if ((int16_t)divisor == 0)
		return;
	q = (long)(uint16_t)dividend / (int16_t)divisor;
	if (q > 32767 || q < -32768)
		return;
	*quo = q;
	*rem = (long)(uint16_t)dividend % (int16_t)divisor;
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

static void
expres(void)
{
	int pend = '+';
	int q, rm;

	opfound = 0;
	r2 = 0;
	r3 = 1;
	goto first;

advanc:
	readop();
first:
	r0 = r4;
	if (r4 < 0 || r4 > 0177) {
		r0 = SYM(r4).type & 0377;
		if (r0 >= 0200)
			r0 -= 256;
		r1 = SYM(r4).value;
		goto oprand;
	}
	if (r4 >= 0141) {
		if (r4 >= 0141 + 10) {
			r3 = 0;
			r2 = 0;
			goto oprand;
		}
		r0 = curfbr[r4 - 0141];
		r2 = curfb[r4 - 0141];
		if (r2 < 0)
			error('f');
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
		expres();
		if (r4 != ']')
			error(']');
		r0 = r3;
		r1 = r2;
		r3 = s3;
		r2 = s2;
		goto oprand;
	}
	case 1:
		r1 = numval;
		r0 = 1;
		goto oprand;
	}
	if (!opfound)
		errore();
	return;

oprand:
	opfound++;
	switch (pend) {
	case '+':
		r3 = combin_type(0);
		r2 = (int16_t)(r2 + r1);
		break;
	case '-':
		r3 = combin_type(1);
		r2 = (int16_t)(r2 - r1);
		break;
	case '*':
		r3 = combin_type(0);
		r2 = (int16_t)(r1 * r2);
		break;
	case '/':
		r3 = combin_type(0);
		dvd16(r2, r1, &q, &rm);
		r2 = (int16_t)q;
		break;
	case 037:
		r3 = combin_type(0);
		r2 = (int16_t)(r2 | r1);
		break;
	case '&':
		r3 = combin_type(0);
		r2 = (int16_t)(r2 & r1);
		break;
	case 035:
		r3 = combin_type(0);
		r2 = ashift(r2, r1);
		break;
	case 036:
		if ((int16_t)(-r1) == 0) {
			r3 = combin_type(0);
			r2 = ashift(r2, 0);
			break;
		}
		r1 = (int16_t)(-r1) + 1;
		r2 = (int16_t)((uint16_t)r2 >> 1);
		r3 = combin_type(0);
		r2 = ashift(r2, r1);
		break;
	case '%':
		r3 = combin_type(0);
		dvd16(r2, r1, &q, &rm);
		r2 = (int16_t)rm;
		break;
	case '!':
		r3 = combin_type(0);
		r2 = (int16_t)(r2 + ~r1);
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
	if ((uint16_t)r2 > 7 || (r3 != 1 && (uint16_t)r3 <= 4))
		errora();
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

static int
addres(void)
{
	int save;

	switch (r4) {
	case '(':
		readop();
		expres();
		checkrp();
		checkreg();
		if (r4 == '+') {
			readop();
			return 0;
		}
		return 2;
	case '-':
		readop();
		if (r4 == '(') {
			readop();
			expres();
			checkrp();
			checkreg();
			return 0;
		}
		savop = r4;
		r4 = '-';
		goto getx;
	case '$':
		readop();
		expres();
		dot += 2;
		return 0;
	case '*':
		readop();
		if (r0 == '*') {
			errora();
			return 0;
		}
		save = addres();
		dot += save;
		return 0;
	}
getx:
	expres();
	if (r4 == '(') {
		readop();
		expres();
		checkreg();
		checkrp();
		dot += 2;
		return 0;
	}
	if (r3 == 024) {
		checkreg();
		return 0;
	}
	dot += 2;
	return 0;
}

static void
opl13(void)
{
	addres();
	if (r4 != ',') {
		errora();
		return;
	}
	readop();
	addres();
	dot += 2;
}

static void
opl15(void)
{
	addres();
	dot += 2;
}

static void
opl6(void)
{
	expres();
	dot += 2;
}

static void
opl31(void)
{
	expres();
	if (r4 != ',')
		errora();
	readop();
	opl6();
}

static void
opl16(void)
{
	for (;;) {
		expres();
		dot += 1;
		if (r4 != ',')
			return;
		readop();
	}
}

static void
opl35(int size)
{
	expres();
	if (r3 == dotrel) {
		int d = r2 - dot;

		d = (int16_t)d;
		if (d < 0 && d >= -0376)
			size = 2;
	}
	dot += size;
}

static void
oplglobl(void)
{
	while (r4 >= 0200) {
		SYM(r4).type |= GLOBAL;
		readop();
		if (r4 != ',')
			return;
		readop();
	}
}

static void
opl32(void)
{
	if (r4 >= 0200) {
		SYM(r4).type |= GLOBAL;
		readop();
		if (r4 == ',') {
			readop();
			expres();
			return;
		}
	}
	error('x');
}

static void
oplseg(int type)
{
	int i = dotrel - 2 > 2 ? 2 : (int)(dotrel - 2);

	savdot[i] = dot;
	dot = savdot[type - T_TEXTSEG];
	dotrel = type - T_TEXTSEG + 2;
}

static void
xpr(void)
{
	expres();
	dot += 2;
}

static void
opline(void)
{
	int t;

	if (r4 >= 0 && r4 <= 0200) {
		if (r4 != '<') {
			xpr();
			return;
		}
		dot += numval;
		readop();
		return;
	}
	t = SYM(r4).type & 0377;
	if (t >= 0200)
		t -= 256;
	r0 = t;
	if (t == T_REG || t < T_FLOP_FREG || t > T_JCC) {
		xpr();
		return;
	}
	readop();
	switch (t) {
	case 5: case 10: case 11: case 12: case 24:
		opl13();
		break;
	case 6: case 8: case 9:
		opl6();
		break;
	case 7:
		opl13();
		break;
	case 13:
		opl15();
		break;
	case 14:
		opl16();
		break;
	case 15:
		dot += numval;
		readop();
		break;
	case 16:
		dot++;
		dot &= ~1;
		break;
	case 17:
		expres();
		if (r3 == 0)
			error('U');
		if (r2 == 0)
			ifflg++;
		break;
	case 18:
		break;
	case 19:
		oplglobl();
		break;
	case 20:
	case 27:
	case 28:
		xpr();
		break;
	case 21: case 22: case 23:
		oplseg(t);
		break;
	case 25:
		opl31();
		break;
	case 26:
		opl32();
		break;
	case 29:
		opl35(4);
		break;
	case 30:
		opl35(6);
		break;
	}
}

static void
assem(void)
{
	int s;

	for (;;) {
		readop();
		if (r4 == '\n' || r4 == ';' || r4 == EOT)
			goto ealoop;
		if (ifflg) {
			if (r4 <= 0200)
				continue;
			if ((SYM(r4).type & 0377) == T_IF)
				ifflg++;
			if ((SYM(r4).type & 0377) == T_ENDIF)
				ifflg--;
			continue;
		}
		s = r4;
		readop();
		if (r4 == '=') {
			int p;

			readop();
			expres();
			p = s;
			if (p < 0200) {
				error('x');
				goto ealoop;
			}
			if (p == SYMTOK(0)) {
				r3 &= ~040;
				if (r3 != dotrel) {
					error('.');
					dotrel = 2;
					goto ealoop;
				}
			}
			SYM(p).type &= ~TYPEMASK;
			r3 &= TYPEMASK;
			if (r3 == 0)
				r2 = 0;
			SYM(p).type |= r3;
			SYM(p).value = r2;
			goto ealoop;
		}
		if (r4 == ':') {
			if (s >= 0200) {
				if ((SYM(s).type & TYPEMASK) != 0)
					error('m');
				SYM(s).type |= dotrel & 0377;
				SYM(s).value = dot;
				continue;
			}
			if (s == 1) {
				int n = fbcheck(numval);
				unsigned char rec[4];

				curfbr[n] = dotrel & 0377;
				rec[0] = dotrel & 0377;
				rec[1] = n * 2;
				rec[2] = dot & 0377;
				rec[3] = (dot >> 8) & 0377;
				curfb[n] = dot;
				wr(fbfil, rec, 4);
				continue;
			}
			error('x');
			continue;
		}
		savop = r4;
		r4 = s;
		opline();
ealoop:
		if (r4 == ';')
			continue;
		if (r4 == '\n') {
			line++;
			continue;
		}
		if (r4 == EOT) {
			if (ifflg)
				error('x');
			return;
		}
		error('x');
		for (;;) {
			if (r4 == '\n' || r4 == ';' || r4 == EOT)
				break;
			readop();
		}
	}
}

static void
mktmpnames(void)
{
	const char *d = as_tmpdir();
	int i;

	for (i = 0; i < 4; i++)
		snprintf(tmpname[i], sizeof tmpname[i], "%s/atm%d%c", d, i + 1,
		    i == 3 ? 'Z' : 'a');
}

static void
exec_pass2(char **args)
{
	char self[1024], path[1100];
	const char *e = getenv("AS2");
	ssize_t n;
	char *sl;

	if (e != NULL && *e != '\0') {
		execv(e, args);
	} else {
		n = readlink("/proc/self/exe", self, sizeof self - 1);
		if (n > 0) {
			self[n] = '\0';
		} else if (argv0 != NULL && argv0[0] != NULL && strchr(argv0[0], '/')) {
			snprintf(self, sizeof self, "%s", argv0[0]);
			n = 1;
		} else
			n = 0;
		if (n > 0 && (sl = strrchr(self, '/')) != NULL) {
			snprintf(path, sizeof path, "%.*s/as2", (int)(sl - self), self);
			execv(path, args);
		} else
			execvp("as2", args);
	}
	fprintf(stderr, "as: cannot execute pass 2 (as2)\n");
}

int
main(int argc, char **argv)
{
	int fd, i, n;
	char *args[8];

	argv0 = argv;
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, sintr);
	if (argc > 1 && argv[1][0] == '-') {
		nargs = argc - 1;
		curarg = argv + 1;
	} else {
		unglob = 0;
		nargs = argc;
		curarg = argv;
	}
	if (nargs < 2 && argc < 2) {
		fprintf(stderr, "usage: as [-] file ...\n");
		return ASEXIT_ERR;
	}
	mktmpnames();
	pof = fcreat(0);
	fbfil = fcreat(1);
	symsetup();
	dot = 0;
	dotrel = 2;

	assem();

	wr(pof, outbuf, 512);
	close(pof);
	close(fbfil);
	if (errflg)
		aexit();

	fd = fcreat(2);
	{
		size_t cnt = nS - NBSYM;
		unsigned char rec[12];

		for (i = 0; (size_t)i < cnt; i++) {
			struct usym *u = &S[NBSYM + i];

			memcpy(rec, u->name, 8);
			rec[8] = u->type & 0377;
			rec[9] = u->type >> 8;
			rec[10] = u->value & 0377;
			rec[11] = u->value >> 8;
			wr(fd, rec, 12);
		}
	}
	close(fd);

	n = 0;
	args[n++] = (char *)"as2";
	for (i = 0; i < 4; i++)
		args[n++] = tmpname[i];
	if (unglob)
		args[n++] = (char *)"-g";
	args[n] = NULL;
	exec_pass2(args);
	aexit();
	return ASEXIT_ERR;
}
