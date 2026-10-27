/*
 * Unix Fourth Edition -- strip.c -- strip (1)
 *
 * Original assembly:
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 *
 * C Port:
 * Copyright 2026 Daniel McGuire
 */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define STEXIT_OK	0
#define STEXIT_ERR	1

#define BLK		512
#define HDRSZ		16
#define OMAGIC		0407
#define NMAGIC		0410

static char tmpnam_[PATH_MAX + 16];
static int tmpmade;
static int errflg;
static int catchint;
static uint8_t buf[BLK];

static void
putstr(const char *s)
{
	size_t n = strlen(s);

	if (n != 0)
		write(2, s, n);
}

static void
mesg(const char *m, const char *name)
{
	putstr(m);
	putstr(name);
	putstr("\n");
	errflg++;
}

static void
rmtmp(void)
{
	if (tmpmade) {
		unlink(tmpnam_);
		tmpmade = 0;
	}
}

static void
sigint(int sig)
{
	(void)sig;
	rmtmp();
	_exit(STEXIT_ERR);
}

static uint16_t
rd16(const uint8_t *p)
{
	return (uint16_t)(p[0] | (p[1] << 8));
}

static void
wr16(uint8_t *p, uint16_t v)
{
	p[0] = v & 0377;
	p[1] = v >> 8;
}

static int
mktmp(void)
{
	const char *td = getenv("TMPDIR");
	char c;
	int fd;

	if (td == NULL || *td == '\0')
		td = "/tmp";
	for (c = 'a'; c <= 'z'; c++) {
		snprintf(tmpnam_, sizeof tmpnam_, "%s/stm%c", td, c);
		fd = open(tmpnam_, O_WRONLY | O_CREAT | O_EXCL, 0400);
		if (fd >= 0) {
			tmpmade = 1;
			return fd;
		}
	}
	return -1;
}

static int
strip1(const char *name)
{
	long r2;
	ssize_t r3;
	int fi, fo, ti;

	fi = open(name, O_RDONLY);
	if (fi < 0) {
		mesg("open error: ", name);
		return 0;
	}
	fo = mktmp();
	if (fo < 0) {
		mesg("can't create temp file for ", name);
		close(fi);
		return -1;
	}
	r3 = read(fi, buf, BLK);
	if (r3 < 2 || (rd16(buf) != OMAGIC && rd16(buf) != NMAGIC)) {
		mesg("improper format: ", name);
		close(fo);
		close(fi);
		rmtmp();
		return 0;
	}
	if (r3 < HDRSZ) {
		mesg("unexpected EOF: ", name);
		close(fo);
		close(fi);
		rmtmp();
		return 0;
	}
	r2 = (long)rd16(buf + 2) + rd16(buf + 4) + HDRSZ;
	wr16(buf + 8, 0);
	wr16(buf + 14, 1);
	for (;;) {
		if (r2 < r3)
			r3 = r2;
		if (write(fo, buf, r3) != r3) {
			mesg("can't write temp file for: ", name);
			close(fo);
			close(fi);
			rmtmp();
			return 0;
		}
		r2 -= r3;
		if (r2 <= 0)
			break;
		r3 = read(fi, buf, BLK);
		if (r3 <= 0) {
			mesg("unexpected EOF: ", name);
			break;
		}
	}
	close(fo);
	close(fi);

	ti = open(tmpnam_, O_RDONLY);
	if (ti < 0) {
		mesg("can't read temp file for: ", name);
		rmtmp();
		return -1;
	}
	fo = open(name, O_WRONLY | O_TRUNC);
	if (fo < 0) {
		mesg("can't rewrite: ", name);
		close(ti);
		rmtmp();
		return 0;
	}
	signal(SIGINT, SIG_IGN);
	while ((r3 = read(ti, buf, BLK)) > 0)
		write(fo, buf, r3);
	close(ti);
	close(fo);
	if (catchint)
		signal(SIGINT, sigint);
	rmtmp();
	return 0;
}

int
main(int argc, char **argv)
{
	int i;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN) {
		catchint = 1;
		signal(SIGINT, sigint);
	}
	for (i = 1; i < argc; i++)
		if (strip1(argv[i]) < 0)
			break;
	return errflg ? STEXIT_ERR : STEXIT_OK;
}
