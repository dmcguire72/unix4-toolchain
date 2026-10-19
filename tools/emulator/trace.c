#include "pdp11.h"

#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int trace_multi;
static int trace_fd = 2;
static char note_buf[256];

void trace_open(const char *path)
{
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);

	if (fd < 0) {
		perror("v4linux: trace file");
		return;
	}
	trace_fd = fd;
}

void tr_line(const char *fmt, ...)
{
	char buf[4096];
	int n = 0;
	va_list ap;

	if (trace_multi) n = snprintf(buf, sizeof buf, "[pid %u] ", cur_gpid());
	va_start(ap, fmt);
	n += vsnprintf(buf + n, sizeof buf - n - 2, fmt, ap);
	va_end(ap);
	if (n > (int)sizeof buf - 2) n = sizeof buf - 2;
	buf[n++] = '\n';
	if (write(trace_fd, buf, n) < 0) { }
}

void tnote(const char *fmt, ...)
{
	size_t l = strlen(note_buf);
	va_list ap;

	if (!(trace & TR_SYS)) return;
	if (l) { note_buf[l++] = ';'; note_buf[l++] = ' '; }
	va_start(ap, fmt);
	vsnprintf(note_buf + l, sizeof note_buf - l, fmt, ap);
	va_end(ap);
}

static const char *const sysnames[64] = {
	"indir", "exit", "fork", "read", "write", "open", "close", "wait",
	"creat", "link", "unlink", "exec", "chdir", "time", "mknod", "chmod",
	"chown", "break", "stat", "seek", 0, "mount", "umount", "setuid",
	"getuid", "stime", 0, 0, "fstat", 0, "smdate", "stty", "gtty", 0,
	"nice", "sleep", "sync", "kill", "getswit", 0, 0, "dup", "pipe",
	"times", "profil", 0, "setgid", "getgid", "signal"
};

static const char *const errnames[31] = {
	0, "EPERM", "ENOENT", "ESRCH", 0, "EIO", "ENXIO", "E2BIG", "ENOEXEC",
	"EBADF", "ECHILD", "EAGAIN", "ENOMEM", "EACCES", 0, "ENOTBLK", "EBUSY",
	"EEXIST", "EXDEV", "ENODEV", "ENOTDIR", "EISDIR", "EINVAL", "ENFILE",
	"EMFILE", "ENOTTY", "ETXTBSY", "EFBIG", "ENOSPC", "ESPIPE", "EROFS"
};

static const char *const errtext[31] = {
	0, "Not owner", "No such file or directory", "No such process", 0,
	"I/O error", "No such device or address", "Arg list too long",
	"Exec format error", "Bad file number", "No children",
	"No more processes", "Not enough core", "Permission denied", 0,
	"Block device required", "Mount device busy", "File exists",
	"Cross-device link", "No such device", "Not a directory",
	"Is a directory", "Invalid argument", "File table overflow",
	"Too many open files", "Not a typewriter", "Text file busy",
	"File too large", "No space left on device", "Illegal seek",
	"Read-only file system"
};

static const char *const signames[V4_NSIG] = {
	"0", "SIGHUP", "SIGINT", "SIGQIT", "SIGINS", "SIGTRC", "SIGIOT",
	"SIGEMT", "SIGFPT", "SIGKIL", "SIGBUS", "SIGSEG", "SIGSYS"
};

const char *trace_signame(int s)
{
	return (s >= 0 && s < V4_NSIG) ? signames[s] : "SIG?";
}

static inline void cat(char *out, size_t sz, const char *fmt, ...)
{
	size_t l = strlen(out);
	va_list ap;

	if (l >= sz - 1) return;
	va_start(ap, fmt);
	vsnprintf(out + l, sz - l, fmt, ap);
	va_end(ap);
}

static void qbytes(char *out, size_t sz, uint16_t addr, int len, int stop_nul)
{
	int i, shown = len > 32 ? 32 : len, done = 0;

	cat(out, sz, "\"");
	for (i = 0; i < shown; i++) {
		uint8_t c = mem[(uint16_t)(addr + i)];

		if (stop_nul && c == 0) { done = 1; break; }
		switch (c) {
		case '\n': cat(out, sz, "\\n"); break;
		case '\t': cat(out, sz, "\\t"); break;
		case '\r': cat(out, sz, "\\r"); break;
		case '"': cat(out, sz, "\\\""); break;
		case '\\': cat(out, sz, "\\\\"); break;
		default:
			if (c >= 32 && c < 127) cat(out, sz, "%c", c);
			else cat(out, sz, "\\%o", c);
		}
	}
	cat(out, sz, "\"");
	if (stop_nul ? (!done && mem[(uint16_t)(addr + shown)] != 0) : len > shown)
		cat(out, sz, "...");
}

static inline void qstr(char *out, size_t sz, uint16_t addr)
{
	qbytes(out, sz, addr, 4096, 1);
}

static inline void fmt_sig(char *out, size_t sz, unsigned s)
{
	if (s < V4_NSIG) cat(out, sz, "%s", signames[s]);
	else cat(out, sz, "%u", s);
}

static inline void fmt_handler(char *out, size_t sz, uint16_t h)
{
	if (h == 0) cat(out, sz, "SIG_DFL");
	else if (h & 1) cat(out, sz, "SIG_IGN");
	else cat(out, sz, "0%o", h);
}

void trace_sys_args(int n, const uint16_t *a, uint16_t r0, uint16_t r1,
		    char *out, size_t sz)
{
	out[0] = 0;
	(void)r1;
	switch (n) {
	case 1: cat(out, sz, "%d", r0 & 0377); break;
	case 3: cat(out, sz, "%d, buf=0%o, %u", r0, a[0], a[1]); break;
	case 4:
		cat(out, sz, "%d, ", r0);
		qbytes(out, sz, a[0], a[1], 0);
		cat(out, sz, ", %u", a[1]);
		break;
	case 5:
		qstr(out, sz, a[0]);
		cat(out, sz, ", %s", a[1] == 0 ? "O_RDONLY" : a[1] == 1 ? "O_WRONLY" :
		    a[1] == 2 ? "O_RDWR" : "?");
		break;
	case 8: case 15:
		qstr(out, sz, a[0]);
		cat(out, sz, ", 0%o", a[1]);
		break;
	case 9:
		qstr(out, sz, a[0]); cat(out, sz, ", "); qstr(out, sz, a[1]);
		break;
	case 6: cat(out, sz, "%d", r0); break;
	case 10: case 12: case 22:
		qstr(out, sz, a[0]);
		break;
	case 11: {
		uint16_t ap = a[1], s;
		int k = 0;

		qstr(out, sz, a[0]);
		cat(out, sz, ", [");
		while ((s = mem[ap] | mem[(uint16_t)(ap + 1)] << 8) != 0 &&
		       s != 0177777 && k < 8) {
			if (k++) cat(out, sz, ", ");
			qstr(out, sz, s);
			ap += 2;
		}
		if ((mem[ap] | mem[(uint16_t)(ap + 1)] << 8) != 0) cat(out, sz, ", ...");
		cat(out, sz, "]");
		break;
	}
	case 14:
		qstr(out, sz, a[0]);
		cat(out, sz, ", 0%o, 0%o", a[1], a[2]);
		break;
	case 16:
		qstr(out, sz, a[0]); cat(out, sz, ", %u", a[1]);
		break;
	case 17: cat(out, sz, "0%o", a[0]); break;
	case 18:
		qstr(out, sz, a[0]); cat(out, sz, ", buf=0%o", a[1]);
		break;
	case 19:
		cat(out, sz, "%d, %d, %s%u", r0, (int16_t)a[0],
		    a[1] > 2 ? "blocks:" : "", a[1]);
		break;
	case 21:
		qstr(out, sz, a[0]); cat(out, sz, ", "); qstr(out, sz, a[1]);
		cat(out, sz, ", %u", a[2]);
		break;
	case 23: case 46: case 34: case 35: cat(out, sz, "%u", r0); break;
	case 25: cat(out, sz, "%u", (unsigned)(r0 << 16 | r1)); break;
	case 28: case 31: case 32:
		cat(out, sz, "%d, buf=0%o", r0, a[0]);
		break;
	case 30: cat(out, sz, "0%o", a[0]); break;
	case 37:
		cat(out, sz, "%u, ", r0);
		fmt_sig(out, sz, a[0]);
		break;
	case 41: cat(out, sz, "%d", r0); break;
	case 43: cat(out, sz, "buf=0%o", a[0]); break;
	case 44:
		cat(out, sz, "buf=0%o, %u, 0%o, 0%o", a[0], a[1], a[2], a[3]);
		break;
	case 48:
		fmt_sig(out, sz, a[0]);
		cat(out, sz, ", ");
		fmt_handler(out, sz, a[1]);
		break;
	}
}

static void post_detail(int n, const uint16_t *a, uint16_t r0, char *out,
			size_t sz)
{
	switch (n) {
	case 3:
		cat(out, sz, " /* ");
		qbytes(out, sz, a[0], r0, 0);
		cat(out, sz, " */");
		break;
	case 18: case 28: {
		uint16_t b = a[n == 18 ? 1 : 0];
		uint16_t mode = mem[(uint16_t)(b + 4)] | mem[(uint16_t)(b + 5)] << 8;
		unsigned size = mem[(uint16_t)(b + 9)] << 16 |
		    (mem[(uint16_t)(b + 10)] | mem[(uint16_t)(b + 11)] << 8);

		cat(out, sz, " /* {ino=%u, mode=0%o, nlink=%u, uid=%u, gid=%u, size=%u} */",
		    mem[(uint16_t)(b + 2)] | mem[(uint16_t)(b + 3)] << 8, mode,
		    mem[(uint16_t)(b + 6)], mem[(uint16_t)(b + 7)],
		    mem[(uint16_t)(b + 8)], size);
		break;
	}
	case 32:
		cat(out, sz, " /* flags=0%o */",
		    mem[(uint16_t)(a[0] + 4)] | mem[(uint16_t)(a[0] + 5)] << 8);
		break;
	}
}

void trace_sys_result(int n, int err, uint16_t r0, uint16_t r1, char *out,
		      size_t sz)
{
	out[0] = 0;
	if (err) {
		if (r0 == 100) cat(out, sz, "= -1 ENOSYS (Bad system call)");
		else if (r0 < 31 && errnames[r0])
			cat(out, sz, "= -1 %s (%s)", errnames[r0], errtext[r0]);
		else cat(out, sz, "= -1 E%u", r0);
		return;
	}
	switch (n) {
	case 2: cat(out, sz, "= %u", r0); break;
	case 3: case 4: cat(out, sz, "= %u", r0); break;
	case 5: case 8: case 41: cat(out, sz, "= %u", r0); break;
	case 7:
		cat(out, sz, "= %u, ", r0);
		if ((r1 & 0177) == 0) cat(out, sz, "status=exit(%u)", r1 >> 8);
		else {
			cat(out, sz, "status=killed by ");
			fmt_sig(out, sz, r1 & 0177);
			if (r1 & 0200) cat(out, sz, " (core dumped)");
		}
		break;
	case 13: cat(out, sz, "= %u", (unsigned)(r0 << 16 | r1)); break;
	case 24: case 47:
		cat(out, sz, "= {effective=%u, real=%u}", r0 >> 8, r0 & 0377);
		break;
	case 38: cat(out, sz, "= 0%o", r0); break;
	case 42: cat(out, sz, "= [%u, %u]", r0, r1); break;
	case 48: cat(out, sz, "= "); fmt_handler(out, sz, r0); break;
	default: cat(out, sz, "= 0"); break;
	}
}

void trace_sys_line(int n, const uint16_t *a, uint16_t r0in, uint16_t r1in,
		    const char *pre_args, const char *hostnote, int deferred)
{
	char args[1024], res[256], post[512] = "", extra[512] = "";
	const char *nm = (n >= 0 && n < 64) ? sysnames[n] : 0;
	char namebuf[16];
	int err = fC;
	uint16_t r0 = R[0], r1 = R[1];

	if (!nm) { snprintf(namebuf, sizeof namebuf, "sys%d", n); nm = namebuf; }
	if (deferred) trace_sys_args(n, a, r0in, r1in, args, sizeof args);
	else snprintf(args, sizeof args, "%s", pre_args);
	trace_sys_result(n, err, r0, r1, res, sizeof res);
	if (!err) post_detail(n, a, r0, post, sizeof post);
	if (hostnote[0] || note_buf[0])
		snprintf(extra, sizeof extra, "  /* %s%s%s */", hostnote,
		    hostnote[0] && note_buf[0] ? "; " : "", note_buf);
	tr_line("[sys] %s(%s) %s%s%s", nm, args, res, post, extra);
	note_buf[0] = 0;
}

void trace_note_reset(void)
{
	note_buf[0] = 0;
}

static const char *const rn[8] = { "r0", "r1", "r2", "r3", "r4", "r5", "sp", "pc" };

static inline uint16_t rdmem(uint16_t a)
{
	return mem[a] | mem[(uint16_t)(a + 1)] << 8;
}

static inline void oct(char *o, size_t n, uint16_t v)
{
	snprintf(o, n, "%o", v);
}

static inline void operand(char *o, size_t n, int mode, int reg, uint16_t *ext, int fp)
{
	uint16_t x;
	char t[24];

	switch (mode) {
	case 0:
		if (fp) snprintf(o, n, "ac%d", reg & 7);
		else snprintf(o, n, "%s", rn[reg]);
		return;
	case 1: snprintf(o, n, "(%s)", rn[reg]); return;
	case 2:
		if (reg == 7) {
			x = rdmem(*ext); *ext += 2;
			snprintf(o, n, "$%o", x);
		} else snprintf(o, n, "(%s)+", rn[reg]);
		return;
	case 3:
		if (reg == 7) {
			x = rdmem(*ext); *ext += 2;
			snprintf(o, n, "@$%o", x);
		} else snprintf(o, n, "@(%s)+", rn[reg]);
		return;
	case 4: snprintf(o, n, "-(%s)", rn[reg]); return;
	case 5: snprintf(o, n, "@-(%s)", rn[reg]); return;
	case 6: case 7:
		x = rdmem(*ext); *ext += 2;
		if (reg == 7) {
			oct(t, sizeof t, (uint16_t)(*ext + x));
			snprintf(o, n, "%s%s", mode == 7 ? "@" : "", t);
		} else {
			if (x & 0100000) snprintf(t, sizeof t, "-%o", (uint16_t)-x);
			else snprintf(t, sizeof t, "%o", x);
			snprintf(o, n, "%s%s(%s)", mode == 7 ? "@" : "", t, rn[reg]);
		}
		return;
	}
}

static inline void ccop(char *o, size_t n, uint16_t ir)
{
	int set = ir & 020, i;
	static const char L[4] = { 'c', 'v', 'z', 'n' };

	if (ir == 0000240) { snprintf(o, n, "nop"); return; }
	if ((ir & 017) == 017) { snprintf(o, n, set ? "scc" : "ccc"); return; }
	o[0] = 0;
	for (i = 0; i < 4; i++) {
		if (!(ir & (1 << i))) continue;
		snprintf(o + strlen(o), n - strlen(o), "%s%s%c", o[0] ? " " : "",
		    set ? "se" : "cl", L[i]);
	}
}

int disasm(uint16_t pc, char *out, size_t n)
{
	uint16_t ir = rdmem(pc), ext = pc + 2;
	int opc = ir >> 12, byte = ir >> 15;
	int sm = (ir >> 9) & 7, sr = (ir >> 6) & 7, dm = (ir >> 3) & 7, dr = ir & 7;
	char s[40], d[40], t[40];
	const char *nm = 0;
	static const char *const dbl[8] = { 0, "mov", "cmp", "bit", "bic", "bis", "add", 0 };
	static const char *const dblb[8] = { 0, "movb", "cmpb", "bitb", "bicb", "bisb", "sub", 0 };
	static const char *const sgl[16] = { "clr", "com", "inc", "dec", "neg", "adc",
	    "sbc", "tst", "ror", "rol", "asr", "asl", "mark", "mfpi", "mtpi", "sxt" };
	static const char *const sglb[16] = { "clrb", "comb", "incb", "decb", "negb",
	    "adcb", "sbcb", "tstb", "rorb", "rolb", "asrb", "aslb", "mtps", "mfpd",
	    "mtpd", "mfps" };
	static const char *const br[8] = { 0, "br", "bne", "beq", "bge", "blt", "bgt", "ble" };
	static const char *const brb[8] = { "bpl", "bmi", "bhi", "blos", "bvc", "bvs",
	    "bcc", "bcs" };

	if ((ir & 0170000) == 0170000) {
		int ac = (ir >> 6) & 3;
		static const struct { uint16_t m; const char *nm; int f; } ft[] = {
		    {0170100, "ldfps", 6}, {0170200, "stfps", 6}, {0170300, "stst", 6},
		    {0170400, "clrf", 1}, {0170500, "tstf", 1}, {0170600, "absf", 1},
		    {0170700, "negf", 1}, {0 , 0, 0}
		};
		static const struct { uint16_t m; const char *nm; int f; } fh[] = {
		    {0171000, "mulf", 2}, {0171400, "modf", 2}, {0172000, "addf", 2},
		    {0172400, "ldf", 2}, {0173000, "subf", 2}, {0173400, "cmpf", 2},
		    {0174000, "stf", 3}, {0174400, "divf", 2}, {0175000, "stexp", 5},
		    {0175400, "stcfi", 5}, {0176000, "stcfd", 3}, {0176400, "ldexp", 4},
		    {0177000, "ldcif", 4}, {0177400, "ldcdf", 2}, {0, 0, 0}
		};
		int i, f = 0;

		switch (ir) {
		case 0170000: snprintf(out, n, "cfcc"); return pc + 2;
		case 0170001: snprintf(out, n, "setf"); return pc + 2;
		case 0170002: snprintf(out, n, "seti"); return pc + 2;
		case 0170011: snprintf(out, n, "setd"); return pc + 2;
		case 0170012: snprintf(out, n, "setl"); return pc + 2;
		}
		for (i = 0; ft[i].nm; i++)
			if ((ir & 0177700) == ft[i].m) { nm = ft[i].nm; f = ft[i].f; }
		if (!nm)
			for (i = 0; fh[i].nm; i++)
				if ((ir & 0177400) == fh[i].m) { nm = fh[i].nm; f = fh[i].f; }
		if (!nm) { snprintf(out, n, ".word %o", ir); return pc + 2; }
		switch (f) {
		case 1: operand(d, sizeof d, dm, dr, &ext, 1); snprintf(out, n, "%s %s", nm, d); break;
		case 2: operand(s, sizeof s, dm, dr, &ext, 1); snprintf(out, n, "%s %s,ac%d", nm, s, ac); break;
		case 3: operand(d, sizeof d, dm, dr, &ext, 1); snprintf(out, n, "%s ac%d,%s", nm, ac, d); break;
		case 4: operand(s, sizeof s, dm, dr, &ext, 0); snprintf(out, n, "%s %s,ac%d", nm, s, ac); break;
		case 5: operand(d, sizeof d, dm, dr, &ext, 0); snprintf(out, n, "%s ac%d,%s", nm, ac, d); break;
		default: operand(d, sizeof d, dm, dr, &ext, 0); snprintf(out, n, "%s %s", nm, d); break;
		}
		return ext;
	}

	if (opc == 7) {
		static const char *const e[8] = { "mul", "div", "ash", "ashc", "xor", 0, 0, "sob" };

		nm = e[sm];
		if (!nm) { snprintf(out, n, ".word %o", ir); return pc + 2; }
		if (sm == 7) {
			snprintf(out, n, "sob %s,%o", rn[sr], (uint16_t)(pc + 2 - 2 * (ir & 077)));
			return pc + 2;
		}
		operand(d, sizeof d, dm, dr, &ext, 0);
		snprintf(out, n, "%s %s,%s", nm, d, rn[sr]);
		if (sm == 4) snprintf(out, n, "xor %s,%s", rn[sr], d);
		return ext;
	}

	if ((opc & 7) >= 1 && (opc & 7) <= 6) {
		nm = byte ? dblb[opc & 7] : dbl[opc & 7];
		operand(s, sizeof s, sm, sr, &ext, 0);
		operand(d, sizeof d, dm, dr, &ext, 0);
		snprintf(out, n, "%s %s,%s", nm, s, d);
		return ext;
	}

	if (!byte) {
		int8_t off = ir & 0377;
		uint16_t tgt = pc + 2 + 2 * off;

		if (ir == 0) { snprintf(out, n, "halt"); return pc + 2; }
		if (ir < 0000010) {
			static const char *const z[8] = { "halt", "wait", "rti", "bpt", "iot",
			    "reset", "rtt", "mfpt" };
			snprintf(out, n, "%s", z[ir]);
			return pc + 2;
		}
		if ((ir & 0177700) == 0000100) {
			operand(d, sizeof d, dm, dr, &ext, 0);
			snprintf(out, n, "jmp %s", d);
			return ext;
		}
		if ((ir & 0177770) == 0000200) { snprintf(out, n, "rts %s", rn[dr]); return pc + 2; }
		if ((ir & 0177770) == 0000230) { snprintf(out, n, "spl %d", dr); return pc + 2; }
		if ((ir & 0177760) == 0000240 || (ir & 0177760) == 0000260) {
			ccop(t, sizeof t, ir);
			snprintf(out, n, "%s", t);
			return pc + 2;
		}
		if ((ir & 0177700) == 0000300) {
			operand(d, sizeof d, dm, dr, &ext, 0);
			snprintf(out, n, "swab %s", d);
			return ext;
		}
		if (ir >= 0000400 && ir < 0004000 && br[ir >> 8]) {
			snprintf(out, n, "%s %o", br[ir >> 8], tgt);
			return pc + 2;
		}
		if ((ir & 0177000) == 0004000) {
			operand(d, sizeof d, dm, dr, &ext, 0);
			snprintf(out, n, "jsr %s,%s", rn[sr], d);
			return ext;
		}
		if (ir >= 0005000 && ir < 0007000) {
			int k = ((ir >> 6) & 077) - 050;

			if (k >= 0 && k < 16) {
				if (k == 12) { snprintf(out, n, "mark %o", ir & 077); return pc + 2; }
				operand(d, sizeof d, dm, dr, &ext, 0);
				snprintf(out, n, "%s %s", sgl[k], d);
				return ext;
			}
		}
	} else {
		if ((ir & 0177400) == 0104000) { snprintf(out, n, "emt %o", ir & 0377); return pc + 2; }
		if ((ir & 0177400) == 0104400) {
			int sn = ir & 077;

			if (sn < 64 && sysnames[sn]) snprintf(out, n, "sys %s", sysnames[sn]);
			else snprintf(out, n, "trap %o", ir & 0377);
			return pc + 2;
		}
		if ((ir & 0177400) >= 0100000 && (ir & 0177400) < 0104000) {
			int8_t off = ir & 0377;

			snprintf(out, n, "%s %o", brb[(ir >> 8) & 7], (uint16_t)(pc + 2 + 2 * off));
			return pc + 2;
		}
		{
			int k = ((ir >> 6) & 077) - 050;

			if (k >= 0 && k < 16 && 1) {
				operand(d, sizeof d, dm, dr, &ext, 0);
				snprintf(out, n, "%s %s", sglb[k], d);
				return ext;
			}
		}
	}
	snprintf(out, n, ".word %o", ir);
	return pc + 2;
}

static inline void raw_words(char *o, size_t n, uint16_t pc, uint16_t end)
{
	int k;

	o[0] = 0;
	for (k = 0; k < 3; k++) {
		if (pc + 2 * k < end || k == 0)
			snprintf(o + strlen(o), n - strlen(o), "%06o ", rdmem(pc + 2 * k));
		else snprintf(o + strlen(o), n - strlen(o), "       ");
	}
}

void trace_cpu(void)
{
	char text[96], raw[32];
	uint16_t end = disasm(PC, text, sizeof text);

	raw_words(raw, sizeof raw, PC, end);
	tr_line("[cpu] %06o: %-22s %-28s r0=%06o r1=%06o r2=%06o r3=%06o "
		"r4=%06o r5=%06o sp=%06o %c%c%c%c",
		PC, raw, text, R[0], R[1], R[2], R[3], R[4], R[5], SP,
		fN ? 'N' : '-', fZ ? 'Z' : '-', fV ? 'V' : '-', fC ? 'C' : '-');
}

