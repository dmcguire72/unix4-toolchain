#include "pdp11.h"

static unsigned long prof_tick;

static void branch(int cond, uint16_t ir)
{
	if (cond) PC += 2 * (int8_t)ir;
}

static void push(uint16_t v)
{
	SP -= 2;
	wrw(SP, v);
}

static uint16_t pop(void)
{
	uint16_t v = rdw(SP);

	SP += 2;
	return v;
}

#define REGBIT 0x10000u

static inline __attribute__((always_inline))
uint32_t opaddr(int mode, int reg, int size)
{
	int inc = (reg >= 6 && size < 2) ? 2 : size;
	uint16_t x, a;

	switch (mode) {
	case 0: return REGBIT | reg;
	case 1: return R[reg];
	case 2: a = R[reg]; R[reg] += inc; return a;
	case 3: a = rdw(R[reg]); R[reg] += 2; return a;
	case 4: R[reg] -= inc; return R[reg];
	case 5: R[reg] -= 2; return rdw(R[reg]);
	case 6: x = rdw(PC); PC += 2; return (uint16_t)(R[reg] + x);
	default: x = rdw(PC); PC += 2; return rdw(R[reg] + x);
	}
}

static inline __attribute__((always_inline))
uint16_t ldv(uint32_t a, int byte)
{
	if (a & REGBIT) {
		uint16_t v = R[a & 7];

		return byte ? v & 0377 : v;
	}
	if (byte) {
		mchk(a);
		return mem[a];
	}
	return rdw(a);
}

static inline __attribute__((always_inline))
void stv(uint32_t a, uint16_t v, int byte)
{
	if (a & REGBIT) {
		if (byte) R[a & 7] = (R[a & 7] & 0177400) | (v & 0377);
		else R[a & 7] = v;
	} else if (byte) {
		mchk(a);
		mem[a] = v;
	} else {
		wrw(a, v);
	}
}

static inline __attribute__((always_inline))
void double_op(uint16_t ir, int opc, int byte)
{
	int sm = (ir >> 9) & 7, sr = (ir >> 6) & 7;
	int dm = (ir >> 3) & 7, dr = ir & 7;
	uint16_t s, d, r;
	uint32_t sa, da;
	uint16_t sign = byte ? 0x80 : 0x8000;

	switch (opc) {
	case 1:	/* MOV / MOVB */
		sa = opaddr(sm, sr, byte ? 1 : 2);
		s = ldv(sa, byte);
		da = opaddr(dm, dr, byte ? 1 : 2);
		nz(s, byte);
		fV = 0;
		if (byte) {
			if (da & REGBIT) R[da & 7] = (uint16_t)(int16_t)(int8_t)s;
			else { mchk(da); mem[da] = s; }
		} else {
			stv(da, s, 0);
		}
		return;
	case 2:	/* CMP / CMPB: src - dst */
		sa = opaddr(sm, sr, byte ? 1 : 2);
		s = ldv(sa, byte);
		da = opaddr(dm, dr, byte ? 1 : 2);
		d = ldv(da, byte);
		r = s - d;
		nz(r, byte);
		fV = ((s ^ d) & (s ^ r) & sign) != 0;
		fC = byte ? (s & 0xff) < (d & 0xff) : s < d;
		return;
	case 3:	/* BIT / BITB */
		sa = opaddr(sm, sr, byte ? 1 : 2);
		s = ldv(sa, byte);
		da = opaddr(dm, dr, byte ? 1 : 2);
		d = ldv(da, byte);
		nz(s & d, byte);
		fV = 0;
		return;
	case 4:	/* BIC / BICB */
		sa = opaddr(sm, sr, byte ? 1 : 2);
		s = ldv(sa, byte);
		da = opaddr(dm, dr, byte ? 1 : 2);
		d = ldv(da, byte);
		r = d & ~s;
		nz(r, byte);
		fV = 0;
		stv(da, r, byte);
		return;
	case 5:	/* BIS / BISB */
		sa = opaddr(sm, sr, byte ? 1 : 2);
		s = ldv(sa, byte);
		da = opaddr(dm, dr, byte ? 1 : 2);
		d = ldv(da, byte);
		r = d | s;
		nz(r, byte);
		fV = 0;
		stv(da, r, byte);
		return;
	default: /* 6: ADD (word) / SUB (byte flag set) */
		sa = opaddr(sm, sr, 2);
		s = ldv(sa, 0);
		da = opaddr(dm, dr, 2);
		d = ldv(da, 0);
		if (!byte) {
			uint32_t t = (uint32_t)s + d;

			r = t;
			fV = (~(s ^ d) & (s ^ r) & 0x8000) != 0;
			fC = (t >> 16) & 1;
		} else {
			r = d - s;
			fV = ((s ^ d) & (d ^ r) & 0x8000) != 0;
			fC = d < s;
		}
		nz(r, 0);
		stv(da, r, 0);
		return;
	}
}

static void eis_op(uint16_t ir)
{
	int rg = (ir >> 6) & 7, dm = (ir >> 3) & 7, dr = ir & 7;
	uint16_t s, d, r;
	uint32_t da;

	switch ((ir >> 9) & 7) {
	case 0: { /* MUL */
		int32_t res;

		da = opaddr(dm, dr, 2);
		s = ldv(da, 0);
		res = (int32_t)(int16_t)R[rg] * (int16_t)s;
		if (rg & 1) R[rg] = res;
		else { R[rg] = res >> 16; R[rg | 1] = res; }
		fN = res < 0;
		fZ = res == 0;
		fV = 0;
		fC = res < -32768 || res > 32767;
		return;
	}
	case 1: { /* DIV */
		int64_t dv, q;
		int16_t sv;

		da = opaddr(dm, dr, 2);
		sv = ldv(da, 0);
		if (sv == 0) {		/* divide by zero */
			fN = 0; fZ = 0; fV = 1; fC = 1;
			return;
		}
		dv = (int32_t)((uint32_t)R[rg] << 16 | R[rg | 1]);
		q = dv / sv;
		if (q > 32767 || q < -32768) {	/* overflow */
			fV = 1; fC = 0;
			fN = dv < 0;
			fZ = 0;
			return;
		}
		R[rg] = (uint16_t)q;
		R[rg | 1] = (uint16_t)(dv % sv);
		fN = q < 0;
		fZ = q == 0;
		fV = 0;
		fC = 0;
		return;
	}
	case 2: { /* ASH */
		int n, k;
		int16_t v = R[rg], cur;

		da = opaddr(dm, dr, 2);
		n = ldv(da, 0) & 077;
		if (n & 040) n -= 64;
		fV = fC = 0;
		cur = v;
		if (n > 0) {
			for (k = 0; k < n; k++) {
				fC = ((uint16_t)cur >> 15) & 1;
				cur = (int16_t)((uint16_t)cur << 1);
				if ((cur < 0) != (v < 0)) fV = 1;
			}
		} else if (n < 0) {
			for (k = 0; k < -n; k++) {
				fC = cur & 1;
				cur >>= 1;
			}
		}
		R[rg] = (uint16_t)cur;
		nz(R[rg], 0);
		return;
	}
	case 3: { /* ASHC */
		int n, k;
		int32_t v = (int32_t)((uint32_t)R[rg] << 16 | R[rg | 1]), cur;

		da = opaddr(dm, dr, 2);
		n = ldv(da, 0) & 077;
		if (n & 040) n -= 64;
		fV = fC = 0;
		cur = v;
		if (n > 0) {
			for (k = 0; k < n; k++) {
				fC = ((uint32_t)cur >> 31) & 1;
				cur = (int32_t)((uint32_t)cur << 1);
				if ((cur < 0) != (v < 0)) fV = 1;
			}
		} else if (n < 0) {
			for (k = 0; k < -n; k++) {
				fC = cur & 1;
				cur >>= 1;
			}
		}
		R[rg] = (uint32_t)cur >> 16;
		R[rg | 1] = (uint16_t)cur;
		fN = cur < 0;
		fZ = cur == 0;
		return;
	}
	case 4: /* XOR */
		da = opaddr(dm, dr, 2);
		d = ldv(da, 0);
		r = d ^ R[rg];
		nz(r, 0);
		fV = 0;
		stv(da, r, 0);
		return;
	case 7: /* SOB */
		if (--R[rg] != 0) PC -= 2 * (ir & 077);
		return;
	}
	/* 075xxx FIS and 076xxx CIS are not present on this CPU */
	trap_sig(V4_SIGINS);
}

static inline __attribute__((always_inline))
void single_op(uint16_t ir, int byte)
{
	int dm = (ir >> 3) & 7, dr = ir & 7;
	uint16_t m = byte ? 0xff : 0xffff;
	uint16_t sb = byte ? 0x80 : 0x8000;
	uint16_t v;
	int c, nc;
	uint32_t da;

	switch ((ir >> 6) & 077) {
	case 050: /* CLR */
		da = opaddr(dm, dr, byte ? 1 : 2);
		stv(da, 0, byte);
		fN = 0; fZ = 1; fV = fC = 0;
		return;
	case 051: /* COM */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ~ldv(da, byte);
		stv(da, v, byte);
		nz(v, byte);
		fV = 0; fC = 1;
		return;
	case 052: /* INC */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		fV = (v & m) == (sb - 1);
		v++;
		stv(da, v, byte);
		nz(v, byte);
		return;
	case 053: /* DEC */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		fV = (v & m) == sb;
		v--;
		stv(da, v, byte);
		nz(v, byte);
		return;
	case 054: /* NEG */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		fV = (v & m) == sb;
		v = -v;
		stv(da, v, byte);
		nz(v, byte);
		fC = (v & m) != 0;
		return;
	case 055: /* ADC */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		c = fC;
		fV = c && (v & m) == (sb - 1);
		fC = c && (v & m) == m;
		v += c;
		stv(da, v, byte);
		nz(v, byte);
		return;
	case 056: /* SBC */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		c = fC;
		fV = c && (v & m) == sb;
		fC = c && (v & m) == 0;
		v -= c;
		stv(da, v, byte);
		nz(v, byte);
		return;
	case 057: /* TST */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		nz(v, byte);
		fV = fC = 0;
		return;
	case 060: /* ROR */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		nc = v & 1;
		v = (v & m) >> 1;
		if (fC) v |= sb;
		stv(da, v, byte);
		nz(v, byte);
		fC = nc;
		fV = fN ^ fC;
		return;
	case 061: /* ROL */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		nc = (v & sb) != 0;
		v = v << 1;
		if (fC) v |= 1;
		stv(da, v, byte);
		nz(v, byte);
		fC = nc;
		fV = fN ^ fC;
		return;
	case 062: /* ASR */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		nc = v & 1;
		v = ((v & m) >> 1) | (v & sb);
		stv(da, v, byte);
		nz(v, byte);
		fC = nc;
		fV = fN ^ fC;
		return;
	case 063: /* ASL */
		da = opaddr(dm, dr, byte ? 1 : 2);
		v = ldv(da, byte);
		nc = (v & sb) != 0;
		v = v << 1;
		stv(da, v, byte);
		nz(v, byte);
		fC = nc;
		fV = fN ^ fC;
		return;
	case 064:
		if (!byte) { /* MARK nn */
			SP = PC + 2 * (ir & 077);
			PC = R[5];
			R[5] = pop();
			return;
		}
		/* MTPS */
		da = opaddr(dm, dr, 1);
		set_ps_cc(ldv(da, 1));
		return;
	case 065: /* MFPI / MFPD */
		if (byte) break;
		da = opaddr(dm, dr, 2);
		v = ldv(da, 0);
		push(v);
		nz(v, 0);
		fV = 0;
		return;
	case 066: /* MTPI / MTPD */
		if (byte) break;
		v = pop();
		da = opaddr(dm, dr, 2);
		stv(da, v, 0);
		nz(v, 0);
		fV = 0;
		return;
	case 067:
		if (!byte) { /* SXT */
			da = opaddr(dm, dr, 2);
			stv(da, fN ? 0xffff : 0, 0);
			fZ = !fN;
			fV = 0;
			return;
		}
		/* MFPS */
		da = opaddr(dm, dr, 1);
		v = fN << 3 | fZ << 2 | fV << 1 | fC;
		if (da & REGBIT) R[da & 7] = (uint16_t)(int16_t)(int8_t)v;
		else {
			mchk(da);
			mem[da] = v;
		}
		nz(v, 1);
		fV = 0;
		return;
	}
	trap_sig(V4_SIGINS);
}

static void misc_op(uint16_t ir)
{
	int dm = (ir >> 3) & 7, dr = ir & 7;
	uint16_t t;
	uint32_t da;

	switch (ir >> 6) {
	case 0:
		switch (ir) {
		case 0000000:	/* HALT */
			trap_sig(V4_SIGBUS);
		case 0000001:	/* WAIT */
		case 0000005:	/* RESET */
			return;
		case 0000002:	/* RTI */
		case 0000006:	/* RTT */
			t = pop();
			{
				uint16_t ps = pop();

				PC = t;
				set_ps_cc(ps);
			}
			return;
		case 0000003:	/* BPT */
			trap_sig(V4_SIGTRC);
		case 0000004:	/* IOT */
			trap_sig(V4_SIGIOT);
		}
		trap_sig(V4_SIGINS);	/* MFPT + unused */
	case 1:	/* JMP */
		da = opaddr(dm, dr, 2);
		if (da & REGBIT) trap_sig(V4_SIGBUS);
		PC = da;
		return;
	case 2:
		if ((ir & 070) == 0) {	/* RTS */
			if (dr == 7 && UNLIKELY(trace & TR_FUNC))
				trace_ret(SP);
			PC = R[dr];
			R[dr] = pop();
			return;
		}
		if ((ir & 0177770) == 0000230)	/* SPL */
			return;
		if ((ir & 0177740) == 0000240) {	/* CCC/SCC/NOP */
			int set = ir & 020;

			if (ir & 1) fC = set != 0;
			if (ir & 2) fV = set != 0;
			if (ir & 4) fZ = set != 0;
			if (ir & 010) fN = set != 0;
			return;
		}
		break;
	case 3:	/* SWAB */
		da = opaddr(dm, dr, 2);
		t = ldv(da, 0);
		t = t >> 8 | t << 8;
		stv(da, t, 0);
		fN = (t & 0x80) != 0;
		fZ = (t & 0xff) == 0;
		fV = fC = 0;
		return;
	}
	trap_sig(V4_SIGINS);
}

static inline __attribute__((always_inline)) void step_one(void)
{
	uint16_t ir;

	if (UNLIKELY(sig_pending || prof_scale || trace)) {
		if (sig_pending) check_signals();
		if (prof_scale && ++prof_tick >= 8192) {
			prof_tick = 0;
			prof_sample();
		}
		if (trace & TR_CPU)
			trace_cpu();
	}

	ir = rdw(PC);
	PC += 2;

	switch (ir >> 12) {
	case 1: double_op(ir, 1, 0); return;
	case 2: double_op(ir, 2, 0); return;
	case 3: double_op(ir, 3, 0); return;
	case 4: double_op(ir, 4, 0); return;
	case 5: double_op(ir, 5, 0); return;
	case 6: double_op(ir, 6, 0); return;
	case 9: double_op(ir, 1, 1); return;
	case 10: double_op(ir, 2, 1); return;
	case 11: double_op(ir, 3, 1); return;
	case 12: double_op(ir, 4, 1); return;
	case 13: double_op(ir, 5, 1); return;
	case 14: double_op(ir, 6, 1); return;
	case 7: eis_op(ir); return;
	case 15: fstep(ir); return;		/* 17xxxx: fp */
	case 0:
		if (ir < 0000400) {
			misc_op(ir);
			return;
		}
		if (ir < 0004000) {
			switch (ir & 0177400) {
			case 0000400: branch(1, ir); return;
			case 0001000: branch(!fZ, ir); return;
			case 0001400: branch(fZ, ir); return;
			case 0002000: branch(!(fN ^ fV), ir); return;
			case 0002400: branch(fN ^ fV, ir); return;
			case 0003000: branch(!(fZ | (fN ^ fV)), ir); return;
			default:      branch(fZ | (fN ^ fV), ir); return;
			}
		}
		if ((ir & 0177000) == 0004000) {	/* JSR */
			int rg = (ir >> 6) & 7;
			uint16_t t;

			uint32_t da = opaddr((ir >> 3) & 7, ir & 7, 2);

			if (da & REGBIT) trap_sig(V4_SIGBUS);
			t = da;
			push(R[rg]);
			R[rg] = PC;
			PC = t;
			if (rg == 7 && UNLIKELY(trace & TR_FUNC))
				trace_call(ir, t);
			if (rg == 7 && UNLIKELY(hooks_on))
				jsr_hook(t);
			return;
		}
		single_op(ir, 0);
		return;
	}

	switch (ir & 0177400) {
	case 0100000: branch(!fN, ir); return;		/* BPL  */
	case 0100400: branch(fN, ir); return;		    /* BMI  */
	case 0101000: branch(!(fC | fZ), ir); return;	/* BHI  */
	case 0101400: branch(fC | fZ, ir); return;	/* BLOS */
	case 0102000: branch(!fV, ir); return;		/* BVC  */
	case 0102400: branch(fV, ir); return;		    /* BVS  */
	case 0103000: branch(!fC, ir); return;		/* BCC  */
	case 0103400: branch(fC, ir); return;		    /* BCS  */
	case 0104000: trap_sig(V4_SIGEMT);		    /* EMT  */
	case 0104400: v4sys(ir & 077); return;		    /* TRAP */
	}
	single_op(ir, 1);
}

void step(void)
{
	step_one();
}

void run_cpu(void)
{
	for (;;)
		step_one();
}