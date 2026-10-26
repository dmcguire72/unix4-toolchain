/*
 * Unix Fourth Edition -- crt.c
 * 
 * Original:
 * Copyright 1972, 1973 Bell Telephone Laboratories, Inc.
 * 
 * Modernized Port:
 * Copyright 2026 Daniel McGuire
 */
#include "ccw.h"
#include <stdarg.h>

FILE *outf;
FILE *ascf, *binf;
int line, nerror;
word ts[TSLEN + 8];
int mask7;

static void
opc(int c)
{
	fputc(mask7 ? (c & 0177) : c, outf);
}

static void
putn(unsigned n, unsigned b, int *cnt)
{
	if (n / b)
		putn(n / b, b, cnt);
	opc("0123456789"[n % b]);
}

static void
vcprintf(const char *fmt, va_list ap)
{
	int c, prec, hasprec, cnt = 0;
	const char *s;

	for (;;) {
		while ((c = (unsigned char)*fmt++) != '%') {
			if (c == '\0')
				return;
			opc(c);
		}
		prec = hasprec = 0;
		c = (unsigned char)*fmt++;
		if (c == '.') {
			hasprec = 1;
			while ((c = (unsigned char)*fmt++) >= '0' && c <= '9')
				prec = prec * 10 + c - '0';
		}
		switch (c) {
		case 'd': {
			int x = (int16_t)va_arg(ap, int);

			if (x < 0) {
				opc('-');
				x = -x;
			}
			putn(x, 10, &cnt);
			break;
		}
		case 'o':
			putn((uint16_t)va_arg(ap, int), 8, &cnt);
			break;
		case 'c': {
			int x = (uint16_t)va_arg(ap, int);

			if (x & 0377)
				opc(x & 0377);
			if (x >> 8)
				opc(x >> 8);
			break;
		}
		case 's':
			s = va_arg(ap, const char *);
			while (*s != '\0' && (!hasprec || prec-- > 0))
				opc(*s++);
			break;
		default:
			opc('%');
			fmt--;
		}
	}
}

void
cprintf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vcprintf(fmt, ap);
	va_end(ap);
}

void
error(const char *fmt, ...)
{
	va_list ap;
	FILE *save = outf;

	nerror++;
	if (outf != NULL)
		fflush(outf);
	outf = stderr;
	cprintf("%d: ", line);
	va_start(ap, fmt);
	vcprintf(fmt, ap);
	va_end(ap);
	fputc('\n', outf);
	outf = save;
}

void
putw16(int w, FILE *f)
{
	fputc(w & 0377, f);
	fputc((w >> 8) & 0377, f);
}

int
getw16(FILE *f, int *eof)
{
	int lo = fgetc(f), hi;

	if (lo == EOF || (hi = fgetc(f)) == EOF) {
		if (eof)
			*eof = 1;
		return 0;
	}
	return (int16_t)(lo | hi << 8);
}

int
nodeinfo(const word *w, int *pm)
{
	int i, k;

	*pm = 0;
	switch (w[0]) {
	case 0:
		return 3;
	case NAME:
		if (w[3] == EXTERN) {
			for (k = 0; k < 4; k++)
				if ((w[5 + k] & ~0177) == 0) {
					k++;
					break;
				}
			if (k > 4)
				k = 4;
			return 5 + k;
		}
		return 6;
	case CON: case FCON: case SFCON:
	case SETREG:
		return 4;
	case CBRANCH:
		*pm = 1 << 1;
		return 4;
	case STAR: case AMPER: case SIZEOF: case NEG: case COMPL: case EXCLA:
	case ITOF: case FTOI: case INIT: case RFORCE: case JUMP:
		*pm = 1 << 3;
		return 4;
	case INCBEF: case DECBEF: case INCAFT: case DECAFT:
		*pm = 1 << 3;
		return 5;
	}
	(void)i;
	*pm = (1 << 3) | (1 << 4);
	return 5;
}

#include <math.h>
void
pdpwords(double v, uint16_t w[4], int dbl)
{
	int e, sign = v < 0;
	double m;
	uint64_t mant;

	w[0] = w[1] = w[2] = w[3] = 0;
	if (v == 0)
		return;
	if (sign)
		v = -v;
	m = frexp(v, &e);
	e += 128;
	if (dbl)
		mant = (uint64_t)ldexp(m, 56);
	else {
		mant = (uint64_t)floor(ldexp(m, 24) + 0.5);
		if (mant >> 24) {
			mant >>= 1;
			e++;
		}
	}
	if (e <= 0 || e > 255)
		return;
	if (dbl) {
		w[0] = (sign << 15) | (e << 7) | ((mant >> 48) & 0x7f);
		w[1] = mant >> 32;
		w[2] = mant >> 16;
		w[3] = mant;
	} else {
		w[0] = (sign << 15) | (e << 7) | ((mant >> 16) & 0x7f);
		w[1] = mant;
	}
}

int
ptrmask(int op)
{
	switch (op) {
	case 0: case NAME: case CON: case FCON: case SFCON: case SETREG:
		return 0;
	case CBRANCH:
		return 1 << 1;
	case STAR: case AMPER: case SIZEOF: case NEG: case COMPL: case EXCLA:
	case ITOF: case FTOI: case INIT: case RFORCE: case JUMP:
	case INCBEF: case DECBEF: case INCAFT: case DECAFT:
		return 1 << 3;
	}
	return (1 << 3) | (1 << 4);
}
