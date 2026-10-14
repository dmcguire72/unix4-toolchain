/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#include "cc.h"
#include <stdarg.h>

int
rdchar(void)
{
	int c = getc(fin);

	return c == EOF ? 0 : c;
}

void
wrchar(int c)
{
	putc(c, fout);
}

void
flush(void)
{
	fflush(fout);
}

static void
printn(FILE *f, unsigned n, unsigned b)
{
	unsigned a;

	if ((a = n / b) != 0)
		printn(f, a, b);
	putc(n % b + '0', f);
}

static void
vfmt(FILE *f, const char *fmt, va_list ap)
{
	int c, x;
	const char *s;

	for (;;) {
		while ((c = *fmt++) != '%') {
			if (c == '\0')
				return;
			putc(c, f);
		}
		switch (c = *fmt++) {

		case 'd':
		case 'o':
			x = (int16_t)va_arg(ap, int);
			if (x < 0) {
				if (x == INT16_MIN) {
					fputs(c == 'o' ? "100000" : "-32768", f);
					continue;
				}
				x = -x;
				putc('-', f);
			}
			printn(f, x, c == 'o' ? 8 : 10);
			continue;

		case 's':
			s = va_arg(ap, const char *);
			while ((c = *s++) != '\0')
				putc(c, f);
			continue;

		case 'p':
			s = va_arg(ap, const char *);
			putc('_', f);
			for (c = NAMSIZ; c-- && *s; )
				putc(*s++, f);
			continue;
		}
		putc('%', f);
		fmt--;
	}
}

void
oprintf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfmt(fout, fmt, ap);
	va_end(ap);
}

void
error(const char *fmt, ...)
{
	va_list ap;

	nerror++;
	flush();
	fprintf(stderr, "%d: ", line);
	va_start(ap, fmt);
	vfmt(stderr, fmt, ap);
	va_end(ap);
	putc('\n', stderr);
}

int
length(int t)
{
	if (t < 0)
		t += 020;
	if (t >= 020)
		return 2;
	switch (t) {
	case 0:				/* int */
		return 2;
	case 1:				/* char */
		return 1;
	case 2:				/* float */
		return 4;
	case 3:				/* double */
		return 8;
	case 4:				/* long */
		return 4;
	}
	return 1024;
}

int
rlength(int t)
{
	int l;

	return (l = length(t)) == 1 ? 2 : l;
}

void
jump(int lab)
{
	oprintf("jmp\tl%d\n", lab);
}

void
label(int l)
{
	oprintf("l%d:", l);
}

static FILE *tmpf;

void
putwrd(int w)
{
	if (tmpf == NULL && (tmpf = fopen(tmpfil, "wb")) == NULL) {
		fputs("Temp file?\n", stderr);
		exit(1);
	}
	putc(w & 0377, tmpf);
	putc((w >> 8) & 0377, tmpf);
}

int
getwrd(void)
{
	int lo, hi;

	if (tmpf == NULL && (tmpf = fopen(tmpfil, "rb")) == NULL) {
		fputs("Temp file botch.\n", stderr);
		exit(1);
	}
	if ((lo = getc(tmpf)) == EOF || (hi = getc(tmpf)) == EOF) {
		fputs("Temp file botch.\n", stderr);
		exit(1);
	}
	return (int16_t)(lo | hi << 8);
}

void
flshw(void)
{
	if (tmpf != NULL) {
		if (fclose(tmpf) == EOF) {
			fputs("Temp file?\n", stderr);
			exit(1);
		}
		tmpf = NULL;
	}
}
