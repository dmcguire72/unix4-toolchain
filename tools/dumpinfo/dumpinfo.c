#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "v4dump.h"

#define MAX_REGS 64

struct sect {
	uint32_t id, off, size;
};

struct reg {
	uint32_t start, size, file_off;
	uint16_t kind, flags;
};

struct sym {
	char name[9];
	uint16_t type, value;
};

static struct {
	const char *path;
	uint8_t *buf;
	size_t len;

	uint16_t ver_major, ver_minor;
	uint32_t file_size, crc, crc_calc, flags;
	int crc_ok;
	uint64_t time;
	uint32_t host_pid;
	uint16_t pid, ppid;
	uint16_t signal, fault_flags, fault_addr, inst_pc;
	uint16_t r[V4D_NREGS], ps, aout_magic, argc;
	uint32_t text_start, text_size, data_start, data_size;
	uint32_t bss_start, bss_size, image_end, brk, stack_lo, stack_top;
	uint32_t initial_sp, entry, mem_size, nsyms;

	struct sect sect[V4D_MAX_SECT];
	unsigned nsect;
	struct reg reg[MAX_REGS];
	unsigned nreg;
	struct sym *sym;
	unsigned nsym;

	uint8_t mem[V4D_MEM_SIZE];
} d;

#define SP d.r[6]
#define PC d.r[7]

static void die(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));

static void die(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "dumpinfo: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

static void warn(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

static void warn(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "dumpinfo: warning: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
}

static const struct sect *find_sect(uint32_t id)
{
	unsigned i;

	for (i = 0; i < d.nsect; i++)
		if (d.sect[i].id == id)
			return &d.sect[i];
	return NULL;
}

static const uint8_t *sect_data(uint32_t id, uint32_t *size)
{
	const struct sect *s = find_sect(id);

	if (!s)
		return NULL;
	*size = s->size;
	return d.buf + s->off;
}

static void read_file(const char *path)
{
	FILE *f = fopen(path, "rb");
	size_t cap = 1 << 16, n;

	if (!f)
		die("%s: %s", path, strerror(errno));
	d.buf = malloc(cap);
	if (!d.buf)
		die("out of memory");
	while ((n = fread(d.buf + d.len, 1, cap - d.len, f)) > 0) {
		d.len += n;
		if (d.len == cap) {
			if (cap > (64u << 20))
				die("%s: too large to be a dump", path);
			cap *= 2;
			d.buf = realloc(d.buf, cap);
			if (!d.buf)
				die("out of memory");
		}
	}
	if (ferror(f))
		die("%s: read error", path);
	fclose(f);
}

static void parse_symbols(void)
{
	uint32_t n;
	const uint8_t *p = sect_data(V4D_SEC_SYMS, &n);
	unsigned i, k;

	if (!p)
		return;
	d.nsym = n / V4D_SYM_SIZE;
	d.sym = calloc(d.nsym ? d.nsym : 1, sizeof *d.sym);
	if (!d.sym)
		die("out of memory");
	for (i = 0; i < d.nsym; i++, p += V4D_SYM_SIZE) {
		for (k = 0; k < 8 && p[k]; k++)
			d.sym[i].name[k] = (char)p[k];
		d.sym[i].name[k] = 0;
		d.sym[i].type = v4d_g16(p + 8);
		d.sym[i].value = v4d_g16(p + 10);
	}
}

static void load_dump(const char *path)
{
	const uint8_t *h;
	uint32_t hdr_size, sect_off, sect_ent, nsect;
	unsigned i;
	uint8_t *copy;
	uint32_t n;
	const uint8_t *mm;

	d.path = path;
	read_file(path);
	h = d.buf;

	if (d.len < V4D_MAGIC_LEN || memcmp(h, V4D_MAGIC, V4D_MAGIC_LEN))
		die("%s: not a V4DUMP crash dump (bad magic)", path);
	if (d.len < V4D_HDR_SIZE)
		die("%s: truncated header (%lu bytes)", path,
		    (unsigned long)d.len);

	d.ver_major = v4d_g16(h + V4D_H_VER_MAJOR);
	d.ver_minor = v4d_g16(h + V4D_H_VER_MINOR);
	if (d.ver_major != V4D_VER_MAJOR)
		die("%s: unsupported dump format version %u.%u "
		    "(this tool reads %u.x)", path, d.ver_major, d.ver_minor,
		    V4D_VER_MAJOR);

	hdr_size = v4d_g32(h + V4D_H_HDR_SIZE);
	d.file_size = v4d_g32(h + V4D_H_FILE_SIZE);
	d.crc = v4d_g32(h + V4D_H_CRC);
	d.flags = v4d_g32(h + V4D_H_FLAGS);
	nsect = v4d_g32(h + V4D_H_NSECT);
	sect_off = v4d_g32(h + V4D_H_SECT_OFF);
	sect_ent = v4d_g32(h + V4D_H_SECT_ENT);

	if (hdr_size < V4D_HDR_SIZE || hdr_size > d.len)
		die("%s: bad header size %lu", path, (unsigned long)hdr_size);

	copy = malloc(d.len);
	if (!copy)
		die("out of memory");
	memcpy(copy, d.buf, d.len);
	v4d_p32(copy + V4D_H_CRC, 0);
	d.crc_calc = v4d_crc32(0, copy, d.len);
	free(copy);
	d.crc_ok = d.crc_calc == d.crc && d.file_size == d.len;
	if (d.file_size != d.len)
		warn("header says %lu bytes but the file has %lu "
		     "(truncated or damaged?)", (unsigned long)d.file_size,
		     (unsigned long)d.len);
	else if (d.crc_calc != d.crc)
		warn("crc32 mismatch: file says %08lx, computed %08lx "
		     "(damaged?)", (unsigned long)d.crc,
		     (unsigned long)d.crc_calc);

	d.time = v4d_g64(h + V4D_H_TIME);
	d.host_pid = v4d_g32(h + V4D_H_HOST_PID);
	d.pid = v4d_g16(h + V4D_H_PID);
	d.ppid = v4d_g16(h + V4D_H_PPID);
	d.signal = v4d_g16(h + V4D_H_SIGNAL);
	d.fault_flags = v4d_g16(h + V4D_H_FAULT_FLAGS);
	d.fault_addr = v4d_g16(h + V4D_H_FAULT_ADDR);
	d.inst_pc = v4d_g16(h + V4D_H_INST_PC);
	for (i = 0; i < V4D_NREGS; i++)
		d.r[i] = v4d_g16(h + V4D_H_R + 2 * i);
	d.ps = v4d_g16(h + V4D_H_PS);
	d.aout_magic = v4d_g16(h + V4D_H_AOUT_MAGIC);
	d.text_start = v4d_g32(h + V4D_H_TEXT_START);
	d.text_size = v4d_g32(h + V4D_H_TEXT_SIZE);
	d.data_start = v4d_g32(h + V4D_H_DATA_START);
	d.data_size = v4d_g32(h + V4D_H_DATA_SIZE);
	d.bss_start = v4d_g32(h + V4D_H_BSS_START);
	d.bss_size = v4d_g32(h + V4D_H_BSS_SIZE);
	d.image_end = v4d_g32(h + V4D_H_IMAGE_END);
	d.brk = v4d_g32(h + V4D_H_BRK);
	d.stack_lo = v4d_g32(h + V4D_H_STACK_LO);
	d.stack_top = v4d_g32(h + V4D_H_STACK_TOP);
	d.initial_sp = v4d_g32(h + V4D_H_INITIAL_SP);
	d.entry = v4d_g32(h + V4D_H_ENTRY);
	d.mem_size = v4d_g32(h + V4D_H_MEM_SIZE);
	d.nsyms = v4d_g32(h + V4D_H_NSYMS);
	d.argc = v4d_g16(h + V4D_H_ARGC);

	if (sect_ent < V4D_SECT_ENT || nsect > V4D_MAX_SECT ||
	    sect_off > d.len || (uint64_t)nsect * sect_ent > d.len - sect_off)
		die("%s: bad section table", path);
	for (i = 0; i < nsect; i++) {
		const uint8_t *e = d.buf + sect_off + (size_t)i * sect_ent;
		struct sect *s = &d.sect[d.nsect];

		s->id = v4d_g32(e);
		s->off = v4d_g32(e + 8);
		s->size = v4d_g32(e + 12);
		if (s->off > d.len || s->size > d.len - s->off) {
			warn("section %c%c%c%c lies outside the file, ignored",
			     (int)(s->id & 255), (int)(s->id >> 8 & 255),
			     (int)(s->id >> 16 & 255), (int)(s->id >> 24 & 255));
			continue;
		}
		d.nsect++;
	}

	mm = sect_data(V4D_SEC_MMAP, &n);
	if (mm) {
		const struct sect *md = find_sect(V4D_SEC_MDAT);

		for (i = 0; i < n / V4D_MMAP_ENT && d.nreg < MAX_REGS; i++) {
			const uint8_t *e = mm + (size_t)i * V4D_MMAP_ENT;
			struct reg *r = &d.reg[d.nreg];

			r->start = v4d_g32(e);
			r->size = v4d_g32(e + 4);
			r->file_off = v4d_g32(e + 8);
			r->kind = v4d_g16(e + 12);
			r->flags = v4d_g16(e + 14);
			if (r->start > V4D_MEM_SIZE ||
			    r->size > V4D_MEM_SIZE - r->start) {
				warn("memory map entry %u is outside the "
				     "address space, ignored", i);
				continue;
			}
			if (r->flags & V4D_SEG_STORED) {
				if (!md || r->file_off < md->off ||
				    r->file_off - md->off > md->size ||
				    r->size > md->size - (r->file_off - md->off)) {
					warn("memory map entry %u points "
					     "outside the data section, "
					     "ignored", i);
					continue;
				}
				memcpy(d.mem + r->start, d.buf + r->file_off,
				       r->size);
			}
			d.nreg++;
		}
	}
	parse_symbols();
}

#define SYM_EXT   040
#define SYM_TYPE(t) ((t) & 037)

static int is_local_label(const char *n)
{
	if (n[0] != 'L' || !n[1])
		return 0;
	for (n++; *n; n++)
		if (!isdigit((unsigned char)*n))
			return 0;
	return 1;
}

static const char *sym_display(const struct sym *s)
{
	return s->name[0] == '_' && s->name[1] ? s->name + 1 : s->name;
}

static const struct sym *sym_nearest(uint16_t addr)
{
	const struct sym *best = NULL;
	unsigned i;

	for (i = 0; i < d.nsym; i++) {
		const struct sym *s = &d.sym[i];

		if (SYM_TYPE(s->type) != 2 || s->value > addr ||
		    is_local_label(s->name))
			continue;
		if (!best || s->value > best->value ||
		    (s->value == best->value &&
		     (s->type & SYM_EXT) && !(best->type & SYM_EXT)))
			best = s;
	}
	return best;
}

static void where(uint16_t addr, char *out, size_t n)
{
	const struct sym *s;

	out[0] = 0;
	if (addr >= d.text_start + d.text_size)
		return;
	s = sym_nearest(addr);
	if (!s)
		return;
	if (addr == s->value)
		snprintf(out, n, "<%s>", sym_display(s));
	else
		snprintf(out, n, "<%s+0%o>", sym_display(s), addr - s->value);
}

static void put_safe(const char *s)
{
	for (; *s; s++)
		putchar(isprint((unsigned char)*s) ? *s : '?');
}

static const struct reg *region_at(uint32_t a)
{
	unsigned i;

	for (i = 0; i < d.nreg; i++)
		if (a >= d.reg[i].start && a - d.reg[i].start < d.reg[i].size)
			return &d.reg[i];
	return NULL;
}

static uint16_t mem16(uint16_t a)
{
	return d.mem[a] | d.mem[(uint16_t)(a + 1)] << 8;
}

static void heading(const char *t)
{
	printf("\n%s\n", t);
}

static void fmt_size(char *out, size_t n, uint32_t v)
{
	snprintf(out, n, "%06o (%lu byte%s)", (unsigned)v, (unsigned long)v,
		 v == 1 ? "" : "s");
}

static const char *cstr(const uint8_t **p, const uint8_t *end)
{
	const char *s = (const char *)*p;
	const uint8_t *z = memchr(*p, 0, (size_t)(end - *p));

	if (!z) {
		*p = end;
		return "";
	}
	*p = z + 1;
	return s;
}

static void show_identity(void)
{
	char tb[64] = "?";
	time_t t = (time_t)d.time;
	struct tm *tm = gmtime(&t);
	uint32_t n;
	const uint8_t *p, *end;
	const char *exe = "";
	unsigned i;

	if (tm)
		strftime(tb, sizeof tb, "%Y-%m-%d %H:%M:%S UTC", tm);

	printf("v4linux crash dump: %s\n", d.path);
	printf("  format      V4DUMP %u.%u, %lu bytes, crc32 %08lx (%s)\n",
	       d.ver_major, d.ver_minor, (unsigned long)d.len,
	       (unsigned long)d.crc, d.crc_ok ? "ok" : "MISMATCH");
	printf("  dumped      %s\n", tb);

	p = sect_data(V4D_SEC_PATH, &n);
	if (p) {
		end = p + n;
		exe = cstr(&p, end);
	}
	printf("  program     ");
	put_safe(*exe ? exe : "(unknown)");
	putchar('\n');

	p = sect_data(V4D_SEC_ARGV, &n);
	if (p && d.argc) {
		end = p + n;
		printf("  arguments   ");
		for (i = 0; i < d.argc && p < end; i++) {
			if (i)
				putchar(' ');
			put_safe(cstr(&p, end));
		}
		putchar('\n');
	}
	printf("  process     pid %u, parent %u, host pid %lu%s\n", d.pid,
	       d.ppid, (unsigned long)d.host_pid,
	       d.flags & V4D_F_CHILD ? " (forked child)" : "");
}

static void show_crash(void)
{
	char w[96];
	unsigned a;

	heading("Crash");
	printf("  signal      %u %s (%s)\n", d.signal, v4d_signame(d.signal),
	       v4d_sigdesc(d.signal));

	if (d.fault_flags & V4D_FF_ADDR) {
		const struct reg *r = region_at(d.fault_addr);

		printf("  fault addr  %06o", d.fault_addr);
		if (d.fault_addr & 1)
			printf("  odd address");
		else if (r)
			printf("  in %s", v4d_kindname(r->kind));
		else
			printf("  not in any mapped region");
		putchar('\n');
	}

	where(d.inst_pc, w, sizeof w);
	printf("  executing   %06o %s", d.inst_pc, w);
	if (PC > d.inst_pc && PC - d.inst_pc <= 6 && !(PC & 1)) {
		printf("  [");
		for (a = d.inst_pc; a < PC; a += 2)
			printf("%s%06o", a == d.inst_pc ? "" : " ", mem16(a));
		printf("]");
	}
	putchar('\n');
	where(PC, w, sizeof w);
	printf("  pc          %06o %s\n", PC, w);
}

static void show_regs(void)
{
	static const char *const nm[V4D_NREGS] = {
		"r0", "r1", "r2", "r3", "r4", "r5", "sp", "pc"
	};
	int i;

	heading("Registers");
	for (i = 0; i < V4D_NREGS; i++)
		printf("  %s %06o%s", nm[i], d.r[i], i % 4 == 3 ? "\n" : "  ");
	printf("  ps %06o  [N=%d Z=%d V=%d C=%d]\n", d.ps, d.ps >> 3 & 1,
	       d.ps >> 2 & 1, d.ps >> 1 & 1, d.ps & 1);
}

static void show_fpu(void)
{
	uint32_t n;
	const uint8_t *p = sect_data(V4D_SEC_FPU, &n);
	unsigned fps, cc, nac, i;

	heading("Floating point");
	if (!p || n < 8) {
		puts("  (not recorded)");
		return;
	}
	fps = v4d_g16(p);
	cc = v4d_g16(p + 2);
	nac = v4d_g16(p + 4);
	if (nac > (n - 8) / 8)
		nac = (n - 8) / 8;
	printf("  fps %06o  [%s %s %s]  cc [N=%u Z=%u V=%u C=%u]\n", fps,
	       fps & 0000200 ? "double" : "float",
	       fps & 0000100 ? "long" : "int",
	       fps & 0000040 ? "truncate" : "round", cc >> 3 & 1,
	       cc >> 2 & 1, cc >> 1 & 1, cc & 1);
	for (i = 0; i < nac; i++) {
		uint64_t bits = v4d_g64(p + 8 + 8 * i);
		double v;

		memcpy(&v, &bits, sizeof v);
		printf("  ac%u  %-24.17g  (0x%016llx)\n", i, v,
		       (unsigned long long)bits);
	}
}

static void show_layout(void)
{
	unsigned i;
	char a[48], b[48];

	heading("Memory layout");
	printf("  image       %s, entry %06o\n",
	       d.aout_magic == 0410 ? "0410 (shared text)" :
	       d.aout_magic == 0407 ? "0407 (impure)" : "unknown",
	       (unsigned)d.entry);
	printf("  text        %06o-%06o\n", (unsigned)d.text_start,
	       (unsigned)(d.text_start + d.text_size));
	printf("  data        %06o-%06o\n", (unsigned)d.data_start,
	       (unsigned)(d.data_start + d.data_size));
	printf("  bss         %06o-%06o\n", (unsigned)d.bss_start,
	       (unsigned)(d.bss_start + d.bss_size));
	fmt_size(a, sizeof a, d.brk > d.image_end ? d.brk - d.image_end : 0);
	printf("  break       %06o   heap %s\n", (unsigned)d.brk, a);
	fmt_size(a, sizeof a, d.initial_sp > SP ? d.initial_sp - SP : 0);
	fmt_size(b, sizeof b,
		 d.initial_sp > d.stack_lo ? d.initial_sp - d.stack_lo : 0);
	printf("  stack       %06o-%06o   backed %s\n",
	       (unsigned)d.stack_lo, (unsigned)d.initial_sp, b);
	printf("  sp          %06o   in use %s\n", SP, a);
	printf("  initial sp  %06o   argc/argv above, up to %06o\n",
	       (unsigned)d.initial_sp, (unsigned)d.stack_top);
	if (SP < d.stack_lo)
		printf("  note        sp is below the backed stack: "
		       "stack overflow\n");
	else if (SP > d.initial_sp)
		printf("  note        sp is above its starting value: "
		       "stack underflow\n");
	if (d.nreg) {
		printf("\n  %-9s %-14s %s\n", "region", "range", "size");
		for (i = 0; i < d.nreg; i++) {
			const struct reg *r = &d.reg[i];

			fmt_size(a, sizeof a, r->size);
			printf("  %-9s %06o-%06o   %s\n",
			       v4d_kindname(r->kind), (unsigned)r->start,
			       (unsigned)(r->start + r->size), a);
		}
	}
}

static void show_sections(void)
{
	unsigned i;

	heading("Sections");
	printf("  %-5s %-8s %s\n", "id", "offset", "size");
	for (i = 0; i < d.nsect; i++) {
		const struct sect *s = &d.sect[i];

		printf("  %c%c%c%c  %08lx %lu\n", (int)(s->id & 255),
		       (int)(s->id >> 8 & 255), (int)(s->id >> 16 & 255),
		       (int)(s->id >> 24 & 255), (unsigned long)s->off,
		       (unsigned long)s->size);
	}
}

static void show_process(void)
{
	uint32_t n;
	const uint8_t *p, *end;
	unsigned i;

	heading("Process");
	printf("  guest pid   %u (parent %u)\n", d.pid, d.ppid);
	printf("  host pid    %lu\n", (unsigned long)d.host_pid);

	p = sect_data(V4D_SEC_PATH, &n);
	if (p) {
		const char *exe, *root, *cwd;

		end = p + n;
		exe = cstr(&p, end);
		root = cstr(&p, end);
		cwd = cstr(&p, end);
		printf("  executable  ");
		put_safe(exe);
		printf("\n  root        ");
		put_safe(*root ? root : "(none)");
		printf("\n  cwd         ");
		put_safe(cwd);
		putchar('\n');
	}

	p = sect_data(V4D_SEC_ARGV, &n);
	if (p) {
		end = p + n;
		for (i = 0; i < d.argc && p < end; i++) {
			printf("  argv[%u]%*s", i, i < 10 ? 4 : 3, "");
			put_safe(cstr(&p, end));
			putchar('\n');
		}
	}

	p = sect_data(V4D_SEC_SIGS, &n);
	if (p && n >= 8) {
		uint32_t pend = v4d_g32(p);
		unsigned ns = v4d_g16(p + 4), shown = 0;

		if (ns > (n - 8) / 2)
			ns = (n - 8) / 2;
		for (i = 1; i < ns; i++) {
			unsigned h = v4d_g16(p + 8 + 2 * i);

			if (!h)
				continue;
			if (!shown++)
				puts("  signals");
			if (h == 1)
				printf("    %-8s ignored\n", v4d_signame(i));
			else
				printf("    %-8s handler %06o\n",
				       v4d_signame(i), h);
		}
		if (!shown)
			puts("  signals     all default");
		if (pend) {
			printf("  pending    ");
			for (i = 1; i < 32; i++)
				if (pend >> i & 1)
					printf(" %s", v4d_signame(i));
			putchar('\n');
		}
	}

	p = sect_data(V4D_SEC_FDS, &n);
	if (p && n >= 4) {
		unsigned nf = v4d_g32(p), shown = 0;

		if (nf > (n - 4) / 4)
			nf = (n - 4) / 4;
		printf("  open files ");
		for (i = 0; i < nf; i++) {
			int32_t hfd = (int32_t)v4d_g32(p + 4 + 4 * i);

			if (hfd >= 0) {
				printf(" %u->%d", i, (int)hfd);
				shown++;
			}
		}
		if (!shown)
			printf(" none");
		puts("   (guest fd -> host fd)");
	}
}

static void show_backtrace(void)
{
	uint32_t n;
	const uint8_t *p = sect_data(V4D_SEC_BT, &n);

	heading("Backtrace at crash (most recent call first)");
	if (!p || !n) {
		puts("  (none recorded)");
		return;
	}
	fwrite(p, 1, n, stdout);
	if (p[n - 1] != '\n')
		putchar('\n');
	if (!d.nsym)
		puts("  (no symbols: the program was stripped)");
}

static void show_stack(unsigned nwords)
{
	unsigned a, i;
	char w[96];

	heading("Stack");
	printf("  %-7s %-7s\n", "addr", "word");
	for (i = 0, a = SP; i < nwords && a + 2 <= V4D_MEM_SIZE; i++, a += 2) {
		uint16_t v = mem16((uint16_t)a);

		w[0] = 0;
		if (v)
			where(v, w, sizeof w);
		printf("  %06o  %06o  %-s%s\n", a, v, w,
		       a == SP ? "  <- sp" :
		       a == d.r[5] ? "  <- r5" : "");
	}
}

static int cmp_sym(const void *x, const void *y)
{
	const struct sym *a = x, *b = y;

	if (a->value != b->value)
		return a->value < b->value ? -1 : 1;
	return strcmp(a->name, b->name);
}

static void show_symbols(void)
{
	struct sym *tmp;
	unsigned i;
	static const char letters[] = "UATDB";

	heading("Symbols");
	if (!d.nsym) {
		puts("  (none: the program was stripped)");
		return;
	}
	tmp = malloc(d.nsym * sizeof *tmp);
	if (!tmp)
		die("out of memory");
	memcpy(tmp, d.sym, d.nsym * sizeof *tmp);
	qsort(tmp, d.nsym, sizeof *tmp, cmp_sym);
	for (i = 0; i < d.nsym; i++) {
		unsigned t = SYM_TYPE(tmp[i].type);
		char c = t < sizeof letters - 1 ? letters[t] : '?';

		if (!(tmp[i].type & SYM_EXT))
			c = (char)tolower((unsigned char)c);
		printf("  %06o %c %s\n", tmp[i].value, c, tmp[i].name);
	}
	free(tmp);
}

static void hexdump(uint32_t start, uint32_t len)
{
	uint32_t a, end = start + len, i;

	heading("Memory");
	if (start >= V4D_MEM_SIZE) {
		puts("  (address outside the 64K address space)");
		return;
	}
	if (end > V4D_MEM_SIZE || end < start)
		end = V4D_MEM_SIZE;
	for (a = start & ~15u; a < end; a += 16) {
		const struct reg *r = region_at(a);

		printf("  %06o ", (unsigned)a);
		for (i = 0; i < 16; i += 2) {
			if (a + i < start || a + i + 1 >= end)
				printf(" ------");
			else
				printf(" %06o", mem16((uint16_t)(a + i)));
		}
		printf("  |");
		for (i = 0; i < 16; i++) {
			uint8_t c = d.mem[a + i];

			putchar(a + i < start || a + i >= end ? ' ' :
				isprint(c) ? c : '.');
		}
		printf("|%s%s\n", r ? "  " : "  (unmapped) ",
		       r ? v4d_kindname(r->kind) : "");
	}
}

static void put16(FILE *f, unsigned v)
{
	fputc(v & 255, f);
	fputc(v >> 8 & 255, f);
}

static void extract_aout(FILE *f)
{
	uint32_t n = 0;
	const uint8_t *syms = sect_data(V4D_SEC_SYMS, &n);

	if (d.aout_magic != 0407 && d.aout_magic != 0410)
		die("dump has no recognisable program image (magic %o)",
		    d.aout_magic);
	if (d.text_size > 0xffff || d.data_size > 0xffff ||
	    d.bss_size > 0xffff || (syms ? n : 0) > 0xffff)
		die("program sizes do not fit an a.out header");
	if (d.text_start + d.text_size > V4D_MEM_SIZE ||
	    d.data_start + d.data_size > V4D_MEM_SIZE)
		die("program image lies outside the address space");

	put16(f, d.aout_magic);
	put16(f, d.text_size);
	put16(f, d.data_size);
	put16(f, d.bss_size);
	put16(f, syms ? n : 0);
	put16(f, d.entry);
	put16(f, 0);
	put16(f, 1);
	fwrite(d.mem + d.text_start, 1, d.text_size, f);
	fwrite(d.mem + d.data_start, 1, d.data_size, f);
	if (syms)
		fwrite(syms, 1, n, f);
}

static void extract_raw(FILE *f)
{
	fwrite(d.mem, 1, V4D_MEM_SIZE, f);
}

static void usage(FILE *o)
{
	fprintf(o,
"usage: dumpinfo [options] dumpfile\n"
"\n"
"Inspect a v4linux crash dump (v4linux-PID.v4dump).  With no selection\n"
"options a summary is printed: process, crash, registers, memory layout.\n"
"\n"
"  -a          everything below except symbols and memory\n"
"  -r          registers and condition codes\n"
"  -f          floating point accumulators\n"
"  -l          memory layout, region map and file sections\n"
"  -p          process details: paths, arguments, signals, open files\n"
"  -b          backtrace recorded at crash time\n"
"  -y          program symbol table\n"
"  -s N        N words of stack starting at sp\n"
"  -m ADDR[,LEN]  hex dump of memory (C number syntax: 0 prefix is octal)\n"
"  -c          only verify the file's integrity (exit status 0 = intact)\n"
"  -x FORMAT -o FILE\n"
"              extract the program image; FORMAT is one of\n"
"                aout  a PDP-11 executable (0407/0410) with symbols, for\n"
"                      v4-disasm; text and data are as they were at the\n"
"                      time of the crash\n"
"                raw   the flat 64K memory image\n"
"  -h          this help\n");
}

static uint32_t parse_num(const char *s, const char *what)
{
	char *e;
	unsigned long v;

	errno = 0;
	v = strtoul(s, &e, 0);
	if (errno || e == s || *e || v > 0xffffffffu)
		die("bad %s '%s'", what, s);
	return (uint32_t)v;
}

int main(int argc, char **argv)
{
	int opt, all = 0, regs = 0, fpu = 0, lay = 0, proc = 0, bt = 0;
	int syms = 0, check = 0, selected = 0;
	long stack_words = -1;
	int have_mem = 0;
	uint32_t mem_start = 0, mem_len = 0200;
	const char *xfmt = NULL, *outpath = NULL;

	while ((opt = getopt(argc, argv, "arflpbycs:m:x:o:h")) != -1) {
		switch (opt) {
		case 'a': all = 1; break;
		case 'r': regs = selected = 1; break;
		case 'f': fpu = selected = 1; break;
		case 'l': lay = selected = 1; break;
		case 'p': proc = selected = 1; break;
		case 'b': bt = selected = 1; break;
		case 'y': syms = selected = 1; break;
		case 'c': check = 1; break;
		case 's':
			stack_words = parse_num(optarg, "word count");
			selected = 1;
			break;
		case 'm': {
			char *comma = strchr(optarg, ',');

			if (comma)
				*comma = 0;
			mem_start = parse_num(optarg, "address");
			if (comma)
				mem_len = parse_num(comma + 1, "length");
			have_mem = selected = 1;
			break;
		}
		case 'x': xfmt = optarg; break;
		case 'o': outpath = optarg; break;
		case 'h': usage(stdout); return 0;
		default: usage(stderr); return 2;
		}
	}
	if (optind != argc - 1) {
		usage(stderr);
		return 2;
	}
	if (xfmt && strcmp(xfmt, "aout") && strcmp(xfmt, "raw"))
		die("unknown extraction format '%s' (use aout or raw)", xfmt);
	if (xfmt && !outpath)
		die("-x needs -o FILE");
	if (outpath && !xfmt)
		die("-o needs -x FORMAT");

	load_dump(argv[optind]);

	if (check) {
		printf("%s: %s\n", d.path, d.crc_ok ? "ok" : "DAMAGED");
		return d.crc_ok ? 0 : 1;
	}

	if (xfmt) {
		FILE *f = fopen(outpath, "wb");

		if (!f)
			die("%s: %s", outpath, strerror(errno));
		if (!strcmp(xfmt, "aout"))
			extract_aout(f);
		else
			extract_raw(f);
		if (fclose(f) != 0)
			die("%s: write error", outpath);
		fprintf(stderr, "dumpinfo: wrote %s image to %s\n", xfmt,
			outpath);
		return d.crc_ok ? 0 : 1;
	}

	show_identity();
	if (!selected || all) {
		show_crash();
		show_regs();
		show_layout();
	}
	if (all) {
		show_fpu();
		show_process();
		show_backtrace();
		show_sections();
		show_stack(16);
	} else {
		if (regs)
			show_regs();
		if (fpu)
			show_fpu();
		if (lay) {
			show_layout();
			show_sections();
		}
		if (proc)
			show_process();
		if (bt)
			show_backtrace();
		if (stack_words >= 0)
			show_stack((unsigned)stack_words);
	}
	if (syms)
		show_symbols();
	if (have_mem)
		hexdump(mem_start, mem_len);
	return d.crc_ok ? 0 : 1;
}
