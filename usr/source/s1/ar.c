/*
 * Unix Fourth Edition -- ar.c -- ar (1)
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

#define AREXIT_OK	0
#define AREXIT_ERR	1

#define DIRSZ		16
#define BLK		    512

#define D_NAME		0
#define D_MTIM		8
#define D_UID		12
#define D_MODE		13
#define D_SIZE		14

#define USED		((char *)-1)

static const uint8_t magic[2] = { 0155, 0377 };

static int afi = -1;
static int afo = -1;
static int tfi = -1;
static int tfo = -1;
static int fio = -1;
static int vflg;
static int errflg;

static uint8_t dir[DIRSZ];
static char rname[9];
static char tfil[PATH_MAX + 16];
static uint8_t buf[BLK];

static char **arglst;
static const char *badpath;

static uint16_t
size16(void)
{
	return (uint16_t)(dir[D_SIZE] | (dir[D_SIZE + 1] << 8));
}

static uint32_t
mtim32(void)
{
	return ((uint32_t)(dir[D_MTIM] | (dir[D_MTIM + 1] << 8)) << 16) |
	    (uint32_t)(dir[D_MTIM + 2] | (dir[D_MTIM + 3] << 8));
}

static void
setmtim(uint32_t t)
{
	dir[D_MTIM] = (t >> 16) & 0377;
	dir[D_MTIM + 1] = (t >> 24) & 0377;
	dir[D_MTIM + 2] = t & 0377;
	dir[D_MTIM + 3] = (t >> 8) & 0377;
}

static void
putstr(int fd, const char *s)
{
	size_t n = strlen(s);

	if (n != 0)
		write(fd, s, n);
}

static void
diag(const char *msg, int status)
{
	putstr(2, msg);
	if (tfo >= 0)
		unlink(tfil);
	exit(status);
}

static void
done(void)
{
	if (tfo >= 0)
		unlink(tfil);
	exit(errflg ? AREXIT_ERR : AREXIT_OK);
}

static void
sigdone(int sig)
{
	(void)sig;
	if (tfo >= 0)
		unlink(tfil);
	_exit(AREXIT_ERR);
}

static void
userr(void)
{
	diag("bad usage\n", AREXIT_ERR);
}

static void
namediag(const char *name, const char *what)
{
	putstr(2, name);
	diag(what, AREXIT_ERR);
}

static void
phserr(void)
{
	namediag(badpath, " -- phase error\n");
}

static void
mesg(int c)
{
	char b[2];

	if (!vflg)
		return;
	b[0] = c;
	b[1] = ' ';
	write(1, b, 2);
	putstr(1, rname);
	write(1, "\n", 1);
}

static void
table(void)
{
	putstr(1, rname);
	write(1, "\n", 1);
}

static int
getaf(void)
{
	ssize_t n;

	afi = open(arglst[0], O_RDONLY);
	if (afi < 0)
		return 0;
	n = read(afi, buf, 2);
	if (n != 2 || memcmp(buf, magic, 2) != 0)
		namediag(arglst[0], " -- not in archive format\n");
	return 1;
}

static void
mktmp(void)
{
	const char *td = getenv("TMPDIR");
	char c;

	if (td == NULL || *td == '\0')
		td = "/tmp";

	for (c = 'a'; c < 'z'; c++) {
		snprintf(tfil, sizeof tfil, "%s/vtm%c", td, c);
		tfo = open(tfil, O_WRONLY | O_CREAT | O_EXCL, 0600);
		if (tfo >= 0)
			break;
		if (errno != EEXIST)
			break;
	}
	if (tfo < 0)
		diag("cannot open temp file\n", AREXIT_ERR);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, sigdone);
	tfi = open(tfil, O_RDONLY);
	if (tfi < 0)
		diag("cannot open temp file\n", AREXIT_ERR);
}

static int
getdir(void)
{
	if (read(afi, dir, DIRSZ) != DIRSZ)
		return 0;
	memcpy(rname, dir + D_NAME, 8);
	rname[8] = '\0';
	return 1;
}

static void
skip(void)
{
	lseek(afi, (off_t)((size16() + 1u) & ~1u), SEEK_CUR);
}

static const char *
trim(const char *p)
{
	const char *s = strrchr(p, '/');

	return s != NULL ? s + 1 : p;
}

static char **
match(void)
{
	char **a;
	const char *b;
	int i;

	for (a = arglst + 1; *a != NULL; a++) {
		if (*a == USED)
			continue;
		b = trim(*a);
		for (i = 0; i < 8; i++) {
			if ((char)dir[D_NAME + i] != b[i])
				break;
			if (b[i] == '\0') {
				i = 8;
				break;
			}
		}
		if (i == 8)
			return a;
	}
	return NULL;
}

static void
xfer(int from, int to)
{
	uint16_t r1 = size16();
	ssize_t n;

	while (r1 != 0) {
		n = r1 < BLK ? r1 : BLK;
		n = read(from, buf, n);
		if (n <= 0)
			phserr();
		r1 -= (uint16_t)n;
		write(to, buf, n);
	}
}

static void
copyfl(void)
{
	write(tfo, dir, DIRSZ);
	badpath = rname;
	xfer(afi, tfo);
	if (size16() & 1) {
		lseek(afi, 1, SEEK_CUR);
		lseek(tfo, 1, SEEK_CUR);
	}
}

static void
mvfil(char **slot)
{
	const char *path = *slot, *b;
	struct stat st;
	uint16_t r2;
	ssize_t n;
	int i;

	badpath = path;
	if (stat(path, &st) < 0 || S_ISDIR(st.st_mode))
		namediag(path, " -- cannot open\n");
	fio = open(path, O_RDONLY);
	if (fio < 0)
		namediag(path, " -- cannot open\n");
	*slot = USED;
	b = trim(path);
	for (i = 0; i < 8; i++) {
		dir[D_NAME + i] = (uint8_t)b[i];
		if (b[i] == '\0')
			break;
	}
	setmtim((uint32_t)st.st_mtime);
	dir[D_UID] = (uint8_t)st.st_uid;
	dir[D_MODE] = (uint8_t)(st.st_mode & 0377);
	if (st.st_size > 0xffff)
		namediag(path, " -- too large\n");
	dir[D_SIZE] = st.st_size & 0377;
	dir[D_SIZE + 1] = (st.st_size >> 8) & 0377;
	write(tfo, dir, DIRSZ);
	r2 = size16();
	for (;;) {
		n = read(fio, buf, BLK);
		if (n <= 0)
			break;
		r2 -= (uint16_t)n;
		write(tfo, buf, n);
	}
	if (r2 != 0)
		phserr();
	if (size16() & 1)
		lseek(tfo, 1, SEEK_CUR);
	close(fio);
	memcpy(rname, dir + D_NAME, 8);
	rname[8] = '\0';
}

static void
xtract(void)
{
	if (strchr(rname, '/') != NULL)
		namediag(rname, " -- cannot create\n");
	fio = open(rname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fio < 0)
		namediag(rname, " -- cannot create\n");
	badpath = rname;
	xfer(afi, fio);
	close(fio);
	if (size16() & 1)
		lseek(afi, 1, SEEK_CUR);
}

static char **
oldnew(char **tname)
{
	struct stat st;

	if (stat(rname, &st) < 0)
		return NULL;
	if ((uint32_t)st.st_mtime <= mtim32())
		return NULL;
	*tname = rname;
	return tname;
}

static void
nfound(void)
{
	char **a;

	for (a = arglst + 1; *a != NULL; a++) {
		if (*a == USED)
			continue;
		putstr(2, *a);
		putstr(2, " -- not found\n");
		*a = USED;
		errflg++;
	}
}

static void
copfl(void)
{
	char **a;
	ssize_t n;

	for (a = arglst + 1; *a != NULL; a++) {
		if (*a == USED)
			continue;
		mvfil(a);
		mesg('a');
	}
	signal(SIGINT, SIG_IGN);
	afo = open(arglst[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (afo < 0)
		diag("cannot create archive file\n", AREXIT_ERR);
	write(afo, magic, 2);
	while ((n = read(tfi, buf, BLK)) > 0)
		write(afo, buf, n);
	done();
}

static void
comr(void)
{
	char **s;

	mktmp();
	if (!getaf())
		copfl();
	while (getdir()) {
		s = match();
		if (s != NULL) {
			mesg('r');
			skip();
			mvfil(s);
		} else {
			copyfl();
			mesg('c');
		}
	}
	copfl();
}

static void
comu(void)
{
	char *tname;
	char **s;

	mktmp();
	if (!getaf())
		diag("no archive file\n", AREXIT_ERR);
	while (getdir()) {
		if (arglst[1] != NULL) {
			s = match();
			if (s == NULL) {
				copyfl();
				mesg('c');
				continue;
			}
			*s = USED;
		}
		s = oldnew(&tname);
		if (s == NULL) {
			copyfl();
			mesg('c');
			continue;
		}
		mesg('r');
		skip();
		mvfil(s);
	}
	copfl();
}

static void
comd(void)
{
	char **s;

	mktmp();
	if (!getaf())
		diag("no archive file\n", AREXIT_ERR);
	while (getdir()) {
		s = match();
		if (s == NULL) {
			copyfl();
			mesg('c');
			continue;
		}
		*s = USED;
		skip();
		mesg('d');
	}
	nfound();
	copfl();
}

static void
comx(void)
{
	char **s;

	if (!getaf())
		diag("no archive file\n", AREXIT_ERR);
	while (getdir()) {
		if (arglst[1] != NULL) {
			s = match();
			if (s == NULL) {
				skip();
				continue;
			}
			*s = USED;
		}
		xtract();
		mesg('x');
	}
	nfound();
	done();
}

static void
comt(void)
{
	char **s;

	if (!getaf())
		diag("no archive file\n", AREXIT_ERR);
	while (getdir()) {
		if (arglst[1] != NULL) {
			s = match();
			if (s != NULL)
				*s = USED;
			else {
				skip();
				continue;
			}
		}
		table();
		skip();
	}
	nfound();
	done();
}

int
main(int argc, char **argv)
{
	const char *k;
	int key = 0;
	int i;

	if (argc - 2 <= 0)
		userr();
	for (k = argv[1]; *k != '\0'; k++) {
		if (*k == 'v') {
			vflg++;
			continue;
		}
		if (key != 0)
			userr();
		key = (unsigned char)*k;
	}
	if (key == 0)
		userr();
	arglst = calloc(argc, sizeof *arglst);
	if (arglst == NULL)
		diag("out of memory\n", AREXIT_ERR);
	for (i = 2; i < argc; i++)
		arglst[i - 2] = argv[i];
	arglst[argc - 2] = NULL;

	switch (key) {
	case 'r':
		comr();
		break;
	case 'u':
		comu();
		break;
	case 'd':
		comd();
		break;
	case 'x':
		comx();
		break;
	case 't':
		comt();
		break;
	default:
		userr();
	}
	return AREXIT_OK;
}
