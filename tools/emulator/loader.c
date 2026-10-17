#include "pdp11.h"

#include <stdio.h>
#include <string.h>

int load_image(const char *path, int argc, char **argv)
{
	FILE *f = fopen(path, "rb");
	uint8_t h[16];
	size_t n;
	int i, k;
	uint16_t sp, magic, ts, ds, bs;
	int nc = 0;

	if (!f) return -1;
	if (fread(h, 1, 16, f) != 16) {
		fclose(f);
		return -1;
	}
	magic = h[0] | h[1] << 8;
	ts = h[2] | h[3] << 8;
	ds = h[4] | h[5] << 8;
	bs = h[6] | h[7] << 8;
	(void)bs;
	if (magic != 0407 && magic != 0410) {
		fclose(f);
		return -1;
	}
	memset(mem, 0, sizeof mem);
	if (magic == 0410) {
		size_t dbase = ((size_t)ts + 8191) & ~(size_t)8191;

		n = fread(mem, 1, ts, f);
		if (dbase + ds + bs > 0160000) {
			fclose(f);
			die("image too large");
		}
		n = fread(mem + dbase, 1, ds, f);
	} else {
		n = fread(mem, 1, (size_t)ts + ds, f);
	}
	(void)n;
	fclose(f);

	for (i = 0; i < argc; i++) nc += strlen(argv[i]) + 1;
	if (nc & 1) nc++;
	sp = (uint16_t)(-nc - argc * 2 - 4);
	SP = sp;
	wrw(sp, argc);
	k = (uint16_t)-nc;
	{
		uint16_t ap = sp, c = (uint16_t)-nc;
		for (i = 0; i < argc; i++) {
			ap += 2;
			wrw(ap, c);
			for (const char *s = argv[i]; ; s++) {
				mem[c++] = *s;
				if (!*s) break;
			}
		}
		wrw(ap + 2, 0177777);
	}
	(void)k;
	for (i = 0; i < 6; i++) R[i] = 0;
	PC = 0;
	for (i = 0; i < 64; i++) if (!(sigtab[i] & 1)) sigtab[i] = 0;
	fN = fZ = fV = fC = 0;
	return 0;
}
