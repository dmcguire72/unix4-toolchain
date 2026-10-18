#ifndef PDP11_H
#define PDP11_H

#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

#define V4_NOFILE	15
#define V4_NSIG		13
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

extern uint8_t mem[65536];
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

#define SP R[6]
#define PC R[7]

extern int regdst;
extern uint16_t ea;

extern jmp_buf trap_jb;
extern volatile sig_atomic_t sig_pending;
extern int is_child;

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
uint16_t rdw(uint16_t a);
void wrw(uint16_t a, uint16_t v);
void getops(int mode, int reg, int size);
void getop(int mode, int reg, int byte);
uint16_t load(int byte);
void store(uint16_t v, int byte);
void storeb_sext(uint16_t v);
void nz(uint16_t v, int byte);
uint16_t get_ps(void);
void set_ps_cc(uint16_t ps);

extern int load_errno;
int load_image(const char *path, int argc, char **argv);
int exec_host_program(const char *path, char **argv);
void v4sys(int n);
void syscalls_init(void);
void fstep(uint16_t ir);
void fp_reset(void);
void step(void);

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

#endif
