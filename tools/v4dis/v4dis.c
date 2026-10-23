// PDP-11 Disassembler
// Requires Unix V4 Binaries

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM 0x10000

static uint8_t img[MEM];
static unsigned ts, ds, bs, dbase, bbase, iend, magic;
static int opt_addr, opt_k, opt_v, opt_sig, opt_pad = 1, opt_budget = 700;
static bool collecting;

static const char *const as_builtin[] = {
	".", "..", "r0", "r1", "r2", "r3", "r4", "r5", "sp", "pc", "csw",
	"div", "ac", "mq", "mul", "sc", "sr", "nor", "lsh", "ash", "exit",
	"fork", "read", "write", "open", "close", "wait", "creat", "link",
	"unlink", "exec", "chdir", "time", "makdir", "chmod", "chown",
	"break", "stat", "seek", "tell", "mount", "umount", "setuid",
	"getuid", "stime", "fstat", "mdate", "stty", "gtty", "nice", "signal",
	"mov", "movb", "cmp", "cmpb", "bit", "bitb", "bic", "bicb", "bis",
	"bisb", "add", "sub", "br", "bne", "beq", "bge", "blt", "bgt", "ble",
	"bpl", "bmi", "bhi", "blos", "bvc", "bvs", "bhis", "bec", "bcc",
	"blo", "bcs", "bes", "jbr", "jne", "jeq", "jge", "jlt", "jgt", "jle",
	"jpl", "jmi", "jhi", "jlos", "jvc", "jvs", "jhis", "jec", "jcc",
	"jlo", "jcs", "jes", "clr", "clrb", "com", "comb", "inc", "incb",
	"dec", "decb", "neg", "negb", "adc", "adcb", "sbc", "sbcb", "tst",
	"tstb", "ror", "rorb", "rol", "rolb", "asr", "asrb", "asl", "aslb",
	"jmp", "swab", "jsr", "rts", "sys", "clc", "clv", "clz", "cln", "sec",
	"sev", "sez", "sen", "cfcc", "setf", "setd", "seti", "setl", "clrf",
	"negf", "absf", "tstf", "movf", "movif", "movfi", "movof", "movfo",
	"addf", "subf", "mulf", "divf", "cmpf", "modf", "movie", "movei",
	"ldfps", "stfps", "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "als",
	"alsc", "mpy", "mul", "div", "ash", "ashc", "dvd", "xor", "sxt",
	"mark", "sob", ".byte", ".even", ".if", ".endif", ".globl", ".text",
	".data", ".bss", ".comm",
	0
};

static unsigned rw(unsigned a) {
	a &= 0xffff; 
	return img[a] | img[(a + 1) & 0xffff] << 8; 
}

static uint8_t lpr[MEM + 1];
static char *lname[MEM + 1];
static char *named[MEM + 1];
static uint8_t callt[MEM + 1];
static uint8_t tblt[MEM + 1];
static uint8_t hasal[MEM + 1];
static struct sm { unsigned a; char *n; int ext, primary; } *sms;
static int nsm;
static char **used; static int nused;

static int in_seg(unsigned a) {
	return a < ts || (a >= dbase && a <= iend);
}

static int name_used(const char *n)
{
	int i;
	for (i = 0; i < nused; i++)
		if (!strcmp(used[i], n)) return 1;
	return 0;
}
static void name_add(const char *n)
{
	used = realloc(used, (nused + 1) * sizeof *used);
	used[nused++] = strdup(n);
}
static int is_builtin(const char *n)
{
	int i;
	for (i = 0; as_builtin[i]; i++)
		if (!strcmp(as_builtin[i], n)) return 1;
	return 0;
}
static int is_local(const char *n)
{
	if (n[0] != 'L' || !n[1]) return 0;
	for (n++; *n; n++)
		if (*n < '0' || *n > '9') return 0;
	return 1;
}
static int valid_name(const char *n)
{
	if (!*n || (*n >= '0' && *n <= '9')) return 0;
	for (; *n; n++)
		if (!((*n >= 'a' && *n <= 'z') || (*n >= 'A' && *n <= 'Z') ||
		      (*n >= '0' && *n <= '9') || *n == '_' || *n == '.'))
			return 0;
	return 1;
}
static void want(unsigned a, int prio)
{
	if (a > iend || !in_seg(a)) return;
	if (!lpr[a] || prio < lpr[a]) lpr[a] = prio;
}

static int load_aout(const char *path)
{
	FILE *f = fopen(path, "rb");
	uint8_t *buf;
	long len;
	unsigned h[8], ss, rf, off, i;
	int undef = 0;

	if (!f) {
		perror(path);
		return -1; 
	}
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = malloc(len + 1);
	if (fread(buf, 1, len, f) != (size_t)len)
	{ 
		fclose(f);
		return -1; 
	}
	fclose(f);
	if (len < 16) {
		fprintf(stderr, "%s: too short\n", path);
		return -1; 
	}

	for (i = 0; i < 8; i++) 
		h[i] = buf[2 * i] | buf[2 * i + 1] << 8;

	magic = h[0]; ts = h[1]; ds = h[2]; bs = h[3]; ss = h[4]; rf = h[7];

	if (magic != 0407 && magic != 0410) {
		fprintf(stderr, "%s: not a 0407/0410 a.out (magic %o)\n", path, magic);
		return -1;
	}

	if (16 + (long)ts + ds > len) {
		fprintf(stderr, "%s: truncated\n", path);
		return -1; 
	}

	dbase = magic == 0410 ? (ts + 8191) & ~8191u : ts;
	bbase = dbase + ds; iend = bbase + bs;

	if (iend > 0160000) {
		fprintf(stderr, "%s: image too large\n", path);
		return -1;
	}
	memcpy(img, buf + 16, ts);
	memcpy(img + dbase, buf + 16 + ts, ds);
	if ((ts | ds) & 1) fprintf(stderr, "warning: odd segment size; round trip not exact\n");
	off = 16 + ts + ds + (rf != 1 ? ts + ds : 0);
	for (i = 0; ss >= 12 && off + 12 * (i + 1) <= (unsigned long)len && i < ss / 12; i++) {
		const uint8_t *e = buf + off + 12 * i;
		char nm[16];
		unsigned fl = e[8] | e[9] << 8, val = e[10] | e[11] << 8, type = fl & 037;
		int k;

		if (type == 0 && (fl & 040)) undef++;
		if (type < 2 || type > 4 || val > iend || !in_seg(val)) continue;
		for (k = 0; k < 8 && e[k]; k++) nm[k] = e[k];
		nm[k] = 0;
		if (!valid_name(nm) || is_local(nm)) continue;
		if (is_builtin(nm)) {
			char t[16];
			snprintf(t, sizeof t, "%.7s_", nm);
			strcpy(nm, t);
		}
		if (name_used(nm)) continue;
		name_add(nm);
		sms = realloc(sms, (nsm + 1) * sizeof *sms);
		sms[nsm].a = val; sms[nsm].n = strdup(nm);
		sms[nsm].ext = (fl & 040) != 0;
		sms[nsm].primary = !named[val];
		if (!named[val]) named[val] = sms[nsm].n; else hasal[val] = 1;
		nsm++;
	}
	if (rf != 1 && undef)
		fprintf(stderr, "warning: %d undefined symbols; this is an unlinked object file\n", undef);
	free(buf);
	return 0;
}

enum { C_BAD, C_DBL, C_SGL, C_BR, C_JSR, C_RTS, C_SOB, C_SYS, C_MULDIV, C_XOR,
       C_CC, C_NOARG, C_MARK, C_FP, C_JMP, C_RAW1 };
enum { F_DST, F_SRCAC, F_ACDST };

struct insn {
	unsigned pc, ir;
	int len, cls;
	const char *mn; 
	char dn[24];
	int sm, sr, dm, dr, sx, dx, nx;
	unsigned xw[2], xa[2];
	int term, implaus, byteop, wdst, wreg, hastgt, fpk, ac;
	unsigned tgt;
	int sysn;
};

static const char *const rn[8] = { "r0", "r1", "r2", "r3", "r4", "r5", "sp", "pc" };
static const char *const sysname[64] = {
	0, "exit", "fork", "read", "write", "open", "close", "wait", "creat", "link",
	"unlink", "exec", "chdir", "time", "makdir", "chmod", "chown", "break", "stat",
	"seek", "tell", "mount", "umount", "setuid", "getuid", "stime", 0, 0, "fstat", 0,
	"mdate", "stty", "gtty", 0, "nice", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "signal"
};
static const uint8_t sysnargs[64] = {
	0,0,0,2,2,2,0,0,2,2,1,2,1,0,3,2,2,1,2,2,0,3,1,0,0,0,0,0,1,0,1,1,
	1,0,0,0,0,1,0,0,0,0,0,1,4,0,0,0,2
};
static const uint8_t sysptr[64] = {
	1,0,0,1,1,1,0,0,1,3,1,3,1,0,1,1,1,1,3,0,0,3,1,0,0,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,2
};

static int opx(struct insn *I, int mode, int reg, int *idx)
{
	if (mode == 6 || mode == 7 || ((mode == 2 || mode == 3) && reg == 7)) {
		unsigned a = I->pc + 2 + 2 * I->nx;

		if (I->nx >= 2 || a + 1 >= MEM) return 0;
		I->xa[I->nx] = a; I->xw[I->nx] = rw(a);
		*idx = I->nx++; I->len += 2;
	} else
		*idx = -1;
	return 1;
}

static int mk_dbl(struct insn *I, const char *mn, int wr)
{
	I->cls = C_DBL; I->mn = mn; snprintf(I->dn, sizeof I->dn, "%s", mn);
	I->sm = (I->ir >> 9) & 7; I->sr = (I->ir >> 6) & 7;
	I->dm = (I->ir >> 3) & 7; I->dr = I->ir & 7;
	I->wdst = wr; I->byteop = mn[strlen(mn) - 1] == 'b' && strcmp(mn, "sub");
	return opx(I, I->sm, I->sr, &I->sx) && opx(I, I->dm, I->dr, &I->dx);
}
static int mk_sgl(struct insn *I, const char *mn, const char *dn, int wr)
{
	I->cls = C_SGL; I->mn = mn; snprintf(I->dn, sizeof I->dn, "%s", dn);
	I->dm = (I->ir >> 3) & 7; I->dr = I->ir & 7; I->wdst = wr;
	I->byteop = (I->ir & 0100000) != 0;
	return opx(I, I->dm, I->dr, &I->dx);
}

static int decode(unsigned pc, struct insn *I)
{
	unsigned ir;
	int sm, sr, dm, dr, k;
	static const char *const sgl[16] = { "clr", "com", "inc", "dec", "neg", "adc",
	    "sbc", "tst", "ror", "rol", "asr", "asl", "mark", "mfpi", "mtpi", "sxt" };
	static const char *const sglb[16] = { "clrb", "comb", "incb", "decb", "negb",
	    "adcb", "sbcb", "tstb", "rorb", "rolb", "asrb", "aslb", "mtps", "mfpd",
	    "mtpd", "mfps" };
	static const char *const dbl[7] = { 0, "mov", "cmp", "bit", "bic", "bis", "add" };
	static const char *const dblb[7] = { 0, "movb", "cmpb", "bitb", "bicb", "bisb", "sub" };
	static const char *const br[8] = { 0, "br", "bne", "beq", "bge", "blt", "bgt", "ble" };
	static const char *const brb[8] = { "bpl", "bmi", "bhi", "blos", "bvc", "bvs", "bcc", "bcs" };

	memset(I, 0, sizeof *I);
	I->pc = pc; I->sm = I->sr = I->dm = I->dr = I->sx = I->dx = -1;
	if ((pc & 1) || pc + 1 >= MEM) return 0;
	ir = I->ir = rw(pc);
	I->len = 2;
	sm = (ir >> 9) & 7; sr = (ir >> 6) & 7; dm = (ir >> 3) & 7; dr = ir & 7;

	if (ir < 010) {
		static const char *const z[8] = { "halt", "wait", "rti", "bpt", "iot", "reset", "rtt", 0 };
		if (!z[ir] || (ir == 0 && !opt_k)) return 0;
		I->cls = C_NOARG; snprintf(I->dn, sizeof I->dn, "%s", z[ir]);
		I->term = ir == 0 || ir == 2 || ir == 6; I->implaus = !opt_k;
		return 1;
	}
	if (ir < 0100) return 0;
	if ((ir & 0177700) == 0100) {
		if (dm == 0) return 0;
		I->term = 1;
		I->cls = C_JMP;
		return mk_sgl(I, "jmp", "jmp", 0) && (I->cls = C_JMP);
	}
	if ((ir & 0177770) == 0200) {
		I->cls = C_RTS; I->mn = "rts"; I->dr = dr; I->term = 1;
		snprintf(I->dn, sizeof I->dn, "rts");
		return 1;
	}
	if ((ir & 0177770) == 0230) {
		I->cls = C_NOARG; I->implaus = !opt_k;
		snprintf(I->dn, sizeof I->dn, "spl %d", dr);
		return 1;
	}
	if ((ir & 0177740) == 0240) {
		static const struct { unsigned v; const char *n; } cc[] = {
		    {0241, "clc"}, {0242, "clv"}, {0244, "clz"}, {0250, "cln"},
		    {0261, "sec"}, {0262, "sev"}, {0264, "sez"}, {0270, "sen"}, {0, 0} };
		int i;

		I->cls = C_CC;
		for (i = 0; cc[i].n; i++)
			if (cc[i].v == ir) I->mn = cc[i].n;
		if (ir == 0240) snprintf(I->dn, sizeof I->dn, "nop");
		else {
			static const char L[4] = { 'c', 'v', 'z', 'n' };
			char t[24] = ""; int b;
			for (b = 0; b < 4; b++)
				if (ir & (1 << b))
					snprintf(t + strlen(t), sizeof t - strlen(t), "%s%s%c",
					    t[0] ? " " : "", ir & 020 ? "se" : "cl", L[b]);
			snprintf(I->dn, sizeof I->dn, "%s", t);
		}
		return 1;
	}
	if ((ir & 0177700) == 0300)
		return mk_sgl(I, "swab", "swab", 1);
	if (ir >= 0400 && ir < 04000) {
		I->cls = C_BR; I->mn = br[ir >> 8]; snprintf(I->dn, sizeof I->dn, "%s", I->mn);
		I->hastgt = 1; I->tgt = (pc + 2 + 2 * (int8_t)(ir & 0377)) & 0xffff;
		I->term = (ir >> 8) == 1;
		return 1;
	}
	if ((ir & 0177000) == 04000) {
		if (dm == 0) return 0;
		I->cls = C_JSR; I->mn = "jsr"; I->sr = sr; snprintf(I->dn, sizeof I->dn, "jsr");
		I->dm = dm; I->dr = dr;
		return opx(I, dm, dr, &I->dx);
	}
	if (ir >= 05000 && ir < 010000 && (ir >> 12) == 0) {
		k = ((ir >> 6) & 077) - 050;
		if (k < 0 || k > 15) return 0;
		if (k == 12) {
			I->cls = C_MARK; I->mn = "mark"; I->implaus = 1;
			snprintf(I->dn, sizeof I->dn, "mark %o", ir & 077);
			return 1;
		}
		if (k == 13 || k == 14) {
			I->implaus = 1;
			return mk_sgl(I, NULL, sgl[k], 0);
		}
		return mk_sgl(I, k == 15 ? "sxt" : sgl[k], sgl[k], k != 7);
	}
	if ((ir >> 12) == 7) {
		static const char *const e[8] = { "mul", "div", "ash", "ashc", "xor", 0, 0, "sob" };

		if (!e[sm]) return 0;
		if (sm == 7) {
			I->cls = C_SOB; I->mn = "sob"; I->sr = sr; I->wreg = 1;
			snprintf(I->dn, sizeof I->dn, "sob");
			I->hastgt = 1; I->tgt = (pc + 2 - 2 * (ir & 077)) & 0xffff;
			return 1;
		}
		I->mn = e[sm]; snprintf(I->dn, sizeof I->dn, "%s", e[sm]);
		I->sr = sr; I->dm = dm; I->dr = dr;
		I->cls = sm == 4 ? C_XOR : C_MULDIV;
		I->wreg = sm != 4; I->wdst = sm == 4;
		return opx(I, dm, dr, &I->dx);
	}
	if ((ir >> 12) >= 1 && (ir >> 12) <= 6)
		return mk_dbl(I, dbl[ir >> 12], (ir >> 12) != 2 && (ir >> 12) != 3);
	if (ir >= 0100000 && ir < 0104000) {
		I->cls = C_BR; I->mn = brb[(ir >> 8) & 7]; snprintf(I->dn, sizeof I->dn, "%s", I->mn);
		I->hastgt = 1; I->tgt = (pc + 2 + 2 * (int8_t)(ir & 0377)) & 0xffff;
		return 1;
	}
	if (ir >= 0104000 && ir < 0104400) {
		I->cls = C_NOARG; I->implaus = 1; snprintf(I->dn, sizeof I->dn, "emt %o", ir & 0377);
		return 1;
	}
	if (ir >= 0104400 && ir < 0105000) {
		I->cls = C_SYS; I->sysn = ir & 077;
		if ((ir & 0377) < 64) {
			I->mn = "sys";
			snprintf(I->dn, sizeof I->dn, "sys %d", I->sysn);
		} else {
			I->implaus = 1;
			snprintf(I->dn, sizeof I->dn, "trap %o", ir & 0377);
			I->cls = C_NOARG;
		}
		return 1;
	}
	if (ir >= 0105000 && ir < 0110000) {
		k = ((ir >> 6) & 077) - 050;
		if (k < 0 || k > 15) return 0;
		if (k >= 12) I->implaus = 1;
		return mk_sgl(I, k >= 12 ? NULL : sglb[k], sglb[k], k != 7);
	}
	if (ir >= 0110000 && ir < 0170000)
		return mk_dbl(I, dblb[(ir >> 12) & 7], ((ir >> 12) & 7) != 2 && ((ir >> 12) & 7) != 3);
	{
		static const struct { unsigned m; const char *mn, *dn; int k; } ft[] = {
		    {0170100, "ldfps", "ldfps", F_DST}, {0170200, "stfps", "stfps", F_DST},
		    {0170300, NULL, "stst", F_DST}, {0170400, "clrf", "clrf", F_DST},
		    {0170500, "tstf", "tstf", F_DST}, {0170600, "absf", "absf", F_DST},
		    {0170700, "negf", "negf", F_DST}, {0, 0, 0, 0} };
		static const struct { unsigned m; const char *mn, *dn; int k; } fh[] = {
		    {0171000, "mulf", "mulf", F_SRCAC}, {0171400, "modf", "modf", F_SRCAC},
		    {0172000, "addf", "addf", F_SRCAC}, {0172400, "movf", "ldf", F_SRCAC},
		    {0173000, "subf", "subf", F_SRCAC}, {0173400, "cmpf", "cmpf", F_SRCAC},
		    {0174000, "movf", "stf", F_ACDST}, {0174400, "divf", "divf", F_SRCAC},
		    {0175000, "movei", "stexp", F_ACDST}, {0175400, "movfi", "stcfi", F_ACDST},
		    {0176000, "movfo", "stcfd", F_ACDST}, {0176400, "movie", "ldexp", F_SRCAC},
		    {0177000, "movif", "ldcif", F_SRCAC}, {0177400, "movof", "ldcdf", F_SRCAC},
		    {0, 0, 0, 0} };
		int i;

		switch (ir) {
		case 0170000: I->mn = "cfcc"; break;
		case 0170001: I->mn = "setf"; break;
		case 0170002: I->mn = "seti"; break;
		case 0170011: I->mn = "setd"; break;
		case 0170012: I->mn = "setl"; break;
		}
		if (I->mn) {
			I->cls = C_CC; snprintf(I->dn, sizeof I->dn, "%s", I->mn);
			return 1;
		}
		if (ir < 0170100) return 0;
		for (i = 0; ft[i].dn; i++)
			if ((ir & 0177700) == ft[i].m) {
				I->cls = C_FP; I->fpk = F_DST; I->mn = ft[i].mn;
				snprintf(I->dn, sizeof I->dn, "%s", ft[i].dn);
				I->dm = dm; I->dr = dr;
				I->wdst = ft[i].m != 0170500 && ft[i].m != 0170100;
				return opx(I, dm, dr, &I->dx);
			}
		for (i = 0; fh[i].dn; i++)
			if ((ir & 0177400) == fh[i].m) {
				I->cls = C_FP; I->fpk = fh[i].k; I->mn = fh[i].mn; I->ac = sr & 3;
				snprintf(I->dn, sizeof I->dn, "%s", fh[i].dn);
				I->dm = dm; I->dr = dr;
				if (fh[i].k == F_SRCAC && fh[i].m == 0172400 && dm == 0 && dr < 4)
					I->mn = NULL;
				return opx(I, dm, dr, &I->dx);
			}
	}
	return 0;
}

static void finish(struct insn *I)
{
	if ((I->cls == C_DBL && I->wdst && I->dm == 0 && I->dr == 7) ||
	    (I->cls == C_SGL && I->wdst && I->dm == 0 && I->dr == 7))
		I->term = 1;
	if (I->cls == C_SYS && I->sysn == 1) I->term = 1;
}

static int implausible(const struct insn *I)
{
	if (I->implaus) return 1;
	if (I->cls == C_DBL || I->cls == C_SGL || I->cls == C_MULDIV || I->cls == C_XOR ||
	    I->cls == C_JMP || I->cls == C_JSR || I->cls == C_FP) {
		if (I->cls == C_DBL && I->sr == 7 && (I->sm == 1 || I->sm == 4 || I->sm == 5)) return 1;
		if (I->dr == 7 && (I->dm == 1 || I->dm == 4 || I->dm == 5) && I->cls != C_JMP) return 1;
		if (I->wdst && I->dm == 2 && I->dr == 7) return 1;
		if (I->wdst && I->dm == 0 && I->dr == 6 && I->cls == C_DBL && I->ir >> 12 == 1) return 0;
	}
	if (I->hastgt && I->tgt >= ts) return 1;
	return 0;
}

enum { I_NONE = 0, I_INSN = 1, I_WORD = 2, I_BODY = 3 };
static uint8_t cov[MEM + 2], ilen[MEM + 2], wfl[MEM + 2];

static int decode_ok(unsigned pc, struct insn *I)
{
	if (!decode(pc, I)) return 0;
	finish(I);
	return 1;
}

static int static_target(const struct insn *I, unsigned *t)
{
	if (I->dm == 6 && I->dr == 7) {
		*t = (I->xa[I->dx] + 2 + I->xw[I->dx]) & 0xffff;
		return 1; 
	}
	if (I->dm == 3 && I->dr == 7) {
		*t = I->xw[I->dx];
		return 1; 
	}
	return 0;
}

#define UNKO 0x7fff
struct cs { unsigned a; int off[8]; };
static struct { unsigned ent; int reg; int n; int ex[6]; } ccache[256];
static int nccache;

static void cs_effect(struct cs *s, const struct insn *I)
{
	int opn;

	for (opn = 0; opn < 2; opn++) {
		int m = opn ? I->dm : I->sm, r = opn ? I->dr : I->sr;

		if (I->cls == C_DBL ? 0 : opn == 0) continue;
		if (m < 0 || r < 0 || r > 7 || s->off[r] == UNKO) continue;
		if (m == 2 && r != 7) s->off[r] += (I->byteop && r < 6) ? 1 : 2;
		else if (m == 3) s->off[r] += 2;
		else if (m == 4) s->off[r] -= (I->byteop && r < 6) ? 1 : 2;
		else if (m == 5) s->off[r] -= 2;
	}
	if (I->cls == C_DBL && I->wdst && I->dm == 0 && I->dr < 7) {
		int d = I->dr;
		unsigned op = I->ir >> 12 & 7;

		if (op == 1 && !I->byteop && I->sm == 0 && s->off[I->sr] != UNKO)
			s->off[d] = s->off[I->sr];
		else if ((op == 6) && I->sm == 2 && I->sr == 7 && s->off[d] != UNKO)
			s->off[d] += (I->ir & 0100000) ? -(int)I->xw[I->sx] : (int16_t)I->xw[I->sx];
		else
			s->off[d] = UNKO;
	} else if ((I->cls == C_SGL || I->cls == C_XOR) && I->wdst && I->dm == 0 && I->dr < 7)
		s->off[I->dr] = UNKO;
	if (I->wreg && I->sr < 8) s->off[I->sr] = UNKO;
	if (I->cls == C_JSR && I->sr < 7) s->off[I->sr] = UNKO;
}

static int callee_args(unsigned ent, int reg, int *ex)
{
	struct cs st[64], s;
	struct { unsigned a; int o[8]; } seen[700];
	int nst = 0, nseen = 0, steps = 0, nex = 0, i, k;

	for (i = 0; i < nccache; i++)
		if (ccache[i].ent == ent && ccache[i].reg == reg) {
			memcpy(ex, ccache[i].ex, sizeof ccache[i].ex);
			return ccache[i].n;
		}
	if (ent >= ts || (ent & 1)) return 0;
	for (i = 0; i < 8; i++) st[0].off[i] = UNKO;
	st[0].off[reg] = 0; st[0].a = ent; nst = 1;
	while (nst && steps < 1500) {
		s = st[--nst];
		for (;;) {
			struct insn I;
			int dup = 0;

			if (++steps > 1500) break;
			for (i = 0; i < nseen; i++)
				if (seen[i].a == s.a && !memcmp(seen[i].o, s.off, sizeof s.off)) {
					dup = 1;
					break; 
				}
			if (dup || nseen >= 700) break;
			seen[nseen].a = s.a; memcpy(seen[nseen++].o, s.off, sizeof s.off);
			if (!decode_ok(s.a, &I)) break;
			cs_effect(&s, &I);
			{
				unsigned nxt = s.a + I.len, t;
				int tracked_exit = -1;

				if (I.cls == C_RTS && s.off[I.dr] != UNKO) tracked_exit = s.off[I.dr];
				else if (I.cls == C_JMP && I.dm == 1 && s.off[I.dr] != UNKO) tracked_exit = s.off[I.dr];
				else if (I.cls == C_DBL && I.dm == 0 && I.dr == 7 && I.sm == 0 &&
				    (I.ir >> 12) == 1 && I.sr < 7 && s.off[I.sr] != UNKO) tracked_exit = s.off[I.sr];
				if (tracked_exit >= 0) {
					if (tracked_exit <= 32) {
						for (k = 0; k < nex && ex[k] != tracked_exit; k++) ;
						if (k == nex && nex < 6) ex[nex++] = tracked_exit;
					}
					break;
				}
				if (I.cls == C_BR) {
					if (!I.term && nst < 63) {
						st[nst] = s;
						st[nst++].a = nxt; 
					}
					s.a = I.tgt; continue;
				}
				if (I.cls == C_SOB) {
					if (nst < 63) {
						st[nst] = s;
						st[nst++].a = nxt; 
					}
					s.a = I.tgt; continue;
				}
				if (I.cls == C_JMP && static_target(&I, &t)) {
					s.a = t; 
					continue; 
				}
				if (I.cls == C_RTS || I.term) break;
				if (I.cls == C_SYS) nxt += 2 * (I.sysn < 49 ? sysnargs[I.sysn] : 0);
				s.a = nxt;
			}
		}
	}
	for (i = 0; i < nex; i++)
		for (k = i + 1; k < nex; k++)
			if (ex[k] < ex[i]) {
				int t = ex[i]; 
				ex[i] = ex[k]; 
				ex[k] = t; 
			}
	if (nccache < 256) {
		ccache[nccache].ent = ent; ccache[nccache].reg = reg; ccache[nccache].n = nex;
		memset(ccache[nccache].ex, 0, sizeof ccache[nccache].ex);
		memcpy(ccache[nccache].ex, ex, nex * sizeof *ex);
		nccache++;
	}
	return nex;
}

static struct jent { unsigned a; int len, type, wf; } jn[20000];
static int njn;
static uint8_t tcov[MEM + 2];
static unsigned jcalls[4096]; static int njc;
static struct { unsigned a; int tbl; } jw[4096]; static int njw;
static unsigned wq[8192]; static int wqn;
static uint8_t specfail[MEM + 2];
static int stat_spec_ok, stat_spec_bad, walk_rts;

static int jadd(unsigned a, int len, int type, int wf)
{
	int k;

	if (a + len > MEM || njn >= 20000) return 0;
	for (k = 0; k < len; k++)
		if (cov[a + k] || tcov[a + k]) return 0;
	tcov[a] = type == I_INSN ? 2 : 3;
	for (k = 1; k < len; k++) tcov[a + k] = 1;
	jn[njn].a = a; jn[njn].len = len; jn[njn].type = type; jn[njn].wf = wf;
	njn++;
	return 1;
}
static void wpush(unsigned a) {
	if (wqn < 8192) 
		wq[wqn++] = a; 
}
static void jcall(unsigned t) {
	if (njc < 4096) 
		jcalls[njc++] = t; 
}
static void jwant(unsigned t, int tbl) {
	if (njw < 4096) {
		jw[njw].a = t;
		jw[njw++].tbl = tbl; 
	} 
}

static void rollback(void)
{
	int i, k;
	for (i = 0; i < njn; i++)
		for (k = 0; k < jn[i].len; k++) tcov[jn[i].a + k] = 0;
	njn = 0; njc = 0; njw = 0;
}
static void commit(void)
{
	int i, k;
	for (i = 0; i < njn; i++) {
		struct jent *j = &jn[i];

		for (k = 0; k < j->len; k++) {
			tcov[j->a + k] = 0;
			cov[j->a + k] = I_BODY; 
		}
		cov[j->a] = j->type; ilen[j->a] = j->len; wfl[j->a] = j->wf;
	}
	for (i = 0; i < njc; i++)
		callt[jcalls[i]] = 1;

	for (i = 0; i < njw; i++) {
		want(jw[i].a, 2);
		if (jw[i].tbl) 
			tblt[jw[i].a] = 1; 
	}
	njn = 0; njc = 0; njw = 0;
}

static int walk(unsigned start, int spec, int minins)
{
	int ninsn = 0, bad = 0;

	walk_rts = 0;
	njn = 0; njc = 0; njw = 0; wqn = 0;
	wpush(start);
	while (wqn) {
		unsigned a = wq[--wqn];
		int cmpn = -1, cmpage = 99;

		for (;;) {
			struct insn I;
			unsigned nxt;
			int fall;

			if ((a & 1) || a >= ts) {
				bad = 1; 
				break; 
			}
			if (cov[a] == I_INSN || tcov[a] == 2) break;
			if (cov[a] || tcov[a]) {
				bad = 1; 
				break; 
			}
			if (!decode_ok(a, &I)) {
				bad = 1; 
				break; 
			}
			if (spec && implausible(&I)) {
				bad = 1; 
				break; 
			}
			if (!jadd(a, I.len, I_INSN, 0)) {
				bad = 1; 
				break; 
			}
			ninsn++; cmpage++;
			if (I.cls == C_RTS) walk_rts = 1;
			nxt = a + I.len; fall = !I.term;
			if (I.cls == C_DBL && I.mn && !strcmp(I.mn, "cmp") && I.sm == 0 &&
			    I.dm == 2 && I.dr == 7) {
					cmpn = I.xw[I.dx];
					cmpage = 0; 
				}
			switch (I.cls) {
			case C_BR: case C_SOB:
				if (I.tgt < ts) wpush(I.tgt);
				else bad = 1;
				break;
			case C_SYS: {
				int i, n = I.sysn < 49 ? sysnargs[I.sysn] : 0;
				for (i = 0; i < n; i++, nxt += 2)
					if (!jadd(nxt, 2, I_WORD, (sysptr[I.sysn] >> i) & 1 ? 2 : 3)) {
						bad = 1; 
						break; 
					}
				break;
			}
			case C_JSR: case C_JMP: {
				unsigned t;

				if (static_target(&I, &t)) {
					if (t < ts) {
						wpush(t);
						if (I.cls == C_JSR) 
							jcall(t);
						else jwant(t, 0);
					}
					if (I.cls == C_JSR && I.sr != 7 && t < ts) {
						int ex[6] = { 0 }, nex = callee_args(t, I.sr, ex), i;

						if (nex) {
							for (i = 0; i < ex[0] / 2; i++)
								if (!jadd(a + I.len + 2 * i, 2, I_WORD, 3)) {
									bad = 1; 
									break; 
								}
							for (i = 1; i < nex; i++) 
								wpush(a + I.len + ex[i]);
							nxt = a + I.len + ex[0];
						}
					}
				} else if (I.cls == C_JMP && I.dm == 7 && I.dr != 7 &&
				    I.xw[I.dx] == a + 4 && !(a & 1)) {
					int cnt = 0, i;
					unsigned m = 0xffff;

					if (cmpn >= 0 && cmpage < 6) cnt = cmpn + 1;
					else
						for (i = 0; i < 256; i++) {
							unsigned ea = a + 4 + 2 * i, v = rw(ea);

							if (ea >= m || (v & 1) || v >= ts || v < 2) break;
							if (v > ea && v < m) m = v;
							cnt++;
						}
					if (cnt > 0 && cnt < 1024 && a + 4 + 2 * cnt <= ts) {
						jwant(a + 4, 1);
						for (i = 0; i < cnt; i++) {
							unsigned v = rw(a + 4 + 2 * i);

							if (!jadd(a + 4 + 2 * i, 2, I_WORD, 1)) {
								bad = 1;
								break;
							}
							if (v < ts && !(v & 1)) {
								wpush(v);
								jwant(v, 0);
							}
							else bad = 1;
						}
					}
				}
				break;
			}
			}
			if (!fall) break;
			a = nxt;
		}
	}
	if (spec && (bad || (ninsn < minins && !(ninsn == 1 && walk_rts)) || (ninsn == 1 && !walk_rts))) {
		rollback();
		stat_spec_bad++;
		return 0;
	}
	if (spec) stat_spec_ok++;
	if (ninsn) jcall(start);
	commit();
	return ninsn;
}

static short fnargs[MEM + 1];
static uint8_t fret[MEM + 1];
static uint8_t fcalls[MEM + 1];
static uint8_t fw0[MEM + 1];
static unsigned fstamp[MEM + 2], fcur;
static unsigned csv_addr;
static int have_csv;

static int c_entry(unsigned a, struct insn *I, unsigned *t)
{
	if (a >= ts || (a & 1) || cov[a] != I_INSN) return 0;
	if (!decode_ok(a, I)) return 0;
	return I->cls == C_JSR && I->sr == 5 && static_target(I, t) && *t < ts;
}

static void find_csv(void)
{
	struct { unsigned t; int n; } c[64];
	struct insn I;
	unsigned a, t;
	int nc = 0, i, best = -1;

	for (a = 0; a < ts; a += 2) {
		if (!(callt[a] || named[a]) || !c_entry(a, &I, &t)) continue;
		for (i = 0; i < nc && c[i].t != t; i++) ;
		if (i == nc) {
			if (nc == 64) continue;
			c[nc].t = t; c[nc++].n = 0;
		}
		c[i].n++;
	}
	for (i = 0; i < nc; i++)
		if (c[i].n >= 2 || named[c[i].t])
			if (best < 0 || c[i].n > c[best].n) best = i;
	if (best >= 0) {
		have_csv = 1;
		csv_addr = c[best].t;
	}
}

static int plain_entry(unsigned a)
{
	return a + 2 < ts && cov[a] == I_INSN && cov[a + 2] == I_INSN &&
	    rw(a) == 010546 && rw(a + 2) == 010605;
}

static void arg_uses(const struct insn *I, int *mx)
{
	int o;

	for (o = 0; o < 2; o++) {
		int m = o ? I->dm : I->sm, r = o ? I->dr : I->sr, x = o ? I->dx : I->sx;

		if ((m == 6 || m == 7) && r == 5 && x >= 0) {
			int off = (int16_t)I->xw[x];

			if (off >= 4 && off < 40 && off > *mx) *mx = off;
		}
	}
}

static int scan_body(unsigned ent)
{
	unsigned st[256];
	int nst = 0, steps = 0, mx = 0;

	fcur++;
	st[nst++] = ent;
	while (nst && steps < 4000) {
		unsigned a = st[--nst];

		for (;;) {
			struct insn I;
			unsigned nxt, t;

			if (a >= ts || (a & 1) || cov[a] != I_INSN || fstamp[a] == fcur) break;
			fstamp[a] = fcur;
			if (!decode_ok(a, &I) || ++steps > 4000) break;
			arg_uses(&I, &mx);
			if ((I.cls == C_DBL || I.cls == C_SGL) && I.wdst && I.dm == 0 && I.dr == 0)
				fw0[ent] = 1;
			nxt = a + I.len;
			if (I.cls == C_JSR && I.sr != 7)
				while (nxt < ts && cov[nxt] == I_WORD) nxt += 2;
			if (I.cls == C_BR || I.cls == C_SOB) {
				if (nst < 255) st[nst++] = I.tgt;
				if (I.term) break;
			} else if (I.cls == C_JMP) {
				if (static_target(&I, &t) && t < ts &&
				    (t > a ? t - a : a - t) < 01000 && nst < 255)
					st[nst++] = t;
				break;
			} else if (I.cls == C_RTS || I.term)
				break;
			else if (I.cls == C_SYS)
				nxt += 2 * (I.sysn < 49 ? sysnargs[I.sysn] : 0);
			a = nxt;
		}
	}
	return mx >= 4 ? (mx - 4) / 2 + 1 : 0;
}

static int cleanup(unsigned a, unsigned *after)
{
	int n = 0;

	for (;;) {
		struct insn J;

		if (a >= ts || cov[a] != I_INSN || !decode_ok(a, &J)) break;
		if (J.ir == 0005726) { n += 1; a += 2; }
		else if (J.ir == 0022626) { n += 2; a += 2; }
		else if (J.cls == C_DBL && (J.ir >> 12) == 6 && J.sm == 2 && J.sr == 7 &&
		    J.dm == 0 && J.dr == 6) {
			unsigned w = J.xw[J.sx] / 2;

			if (w <= 20) n += w;
			a += J.len;
			break;
		}
		else break;
	}
	*after = a;
	return n;
}

static int uses_r0(const struct insn *K)
{
	int op = (K->ir >> 12) & 7;

	switch (K->cls) {
	case C_DBL:
		if (K->sr == 0) return 1;
		if (K->dr == 0) return K->dm != 0 || op != 1;
		return 0;
	case C_SGL: case C_FP: case C_JMP:
		if (K->dr != 0) return 0;
		return K->dm != 0 || (K->ir & 077700) != 050000;
	case C_MULDIV: case C_XOR:
		return K->sr == 0 || K->dr == 0;
	case C_JSR:
		return K->dr == 0;
	}
	return 0;
}

static void scan_calls(void)
{
	struct insn I, K;
	unsigned a, t, after;

	for (a = 0; a < ts; a += 2) {
		int n;

		if (cov[a] != I_INSN || !decode_ok(a, &I)) continue;
		if (I.cls != C_JSR || I.sr != 7 || !static_target(&I, &t)) continue;
		if (t >= ts || fnargs[t] < 0) continue;
		n = cleanup(a + I.len, &after);
		if (n > fnargs[t]) fnargs[t] = n;
		if (fcalls[t] < 255) fcalls[t]++;
		if (after < ts && cov[after] == I_INSN && decode_ok(after, &K) && uses_r0(&K))
			fret[t] = 1;
	}
}

static void build_sigs(void)
{
	struct insn I;
	unsigned a, t;
	int i;

	for (i = 0; i <= MEM; i++) fnargs[i] = -1;
	find_csv();
	for (a = 0; a < ts; a += 2) {
		if (!(callt[a] || named[a])) continue;
		if (plain_entry(a) ||
		    (have_csv && c_entry(a, &I, &t) && t == csv_addr))
			fnargs[a] = scan_body(a);
	}
	scan_calls();
}

enum { K_BR, K_CALL, K_CPTR, K_PCREL, K_SPTR, K_IMM, K_ABS, K_INDEX, K_WORD };
static int nosym;

static int printable(unsigned c) {
	return c >= 0x20 && c < 0x7f;
}
static int istextc(unsigned c) {
	return printable(c) || c == '\n' || c == '\t' || c == '\r';
}

static int isstr_start(unsigned v)
{
	unsigned n = 0;

	if (v >= iend || !(v < ts || v >= dbase)) return 0;
	if (v > 0 && v != dbase && img[v - 1] != 0 && !named[v]) return 0;
	while (v + n < iend && istextc(img[v + n]) && n < 200) n++;
	return n >= 3 && printable(img[v]) && (v + n >= iend || img[v + n] == 0);
}

static int looks_addr(unsigned v, int kind)
{
	(void)kind;
	if (v > iend || !in_seg(v)) return 0;
	if (named[v] && v >= 0100) return 1;
	if (v < ts) return ((callt[v] || tblt[v]) && v >= 040) || isstr_start(v);
	return v >= dbase && v >= 0200 && v < iend;
}

static const char *sref(int kind, unsigned v)
{
	int ok = 0, prio = 3;

	v &= 0xffff;
	if (nosym) return NULL;
	switch (kind) {
	case K_BR: 
		ok = v < ts && !(v & 1);
		prio = 4; 
		break;
	case K_CALL: 
	case K_CPTR: 
		ok = v < ts;
		prio = 2;
		break;
	case K_PCREL: 
		ok = in_seg(v) && v <= iend; 
		break;
	case K_SPTR:
		ok = v >= 2 && in_seg(v) && v < iend;
		break;
	default: 
		ok = looks_addr(v, kind);
		break;
	}
	if (!ok) return NULL;
	if (collecting) {
		want(v, prio);
		return NULL;
	}
	return lname[v];
}

static void num(char *o, unsigned v)
{
	v &= 0xffff;
	if (v < 8) sprintf(o, "%o", v);
	else if (v >= 0170000) {
		v = 0x10000 - v;
		sprintf(o, v < 8 ? "-%o" : "-0%o", v);
	}
	else sprintf(o, "0%o", v);
}

static void fmt_op(char *o, const struct insn *I, int mode, int reg, int xi, int fpr)
{
	char t[24];
	const char *s;
	unsigned x = xi >= 0 ? I->xw[xi] : 0;

	switch (mode) {
	case 0:
		if (fpr && reg < 6) sprintf(o, "fr%d", reg);
		else strcpy(o, rn[reg]);
		return;
	case 1: 
		sprintf(o, "(%s)", rn[reg]);
		return;
	case 2:
		if (reg == 7) {
			s = sref(K_IMM, x);
			if (!s) {
			 	num(t, x);
				s = t;
			}
			sprintf(o, "$%s", s);
		} else sprintf(o, "(%s)+", rn[reg]);
		return;
	case 3:
		if (reg == 7) {
			s = sref(K_ABS, x);
			if (!s) {
				num(t, x);
				s = t;
			}
			sprintf(o, "*$%s", s);
		} else sprintf(o, "*(%s)+", rn[reg]);
		return;
	case 4: 
		sprintf(o, "-(%s)", rn[reg]);
		return;
	case 5: 
		sprintf(o, "*-(%s)", rn[reg]);
		return;
	}
	if (reg == 7) {
		unsigned tg = (I->xa[xi] + 2 + x) & 0xffff;

		s = sref(K_PCREL, tg);
		if (s) sprintf(o, "%s%s", mode == 7 ? "*" : "", s);
		else {
			num(t, x);
			if (mode == 7 && x == 052) strcpy(t, "0+052");
		    sprintf(o, "%s%s(pc)", mode == 7 ? "*" : "", t);
		}
	} else {
		s = sref(K_INDEX, x);
		if (!s) {
			num(t, x);
			s = t;
			if (mode == 7 && x == 052) s = "0+052";
		}
		sprintf(o, "%s%s(%s)", mode == 7 ? "*" : "", s, rn[reg]);
	}
}

static void fmt_tgt(char *o, const struct insn *I)
{
	const char *s = sref(K_BR, I->tgt);

	if (s) strcpy(o, s);
	else {
		int d = (int)I->tgt - (int)I->pc;
		sprintf(o, d < 0 ? ".-0%o" : ".+0%o", d < 0 ? -d : d);
	}
}

static int build(const struct insn *I, char *o, size_t n)
{
	char a[64], b[64], t[32];
	const char *mn = I->mn;
	int raw = !mn, fpr;

	(void)n;
	if (raw) {
		mn = I->dn;
		nosym = 1;
	}
	fpr = I->cls == C_FP && strcmp(I->dn, "ldexp") && strcmp(I->dn, "ldcif") &&
	    strcmp(I->dn, "stexp") && strcmp(I->dn, "stcfi");
	switch (I->cls) {
	case C_DBL:
		fmt_op(a, I, I->sm, I->sr, I->sx, 0);
		fmt_op(b, I, I->dm, I->dr, I->dx, 0);
		sprintf(o, "%s\t%s,%s", mn, a, b);
		break;
	case C_SGL: case C_JMP:
		fmt_op(a, I, I->dm, I->dr, I->dx, 0);
		sprintf(o, "%s\t%s", mn, a);
		break;
	case C_FP:
		if (I->fpk == F_DST) {
			fmt_op(a, I, I->dm, I->dr, I->dx, fpr);
			sprintf(o, "%s\t%s", mn, a);
		}
		else if (I->fpk == F_SRCAC) {
			fmt_op(a, I, I->dm, I->dr, I->dx, fpr);
			sprintf(o, "%s\t%s,fr%d", mn, a, I->ac);
		} else {
			fmt_op(a, I, I->dm, I->dr, I->dx, fpr);
			sprintf(o, "%s\tfr%d,%s", mn, I->ac, a);
		}
		break;
	case C_JSR:
		fmt_op(a, I, I->dm, I->dr, I->dx, 0);
		sprintf(o, "%s\t%s,%s", mn, rn[I->sr], a);
		break;
	case C_XOR:
		fmt_op(a, I, I->dm, I->dr, I->dx, 0);
		sprintf(o, "%s\t%s,%s", mn, rn[I->sr], a);
		break;
	case C_MULDIV:
		fmt_op(a, I, I->dm, I->dr, I->dx, 0);
		sprintf(o, "%s\t%s,%s", mn, a, rn[I->sr]);
		break;
	case C_RTS: sprintf(o, "%s\t%s", mn, rn[I->dr]);
	break;
	case C_BR: fmt_tgt(a, I);
	sprintf(o, "%s\t%s", mn, a);
	break;
	case C_SOB: fmt_tgt(a, I);
	sprintf(o, "%s\t%s,%s", mn, rn[I->sr], a);
	break;
	case C_SYS:
		if (!raw && sysname[I->sysn]) sprintf(o, "sys\t%s", sysname[I->sysn]);
		else {
			num(t, I->sysn);
			sprintf(o, "sys\t%s", t);
		}
		break;
	case C_MARK: 
		sprintf(o, "mark\t%o", I->ir & 077);
		break;
	default: 
		sprintf(o, "%s", mn);
		break;
	}
	nosym = 0;
	return !raw;
}

static FILE *out;
static int stat_insn, stat_data;

#define P(...) do { if (out) fprintf(out, __VA_ARGS__); } while (0)

static void print_padded(const char *s, int target_col) {
	int col = 0;

	for (; col < 8; col++) P(" ");
	for (; *s; s++) {
		if (*s == '\t') {
			do { P(" "); col++; } while (col & 7);
		} else {
			P("%c", *s);
			col++;
		}
	}
	do { P(" "); col++; } while (col < target_col);
}

static void print_sig(unsigned a)
{
	const char *nm = lname[a] ? lname[a] : "func";
	int i, n = fnargs[a];
	int isvoid = fcalls[a] && !fret[a] && !fw0[a];

	if (nm[0] == '_' && nm[1]) nm++;
	P("/ %s %s(", isvoid ? "void" : "int", nm);
	if (!n) P("void");
	for (i = 0; i < n; i++) P("%sint a%d", i ? ", " : "", i);
	P(");\n");
}

static void print_labels(unsigned a)
{
	int i;

	if (!collecting && opt_sig && a < ts && fnargs[a] >= 0) print_sig(a);
	if (!lname[a] || collecting) return;
	P("%s:\n", lname[a]);
	if (hasal[a])
		for (i = 0; i < nsm; i++)
			if (sms[i].a == a && !sms[i].primary) P("%s:\n", sms[i].n);
}

static void addr_comment(unsigned a, int len)
{
	int k;

	if (!opt_addr) return;
	P("/ %06o%s", a, len > 0 ? ":" : "");
	for (k = 0; k < len; k += 2) P(" %06o", rw(a + k));
}

static void line(const char *text, unsigned a, int len)
{
	if (opt_addr)
		print_padded(text, 40);
	else
		P("\t%s", text);
	addr_comment(a, len);
	P("\n");
}

static void emit_insn(unsigned a)
{
	struct insn I;
	char buf[200], t[32];
	int k, ok;

	decode_ok(a, &I);
	ok = build(&I, buf, sizeof buf);
	stat_insn += I.len;
	if (ok) {
		line(buf, a, I.len);
		return;
	}
	num(t, I.ir);
	if (opt_addr) {
		print_padded(t, 40);
		addr_comment(a, 2);
		P("  [%s]\n", buf);
	} else
		P("\t%s\t/ %s\n", t, buf);
	for (k = 0; k < I.nx; k++) {
		num(t, I.xw[k]);
		line(t, I.xa[k], 2);
	}
}

static void emit_word(unsigned a)
{
	char t[24];
	unsigned v = rw(a);
	int f = wfl[a];
	const char *s = sref(f == 1 ? K_CPTR : f == 2 ? K_SPTR : K_WORD, v);

	if (!s) {
		num(t, v);
		s = t;
	}
	stat_data += 2;
	line(s, a, 2);
}

static int str_at(unsigned x, unsigned b, unsigned *n, int *term)
{
	unsigned k = 0;

	if (!printable(img[x])) return 0;
	while (x + k < b && istextc(img[x + k])) k++;
	*term = x + k < b && img[x + k] == 0;
	*n = k;
	return k >= 4 || (k >= 2 && *term);
}

static void emit_gap(unsigned a, unsigned b)
{
	unsigned n;
	int term;

	while (a < b) {
		if (str_at(a, b, &n, &term)) {
			char *t = malloc(2 * n + 8), *p = t;
			unsigned k;

			*p++ = '<';
			for (k = 0; k < n; k++) {
				unsigned c = img[a + k];
				if (c == '\n') { *p++ = '\\'; *p++ = 'n'; }
				else if (c == '\t') { *p++ = '\\'; *p++ = 't'; }
				else if (c == '\r') { *p++ = '\\'; *p++ = 'r'; }
				else if (c == '>' || c == '\\') { *p++ = '\\'; *p++ = c; }
				else *p++ = c;
			}
			if (term) { *p++ = '\\'; *p++ = '0'; }
			*p++ = '>'; *p = 0;
			line(t, a, 0);
			free(t);
			stat_data += n + term; a += n + term;
		} else if (!(a & 1) && b - a >= 2) {
			char t[512];
			int cnt = 0, pos = 0;
			unsigned start = a;

			t[0] = 0;
			while (a + 1 < b && cnt < 6 && !(cnt && str_at(a, b, &n, &term))) {
				char w[24];
				const char *s = sref(K_WORD, rw(a));

				if (!s) {
					num(w, rw(a));
					s = w;
				}
				pos += snprintf(t + pos, sizeof t - pos, "%s%s", cnt ? "; " : "", s);
				cnt++; a += 2; stat_data += 2;
			}
			line(t, start, a - start);
		} else {
			char t[96];
			int cnt = 0, pos;
			unsigned start = a;

			pos = snprintf(t, sizeof t, ".byte ");
			while (a < b && cnt < 8 && !(cnt && str_at(a, b, &n, &term)) &&
			    !(cnt && !(a & 1) && b - a >= 2)) {
				pos += snprintf(t + pos, sizeof t - pos, "%s0%o", cnt ? "," : "", img[a]);
				cnt++; a++; stat_data++;
			}
			line(t, start, 0);
		}
	}
}

static unsigned nextbound(unsigned a, unsigned to)
{
	unsigned x = a + 1;

	while (x < to && cov[x] != I_INSN && cov[x] != I_WORD && !lname[x]) x++;
	return x;
}

static void emit_seg(unsigned from, unsigned to)
{
	unsigned a = from;

	while (a < to) {
		print_labels(a);
		if (cov[a] == I_INSN) {
			emit_insn(a);
			a += ilen[a]; 
		}
		else if (cov[a] == I_WORD) {
			emit_word(a);
			a += 2; 
		}
		else {
			unsigned b = nextbound(a, to);
			emit_gap(a, b);
			a = b; 
		}
	}
}

static void emit_all(const char *name)
{
	unsigned a, cur;
	int i;
	char t[24];

	if (!collecting) {
		P("/ %s: magic %o, text %o, data %o, bss %o\n", name, magic, ts, ds, bs);
		for (i = 0; i < nsm; i++)
			if (sms[i].ext && lname[sms[i].a]) P(".globl\t%s\n", sms[i].n);
	}
	P(".text\n");
	emit_seg(0, ts);
	if (magic == 0410 && opt_pad && dbase > ts) {
		num(t, dbase - ts);
		P("\t.=.+%s\t/ pad text to the 8K data boundary (NMAGIC)\n", t);
	}
	if (ds) {
		P(".data\n");
		emit_seg(dbase, bbase);
	}
	P(".bss\n");
	cur = bbase;
	for (a = bbase; a <= iend; a++) {
		if (!lname[a]) continue;
		if (a > cur) {
			num(t, a - cur);
			P("\t.=.+%s\n", t);
			cur = a;
		}
		print_labels(a);
	}
	if (iend > cur) {
		num(t, iend - cur);
		P("\t.=.+%s\n", t);
	}
}

static unsigned *cand; static int ncand;
static int cmp_cand(const void *x, const void *y)
{
	unsigned a = *(const unsigned *)x, b = *(const unsigned *)y;

	if (lpr[a] != lpr[b]) return lpr[a] - lpr[b];
	return a < b ? -1 : a > b;
}

static void demote_item(unsigned s)
{
	unsigned k, e = s + ilen[s];

	if (cov[s] == I_INSN) {
		for (k = s; k < e; k += 2) {
			cov[k] = I_WORD;
			cov[k + 1] = I_BODY;
			ilen[k] = 2;
			wfl[k] = 0;
		}
	} else {
		cov[s] = cov[s + 1] = 0;
	}
}

static void finalize_labels(void)
{
	unsigned a;
	int i, cost = 0, kept = 0;
	char nm[16];

	for (i = 0; i < nsm; i++) want(sms[i].a, 1);
	for (a = 0; a <= iend; a++) {
		if (!lpr[a]) continue;
		while (a < MEM && cov[a] == I_BODY) {
			unsigned s = a;

			while (cov[s] == I_BODY) s--;
			if (lpr[a] <= 3) demote_item(s);
			else {
				lpr[a] = 0;
				break;
			}
		}
	}
	ncand = 0;
	cand = malloc((iend + 2) * sizeof *cand);
	for (a = 0; a <= iend; a++) if (lpr[a]) cand[ncand++] = a;
	qsort(cand, ncand, sizeof *cand, cmp_cand);
	for (i = 0; i < ncand; i++) {
		unsigned x = cand[i];
		int c = 1;

		if (hasal[x]) {
			int j; 
			for (j = 0; j < nsm; j++)
				if (sms[j].a == x && !sms[j].primary)
					c++;
		}
		if (cost + c > opt_budget) {
			lpr[x] = 0;
			continue;
		}
		cost += c; kept++;
	}
	for (a = 0; a <= iend; a++) {
		if (!lpr[a]) continue;
		if (named[a]) {
			lname[a] = named[a]; 
			continue; 
		}
		sprintf(nm, "%c%06o", callt[a] ? 'F' : a < ts ? 'L' : a < bbase ? 'D' : 'B', a);
		while (name_used(nm)) nm[0] = nm[0] == 'Z' ? 'Y' : 'Z';
		name_add(nm);
		lname[a] = strdup(nm);
	}
	if (opt_v) fprintf(stderr, "labels: %d wanted, %d kept (budget %d)\n", ncand, kept, opt_budget);
}

static int scan_ptrs(void)
{
	unsigned a;
	int changed = 0;

	for (a = 0; a + 1 < bbase; a += 2) {
		unsigned v;

		if (a >= ts && a < dbase) {
			a = dbase - 2;
			continue;
		}
		if (cov[a] || cov[a + 1]) continue;
		v = rw(a);
		if ((v & 1) || v < 2 || v >= ts || cov[v] || specfail[v]) continue;
		if (walk(v, 1, 3)) changed = 1; else specfail[v] = 1;
	}
	return changed;
}

int main(int argc, char **argv)
{
	const char *file = NULL;
	int i, pass;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-a")) opt_addr = 1;
		else if (!strcmp(argv[i], "-s")) opt_sig = 1;
		else if (!strcmp(argv[i], "-k")) opt_k = 1;
		else if (!strcmp(argv[i], "-n")) opt_pad = 0;
		else if (!strcmp(argv[i], "-v")) opt_v = 1;
		else if (!strcmp(argv[i], "-L") && i + 1 < argc) opt_budget = atoi(argv[++i]);
		else if (argv[i][0] == '-' && argv[i][1]) goto usage;
		else file = argv[i];
	}
	if (!file) {
usage:
		fprintf(stderr, "usage: %s [-a] [-k] [-n] [-s] [-v] [-L labels] a.out\n", argv[0]);
		return 1;
	}
	if (load_aout(file)) return 1;

	walk(0, 0, 0);
	for (pass = 0; pass < 6; pass++) {
		int changed = 0;

		for (i = 0; i < nsm; i++) {
			unsigned a = sms[i].a;

			if (a < ts && !(a & 1) && !cov[a] && !specfail[a]) {
				if (walk(a, 1, 1)) changed = 1; else specfail[a] = 1;
			}
		}
		if (scan_ptrs()) changed = 1;
		if (!changed) break;
	}

	collecting = true; out = fopen("/dev/null", "w");
	emit_all(file);
	fclose(out);
	finalize_labels();
	if (opt_sig) build_sigs();
	collecting = false; out = stdout; stat_insn = stat_data = 0;
	emit_all(file);
	if (opt_v)
		fprintf(stderr, "code %d bytes, data %d bytes; speculative walks: %d ok, %d rejected\n",
		    stat_insn, stat_data, stat_spec_ok, stat_spec_bad);
	return 0;
}