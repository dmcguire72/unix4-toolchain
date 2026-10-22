#ifndef PDP11_H
#define PDP11_H

#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

#include "numbers.h"

#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

void die(const char *m, ...) __attribute__((noreturn));

#pragma region CPU

// CPU
#define REGBIT 0x10000u

extern uint16_t R[8];
#define SP R[6]
#define PC R[7]
extern int fN, fZ, fV, fC;

static inline uint16_t get_ps(void)
{
	return 0170000 | fN << 3 | fZ << 2 | fV << 1 | fC;
}

static inline void set_ps_cc(uint16_t ps)
{
	fC = ps & 1;
	fV = (ps >> 1) & 1;
	fZ = (ps >> 2) & 1;
	fN = (ps >> 3) & 1;
}

static inline void nz(uint16_t v, int byte)
{
	if (byte) {
		fN = (v & 0200) != 0;
		fZ = (v & 0377) == 0;
	} else {
		fN = (v & 0100000) != 0;
		fZ = v == 0;
	}
}

#pragma endregion
#pragma region FPU

#define FPS_FER	0100000
#define FPS_FIV	0001000
#define FPS_FD	0000200
#define FPS_FL	0000100
#define FPS_FT	0000040

#define DBL	((fps & FPS_FD) != 0)
#define LNG	((fps & FPS_FL) != 0)
#define TRUNC	((fps & FPS_FT) != 0)

extern double ac[8];
extern uint16_t fps;
extern int fpN, fpZ, fpV, fpC;

#pragma endregion
#pragma region SIGNALS

extern jmp_buf trap_jb;
extern volatile sig_atomic_t sig_pending;
extern int is_child;
extern uint16_t sigtab[64];

void trap_sig(int gsig) __attribute__((noreturn));
void check_signals(void);
void signals_init(void);
void sig_set(int gsig, uint16_t handler);
void exec_reset_signals(void);
int  guest_to_host_sig(int g);
int  host_to_guest_sig(int h);
void die_by_sig(int gsig) __attribute__((noreturn));
void tty_restore(void);

#pragma endregion
#pragma region MEMORY

extern uint8_t  mem[V4_MEM_LOGICAL];
extern uint32_t mem_brk;
extern uint32_t stack_lo;

void mem_reset(uint32_t image_end);
void mem_set_brk(uint32_t end);
void mem_fault(uint16_t a);

static inline void mchk(uint16_t a)
{
	if (UNLIKELY(a >= mem_brk && a < stack_lo))
		mem_fault(a);
}

static inline void mem_check(uint16_t a)
{
	mchk(a);
}

static inline uint16_t rdw(uint16_t a)
{
	if (UNLIKELY(a & 1)) trap_sig(V4_SIGBUS);
	mchk(a);
	return mem[a] | (mem[a + 1] << 8);
}

static inline void wrw(uint16_t a, uint16_t v)
{
	if (UNLIKELY(a & 1)) trap_sig(V4_SIGBUS);
	mchk(a);
	mem[a] = v;
	mem[a + 1] = v >> 8;
}


#pragma endregion
#pragma region DECODE

extern int regdst;
extern uint16_t ea;

static inline void getops(int mode, int reg, int size)
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

static inline void getop(int mode, int reg, int byte)
{
	getops(mode, reg, byte ? 1 : 2);
}

static inline uint16_t load(int byte)
{
	if (regdst >= 0) return byte ? (R[regdst] & 0377) : R[regdst];
	if (byte) {
		mchk(ea);
		return mem[ea];
	}
	return rdw(ea);
}

static inline void store(uint16_t v, int byte)
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

static inline void storeb_sext(uint16_t v)
{
	if (regdst >= 0) {
		R[regdst] = (uint16_t)(int16_t)(int8_t)v;
	} else {
		mchk(ea);
		mem[ea] = v;
	}
}

#pragma endregion
#pragma region EXECUTION

void step(void);
void run_cpu(void);

void fstep(uint16_t ir);
void fp_reset(void);

#pragma endregion
#pragma region KERNEL

extern const char *root;
extern int stop_at_exec;
extern int host_enabled;
extern int load_errno;
extern int fdtab[16];

void syscalls_init(void);
void v4sys(int n);
int  load_image(const char *path, int argc, char **argv);
int  exec_host_program(const char *path, char **argv);
void mappath(uint16_t a, char *out);
uint16_t cur_gpid(void);
long guest_time_shift(void);

#pragma endregion
#pragma region DEBUGGING

int  disasm(uint16_t pc, char *out, size_t n);

void sym_load(FILE *f, const uint8_t *hdr);
void sym_free(void);
int  sym_count(void);
int  sym_lookup(const char *name, uint16_t *v);
const char *sym_exact(uint16_t v);
const char *sym_nearest(uint16_t addr, unsigned *off);
void sym_describe(uint16_t addr, char *out, size_t n);
void sym_target_name(uint16_t target, char *out, size_t n);

int  call_target(uint16_t inst, uint16_t ret);
int  call_nargs(uint16_t inst, uint16_t ret);
void fmt_arg(char *out, size_t n, uint16_t v);
void backtrace_walk(void (*emit)(const char *line));

#pragma endregion
#pragma region HOOKS

extern int hooks_on;
void hooks_load(void);
int  jsr_hook(uint16_t target);

#define TR_CPU  1
#define TR_FPU  2
#define TR_SYS  4
#define TR_FUNC 8
#define TR_ALL  (TR_CPU | TR_FPU | TR_SYS | TR_FUNC)


#pragma endregion
#pragma region TRACING

extern int trace;
extern int crash_trace;
extern int trace_multi;

void trace_open(const char *path);
void tr_line(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void tnote(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void trace_note_reset(void);
void trace_cpu(void);

void trace_sys_line(int n, const uint16_t *a, uint16_t r0in, uint16_t r1in,
		    const char *pre_args, const char *hostnote, int deferred);
void trace_sys_args(int n, const uint16_t *a, uint16_t r0, uint16_t r1,
		    char *out, size_t sz);
void trace_sys_result(int n, int err, uint16_t r0, uint16_t r1, char *out,
		      size_t sz);
const char *trace_signame(int s);

void trace_call(uint16_t inst, uint16_t target);
void trace_ret(uint16_t slot);
void trace_calls_reset(void);
void trace_backtrace(void);

#pragma endregion
#pragma region PROFILING

extern uint16_t prof_buf, prof_len, prof_off, prof_scale;
void prof_sample(void);

#pragma endregion

#endif /* PDP11_H */