#include "pdp11.h"

#include <stdlib.h>
#include <stdarg.h>

int regdst;
uint16_t ea;

uint32_t mem_brk = 0200000;
uint32_t stack_lo = 0200000;

#define SINCR	(20 * 64)

void mem_reset(uint32_t image_end)
{
	mem_brk = (image_end + 63) & ~63u;
	stack_lo = (uint32_t)SP & ~63u;
}

void mem_set_brk(uint32_t end)
{
	mem_brk = (end + 63) & ~63u;
}

void mem_fault(uint16_t a)
{
	uint32_t sp = SP;

	if (sp < stack_lo && sp >= mem_brk) {
		uint32_t lo = (sp & ~63u);

		lo = lo > SINCR + mem_brk ? lo - SINCR : mem_brk;
		if (lo < stack_lo)
			stack_lo = lo;
		if (a >= stack_lo)
			return;
	}
	trap_sig(V4_SIGSEG);
}

static inline void mchk(uint16_t a)
{
	if (a >= mem_brk && a < stack_lo)
		mem_fault(a);
}

void mem_check(uint16_t a)
{
	mchk(a);
}

void die(const char *m, ...)
{
	fprintf(stderr, "v4linux: %s (pc=%06o)\n", m, PC);
	exit(99);
}

uint16_t rdw(uint16_t a)
{
	if (a & 1) trap_sig(V4_SIGBUS);
	mchk(a);
	return mem[a] | mem[a + 1] << 8;
}

void wrw(uint16_t a, uint16_t v)
{
	if (a & 1) trap_sig(V4_SIGBUS);
	mchk(a);
	mem[a] = v;
	mem[a + 1] = v >> 8;
}

void getops(int mode, int reg, int size)
{
	int inc = (reg >= 6 && size < 2) ? 2 : size;
	uint16_t x;

	regdst = -1;
	switch (mode) {
	case 0: regdst = reg; return;
	case 1: ea = R[reg]; return;
	case 2: ea = R[reg]; R[reg] += inc; return;
	case 3: ea = rdw(R[reg]); R[reg] += 2; return;
	case 4: R[reg] -= inc; ea = R[reg]; return;
	case 5: R[reg] -= 2; ea = rdw(R[reg]); return;
	case 6: x = rdw(PC); PC += 2; ea = R[reg] + x; return;
	case 7: x = rdw(PC); PC += 2; ea = rdw(R[reg] + x); return;
	}
}

void getop(int mode, int reg, int byte)
{
	getops(mode, reg, byte ? 1 : 2);
}

uint16_t load(int byte)
{
	if (regdst >= 0) return byte ? R[regdst] & 0377 : R[regdst];
	if (byte) {
		mchk(ea);
		return mem[ea];
	}
	return rdw(ea);
}

void store(uint16_t v, int byte)
{
	if (regdst >= 0) {
		if (byte) R[regdst] = (R[regdst] & 0177400) | (v & 0377);
		else R[regdst] = v;
	} else if (byte) {
		mchk(ea);
		mem[ea] = v;
	} else {
		wrw(ea, v);
	}
}

void storeb_sext(uint16_t v)
{
	if (regdst >= 0) R[regdst] = (uint16_t)(int16_t)(int8_t)v;
	else {
		mchk(ea);
		mem[ea] = v;
	}
}

void nz(uint16_t v, int byte)
{
	if (byte) {
		fN = (v & 0200) != 0;
		fZ = (v & 0377) == 0;
	} else {
		fN = (v & 0100000) != 0;
		fZ = v == 0;
	}
}

uint16_t get_ps(void)
{
	return 0170000 | fN << 3 | fZ << 2 | fV << 1 | fC;
}

void set_ps_cc(uint16_t ps)
{
	fC = ps & 1;
	fV = (ps >> 1) & 1;
	fZ = (ps >> 2) & 1;
	fN = (ps >> 3) & 1;
}
