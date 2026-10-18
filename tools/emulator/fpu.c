#include "pdp11.h"

#include <math.h>
#include <stdint.h>

#define FPS_FER	0100000
#define FPS_FIV	0001000
#define FPS_FD	0000200
#define FPS_FL	0000100
#define FPS_FT	0000040

static double ac[8];
static uint16_t fps;
static int fpN, fpZ, fpV, fpC;

#define DBL	((fps & FPS_FD) != 0)
#define LNG	((fps & FPS_FL) != 0)
#define TRUNC	((fps & FPS_FT) != 0)

void fp_reset(void)
{
	int i;

	for (i = 0; i < 8; i++) ac[i] = 0;
	fps = 0;
	fpN = fpZ = fpV = fpC = 0;
}

static double fget(uint16_t a, int dbl)
{
	uint16_t w0 = rdw(a);
	int e = (w0 >> 7) & 0xff;
	uint64_t m;
	double v;

	if (e == 0) return 0.0;
	m = 0x80 | (w0 & 0x7f);
	m = (m << 16) | rdw(a + 2);
	if (dbl) {
		m = (m << 16) | rdw(a + 4);
		m = (m << 16) | rdw(a + 6);
		v = ldexp((double)m, e - 128 - 56);
	} else {
		v = ldexp((double)m, e - 128 - 24);
	}
	return (w0 & 0x8000) ? -v : v;
}

static void fput(uint16_t a, double v, int dbl)
{
	int e, sign = v < 0;
	double m;
	uint64_t mant;
	uint16_t w0;

	if (v == 0 || isnan(v) || isinf(v)) goto zero;
	if (sign) v = -v;
	m = frexp(v, &e);
	e += 128;
	if (dbl) {
		mant = (uint64_t)ldexp(m, 56);
	} else {
		mant = (uint64_t)floor(ldexp(m, 24) + (TRUNC ? 0 : 0.5));
		if (mant >> 24) { mant >>= 1; e++; }
	}
	if (e <= 0) goto zero;
	if (e > 255) {
		if (fps & FPS_FIV) trap_sig(V4_SIGFPT);
		goto zero;
	}
	if (dbl) {
		w0 = (sign << 15) | (e << 7) | ((mant >> 48) & 0x7f);
		wrw(a, w0);
		wrw(a + 2, mant >> 32);
		wrw(a + 4, mant >> 16);
		wrw(a + 6, mant);
	} else {
		w0 = (sign << 15) | (e << 7) | ((mant >> 16) & 0x7f);
		wrw(a, w0);
		wrw(a + 2, mant);
	}
	return;
zero:
	wrw(a, 0);
	wrw(a + 2, 0);
	if (dbl) { wrw(a + 4, 0); wrw(a + 6, 0); }
}

static double rnd(double x)
{
	int e;
	double m;

	if (DBL || x == 0 || isnan(x) || isinf(x)) return x;
	m = frexp(x, &e);
	m = ldexp(m, 24);
	m = TRUNC ? trunc(m) : (m < 0 ? -floor(-m + 0.5) : floor(m + 0.5));
	return ldexp(m, e - 24);
}

static void fcc(double v)
{
	fpN = v < 0;
	fpZ = v == 0;
	fpV = fpC = 0;
}

static double fsrc(int mode, int reg, int dbl)
{
	if (mode == 0) return ac[reg & 7];
	getops(mode, reg, dbl ? 8 : 4);
	return fget(ea, dbl);
}

static uint16_t get_fps(void)
{
	return (fps & ~017) | fpN << 3 | fpZ << 2 | fpV << 1 | fpC;
}

static void fstep_exec(uint16_t ir)
{
	int a = (ir >> 6) & 3, dm = (ir >> 3) & 7, dr = ir & 7, dbl = DBL;
	double v;

	switch (ir & 0177700) {
	case 0170100: { /* LDFPS */
		uint16_t w;

		getop(dm, dr, 0);
		w = load(0);
		fps = w & ~017;
		fpN = (w >> 3) & 1;
		fpZ = (w >> 2) & 1;
		fpV = (w >> 1) & 1;
		fpC = w & 1;
		return;
	}
	case 0170200: /* STFPS */
		getop(dm, dr, 0);
		store(get_fps(), 0);
		return;
	case 0170300: /* STST: FEC & FEA */
		if (dm == 0) { R[dr] = 0; return; }
		getops(dm, dr, 4);
		wrw(ea, 0);
		wrw(ea + 2, 0);
		return;
	case 0170400: /* CLRF */
		if (dm == 0) ac[dr & 7] = 0;
		else { getops(dm, dr, dbl ? 8 : 4); fput(ea, 0, dbl); }
		fcc(0);
		return;
	case 0170500: /* TSTF */
		fcc(fsrc(dm, dr, dbl));
		return;
	case 0170600: case 0170700: /* ABSF NEGF */
		if (dm == 0) {
			v = ac[dr & 7];
			v = (ir & 0100) ? -v : fabs(v);
			ac[dr & 7] = v;
		} else {
			getops(dm, dr, dbl ? 8 : 4);
			v = fget(ea, dbl);
			v = (ir & 0100) ? -v : fabs(v);
			fput(ea, v, dbl);
		}
		fcc(v);
		return;
	}
	switch (ir & 0177400) {
	case 0171000: v = fsrc(dm, dr, dbl); ac[a] = rnd(ac[a] * v); fcc(ac[a]); return; /* MULF */
	case 0172000: v = fsrc(dm, dr, dbl); ac[a] = rnd(ac[a] + v); fcc(ac[a]); return; /* ADDF */
	case 0172400: v = fsrc(dm, dr, dbl); ac[a] = v; fcc(v); return;		/* LDF */
	case 0173000: v = fsrc(dm, dr, dbl); ac[a] = rnd(ac[a] - v); fcc(ac[a]); return; /* SUBF */
	case 0171400: { /* MODF: AC*src split into integer and fraction */
		double ip, fr;

		v = fsrc(dm, dr, dbl);
		fr = modf(rnd(ac[a] * v), &ip);
		if (!(a & 1)) ac[a | 1] = ip;
		ac[a] = fr;
		fcc(fr);
		return;
	}
	case 0174400: /* DIVF */
		v = fsrc(dm, dr, dbl);
		if (v == 0) {
			fpV = 1;
			if (fps & FPS_FIV) trap_sig(V4_SIGFPT);
			return;
		}
		ac[a] = rnd(ac[a] / v);
		fcc(ac[a]);
		return;
	case 0173400: /* CMPF */
		v = fsrc(dm, dr, dbl) - ac[a];
		fpN = v < 0;
		fpZ = v == 0;
		fpV = fpC = 0;
		return;
	case 0174000: /* STF */
		if (dm == 0) ac[dr & 7] = ac[a];
		else { getops(dm, dr, dbl ? 8 : 4); fput(ea, ac[a], dbl); }
		return;
	case 0177000: { /* LDCIF */
		int sz = LNG ? 4 : 2;
		int32_t n;

		getops(dm, dr, sz);
		if (regdst >= 0) n = (int16_t)R[regdst];
		else if (sz == 2) n = (int16_t)rdw(ea);
		else n = (int32_t)((uint32_t)rdw(ea) << 16 | rdw(ea + 2));
		ac[a] = rnd(n);
		fcc(ac[a]);
		return;
	}
	case 0175400: { /* STCFI */
		double t = trunc(ac[a]);
		int sz = LNG ? 4 : 2;

		getops(dm, dr, sz);
		fpV = fpC = 0;
		if ((sz == 2 && (t > 32767 || t < -32768)) ||
		    (sz == 4 && (t > 2147483647.0 || t < -2147483648.0))) {
			fpC = 1;
			t = 0;
			if (fps & FPS_FIV) trap_sig(V4_SIGFPT);
		}
		fpN = t < 0;
		fpZ = t == 0;
		if (regdst >= 0) R[regdst] = (int16_t)t;
		else if (sz == 2) wrw(ea, (int16_t)t);
		else {
			wrw(ea, (uint32_t)(int32_t)t >> 16);
			wrw(ea + 2, (uint32_t)(int32_t)t);
		}
		return;
	}
	case 0177400: /* LDCDF / LDCFD: load */
		v = fsrc(dm, dr, !dbl);
		if (DBL) ac[a] = v; else ac[a] = rnd(v);
		fcc(ac[a]);
		return;
	case 0176000: /* STCFD / STCDF: store */
		if (dm == 0) ac[dr & 7] = ac[a];
		else { getops(dm, dr, dbl ? 4 : 8); fput(ea, ac[a], !dbl); }
		return;
	case 0175000: { /* STEXP: biased exponent */
		int e = -128;

		if (ac[a] != 0) frexp(ac[a], &e);
		getop(dm, dr, 0);
		store((uint16_t)e, 0);
		fpN = e < 0;
		fpZ = e == 0;
		fpV = fpC = 0;
		return;
	}
	case 0176400: { /* LDEXP */
		int e, dummy;
		double m;

		getop(dm, dr, 0);
		e = (int16_t)load(0);
		if (ac[a] != 0) {
			m = frexp(ac[a], &dummy);
			ac[a] = ldexp(m, e);
		}
		fcc(ac[a]);
		return;
	}
	}
	switch (ir) {
	case 0170000: fN = fpN; fZ = fpZ; fV = fpV; fC = fpC; return; /* CFCC */
	case 0170001: fps &= ~FPS_FD; return;			/* SETF */
	case 0170002: fps &= ~FPS_FL; return;			/* SETI */
	case 0170011: fps |= FPS_FD; return;			/* SETD */
	case 0170012: fps |= FPS_FL; return;			/* SETL */
	}
	trap_sig(V4_SIGINS);
}

void fstep(uint16_t ir)
{
	char text[96];
	uint16_t at = PC - 2;

	if (trace & TR_FPU) disasm(at, text, sizeof text);
	fstep_exec(ir);
	if (trace & TR_FPU)
		tr_line("[fpu] %06o: %06o %-26s ac0=%g ac1=%g ac2=%g ac3=%g "
			"fps=%06o %s/%s cc=%c%c%c%c", at, ir, text, ac[0], ac[1],
			ac[2], ac[3], get_fps(), DBL ? "double" : "float",
			LNG ? "long" : "int", fpN ? 'N' : '-', fpZ ? 'Z' : '-',
			fpV ? 'V' : '-', fpC ? 'C' : '-');
}
