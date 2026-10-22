#include "pdp11.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

double ac[FP_NUM_AC];
uint16_t fps;
int fpN, fpZ, fpV, fpC;

void fp_reset(void)
{
	int i;

	for (i = 0; i < FP_NUM_AC; i++) ac[i] = 0;
	fps = 0;
	fpN = fpZ = fpV = fpC = 0;
}

static inline double pow2i(int k)
{
	uint64_t u = (uint64_t)(k + IEEE754_DOUBLE_EXP_BIAS) << IEEE754_DOUBLE_FRAC_BITS;
	double d;

	memcpy(&d, &u, sizeof d);
	return d;
}

static double fget(uint16_t a, int dbl)
{
	uint16_t w0 = rdw(a);
	int e = (w0 >> FP_EXP_SHIFT) & FP_EXP_MASK;
	uint64_t m;
	double v;

	if (e == 0) return 0.0;
	m = FP_HIDDEN_BIT | (w0 & FP_FRAC_HIGH_MASK);
	m = (m << 16) | rdw(a + PDP11_WORD_SIZE);
	if (dbl) {
		m = (m << 16) | rdw(a + 2 * PDP11_WORD_SIZE);
		m = (m << 16) | rdw(a + 3 * PDP11_WORD_SIZE);
		v = (double)m * pow2i(e - FP_EXP_BIAS - FP_FRAC_BITS_DOUBLE);
	} else {
		v = (double)m * pow2i(e - FP_EXP_BIAS - FP_FRAC_BITS_SINGLE);
	}
	return (w0 & FP_SIGN_BIT) ? -v : v;
}

static void fput_slow(uint16_t a, double v, int dbl)
{
	int e, sign = v < 0;
	double m;
	uint64_t mant;
	uint16_t w0;

	if (v == 0 || isnan(v) || isinf(v)) goto zero;
	if (sign) v = -v;
	m = frexp(v, &e);
	e += FP_EXP_BIAS;
	if (dbl) {
		mant = (uint64_t)ldexp(m, FP_FRAC_BITS_DOUBLE);
	} else {
		mant = (uint64_t)floor(ldexp(m, FP_FRAC_BITS_SINGLE) + (TRUNC ? 0 : FP_ROUND_NEAREST_BIAS));
		if (mant >> FP_FRAC_BITS_SINGLE) { mant >>= 1; e++; }
	}
	if (e <= 0) goto zero;
	if (e > FP_EXP_MAX) {
		if (fps & FPS_FIV) trap_sig(V4_SIGFPT);
		goto zero;
	}
	if (dbl) {
		w0 = (sign << FP_SIGN_SHIFT) | (e << FP_EXP_SHIFT) | ((mant >> FP_MANT_SHIFT_WORD0_DBL) & FP_FRAC_HIGH_MASK);
		wrw(a, w0);
		wrw(a + PDP11_WORD_SIZE, mant >> FP_MANT_SHIFT_WORD1);
		wrw(a + 2 * PDP11_WORD_SIZE, mant >> FP_MANT_SHIFT_WORD2);
		wrw(a + 3 * PDP11_WORD_SIZE, mant);
	} else {
		w0 = (sign << FP_SIGN_SHIFT) | (e << FP_EXP_SHIFT) | ((mant >> FP_MANT_SHIFT_WORD0_SNG) & FP_FRAC_HIGH_MASK);
		wrw(a, w0);
		wrw(a + PDP11_WORD_SIZE, mant);
	}
	return;
zero:
	wrw(a, 0);
	wrw(a + PDP11_WORD_SIZE, 0);
	if (dbl) { wrw(a + 2 * PDP11_WORD_SIZE, 0); wrw(a + 3 * PDP11_WORD_SIZE, 0); }
}

static double rnd_slow(double x)
{
	int e;
	double m;

	if (DBL || x == 0 || isnan(x) || isinf(x)) return x;
	m = frexp(x, &e);
	m = ldexp(m, FP_FRAC_BITS_SINGLE);
	m = TRUNC ? trunc(m) : (m < 0 ? -floor(-m + FP_ROUND_NEAREST_BIAS) : floor(m + FP_ROUND_NEAREST_BIAS));
	return ldexp(m, e - FP_FRAC_BITS_SINGLE);
}

static void fput(uint16_t a, double v, int dbl)
{
	uint64_t b, mant;
	int sign = v < 0, e;
	uint16_t w0;

	if (v == 0 || isnan(v) || isinf(v)) goto zero;
	memcpy(&b, &v, sizeof b);
	b &= ~IEEE754_DOUBLE_SIGN_MASK;
	if (UNLIKELY((b >> IEEE754_DOUBLE_FRAC_BITS) == 0)) {
		fput_slow(a, v, dbl);
		return;
	}
	if (dbl) {
		mant = (IEEE754_DOUBLE_HIDDEN_BIT | (b & IEEE754_DOUBLE_FRAC_MASK)) << FP_FAST_DBL_MANT_SHIFT;
	} else {
		if (!TRUNC) b += FP_FAST_SNG_ROUND_BIT;
		b &= ~FP_FAST_SNG_TRUNC_MASK;
		mant = FP_FAST_SNG_HIDDEN_BIT | ((b >> FP_FAST_SNG_FRAC_SHIFT) & FP_FAST_SNG_FRAC_MASK);
	}
	e = (int)(b >> IEEE754_DOUBLE_FRAC_BITS) - FP_FAST_EXP_BIAS_OFFSET;
	if (e <= 0) goto zero;
	if (e > FP_EXP_MAX) {
		if (fps & FPS_FIV) trap_sig(V4_SIGFPT);
		goto zero;
	}
	if (dbl) {
		w0 = (sign << FP_SIGN_SHIFT) | (e << FP_EXP_SHIFT) | ((mant >> FP_MANT_SHIFT_WORD0_DBL) & FP_FRAC_HIGH_MASK);
		wrw(a, w0);
		wrw(a + PDP11_WORD_SIZE, mant >> FP_MANT_SHIFT_WORD1);
		wrw(a + 2 * PDP11_WORD_SIZE, mant >> FP_MANT_SHIFT_WORD2);
		wrw(a + 3 * PDP11_WORD_SIZE, mant);
	} else {
		w0 = (sign << FP_SIGN_SHIFT) | (e << FP_EXP_SHIFT) | ((mant >> FP_MANT_SHIFT_WORD0_SNG) & FP_FRAC_HIGH_MASK);
		wrw(a, w0);
		wrw(a + PDP11_WORD_SIZE, mant);
	}
	return;
zero:
	wrw(a, 0);
	wrw(a + PDP11_WORD_SIZE, 0);
	if (dbl) { wrw(a + 2 * PDP11_WORD_SIZE, 0); wrw(a + 3 * PDP11_WORD_SIZE, 0); }
}

static double rnd(double x)
{
	uint64_t b, mag;

	if (DBL || x == 0 || isnan(x) || isinf(x)) return x;
	memcpy(&b, &x, sizeof b);
	mag = b & ~IEEE754_DOUBLE_SIGN_MASK;
	if (UNLIKELY((mag >> IEEE754_DOUBLE_FRAC_BITS) == 0))
		return rnd_slow(x);
	if (!TRUNC) mag += FP_FAST_SNG_ROUND_BIT;
	mag &= ~FP_FAST_SNG_TRUNC_MASK;
	b = (b & IEEE754_DOUBLE_SIGN_MASK) | mag;
	memcpy(&x, &b, sizeof x);
	return x;
}

static void fcc(double v)
{
	fpN = v < 0;
	fpZ = v == 0;
	fpV = fpC = 0;
}

static double fsrc(int mode, int reg, int dbl)
{
	if (mode == PDP11_MODE_DIRECT) return ac[reg & FP_IR_DR_MASK];

	if (mode == PDP11_MODE_AUTOINC && reg == PDP11_PC_REG) {
		uint16_t w0 = rdw(PC);
		PC += PDP11_WORD_SIZE;
		int e = (w0 >> FP_EXP_SHIFT) & FP_EXP_MASK;
		if (e == 0) return 0.0;
		uint64_t m = (uint64_t)(FP_HIDDEN_BIT | (w0 & FP_FRAC_HIGH_MASK)) << (dbl ? FP_MANT_SHIFT_WORD0_DBL : FP_MANT_SHIFT_WORD0_SNG);
		double v = (double)m * pow2i(e - FP_EXP_BIAS - (dbl ? FP_FRAC_BITS_DOUBLE : FP_FRAC_BITS_SINGLE));
		return (w0 & FP_SIGN_BIT) ? -v : v;
	}

	getops(mode, reg, dbl ? FP_DOUBLE_BYTES : FP_SINGLE_BYTES);
	return fget(ea, dbl);
}

static uint16_t get_fps(void)
{
	return (fps & ~FPS_CC_MASK) |
	       (fpN << FPS_CC_N_SHIFT) |
	       (fpZ << FPS_CC_Z_SHIFT) |
	       (fpV << FPS_CC_V_SHIFT) |
	       (fpC << FPS_CC_C_SHIFT);
}

static void fstep_exec(uint16_t ir)
{
	int a = (ir >> FP_IR_AC_SHIFT) & FP_IR_AC_MASK;
	int dm = (ir >> FP_IR_DM_SHIFT) & FP_IR_DM_MASK;
	int dr = ir & FP_IR_DR_MASK;
	int dbl = DBL;
	double v;

	switch (ir & FP_OP_MASK_GROUP1) {
	case OP_LDFPS: {
		uint16_t w;

		getop(dm, dr, 0);
		w = load(0);
		fps = w & ~FPS_CC_MASK;
		fpN = (w >> FPS_CC_N_SHIFT) & 1;
		fpZ = (w >> FPS_CC_Z_SHIFT) & 1;
		fpV = (w >> FPS_CC_V_SHIFT) & 1;
		fpC = (w >> FPS_CC_C_SHIFT) & 1;
		return;
	}
	case OP_STFPS:
		getop(dm, dr, 0);
		store(get_fps(), 0);
		return;
	case OP_STST:
		if (dm == PDP11_MODE_DIRECT) { R[dr] = 0; return; }
		getops(dm, dr, INT_LONG_BYTES);
		wrw(ea, 0);
		wrw(ea + PDP11_WORD_SIZE, 0);
		return;
	case OP_CLRF:
		if (dm == PDP11_MODE_DIRECT) ac[dr & FP_IR_DR_MASK] = 0;
		else { getops(dm, dr, dbl ? FP_DOUBLE_BYTES : FP_SINGLE_BYTES); fput(ea, 0, dbl); }
		fcc(0);
		return;
	case OP_TSTF:
		fcc(fsrc(dm, dr, dbl));
		return;
	case OP_ABSF: case OP_NEGF:
		if (dm == PDP11_MODE_DIRECT) {
			v = ac[dr & FP_IR_DR_MASK];
			v = (ir & FP_IR_NEG_BIT) ? -v : fabs(v);
			ac[dr & FP_IR_DR_MASK] = v;
		} else {
			getops(dm, dr, dbl ? FP_DOUBLE_BYTES : FP_SINGLE_BYTES);
			v = fget(ea, dbl);
			v = (ir & FP_IR_NEG_BIT) ? -v : fabs(v);
			fput(ea, v, dbl);
		}
		fcc(v);
		return;
	}
	switch (ir & FP_OP_MASK_GROUP2) {
	case OP_MULF: v = fsrc(dm, dr, dbl); ac[a] = rnd(ac[a] * v); fcc(ac[a]); return;
	case OP_ADDF: v = fsrc(dm, dr, dbl); ac[a] = rnd(ac[a] + v); fcc(ac[a]); return;
	case OP_LDF: v = fsrc(dm, dr, dbl); ac[a] = v; fcc(v); return;
	case OP_SUBF: v = fsrc(dm, dr, dbl); ac[a] = rnd(ac[a] - v); fcc(ac[a]); return;
	case OP_MODF: {
		double ip, fr;

		v = fsrc(dm, dr, dbl);
		fr = modf(rnd(ac[a] * v), &ip);
		if (!(a & 1)) ac[a | 1] = ip;
		ac[a] = fr;
		fcc(fr);
		return;
	}
	case OP_DIVF:
		v = fsrc(dm, dr, dbl);
		if (v == 0) {
			fpV = 1;
			if (fps & FPS_FIV) trap_sig(V4_SIGFPT);
			return;
		}
		ac[a] = rnd(ac[a] / v);
		fcc(ac[a]);
		return;
	case OP_CMPF:
		v = fsrc(dm, dr, dbl) - ac[a];
		fpN = v < 0;
		fpZ = v == 0;
		fpV = fpC = 0;
		return;
	case OP_STF:
		if (dm == PDP11_MODE_DIRECT) ac[dr & FP_IR_DR_MASK] = ac[a];
		else { getops(dm, dr, dbl ? FP_DOUBLE_BYTES : FP_SINGLE_BYTES); fput(ea, ac[a], dbl); }
		return;
	case OP_LDCIF: {
		int sz = LNG ? INT_LONG_BYTES : INT_SHORT_BYTES;
		int32_t n;

		getops(dm, dr, sz);
		if (regdst >= 0) n = (int16_t)R[regdst];
		else if (sz == INT_SHORT_BYTES) n = (int16_t)rdw(ea);
		else n = (int32_t)((uint32_t)rdw(ea) << 16 | rdw(ea + PDP11_WORD_SIZE));
		ac[a] = rnd(n);
		fcc(ac[a]);
		return;
	}
	case OP_STCFI: {
		double t = trunc(ac[a]);
		int sz = LNG ? INT_LONG_BYTES : INT_SHORT_BYTES;

		getops(dm, dr, sz);
		fpV = fpC = 0;
		if ((sz == INT_SHORT_BYTES && (t > FP_INT16_MAX || t < FP_INT16_MIN)) ||
		    (sz == INT_LONG_BYTES && (t > FP_INT32_MAX_F || t < FP_INT32_MIN_F))) {
			fpC = 1;
			t = 0;
			if (fps & FPS_FIV) trap_sig(V4_SIGFPT);
		}
		fpN = t < 0;
		fpZ = t == 0;
		if (regdst >= 0) R[regdst] = (int16_t)t;
		else if (sz == INT_SHORT_BYTES) wrw(ea, (int16_t)t);
		else {
			wrw(ea, (uint32_t)(int32_t)t >> 16);
			wrw(ea + PDP11_WORD_SIZE, (uint32_t)(int32_t)t);
		}
		return;
	}
	case OP_LDCDF:
		v = fsrc(dm, dr, !dbl);
		if (DBL) ac[a] = v; else ac[a] = rnd(v);
		fcc(ac[a]);
		return;
	case OP_STCFD:
		if (dm == PDP11_MODE_DIRECT) ac[dr & FP_IR_DR_MASK] = ac[a];
		else { getops(dm, dr, dbl ? FP_SINGLE_BYTES : FP_DOUBLE_BYTES); fput(ea, ac[a], !dbl); }
		return;
	case OP_STEXP: {
		int e = -FP_EXP_BIAS;

		if (ac[a] != 0) frexp(ac[a], &e);
		getop(dm, dr, 0);
		store((uint16_t)e, 0);
		fpN = e < 0;
		fpZ = e == 0;
		fpV = fpC = 0;
		return;
	}
	case OP_LDEXP: {
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
	case OP_CFCC: fN = fpN; fZ = fpZ; fV = fpV; fC = fpC; return;
	case OP_SETF: fps &= ~FPS_FD; return;
	case OP_SETI: fps &= ~FPS_FL; return;
	case OP_SETD: fps |= FPS_FD; return;
	case OP_SETL: fps |= FPS_FL; return;
	}
	trap_sig(V4_SIGINS);
}

static void fstep_traced(uint16_t ir)
{
	char text[96] = "";
	uint16_t at = PC - PDP11_WORD_SIZE;

	disasm(at, text, sizeof text);

	fstep_exec(ir);

	tr_line("[fpu] %06o: %-22o %-28s "
		"ac0=%-5g ac1=%-5g ac2=%-5g ac3=%-5g "
		"fps=%06o %s/%s cc=%c%c%c%c",
		at, ir, text, ac[0], ac[1], ac[2], ac[3], get_fps(),
		DBL ? "double" : "float", LNG ? "long" : "int",
		fpN ? 'N' : '-', fpZ ? 'Z' : '-', fpV ? 'V' : '-',
		fpC ? 'C' : '-');
}

void fstep(uint16_t ir)
{
	if (UNLIKELY(trace & TR_FPU)) {
		fstep_traced(ir);
		return;
	}
	fstep_exec(ir);
}