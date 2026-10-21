#ifndef PDP11_H
#define PDP11_H

#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

#define V4_SIGHUP	1
#define V4_SIGINT	2
#define V4_SIGQIT	3
#define V4_SIGINS	4
#define V4_SIGTRC	5
#define V4_SIGIOT	6
#define V4_SIGEMT	7
#define V4_SIGFPT	8
#define V4_SIGKIL	9
#define V4_SIGBUS	10
#define V4_SIGSEG	11
#define V4_SIGSYS	12

// @TODO update these comments to match public docs
#define V4_EPERM    1   /* Not owner */
#define V4_ENOENT   2   /* No such file or directory */
#define V4_ESRCH    3   /* No such process */
#define V4_EIO      5   /* I/O error */
#define V4_ENXIO    6   /* No such device or address */
#define V4_E2BIG    7   /* Arg list too long */
#define V4_ENOEXEC  8   /* Exec format error */
#define V4_EBADF    9   /* Bad file number */
#define V4_ECHILD   10  /* No children */
#define V4_EAGAIN   11  /* No more processes */
#define V4_ENOMEM   12  /* Not enough core */
#define V4_EACCES   13  /* Permission denied */
#define V4_EFAULT   14  /* Bad address */
#define V4_ENOTBLK  15  /* Block device required */
#define V4_EBUSY    16  /* Mount device busy */
#define V4_EEXIST   17  /* File exists */
#define V4_EXDEV    18  /* Cross-device link */
#define V4_ENODEV   19  /* No such device */
#define V4_ENOTDIR  20  /* Not a directory */
#define V4_EISDIR   21  /* Is a directory */
#define V4_EINVAL   22  /* Invalid argument */
#define V4_ENFILE   23  /* File table overflow */
#define V4_EMFILE   24  /* Too many open files */
#define V4_ENOTTY   25  /* Not a typewriter */
#define V4_ETXTBSY  26  /* Text file busy */
#define V4_EFBIG    27  /* File too large */
#define V4_ENOSPC   28  /* No space left on device */
#define V4_ESPIPE   29  /* Illegal seek */
#define V4_EROFS    30  /* Read-only file system */
#define V4_ENOSYS   100 /* Bad system call */

#define V4_AOUT_OMAGIC  0407
#define V4_AOUT_NMAGIC  0410
#define V4_MAX_ARGS_LEN 510
#define V4_MEM_MAX      0160000
#define V4_MEM_LOGICAL  0200000
#define V4_NOFILE	    15
#define V4_NSIG		    13

extern uint8_t mem[V4_MEM_LOGICAL];
extern uint16_t R[8];
extern int fN, fZ, fV, fC;
extern const char *root;
extern int stop_at_exec;
extern int fdtab[16];
extern uint16_t sigtab[64];
extern int trace;
#define TR_CPU 1
#define TR_FPU 2
#define TR_SYS 4
#define TR_FUNC 8
#define TR_ALL (TR_CPU | TR_FPU | TR_SYS | TR_FUNC)
extern int crash_trace;

#define SP R[6]
#define PC R[7]

extern int regdst;
extern uint16_t ea;

extern uint32_t mem_brk;
extern uint32_t stack_lo;
void mem_reset(uint32_t image_end);
void mem_set_brk(uint32_t end);
void mem_fault(uint16_t a);

extern jmp_buf trap_jb;
extern volatile sig_atomic_t sig_pending;
extern int is_child;

extern int host_enabled;

void trap_sig(int gsig) __attribute__((noreturn));
void check_signals(void);
void signals_init(void);
void sig_set(int gsig, uint16_t handler);
void exec_reset_signals(void);
int guest_to_host_sig(int g);
int host_to_guest_sig(int h);
void die_by_sig(int gsig) __attribute__((noreturn));
void tty_restore(void);

extern uint16_t prof_buf, prof_len, prof_off, prof_scale;
void prof_sample(void);

void die(const char *m, ...) __attribute__((noreturn));

#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

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
	return mem[a] | mem[a + 1] << 8;
}

static inline void wrw(uint16_t a, uint16_t v)
{
	if (UNLIKELY(a & 1)) trap_sig(V4_SIGBUS);
	mchk(a);
	mem[a] = v;
	mem[a + 1] = v >> 8;
}

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
	if (regdst >= 0) return byte ? R[regdst] & 0377 : R[regdst];
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
	if (regdst >= 0) R[regdst] = (uint16_t)(int16_t)(int8_t)v;
	else {
		mchk(ea);
		mem[ea] = v;
	}
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

extern int load_errno;
int load_image(const char *path, int argc, char **argv);
int exec_host_program(const char *path, char **argv);
void v4sys(int n);
void syscalls_init(void);
void fstep(uint16_t ir);
void fp_reset(void);
void step(void);
void run_cpu(void);

void trace_open(const char *path);
void tr_line(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
extern int trace_multi;
uint16_t cur_gpid(void);
void mappath(uint16_t a, char *out);
int disasm(uint16_t pc, char *out, size_t n);
void trace_cpu(void);
void trace_sys_args(int n, const uint16_t *a, uint16_t r0, uint16_t r1,
		    char *out, size_t sz);
void trace_sys_result(int n, int err, uint16_t r0, uint16_t r1, char *out,
		      size_t sz);
const char *trace_signame(int s);
void tnote(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void trace_sys_line(int n, const uint16_t *a, uint16_t r0in, uint16_t r1in,
		    const char *pre_args, const char *hostnote, int deferred);
void trace_note_reset(void);

void sym_load(FILE *f, const uint8_t *hdr);
int sym_lookup(const char *name, uint16_t *v);
extern int hooks_on;
void hooks_load(void);
int jsr_hook(uint16_t target);
long guest_time_shift(void);
void step(void);
void sym_free(void);
int sym_count(void);
const char *sym_exact(uint16_t v);
const char *sym_nearest(uint16_t addr, unsigned *off);
void sym_describe(uint16_t addr, char *out, size_t n);
void sym_target_name(uint16_t target, char *out, size_t n);
int call_target(uint16_t inst, uint16_t ret);
int call_nargs(uint16_t inst, uint16_t ret);
void fmt_arg(char *out, size_t n, uint16_t v);
void backtrace_walk(void (*emit)(const char *line));

void trace_call(uint16_t inst, uint16_t target);
void trace_ret(uint16_t slot);
void trace_calls_reset(void);
void trace_backtrace(void);

#endif
