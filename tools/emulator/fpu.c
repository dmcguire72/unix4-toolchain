#include "pdp11.h"

#include <math.h>
#include <stdint.h>

static double ac[8];
static int fpFD, fpFL, fpFT, fpN, fpZ, fpV, fpC;

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

	if (v == 0 || !isfinite(v)) {
		wrw(a, 0);
		wrw(a + 2, 0);
		if (dbl) { wrw(a + 4, 0); wrw(a + 6, 0); }
		return;
	}
	if (sign) v = -v;
	m = frexp(v, &e);
	e += 128;
	if (dbl) mant = (uint64_t)ldexp(m, 56);
	else mant = (uint64_t)floor(ldexp(m, 24) + (fpFT ? 0 : 0.5));
	if (!dbl && mant >> 24) { mant >>= 1; e++; }
	if (e <= 0 || e > 255) {
		wrw(a, 0);
		wrw(a + 2, 0);
		if (dbl) { wrw(a + 4, 0); wrw(a + 6, 0); }
		return;
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

void fstep(uint16_t ir)
{
	int a = (ir >> 6) & 3, dm = (ir >> 3) & 7, dr = ir & 7, dbl = fpFD;
	double v;

	switch (ir & 0177700) {
	case 0170100: { /* LDFPS */
		uint16_t w;
		getop(dm, dr, 0);
		w = load(0);
		fpFD = (w >> 7) & 1;
		fpFL = (w >> 6) & 1;
		fpFT = (w >> 5) & 1;
		fpN = (w >> 3) & 1;
		fpZ = (w >> 2) & 1;
		fpV = (w >> 1) & 1;
		fpC = w & 1;
		return;
	}
	case 0170200: case 0170300: { /* STFPS / STST */
		uint16_t w = fpFD << 7 | fpFL << 6 | fpFT << 5 | fpN << 3 | fpZ << 2 | fpV << 1 | fpC;
		getop(dm, dr, 0);
		store(w, 0);
		return;
	}
	case 0170400: /* CLRF */
		if (dm == 0) ac[dr & 7] = 0;
		else { getops(dm, dr, dbl ? 8 : 4); fput(ea, 0, dbl); }
		fcc(0);
		return;
	case 0170500: /* TSTF */
		v = fsrc(dm, dr, dbl);
		fcc(v);
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
	case 0171000: v = fsrc(dm, dr, dbl); ac[a] *= v; fcc(ac[a]); return; /* MULF */
	case 0172000: v = fsrc(dm, dr, dbl); ac[a] += v; fcc(ac[a]); return; /* ADDF */
	case 0172400: v = fsrc(dm, dr, dbl); ac[a] = v; fcc(v); return; /* LDF */
	case 0173000: v = fsrc(dm, dr, dbl); ac[a] -= v; fcc(ac[a]); return; /* SUBF */
	case 0174400: /* DIVF */
		v = fsrc(dm, dr, dbl);
		if (v == 0) { fpV = 1; return; }
		ac[a] /= v;
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
		int sz = fpFL ? 4 : 2;
		int32_t n;
		getops(dm, dr, sz);
		if (regdst >= 0) n = (int16_t)R[regdst];
		else if (sz == 2) n = (int16_t)rdw(ea);
		else n = (int32_t)((uint32_t)rdw(ea) << 16 | rdw(ea + 2));
		ac[a] = n;
		fcc(ac[a]);
		return;
	}
	case 0175400: { /* STCFI */
		double t = trunc(ac[a]);
		int sz = fpFL ? 4 : 2;
		getops(dm, dr, sz);
		fpV = fpC = 0;
		if ((sz == 2 && (t > 32767 || t < -32768)) ||
		    (sz == 4 && (t > 2147483647.0 || t < -2147483648.0))) {
			fpC = 1;
			t = 0;
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
	case 0177400: /* LDCDF (LDCFD): load other precision */
		v = fsrc(dm, dr, !dbl);
		ac[a] = v;
		fcc(v);
		return;
	case 0176000: /* STCFD (STCDF): store in other precision */
		if (dm == 0) ac[dr & 7] = ac[a];
		else { getops(dm, dr, dbl ? 4 : 8); fput(ea, ac[a], !dbl); }
		return;
	case 0175000: { /* STEXP */
		int e = 0;
		double m = ac[a];
		if (m != 0) { frexp(m, &e); e += 128; }
		e -= 128;
		getop(dm, dr, 0);
		store(e, 0);
		fcc(e);
		fpN = e < 0;
		fpZ = e == 0;
		return;
	}
	case 0176400: { /* LDEXP */
		int e;
		double m;
		getop(dm, dr, 0);
		e = (int16_t)load(0);
		m = frexp(ac[a], &(int){0});
		if (ac[a] == 0) { fcc(0); return; }
		ac[a] = ldexp(m, e);
		fcc(ac[a]);
		return;
	}
	}
	if (ir == 0170000) { fN = fpN; fZ = fpZ; fV = fpV; fC = fpC; return; } /* CFCC */
	if (ir == 0170001) { fpFD = 0; return; } /* SETF */
	if (ir == 0170002) { fpFL = 0; return; } /* SETI */
	if (ir == 0170011) { fpFD = 1; return; } /* SETD */
	if (ir == 0170012) { fpFL = 1; return; } /* SETL */
	die("illegal FP instruction");
}
