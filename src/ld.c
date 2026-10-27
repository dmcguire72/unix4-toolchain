/*
 * Unix Fourth Edition -- ld.c -- ld (1)
 *
 * Original assembly:
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 *
 * C Port:
 * Copyright 2026 Daniel McGuire
 */
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define LDEXIT_OK	0
#define LDEXIT_ERR	1

#define T_FILE		037
#define T_UNDEF		040
#define T_ABS		041
#define T_TEXT		042
#define T_DATA		043
#define T_BSS		044
#define T_COMMON	047

#define OMAGIC		0407
#define ARMAGIC		0177555

#define SYMSZ		12
#define HDRSZ		16
#define BLK		512

#define RLISTMAX	128
#define LOCALMAX	256

struct sym {
	char name[8];
	uint16_t type;
	uint16_t value;
};

static const char undmes[] = "un: ";
static const char movemes[] = "Can't move output file";
static const char norel[] = "No relocation bits: ";
static const char toomany[] = "Too many routines loaded at: ";
static const char multi[] = "Multiply defined: ";
static const char locovflo[] = "Too many symbols in: ";
static const char relerr[] = "Relocation error in: ";
static const char premeof[] = "Premature EOF on: ";
static const char outfile[] = "ld: can't create l.out\n";
static const char fnotfound[] = "File not found: ";
static const char format[] = "Format error: ";
static const char snotfound[] = "Symbol not found: ";
static const char mulent[] = "Multiple entry point:";
static const char libfil[] = "-l";

static long libflg;
static long libnxt;
static int sqflg;
static int dcom;
static int nflg;
static int xtflg;
static int entptr;
static uint16_t entry;
static uint16_t exorig;
static int relflg = 1;
static int errflg;
static int outmod = 0777;

static uint16_t txtsiz, datsiz, bsssiz, symsiz, stksiz;
static uint16_t txtorg, datorg, bssorg;
static uint16_t fdatorg, fbssorg;

static long ctxtorg, cdatorg, ctrelorg, cdrelorg, csymorg;
static uint16_t ctxtsiz, cdatsiz, cbsssiz, ctrelsiz, cdrelsiz, csymsiz;

static uint8_t arcmag[42];

static int argc;
static int argp;
static char **av;

static uint16_t locsymsiz;
static int fout = -1;
static int fin = -1;
static int reopened;
static int ndef;
static const char *filnam;
static uint16_t relbas;
static char wlib = 'a';
static struct sym symbol;
static const uint16_t dotdot = 0;
static const char *libdir;
static char libbuf[PATH_MAX + 32];
static mode_t um;

static uint16_t
rd16(const uint8_t *p)
{
	return (uint16_t)(p[0] | (p[1] << 8));
}

static void
wr16(uint8_t *p, uint16_t v)
{
	p[0] = v & 0377;
	p[1] = v >> 8;
}

static void
emit(const char *s, size_t n)
{
	if (n != 0)
		write(2, s, n);
}

static void
sintr(void)
{
	unlink("l.out");
	exit(LDEXIT_ERR);
}

static void
sigint(int sig)
{
	(void)sig;
	sintr();
}

static void
mesg(const char *m)
{
	outmod = 0666;
	errflg++;
	emit(m, strlen(m));
	if (filnam != NULL)
		emit(filnam, strlen(filnam));
	emit("\n", 1);
}

static void
smesg(const char *m, const struct sym *s)
{
	int i;

	outmod = 0666;
	errflg++;
	emit(m, strlen(m));
	if (filnam != NULL) {
		emit(filnam, strlen(filnam));
		emit(";", 1);
	}
	for (i = 0; i < 8; i++)
		if (s->name[i] != '\0')
			emit(&s->name[i], 1);
	emit("\n", 1);
}

static struct sym *symtab;
static int nsym, capsym;
static int *hshtab;
static int hshcap;

static uint32_t
hashname(const char *n)
{
	uint32_t h = 2166136261u;
	int i;

	for (i = 0; i < 8; i++) {
		h ^= (unsigned char)n[i];
		h *= 16777619u;
	}
	return h;
}

static void
hash_insert(int idx)
{
	uint32_t h = hashname(symtab[idx].name) & (hshcap - 1);

	while (hshtab[h] != 0)
		h = (h + 1) & (hshcap - 1);
	hshtab[h] = idx + 1;
}

static void
hash_grow(void)
{
	int i;

	free(hshtab);
	hshcap = hshcap ? hshcap * 2 : 1024;
	hshtab = calloc(hshcap, sizeof *hshtab);
	if (hshtab == NULL) {
		write(2, "ld: out of memory\n", 18);
		sintr();
	}
	for (i = 0; i < nsym; i++)
		hash_insert(i);
}

static int
lookup(void)
{
	uint32_t h;

	if (hshcap == 0)
		return -1;
	h = hashname(symbol.name) & (hshcap - 1);
	while (hshtab[h] != 0) {
		if (memcmp(symtab[hshtab[h] - 1].name, symbol.name, 8) == 0)
			return hshtab[h] - 1;
		h = (h + 1) & (hshcap - 1);
	}
	return -1;
}

static int
enter(void)
{
	if (nsym >= capsym) {
		capsym = capsym ? capsym * 2 : 256;
		symtab = realloc(symtab, capsym * sizeof *symtab);
		if (symtab == NULL) {
			write(2, "ld: out of memory\n", 18);
			sintr();
		}
	}
	if ((nsym + 1) * 2 > hshcap)
		hash_grow();
	symtab[nsym] = symbol;
	hash_insert(nsym);
	return nsym++;
}

static void
unhash(int keep)
{
	uint32_t h;
	int i;

	for (i = nsym - 1; i >= keep; i--) {
		h = hashname(symtab[i].name) & (hshcap - 1);
		while (hshtab[h] != i + 1)
			h = (h + 1) & (hshcap - 1);
		hshtab[h] = 0;
	}
	nsym = keep;
}

static int
issymbol(const struct sym *s, const char *n)
{
	return memcmp(s->name, n, 8) == 0;
}

static const char s_end[8] = "_end";
static const char s_etext[8] = "_etext";
static const char s_edata[8] = "_edata";

struct istr {
	long nextseek;
	long left;
	int wp;
	uint8_t buf[BLK];
};

static struct istr txtp, relp;

static void
rdchk(uint8_t *buf, long cnt, long off)
{
	ssize_t n = pread(fin, buf, cnt, off);

	if (n != cnt) {
		mesg(premeof);
		sintr();
	}
}

static void
attach(struct istr *s, long org, uint16_t size)
{
	long nb = (org | (BLK - 1)) + 1;
	long n = nb - org;

	s->nextseek = nb;
	s->left = size;
	if (size < n)
		n = size;
	rdchk(s->buf + (BLK - n), n, org);
	s->wp = BLK - (int)n;
}

static int
getwrd(struct istr *s, uint16_t *w)
{
	long n;

	s->left -= 2;
	if (s->left < 0)
		return -1;
	if (s->wp < BLK) {
		*w = rd16(s->buf + s->wp);
		s->wp += 2;
		return 0;
	}
	n = s->left + 2;
	if (n > BLK)
		n = BLK;
	rdchk(s->buf, n, s->nextseek);
	s->nextseek += BLK;
	*w = rd16(s->buf);
	s->wp = 2;
	return 0;
}

static int
getsym(void)
{
	uint8_t b[SYMSZ];
	uint16_t w;
	int i;

	for (i = 0; i < SYMSZ / 2; i++) {
		if (getwrd(&txtp, &w) != 0)
			return -1;
		wr16(b + 2 * i, w);
	}
	memcpy(symbol.name, b, 8);
	symbol.type = rd16(b + 8);
	symbol.value = rd16(b + 10);
	return 0;
}

struct ostr {
	long seek;
	int n;
	uint8_t buf[BLK];
};

static struct ostr otxtp, odatp, osymp, otrelp, odrelp;
static long oattpt;

static void
oattach(struct ostr *s, uint16_t size)
{
	oattpt += size;
	s->seek = oattpt;
	s->n = 0;
}

static void
flush(struct ostr *s)
{
	if (s->n > 0)
		pwrite(fout, s->buf, s->n, s->seek);
	s->seek = (s->seek | (BLK - 1)) + 1;
	s->n = 0;
}

static void
putwrd(struct ostr *s, uint16_t w)
{
	if (((s->seek & (BLK - 1)) + s->n) >= BLK)
		flush(s);
	wr16(s->buf + s->n, w);
	s->n += 2;
}

static void
wrsym(struct ostr *s, const struct sym *sy)
{
	uint8_t b[SYMSZ];
	int i;

	memcpy(b, sy->name, 8);
	wr16(b + 8, sy->type);
	wr16(b + 10, sy->value);
	for (i = 0; i < SYMSZ / 2; i++)
		putwrd(s, rd16(b + 2 * i));
}

enum { RL_END, RL_FILE, RL_LIB, RL_CONT };

struct rl {
	int kind;
	const char *name;
	char letter;
	long lflg;
};

static struct rl rlist[RLISTMAX + 1];
static int nrl;
static int rlistp;

static void done(void);

static const char *
libpath(char letter)
{
	snprintf(libbuf, sizeof libbuf, "%s/lib%c.a", libdir, letter);
	if (access(libbuf, F_OK) == 0)
		return libbuf;

	char fallback[PATH_MAX + 32];
	snprintf(fallback, sizeof fallback, "/opt/v4/sys/lib/lib%c.a", letter);
	if (access(fallback, F_OK) == 0) {
		snprintf(libbuf, sizeof libbuf, "%s", fallback);
		return libbuf;
	}

	return libbuf;
}

static void
findlibdir(const char *argv0)
{
	static char dir[PATH_MAX + 16];
	const char *e;
	char exe[PATH_MAX];
	ssize_t n;
	char *sl;

	if ((e = getenv("LD_LIB")) != NULL && *e != '\0') {
		libdir = e;
		return;
	}
	if ((e = getenv("CC11_LIB")) != NULL && *e != '\0') {
		libdir = e;
		return;
	}
	n = readlink("/proc/self/exe", exe, sizeof exe - 1);
	if (n > 0)
		exe[n] = '\0';
	else
		snprintf(exe, sizeof exe, "%s", argv0);
	if ((sl = strrchr(exe, '/')) != NULL) {
		*sl = '\0';
		snprintf(dir, sizeof dir, "%s/../lib", exe);
	} else
		snprintf(dir, sizeof dir, "../lib");
	libdir = dir;
}

static int
aopen(const char *name)
{
	reopened = 0;
	filnam = name;
	if (fin >= 0)
		close(fin);
	fin = open(name, O_RDONLY);
	if (fin < 0) {
		mesg(fnotfound);
		return -1;
	}
	return 0;
}

static void
addin(void)
{
	txtsiz += ctxtsiz;
	datsiz += cdatsiz;
	bsssiz += cbsssiz;
}

static void
relsym(void)
{
	uint16_t t = symbol.type & 037;

	if (t == 0 || t >= 5)
		return;
	switch (t) {
	case 2:
		symbol.value += txtorg;
		break;
	case 3:
		symbol.value += datorg;
		break;
	case 4:
		symbol.value += bssorg;
		break;
	}
}

static void
bform(void)
{
	mesg(format);
	sintr();
}

static int
filstrt(const uint8_t *h)
{
	uint16_t r0;

	if (rd16(h) != OMAGIC)
		bform();
	ctxtorg = libflg + HDRSZ;
	ctxtsiz = rd16(h + 2);
	cdatorg = ctxtorg + ctxtsiz;
	cdatsiz = rd16(h + 4);
	cbsssiz = rd16(h + 6);
	ctrelorg = cdatorg + cdatsiz;
	ctrelsiz = ctxtsiz;
	cdrelorg = ctrelorg + ctrelsiz;
	cdrelsiz = cdatsiz;
	csymorg = cdrelorg + cdrelsiz;
	csymsiz = rd16(h + 8);
	r0 = rd16(h + 10);
	if (r0 >= stksiz)
		stksiz = r0;
	entry = rd16(h + 12);
	if (rd16(h + 14) != 0) {
		mesg(norel);
		return 0;
	}
	return 1;
}

static int
libstrt(uint8_t *h)
{
	uint16_t size = rd16(h + 14);

	libnxt = (uint16_t)((size + 1) & ~1);
	h[12] = h[13] = 0;
	filnam = (const char *)h;
	return filstrt(h + HDRSZ);
}

static int
use(void)
{
	const char *p;
	int i, idx;

	if (--argc < 0)
		return 0;
	p = av[++argp];
	memset(symbol.name, 0, 8);
	for (i = 0; i < 8 && p[i] != '\0'; i++)
		symbol.name[i] = p[i];
	symbol.type = T_UNDEF;
	symbol.value = 0;
	idx = lookup();
	if (idx < 0)
		idx = enter();
	return idx + 1;
}

static int
specarg(void)
{
	char c;

	switch (av[argp][1]) {
	case 'u':
		use();
		break;
	case 'l':
		wlib = 'a';
		c = av[argp][2];
		if (c != '\0')
			wlib = c;
		av[argp] = (char *)libfil;
		return 1;
	case 'x':
		xtflg++;
		break;
	case 'e':
		entptr = use();
		break;
	case 'r':
		relflg = 0;
		break;
	case 's':
		sqflg++;
		xtflg++;
		break;
	case 'n':
		nflg++;
		break;
	case 'd':
		dcom++;
		break;
	}
	return 0;
}

static int
nxtarg(void)
{
	const char *name;
	ssize_t n;

	libflg += libnxt;
	if (libflg != 0) {
		n = pread(fin, arcmag + 2, 32, libflg);
		libflg += HDRSZ;
		if (n == 32)
			return libstrt(arcmag + 2);
	}
	for (;;) {
		if (--argc < 0)
			return 0;
		argp++;
		if (av[argp][0] == '-' && !specarg())
			continue;
		libflg = 0;
		libnxt = 0;
		name = av[argp] == libfil ? libpath(wlib) : av[argp];
		reopened = 0;
		if (aopen(name) < 0)
			continue;
		n = read(fin, arcmag, 34);
		if (n < HDRSZ)
			bform();
		if (rd16(arcmag) != ARMAGIC)
			return filstrt(arcmag);
		if (n < 34)
			bform();
		libflg = 2 + HDRSZ;
		return libstrt(arcmag + 2);
	}
}

static int
nxtarg2(void)
{
	struct rl *e;
	const char *name;
	long pos;
	ssize_t n;

	for (;;) {
		e = &rlist[rlistp++];
		if (e->kind == RL_END)
			return 0;
		if (e->kind != RL_CONT) {
			name = e->kind == RL_LIB ? libpath(e->letter) : e->name;
			if (aopen(name) < 0)
				continue;
		}
		libflg = e->lflg;
		pos = 0;
		if (libflg != 0) {
			e->lflg -= HDRSZ;
			pos = e->lflg;
		}
		n = pread(fin, arcmag, 32, pos);
		if (n < HDRSZ)
			bform();
		if (e->lflg == 0)
			return filstrt(arcmag);
		if (n != 32)
			bform();
		return libstrt(arcmag);
	}
}

static int
load1(void)
{
	int saved = nsym;
	int idx;
	int16_t ot;
	struct rl *e;

	txtorg = txtsiz;
	datorg = datsiz - ctxtsiz;
	bssorg = bsssiz - ctxtsiz - cdatsiz;
	locsymsiz = SYMSZ;
	ndef = 0;
	attach(&txtp, csymorg, csymsiz);
	while (getsym() == 0) {
		if ((int16_t)symbol.type < T_UNDEF) {
			if (xtflg == 0)
				locsymsiz += SYMSZ;
			continue;
		}
		idx = lookup();
		if (idx >= 0) {
			ot = (int16_t)symtab[idx].type;
			if (ot > T_UNDEF)
				continue;
			if (symbol.type == T_TEXT && symtab[idx].value != 0)
				continue;
			ndef++;
			if ((int16_t)symbol.type <= T_UNDEF) {
				ndef--;
				if ((int16_t)symtab[idx].value >=
				    (int16_t)symbol.value)
					continue;
			}
		} else {
			idx = enter();
			if ((int16_t)symbol.type <= T_UNDEF)
				continue;
		}
		relsym();
		symtab[idx].type = symbol.type;
		symtab[idx].value = symbol.value;
	}
	if (libflg != 0 && ndef == 0) {
		unhash(saved);
		return 0;
	}
	if (entry != 0) {
		entry += txtorg;
		if (exorig != 0)
			mesg(mulent);
		exorig = entry;
	}
	symsiz += locsymsiz;
	if (nrl >= RLISTMAX) {
		mesg(toomany);
		done();
	}
	e = &rlist[nrl++];
	e->lflg = libflg;
	if (reopened != 0) {
		e->kind = RL_CONT;
	} else if (av[argp] == libfil) {
		e->kind = RL_LIB;
		e->letter = wlib;
	} else {
		e->kind = RL_FILE;
		e->name = av[argp];
	}
	if (libflg != 0)
		reopened |= 1;
	addin();
	return 1;
}

struct localent {
	int idx;
	int sym;
};

static struct localent local[LOCALMAX];
static int nloc;

static int
lookloc(int n)
{
	int i;

	for (i = 0; i < nloc; i++)
		if (local[i].idx == n)
			return local[i].sym;
	mesg(snotfound);
	sintr();
	return 0;
}

static int
txtlod(uint16_t *wp, uint16_t *rp)
{
	uint16_t w, rw, r0, r2, r3;
	const struct sym *s;
	int sy;

	if (getwrd(&txtp, &w) != 0)
		return 0;
	r3 = w;
	if (getwrd(&relp, &rw) != 0) {
		mesg(relerr);
		exit(LDEXIT_ERR);
	}
	r2 = rw & ~1;
	r0 = rw & 017;
	if (r0 > 7) {
		sy = lookloc(r2 >> 4);
		s = &symtab[sy];
		if ((int16_t)s->type <= T_UNDEF) {
			uint16_t off = (uint16_t)(sy * SYMSZ + locsymsiz);
			r0 |= (uint16_t)((off / SYMSZ) << 4);
		} else {
			r3 += s->value;
			r2 = s->type - T_ABS;
			r0 &= 1;
			if (r0 == 0) {
				if (r2 != 0)
					r3 += dotdot;
			} else {
				if (r2 == 0)
					r3 -= dotdot;
			}
			r2 <<= 1;
			r0 |= r2;
		}
	} else {
		switch ((r2 >> 1) & 3) {
		case 1:
			r3 += txtorg;
			break;
		case 2:
			r3 += datorg;
			break;
		case 3:
			r3 += bssorg;
			break;
		}
	}
	if (r0 & 1)
		r3 -= relbas;
	*wp = r3;
	*rp = r0;
	return 1;
}

static void
load2(void)
{
	const char *p, *base;
	int i, idx, sy;
	uint16_t w, r;

	txtorg = txtsiz;
	datorg = fdatorg + datsiz - ctxtsiz;
	bssorg = fbssorg + bsssiz - ctxtsiz - cdatsiz;
	attach(&txtp, csymorg, csymsiz);
	nloc = 0;
	idx = -1;

	base = filnam;
	for (p = filnam; *p != '\0'; p++)
		if (*p == '/')
			base = p + 1;
	memset(symbol.name, 0, 8);
	for (i = 0; i < 8 && base[i] != '\0'; i++)
		symbol.name[i] = base[i];
	symbol.type = T_FILE;
	symbol.value = txtorg;
	if (sqflg == 0)
		wrsym(&osymp, &symbol);

	while (getsym() == 0) {
		relsym();
		idx++;
		if (symbol.type >= T_UNDEF) {
			sy = lookup();
			if (sy < 0) {
				mesg(snotfound);
				continue;
			}
			if ((int16_t)symbol.type > T_UNDEF) {
				if (symtab[sy].type != symbol.type ||
				    symtab[sy].value != symbol.value)
					smesg(multi, &symtab[sy]);
			} else if ((int16_t)symbol.type != T_UNDEF) {
				goto isloc;
			}
			if (nloc >= LOCALMAX) {
				mesg(locovflo);
				done();
			}
			local[nloc].idx = idx;
			local[nloc].sym = sy;
			nloc++;
			continue;
		}
	isloc:
		if (xtflg != 0)
			continue;
		wrsym(&osymp, &symbol);
	}

	attach(&txtp, ctxtorg, ctxtsiz);
	attach(&relp, ctrelorg, ctrelsiz);
	relbas = txtsiz;
	while (txtlod(&w, &r)) {
		if (relflg == 0)
			putwrd(&otrelp, r);
		putwrd(&otxtp, w);
	}
	attach(&txtp, cdatorg, cdatsiz);
	attach(&relp, cdrelorg, cdrelsiz);
	relbas = datorg;
	while (txtlod(&w, &r)) {
		if (relflg == 0)
			putwrd(&odrelp, r);
		putwrd(&odatp, w);
	}
	addin();
}

static void
done(void)
{
	unlink("a.out");
	if (link("l.out", "a.out") < 0) {
		filnam = NULL;
		mesg(movemes);
		exit(LDEXIT_ERR);
	}
	unlink("l.out");
	chmod("a.out", (outmod == 0777 ? 0777 : 0666) & ~um);
	exit(outmod == 0777 ? LDEXIT_OK : LDEXIT_ERR);
}

static void
pass2(void)
{
	uint16_t r0, r1, r2, r3, r4;
	uint16_t hdr[8];
	int i;
	struct sym *s;

	fout = open("l.out", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fout < 0) {
		filnam = NULL;
		mesg(outfile);
		exit(LDEXIT_ERR);
	}

	for (i = 0; i < nsym; i++) {
		s = &symtab[i];
		if (s->type != T_UNDEF || s->value != 0)
			continue;
		if (issymbol(s, s_end) || issymbol(s, s_etext) ||
		    issymbol(s, s_edata))
			continue;
		relflg = 0;
		break;
	}

	r1 = txtsiz;
	if (nflg) {
		r1 = (r1 + 077) & ~077;
		txtsiz = r1;
		r1 = (r1 + 017777) & ~017777;
	}
	r2 = datsiz;
	r3 = r1 + r2;
	r4 = 0;

	if (dcom != 0 || relflg != 0) {
		for (i = 0; i < nsym; i++) {
			s = &symtab[i];
			if (s->type != T_UNDEF || s->value == 0)
				continue;
			r0 = s->value;
			s->value = r4 + r3;
			r0 = (r0 + 1) & ~1;
			r4 += r0;
			s->type = T_COMMON;
		}
		bsssiz += r4;
	}

	for (i = 0; i < nsym; i++) {
		s = &symtab[i];
		if ((dcom != 0 || relflg != 0) &&
		    s->type == T_UNDEF && s->value == 0) {
			if (issymbol(s, s_end)) {
				s->type = T_BSS;
				s->value = bsssiz - r4;
			} else if (issymbol(s, s_etext)) {
				s->type = T_TEXT;
				s->value = txtsiz;
			} else if (issymbol(s, s_edata)) {
				s->type = T_DATA;
				s->value = datsiz;
			}
		}
		if ((int16_t)s->type < T_DATA)
			continue;
		if ((int16_t)s->type == T_DATA) {
			s->value += r1;
			continue;
		}
		if ((int16_t)s->type == T_COMMON) {
			s->type = T_BSS;
			continue;
		}
		s->value += r2;
		s->value += r4;
		s->value += r1;
	}

	fdatorg = r1;
	fbssorg = r3 + r4;
	locsymsiz = symsiz;
	symsiz += (uint16_t)(nsym * SYMSZ);
	if (sqflg)
		symsiz = 0;

	oattpt = 0;
	oattach(&otxtp, 0);
	oattpt += HDRSZ;
	oattach(&odatp, txtsiz);
	if (relflg == 0) {
		oattach(&otrelp, datsiz);
		oattach(&odrelp, txtsiz);
	}
	oattach(&osymp, datsiz);

	if (entptr != 0) {
		if (exorig != 0)
			mesg(mulent);
		exorig = symtab[entptr - 1].value;
		exorig++;
	}

	hdr[0] = OMAGIC + (nflg ? 1 : 0);
	hdr[1] = txtsiz;
	hdr[2] = datsiz;
	hdr[3] = bsssiz;
	hdr[4] = sqflg ? 0 : symsiz;
	hdr[5] = stksiz;
	hdr[6] = exorig;
	hdr[7] = relflg;
	for (i = 0; i < 8; i++)
		putwrd(&otxtp, hdr[i]);

	txtsiz = 0;
	datsiz = 0;
	bsssiz = 0;
	rlist[nrl].kind = RL_END;
	rlistp = 0;

	while (nxtarg2())
		load2();

	flush(&otxtp);
	flush(&odatp);
	if (relflg == 0) {
		flush(&otrelp);
		flush(&odrelp);
	}
	for (i = 0; i < nsym; i++) {
		s = &symtab[i];
		if (s->type == T_UNDEF && s->value == 0) {
			filnam = NULL;
			smesg(undmes, s);
		}
		if (sqflg)
			continue;
		wrsym(&osymp, s);
	}
	flush(&osymp);
	done();
}

int
main(int argc_, char **argv)
{
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, sigint);
	um = umask(0);
	umask(um);

	argc = argc_ - 1;
	if (argc <= 0)
		exit(LDEXIT_OK);
	av = argv;
	argp = 0;
	findlibdir(argv[0]);

	while (nxtarg())
		load1();
	pass2();
	return LDEXIT_OK;
}
