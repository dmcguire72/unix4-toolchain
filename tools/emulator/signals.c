#include "pdp11.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

volatile sig_atomic_t sig_pending;
jmp_buf trap_jb;
int is_child;

static const int g2h[V4_NSIG] = {
	0, SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT,
	SIGUSR1, SIGFPE, SIGKILL, SIGBUS,
	SIGSEGV, SIGSYS
};

int guest_to_host_sig(int g)
{
	return (g > 0 && g < V4_NSIG) ? g2h[g] : 0;
}

int host_to_guest_sig(int h)
{
	int g;

	for (g = 1; g < V4_NSIG; g++)
		if (g2h[g] == h) return g;
	return 0;
}

static inline void host_handler(int hs)
{
	int g = host_to_guest_sig(hs);

	if (g) sig_pending |= 1 << g;
}

void sig_set(int g, uint16_t h)
{
	struct sigaction sa;
	int hs = guest_to_host_sig(g);

	if (!hs || g == V4_SIGKIL) return;
	if (g == V4_SIGINS || g == V4_SIGTRC || g == V4_SIGFPT ||
	    g == V4_SIGBUS || g == V4_SIGSEG)
		return;
	memset(&sa, 0, sizeof sa);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (h & 1) sa.sa_handler = SIG_IGN;
	else if (h) sa.sa_handler = host_handler;
	else sa.sa_handler = SIG_DFL;
	sigaction(hs, &sa, NULL);
}

void signals_init(void)
{
	struct sigaction old;
	struct rlimit rl = { 0, 0 };
	int g;

	for (g = 1; g <= V4_SIGQIT; g++) {
		if (sigaction(guest_to_host_sig(g), NULL, &old) == 0 &&
		    old.sa_handler == SIG_IGN)
			sigtab[g] = 1;
	}

	signal(SIGPIPE, SIG_IGN);
	setrlimit(RLIMIT_CORE, &rl);
}

void exec_reset_signals(void)
{
	int g;

	for (g = 0; g < 64; g++) {
		if (sigtab[g] & 1) continue;
		if (sigtab[g]) {
			sigtab[g] = 0;
			sig_set(g, 0);
		}
	}
}

void die_by_sig(int g)
{
	int hs = guest_to_host_sig(g);
	struct sigaction sa;
	sigset_t m;

	    if (!is_child && g >= V4_SIGINS && g != V4_SIGKIL) {
        static const char *const nm[V4_NSIG] = {
            0, 0, 0, 0, "illegal instruction", "trace/BPT trap",
            "IOT trap", "EMT trap", "floating exception", 0,
            "bus error / odd address", "segmentation violation",
            "bad system call"
        };

        fprintf(stderr, "v4linux: %s (pc=%06o)\n", nm[g], PC);

        fprintf(stderr, "Registers:\n");
        const char *reg_names[] = {"r0", "r1", "r2", "r3", "r4", "r5", "sp", "pc"};
        for (int i = 0; i < 8; i++)
            fprintf(stderr, "  %s: %06o\n", reg_names[i], R[i]);
        fprintf(stderr, "Flags: N=%d Z=%d V=%d C=%d\n", fN, fZ, fV, fC);
        
        char inst[96];
        disasm(PC, inst, sizeof inst);
        fprintf(stderr, "Instruction at PC: [%s]\n", inst);
    }
	if (!crash_trace){
		fprintf(stderr, "Use V4LINUX_TRACE_STACK to enable stacktracing:\n$ V4LINUX_TRACE_STACK=1 !!\n");
	} else if (g >= V4_SIGINS && g != V4_SIGKIL) {
        if (is_child)
            fprintf(stderr, "v4linux: pid %u: signal %d (pc=%06o)\n",
                    cur_gpid(), g, PC);
        trace_backtrace();
    }
	if (trace & TR_SYS) tr_line("+++ killed by %s +++", trace_signame(g));
	dump_write(g);
    tty_restore();
	if (!hs) _exit(128 + g);
	memset(&sa, 0, sizeof sa);
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sigaction(hs, &sa, NULL);
	sigemptyset(&m);
	sigaddset(&m, hs);
	sigprocmask(SIG_UNBLOCK, &m, NULL);
	raise(hs);
	_exit(128 + g);
}

static inline void deliver(int g)
{
	uint16_t h = sigtab[g];

	if (trace & TR_SYS) {
		if (h & 1) tr_line("--- %s ignored {pc=%06o} ---", trace_signame(g), PC);
		else if (h) tr_line("--- %s {pc=%06o} -> handler 0%o ---",
				    trace_signame(g), PC, h);
		else tr_line("--- %s {pc=%06o} ---", trace_signame(g), PC);
	}
	if (h & 1) {
		fault_valid = 0;
		return;
	}
	if (h) {
		uint16_t sp = SP;

		fault_valid = 0;
		sigtab[g] = 0;
		sp -= 2; mem[sp] = get_ps(); mem[(uint16_t)(sp + 1)] = get_ps() >> 8;
		sp -= 2; mem[sp] = PC; mem[(uint16_t)(sp + 1)] = PC >> 8;
		SP = sp;
		PC = h;
		return;
	}
	die_by_sig(g);
}

void trap_sig(int g)
{
	deliver(g);
	longjmp(trap_jb, 1);
}

void check_signals(void)
{
	int g;

	for (g = 1; g < V4_NSIG; g++) {
		if (!(sig_pending & (1 << g))) continue;
		sig_pending &= ~(1 << g);
		if (sigtab[g] & 1) continue;
		deliver(g);
		return;
	}
}

void prof_sample(void)
{
	uint16_t r0 = (uint16_t)(PC - prof_off) >> 1;
	uint32_t prod = (uint32_t)r0 * prof_scale;
	uint16_t r1 = (uint16_t)(prod >> 14);

	r1 = (r1 + 1) & ~1;
	if (r1 >= prof_len) return;
	r1 += prof_buf;
	{
		uint16_t v = mem[r1] | mem[(uint16_t)(r1 + 1)] << 8;

		v++;
		mem[r1] = v;
		mem[(uint16_t)(r1 + 1)] = v >> 8;
	}
}
