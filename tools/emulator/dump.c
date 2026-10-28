#include "pdp11.h"
#include "v4dump.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct buf {
	uint8_t *p;
	size_t len, cap;
	int err;
};

struct section {
	uint32_t id;
	struct buf b;
	uint32_t off;
};

struct region {
	uint32_t start, size;
	uint16_t kind, flags;
};

#define MAX_REGIONS 16

static void buf_put(struct buf *b, const void *d, size_t n)
{
	if (b->err || !n)
		return;
	if (b->len + n > b->cap) {
		size_t c = b->cap ? b->cap : 256;
		uint8_t *np;

		while (c < b->len + n)
			c *= 2;
		np = realloc(b->p, c);
		if (!np) {
			b->err = 1;
			return;
		}
		b->p = np;
		b->cap = c;
	}
	memcpy(b->p + b->len, d, n);
	b->len += n;
}

static void buf_u16(struct buf *b, uint16_t v)
{
	uint8_t t[2];

	v4d_p16(t, v);
	buf_put(b, t, sizeof t);
}

static void buf_u32(struct buf *b, uint32_t v)
{
	uint8_t t[4];

	v4d_p32(t, v);
	buf_put(b, t, sizeof t);
}

static void buf_u64(struct buf *b, uint64_t v)
{
	uint8_t t[8];

	v4d_p64(t, v);
	buf_put(b, t, sizeof t);
}

static void buf_str(struct buf *b, const char *s)
{
	buf_put(b, s ? s : "", s ? strlen(s) + 1 : 1);
}

static struct buf *bt_buf;

static void bt_emit(const char *line)
{
	buf_put(bt_buf, line, strlen(line));
	buf_put(bt_buf, "\n", 1);
}

static void add_region(struct region *r, int *n, uint32_t *cur,
		       uint16_t kind, uint32_t lo, uint32_t hi)
{
	if (lo < *cur)
		lo = *cur;
	if (hi > V4D_MEM_SIZE)
		hi = V4D_MEM_SIZE;
	if (hi <= lo || *n >= MAX_REGIONS)
		return;
	r[*n].start = lo;
	r[*n].size = hi - lo;
	r[*n].kind = kind;
	r[*n].flags = kind == V4D_K_HOLE ? 0 : V4D_SEG_STORED;
	(*n)++;
	*cur = hi;
}

static int build_regions(struct region *r)
{
	uint32_t cur = 0;
	int n = 0;
	uint32_t hole = mem_brk > img.image_end ? mem_brk : img.image_end;

	add_region(r, &n, &cur, V4D_K_TEXT, 0, img.ts);
	if (img.magic == V4_AOUT_NMAGIC)
		add_region(r, &n, &cur, V4D_K_PAD, img.ts, img.dbase);
	add_region(r, &n, &cur, V4D_K_DATA, img.dbase, img.dbase + img.ds);
	add_region(r, &n, &cur, V4D_K_BSS, img.dbase + img.ds, img.image_end);
	add_region(r, &n, &cur, V4D_K_HEAP, img.image_end, mem_brk);
	add_region(r, &n, &cur, V4D_K_HOLE, hole, stack_lo);
	add_region(r, &n, &cur, V4D_K_STACK, stack_lo, img.initial_sp);
	add_region(r, &n, &cur, V4D_K_ARGS, img.initial_sp, V4D_MEM_SIZE);
	return n;
}

int dump_write(int gsig)
{
	struct section s[V4D_MAX_SECT];
	struct region reg[MAX_REGIONS];
	struct section *mmap, *mdat, *fpu, *sigs, *fds, *syms, *argv, *path, *bt;
	struct section *order[V4D_MAX_SECT];
	uint8_t *file = NULL, *h;
	const uint8_t *symraw;
	unsigned nsect = 0, norder = 0, nsym_bytes = 0;
	size_t off, total = 0;
	int nreg, i, rc = -1, bad = 0;
	char name[64], cwd[PATH_MAX];
	FILE *f;

	memset(s, 0, sizeof s);

	mmap = &s[nsect++];  mmap->id = V4D_SEC_MMAP;
	fpu  = &s[nsect++];  fpu->id  = V4D_SEC_FPU;
	sigs = &s[nsect++];  sigs->id = V4D_SEC_SIGS;
	fds  = &s[nsect++];  fds->id  = V4D_SEC_FDS;
	argv = &s[nsect++];  argv->id = V4D_SEC_ARGV;
	path = &s[nsect++];  path->id = V4D_SEC_PATH;
	bt   = &s[nsect++];  bt->id   = V4D_SEC_BT;
	syms = &s[nsect++];  syms->id = V4D_SEC_SYMS;
	mdat = &s[nsect++];  mdat->id = V4D_SEC_MDAT;

	nreg = build_regions(reg);
	for (i = 0; i < nreg; i++)
		if (reg[i].flags & V4D_SEG_STORED)
			buf_put(&mdat->b, mem + reg[i].start, reg[i].size);

	buf_u16(&fpu->b, fps);
	buf_u16(&fpu->b, fpN << 3 | fpZ << 2 | fpV << 1 | fpC);
	buf_u16(&fpu->b, V4D_NAC);
	buf_u16(&fpu->b, 0);
	for (i = 0; i < V4D_NAC; i++) {
		uint64_t bits;
		double d = i < FP_NUM_AC ? ac[i] : 0;

		memcpy(&bits, &d, sizeof bits);
		buf_u64(&fpu->b, bits);
	}

	buf_u32(&sigs->b, (uint32_t)sig_pending);
	buf_u16(&sigs->b, V4_NSIG);
	buf_u16(&sigs->b, 0);
	for (i = 0; i < V4_NSIG; i++)
		buf_u16(&sigs->b, sigtab[i]);

	buf_u32(&fds->b, V4D_NFD);
	for (i = 0; i < V4D_NFD; i++)
		buf_u32(&fds->b, (uint32_t)(int32_t)fdtab[i]);

	buf_put(&argv->b, img.argv, img.argv_len);

	if (!getcwd(cwd, sizeof cwd))
		cwd[0] = 0;
	buf_str(&path->b, img.path);
	buf_str(&path->b, root);
	buf_str(&path->b, cwd);

	bt_buf = &bt->b;
	backtrace_walk(bt_emit);
	bt_buf = NULL;

	symraw = sym_raw(&nsym_bytes);
	if (symraw)
		buf_put(&syms->b, symraw, nsym_bytes);

	for (i = 0; i < (int)nsect; i++) {
		bad |= s[i].b.err;
		if (&s[i] == mmap || s[i].b.len)
			order[norder++] = &s[i];
	}
	if (bad)
		goto out;

	mmap->b.len = (size_t)nreg * V4D_MMAP_ENT;
	off = V4D_HDR_SIZE + (size_t)norder * V4D_SECT_ENT;
	for (i = 0; i < (int)norder; i++) {
		off = (off + V4D_ALIGN - 1) & ~(size_t)(V4D_ALIGN - 1);
		order[i]->off = (uint32_t)off;
		off += order[i]->b.len;
	}
	total = off;

	memset(&mmap->b, 0, sizeof mmap->b);
	{
		uint32_t rel = 0;

		for (i = 0; i < nreg; i++) {
			int stored = reg[i].flags & V4D_SEG_STORED;

			buf_u32(&mmap->b, reg[i].start);
			buf_u32(&mmap->b, reg[i].size);
			buf_u32(&mmap->b, stored ? mdat->off + rel : 0);
			buf_u16(&mmap->b, reg[i].kind);
			buf_u16(&mmap->b, reg[i].flags);
			if (stored)
				rel += reg[i].size;
		}
	}
	if (mmap->b.err)
		goto out;

	file = calloc(1, total);
	if (!file)
		goto out;
	h = file;

	memcpy(h + V4D_H_MAGIC, V4D_MAGIC, V4D_MAGIC_LEN);
	v4d_p16(h + V4D_H_VER_MAJOR, V4D_VER_MAJOR);
	v4d_p16(h + V4D_H_VER_MINOR, V4D_VER_MINOR);
	v4d_p32(h + V4D_H_HDR_SIZE, V4D_HDR_SIZE);
	v4d_p32(h + V4D_H_FILE_SIZE, (uint32_t)total);
	v4d_p32(h + V4D_H_FLAGS, is_child ? V4D_F_CHILD : 0);
	v4d_p32(h + V4D_H_NSECT, norder);
	v4d_p32(h + V4D_H_SECT_OFF, V4D_HDR_SIZE);
	v4d_p32(h + V4D_H_SECT_ENT, V4D_SECT_ENT);
	v4d_p64(h + V4D_H_TIME, (uint64_t)time(NULL));
	v4d_p32(h + V4D_H_HOST_PID, (uint32_t)getpid());
	v4d_p16(h + V4D_H_PID, cur_gpid());
	v4d_p16(h + V4D_H_PPID, cur_gppid());

	v4d_p16(h + V4D_H_SIGNAL, (uint16_t)gsig);
	v4d_p16(h + V4D_H_FAULT_FLAGS,
		fault_valid && (gsig == V4_SIGSEG || gsig == V4_SIGBUS) ?
		V4D_FF_ADDR : 0);
	v4d_p16(h + V4D_H_FAULT_ADDR, fault_addr);
	v4d_p16(h + V4D_H_INST_PC, inst_pc);

	for (i = 0; i < V4D_NREGS; i++)
		v4d_p16(h + V4D_H_R + 2 * i, R[i]);
	v4d_p16(h + V4D_H_PS, get_ps());
	v4d_p16(h + V4D_H_AOUT_MAGIC, img.magic);

	v4d_p32(h + V4D_H_TEXT_START, 0);
	v4d_p32(h + V4D_H_TEXT_SIZE, img.ts);
	v4d_p32(h + V4D_H_DATA_START, img.dbase);
	v4d_p32(h + V4D_H_DATA_SIZE, img.ds);
	v4d_p32(h + V4D_H_BSS_START, img.dbase + img.ds);
	v4d_p32(h + V4D_H_BSS_SIZE, img.bs);
	v4d_p32(h + V4D_H_IMAGE_END, img.image_end);
	v4d_p32(h + V4D_H_BRK, mem_brk);
	v4d_p32(h + V4D_H_STACK_LO, stack_lo);
	v4d_p32(h + V4D_H_STACK_TOP, V4D_MEM_SIZE);
	v4d_p32(h + V4D_H_INITIAL_SP, img.initial_sp);
	v4d_p32(h + V4D_H_ENTRY, 0);
	v4d_p32(h + V4D_H_MEM_SIZE, V4D_MEM_SIZE);
	v4d_p32(h + V4D_H_NSYMS, nsym_bytes / V4D_SYM_SIZE);
	v4d_p16(h + V4D_H_ARGC, (uint16_t)img.argc);

	for (i = 0; i < (int)norder; i++) {
		uint8_t *e = h + V4D_HDR_SIZE + (size_t)i * V4D_SECT_ENT;

		v4d_p32(e + 0, order[i]->id);
		v4d_p32(e + 4, 0);
		v4d_p32(e + 8, order[i]->off);
		v4d_p32(e + 12, (uint32_t)order[i]->b.len);
		if (order[i]->b.len)
			memcpy(h + order[i]->off, order[i]->b.p,
			       order[i]->b.len);
	}
	v4d_p32(h + V4D_H_CRC, v4d_crc32(0, file, total));

	snprintf(name, sizeof name, "v4linux-%ld.v4dump", (long)getpid());
	f = fopen(name, "wb");
	if (!f) {
		perror("v4linux: cannot create crash dump");
		goto out;
	}
	if (fwrite(file, 1, total, f) != total) {
		perror("v4linux: crash dump write failed");
		fclose(f);
		goto out;
	}
	if (fclose(f) != 0) {
		perror("v4linux: crash dump close failed");
		goto out;
	}
	fprintf(stderr, "v4linux: crash dump written to %s (%lu bytes)\n"
		"         inspect it with: v4-dumpinfo %s\n",
		name, (unsigned long)total, name);
	rc = 0;
out:
	if (bad)
		fprintf(stderr, "v4linux: out of memory writing crash dump\n");
	free(file);
	for (i = 0; i < (int)nsect; i++)
		free(s[i].b.p);
	return rc;
}
