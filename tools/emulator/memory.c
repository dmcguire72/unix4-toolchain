#include "pdp11.h"

#include <stdlib.h>

int regdst;
uint16_t ea;

void die(const char *m, ...)
{
	fprintf(stderr, "pdp11: %s (pc=%06o)\n", m, PC);
	exit(99);
}

uint16_t rdw(uint16_t a)
{
	if (a & 1) die("odd address read");
	return mem[a] | mem[a + 1] << 8;
}

void wrw(uint16_t a, uint16_t v)
{
	if (a & 1) die("odd address write");
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
	return byte ? mem[ea] : rdw(ea);
}

void store(uint16_t v, int byte)
{
	if (regdst >= 0) {
		if (byte) R[regdst] = (R[regdst] & 0177400) | (v & 0377);
		else R[regdst] = v;
	} else if (byte) {
		mem[ea] = v;
	} else {
		wrw(ea, v);
	}
}

void storeb_sext(uint16_t v)
{
	if (regdst >= 0) R[regdst] = (int8_t)v;
	else mem[ea] = v;
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
