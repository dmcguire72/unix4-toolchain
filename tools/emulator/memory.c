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

void die(const char *m, ...)
{
	fprintf(stderr, "v4linux: %s (pc=%06o)\n", m, PC);
	if (crash_trace)
		trace_backtrace();
	exit(99);
}
