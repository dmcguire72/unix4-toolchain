#include "pdp11.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void record_image(const char *path, uint16_t magic, uint16_t ts,
			 uint16_t ds, uint16_t bs, size_t dbase, size_t image_end,
			 uint16_t sp, int argc, char **argv)
{
	size_t len = 0;
	int i;
	char *p;

	free(img.path);
	free(img.argv);
	memset(&img, 0, sizeof img);
	img.path = strdup(path);
	img.magic = magic;
	img.ts = ts;
	img.ds = ds;
	img.bs = bs;
	img.dbase = magic == V4_AOUT_NMAGIC ? (uint32_t)dbase : ts;
	img.image_end = (uint32_t)image_end;
	img.initial_sp = sp;
	for (i = 0; i < argc; i++)
		len += strlen(argv[i]) + 1;
	img.argv = malloc(len ? len : 1);
	if (!img.argv)
		return;
	for (p = img.argv, i = 0; i < argc; i++) {
		size_t n = strlen(argv[i]) + 1;

		memcpy(p, argv[i], n);
		p += n;
	}
	img.argc = argc;
	img.argv_len = len;
}

int load_image(const char *path, int argc, char **argv)
{
	FILE *f;
	struct stat st;
	uint8_t h[16];
	int i, nc = 0;
	uint16_t sp, magic, ts, ds, bs, ap, c;
	size_t dbase = 0, image_end;

	if (stat(path, &st) < 0) {
		load_errno = (errno == ENOENT || errno == ENAMETOOLONG) ? V4_ENOENT :
		             (errno == ENOTDIR) ? V4_ENOTDIR : V4_EACCES;
		return -1;
	}
	if (!S_ISREG(st.st_mode)) {
		load_errno = V4_EACCES;
		return -1;
	}
	f = fopen(path, "rb");
	if (!f) {
		load_errno = V4_EACCES;
		return -1;
	}
	if (fread(h, 1, 16, f) != 16) {
		fclose(f);
		load_errno = V4_ENOEXEC;
		return -1;
	}
	magic = h[0] | h[1] << 8;
	ts = h[2] | h[3] << 8;
	ds = h[4] | h[5] << 8;
	bs = h[6] | h[7] << 8;
	if (magic != V4_AOUT_OMAGIC && magic != V4_AOUT_NMAGIC) {
		fclose(f);
		load_errno = V4_ENOEXEC;
		return -1;
	}
	if (magic == V4_AOUT_NMAGIC) {
		dbase = ((size_t)ts + 8191) & ~(size_t)8191;
		if (dbase + ds + bs > V4_MEM_MAX) {
			fclose(f);
			load_errno = V4_ENOMEM;
			return -1;
		}
	} else if ((size_t)ts + ds + bs > V4_MEM_MAX) {
		fclose(f);
		load_errno = V4_ENOMEM;
		return -1;
	}
	for (i = 0; i < argc; i++) nc += strlen(argv[i]) + 1;
	if (nc & 1) nc++;
	if (nc > V4_MAX_ARGS_LEN + 1) {
		fclose(f);
		load_errno = V4_E2BIG;
		return -1;
	}

	memset(mem, 0, sizeof mem);
	if (magic == V4_AOUT_NMAGIC) {
		fread(mem, 1, ts, f);
		fread(mem + dbase, 1, ds, f);
	} else {
		fread(mem, 1, (size_t)ts + ds, f);
	}
	sp = (uint16_t)(-nc - argc * 2 - 4);
	SP = sp;
	image_end = (magic == V4_AOUT_NMAGIC) ? dbase + ds + bs : (size_t)ts + ds + bs;
	mem_reset(image_end);
	record_image(path, magic, ts, ds, bs, dbase, image_end, sp, argc, argv);
	sym_load(f, h);
	hooks_load();
	trace_calls_reset();
	fclose(f);

	wrw(sp, argc);
	ap = sp;
	c = (uint16_t)-nc;
	for (i = 0; i < argc; i++) {
		ap += 2;
		wrw(ap, c);
		for (const char *s = argv[i]; ; s++) {
			mem[c++] = *s;
			if (!*s) break;
		}
	}
	wrw(ap + 2, 0177777);

	for (i = 0; i < 6; i++) R[i] = 0;
	PC = 0;
	exec_reset_signals();
	fp_reset();
	prof_scale = 0;
	fN = fZ = fV = fC = 0;
	return 0;
}
