#include "pdp11.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct sym {
	char name[9];
	uint16_t flag;
	uint16_t value;
};

static struct sym *symtab;
static unsigned nsyms;
static uint8_t *symraw;

#define SYM_EXT		040
#define SYM_TYPE(f)	((f) & 037)
#define SYM_TEXT	2

#define MAX_FRAMES	64
#define MAX_ARGS	8

static uint16_t peek(uint16_t a)
{
	return mem[a] | mem[(uint16_t)(a + 1)] << 8;
}

void sym_free(void)
{
	free(symtab);
	free(symraw);
	symtab = NULL;
	symraw = NULL;
	nsyms = 0;
}

void sym_load(FILE *f, const uint8_t *hdr)
{
	uint16_t ts = hdr[2] | hdr[3] << 8;
	uint16_t ds = hdr[4] | hdr[5] << 8;
	uint16_t symlen = hdr[8] | hdr[9] << 8;
	uint16_t relflg = hdr[14] | hdr[15] << 8;
	long off = 020L + ts + ds;
	unsigned n, i, k;
	uint8_t e[12];

	sym_free();
	if (symlen < 12)
		return;
	if (relflg != 1)
		off += (long)ts + ds;
	if (fseek(f, off, SEEK_SET) != 0)
		return;
	n = symlen / 12;
	symtab = calloc(n, sizeof *symtab);
	symraw = malloc((size_t)n * sizeof e);
	if (!symtab || !symraw) {
		sym_free();
		return;
	}
	for (i = 0; i < n; i++) {
		struct sym *s = &symtab[nsyms];
		const uint8_t *p = e;

		if (fread(e, 1, 12, f) != 12)
			break;
		memcpy(symraw + (size_t)nsyms * sizeof e, e, sizeof e);
		if (p[0] == '_')
			p++;
		for (k = 0; k < 8 && p < e + 8 && *p; k++, p++)
			s->name[k] = *p;
		s->name[k] = 0;
		s->flag = e[8] | e[9] << 8;
		s->value = e[10] | e[11] << 8;
		nsyms++;
	}
}

int sym_count(void)
{
	return nsyms;
}

const uint8_t *sym_raw(unsigned *len)
{
	*len = nsyms * 12;
	return symraw;
}

static int is_local_label(const char *n)
{
	if (n[0] != 'L' || !n[1])
		return 0;
	for (n++; *n; n++)
		if (!isdigit((unsigned char)*n))
			return 0;
	return 1;
}

static int better(const struct sym *a, const struct sym *b)
{
	if (!b)
		return 1;
	return (a->flag & SYM_EXT) && !(b->flag & SYM_EXT);
}

const char *sym_exact(uint16_t v)
{
	const struct sym *best = NULL;
	unsigned i;

	for (i = 0; i < nsyms; i++) {
		const struct sym *s = &symtab[i];

		if (s->value != v || SYM_TYPE(s->flag) != SYM_TEXT)
			continue;
		if (is_local_label(s->name))
			continue;
		if (better(s, best))
			best = s;
	}
	return best ? best->name : NULL;
}

const char *sym_nearest(uint16_t addr, unsigned *off)
{
	const struct sym *best = NULL;
	unsigned i;

	for (i = 0; i < nsyms; i++) {
		const struct sym *s = &symtab[i];

		if (SYM_TYPE(s->flag) != SYM_TEXT || s->value > addr)
			continue;
		if (is_local_label(s->name))
			continue;
		if (!best || s->value > best->value ||
		    (s->value == best->value && better(s, best)))
			best = s;
	}
	if (!best)
		return NULL;
	if (off)
		*off = addr - best->value;
	return best->name;
}

void sym_describe(uint16_t addr, char *out, size_t n)
{
	unsigned off;
	const char *nm = sym_nearest(addr, &off);

	if (!nm)
		out[0] = 0;
	else if (off)
		snprintf(out, n, "%s+0%o", nm, off);
	else
		snprintf(out, n, "%s", nm);
}

static int sym_value(const char *name, uint16_t *v)
{
	unsigned i;

	for (i = 0; i < nsyms; i++)
		if (!strcmp(symtab[i].name, name)) {
			*v = symtab[i].value;
			return 1;
		}
	return 0;
}

int sym_lookup(const char *name, uint16_t *v)
{
	return sym_value(name, v);
}

int call_target(uint16_t inst, uint16_t ret)
{
	if (inst != 04737 && (inst & ~077) != 04700)
		return -1;
	return (uint16_t)((inst == 04767 ? ret : 0) + peek(ret - 2));
}

int call_nargs(uint16_t inst, uint16_t ret)
{
	int narg = inst == 04737 ? 1 : 0;
	uint16_t next = peek(ret);

	if (next == 05726)			/* tst (sp)+ */
		return narg + 1;
	if (next == 022626)			/* cmp (sp)+,(sp)+ */
		return narg + 2;
	if (next == 062706)			/* add $n,sp */
		return narg + peek(ret + 2) / 2;
	return narg;
}

void sym_target_name(uint16_t target, char *out, size_t n)
{
	const char *nm = sym_exact(target);

	if (nm)
		snprintf(out, n, "%s", nm);
	else {
		unsigned off;

		nm = sym_nearest(target, &off);
		if (nm && off < 0400)
			snprintf(out, n, "%s+0%o", nm, off);
		else
			snprintf(out, n, "0%o", target);
	}
}

void fmt_arg(char *out, size_t n, uint16_t v)
{
	size_t used;
	int i, ok = 0;

	snprintf(out, n, v ? "0%o" : "0", v);
	if (v < 0400 || v >= mem_brk)
		return;
	for (i = 0; i < 64; i++) {
		uint8_t c = mem[(uint16_t)(v + i)];

		if (c == 0) {
			ok = i >= 3;
			break;
		}
		if (!(c >= 32 && c < 127) && c != '\n' && c != '\t')
			break;
	}
	if (!ok)
		return;
	used = strlen(out);
	snprintf(out + used, n - used, "=\"");
	for (i = 0; mem[(uint16_t)(v + i)] && i < 24; i++) {
		uint8_t c = mem[(uint16_t)(v + i)];

		used = strlen(out);
		if (used + 6 >= n)
			break;
		if (c == '\n')
			snprintf(out + used, n - used, "\\n");
		else if (c == '\t')
			snprintf(out + used, n - used, "\\t");
		else if (c == '"' || c == '\\')
			snprintf(out + used, n - used, "\\%c", c);
		else
			snprintf(out + used, n - used, "%c", c);
	}
	used = strlen(out);
	snprintf(out + used, n - used, "%s\"",
	    mem[(uint16_t)(v + i)] ? "..." : "");
}

static void fmt_args(char *out, size_t n, uint16_t argp, int narg)
{
	int i;
	char a[96];

	out[0] = 0;
	for (i = 0; i < narg && i < MAX_ARGS; i++) {
		fmt_arg(a, sizeof a, peek(argp + 2 * i));
		snprintf(out + strlen(out), n - strlen(out), "%s%s",
		    i ? ", " : "", a);
	}
	if (narg > MAX_ARGS)
		snprintf(out + strlen(out), n - strlen(out), ", ...");
}

static int frame_ok(uint16_t r5)
{
	uint16_t ret;

	if ((r5 & 1) || r5 < SP)
		return 0;
	ret = peek(r5 + 2);
	return call_target(peek(ret - 4), ret) >= 0;
}

void backtrace_walk(void (*emit)(const char *line))
{
	char line[512], tgt[64], args[400], where[64], at[80];
	uint16_t pc = PC, r5 = R[5], sv;
	int frame = 0, first = 1;
	unsigned off;

	if (!frame_ok(r5) && sym_value("savr5", &sv) && peek(sv))
		r5 = peek(sv);

	sym_describe(pc, where, sizeof where);
	snprintf(at, sizeof at, "pc=%06o%s%s%s", pc,
	    where[0] ? " <" : "", where, where[0] ? ">" : "");

	
	int t = frame_ok(r5) ?
	    call_target(peek(peek(r5 + 2) - 4), peek(r5 + 2)) : -1;
	const char *cur = sym_nearest(pc, &off);
	const char *fn = t < 0 ? NULL : sym_exact((uint16_t)t);

	if (t < 0 || (cur && fn && strcmp(cur, fn))) {
		snprintf(line, sizeof line, "  #%-2d %s [no C frame]",
		    frame++, at);
		emit(line);
	}

	while (frame < MAX_FRAMES) {
		uint16_t ret, inst, savedr5;
		int t, narg;

		if ((r5 & 1) || r5 < SP)
			break;
		ret = peek(r5 + 2);
		inst = peek(ret - 4);
		t = call_target(inst, ret);
		if (t < 0) {
			if (first)
				break;
			snprintf(line, sizeof line,
			    "  #%-2d ??? (return pc=%06o is not after a call)",
			    frame++, ret);
			emit(line);
			break;
		}
		narg = call_nargs(inst, ret);
		sym_target_name((uint16_t)t, tgt, sizeof tgt);
		fmt_args(args, sizeof args, r5 + 4, narg);
		snprintf(line, sizeof line, "  #%-2d %s(%s)  at %s", frame++,
		    tgt, args, at);
		emit(line);
		first = 0;

		sym_describe(ret, where, sizeof where);
		snprintf(at, sizeof at, "pc=%06o%s%s%s", ret,
		    where[0] ? " <" : "", where, where[0] ? ">" : "");
		savedr5 = peek(r5);
		if (savedr5 == 0 || savedr5 <= r5)
			break;
		r5 = savedr5;
	}
	if (frame == 0) {
		snprintf(line, sizeof line, "  (no stack frames found)");
		emit(line);
	}
}
