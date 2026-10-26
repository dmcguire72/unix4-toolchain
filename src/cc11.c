/*
 * Unix Fourth Edition -- cc11.c -- cc (1)
 * 
 * Original:
 * Copyright 1972, 1973 Bell Telephone Laboratories, Inc.
 * 
 * Modernized Port:
 * Copyright 2026 Daniel McGuire
 */
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define NAV	50
#define SYMSIZ	200

static char *tmpdir_, *tmp1, *tmp2, *tmp3, *tmp4, *tmp5;
static char *av[NAV + 8];
static char *clist[NAV];
static char *llist[NAV];
static int pflag, sflag, cflag, oflag, proflag, errs;
static char bindir[PATH_MAX], libdir[PATH_MAX];
static char pass0[PATH_MAX], pass1[PATH_MAX], pass2c[PATH_MAX];
static char asprog[PATH_MAX], ldprog[PATH_MAX];
static char pref[PATH_MAX];

struct symtab {
	char name[8];
	const char *value;
};
static struct symtab symtab[SYMSIZ];
static struct symtab *defloc, *incloc;
static FILE *ibuf, *ibuf1, *ibuf2, *obuf;
static char *line;
static char *lp;
static char linebuf[196];
static int lineno, exfail, instring;
static char *stringbuf, *stringend;

static void
cerror(const char *s, const char *x)
{
	fprintf(stderr, "%d: ", lineno);
	fprintf(stderr, s, x);
	fputc('\n', stderr);
	exfail++;
	cflag++;
}

static void
sch(int c)
{
	char *rlp = lp;

	if (rlp == line + 194)
		cerror("Line overflow", "");
	*rlp++ = c;
	if (rlp > line + 195)
		rlp = line + 195;
	lp = rlp;
}

static void
savch(int c)
{
	if (stringbuf >= stringend) {
		fprintf(stderr, "cc: define table overflow\n");
		exit(1);
	}
	*stringbuf++ = c;
}

static int
getc1(void)
{
	int c;

	if ((c = getc(ibuf)) == EOF && ibuf == ibuf2) {
		fclose(ibuf2);
		ibuf = ibuf1;
		putc('\n', obuf);
		c = getc1();
	}
	if (c == EOF)
		return 0;
	return c;
}

static int
getch(void)
{
	static int peekc;
	int c;

	if (peekc) {
		c = peekc;
		peekc = 0;
		return c;
	}
loop:
	if ((c = getc1()) == '/' && !instring) {
		if ((peekc = getc1()) != '*')
			return '/';
		peekc = 0;
		for (;;) {
			c = getc1();
cloop:
			switch (c) {
			case '\0':
				return '\0';
			case '*':
				if ((c = getc1()) == '/')
					goto loop;
				goto cloop;
			case '\n':
				if (ibuf == ibuf1) {
					putc('\n', obuf);
					lineno++;
				}
				continue;
			}
		}
	}
	return c;
}

static void
subst(char *np, struct symtab *sp)
{
	const char *vp;

	lp = np;
	if ((vp = sp->value) == NULL)
		return;
	sch(' ');
	while (*vp)
		sch(*vp++);
	sch(' ');
}

static struct symtab *
lookup(char *namep, int enterf)
{
	struct symtab *sp;
	int i = 0, k;
	const char *np;

	for (np = namep; *np; np++)
		i += *np;
	i %= SYMSIZ;
	sp = &symtab[i];
	while (sp->name[0]) {
		for (k = 0; k < 8; k++) {
			if (sp->name[k] != namep[k])
				break;
			if (namep[k] == '\0' || k == 7) {
				if (!enterf)
					subst(namep, sp);
				return sp;
			}
		}
		if (++sp >= &symtab[SYMSIZ])
			sp = symtab;
	}
	if (enterf) {
		const char *snp = namep;

		for (k = 0; k < 8; k++)
			if ((sp->name[k] = *snp) != '\0')
				snp++;
	}
	return sp;
}

static int
getline_(void)
{
	int c, sc, state;
	struct symtab *np;
	char *namep, *filname;

	if (ibuf == ibuf1)
		lineno++;
	lp = line;
	*lp = '\0';
	state = 0;
	if ((c = getch()) == '#')
		state = 1;
	while (c != '\n' && c != '\0') {
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_') {
			namep = lp;
			sch(c);
			while (('a' <= (c = getch()) && c <= 'z')
			    || ('A' <= c && c <= 'Z')
			    || ('0' <= c && c <= '9')
			    || c == '_')
				sch(c);
			sch('\0');
			lp--;
			np = lookup(namep, state);
			if (state == 1) {
				if (np == defloc)
					state = 2;
				else if (np == incloc)
					state = 3;
				else {
					cerror("Undefined control", "");
					while (c != '\n' && c != '\0')
						c = getch();
					return c;
				}
			} else if (state == 2) {
				np->value = stringbuf;
				while ((c = getch()) != '\n' && c != '\0')
					savch(c);
				savch('\0');
				return 1;
			}
			continue;
		} else if ((sc = c) == '\'' || sc == '"') {
			sch(sc);
			filname = lp;
			instring++;
			while ((c = getch()) != sc && c != '\n' && c != '\0') {
				sch(c);
				if (c == '\\')
					sch(getch());
			}
			instring = 0;
			if (state == 3) {
				*lp = '\0';
				while ((c = getch()) != '\n' && c != '\0')
					;
				if (ibuf == ibuf2)
					cerror("Nested 'include'", "");
				if ((ibuf2 = fopen(filname, "r")) == NULL) {
					cerror("Missing file %s", filname);
					ibuf2 = NULL;
				} else
					ibuf = ibuf2;
				return c;
			}
		}
		sch(c);
		c = getch();
	}
	sch('\0');
	if (state > 1)
		cerror("Control syntax", "");
	return c;
}

static const char *
expand(const char *file)
{
	int c;
	char *rlp;
	static char sbf[8192];

	exfail = 0;
	ibuf = ibuf1 = fopen(file, "r");
	ibuf2 = NULL;
	if (ibuf1 == NULL)
		return file;
	if (getc(ibuf1) != '#') {
		fclose(ibuf1);
		return file;
	}
	ungetc('#', ibuf1);
	memset(symtab, 0, sizeof symtab);
	defloc = lookup("define", 1);
	defloc->value = defloc->name;
	incloc = lookup("include", 1);
	incloc->value = incloc->name;
	stringbuf = sbf;
	stringend = sbf + sizeof sbf;
	line = linebuf;
	lineno = 0;
	if ((obuf = fopen(tmp4, "w")) == NULL) {
		fprintf(stderr, "Can't creat %s\n", tmp4);
		exit(1);
	}
	while (getline_()) {
		if (ibuf == ibuf2 && pflag == 0)
			putc(001, obuf);
		if (linebuf[0] != '#')
			for (rlp = line; (c = *rlp++) != '\0'; )
				putc(c, obuf);
		putc('\n', obuf);
	}
	fflush(obuf);
	fclose(obuf);
	fclose(ibuf1);
	return tmp4;
}

static int
getsuf(const char *as)
{
	int c = 0, t;
	const char *s = as;

	while ((t = *s++) != '\0')
		if (t == '/')
			c = 0;
		else
			c++;
	s -= 3;
	if (c <= 14 && c > 2 && *s++ == '.')
		return *s;
	return 0;
}

static char *
setsuf(char *as, int ch)
{
	char *s = as;

	while (*s++)
		;
	s[-2] = ch;
	return as;
}

static char *
copy(const char *s)
{
	char *p = strdup(s);

	if (p == NULL) {
		fprintf(stderr, "cc: out of memory\n");
		exit(1);
	}
	return p;
}

static int
nodup(char **l, const char *os)
{
	if (getsuf(os) != 'o')
		return 1;
	while (*l != NULL)
		if (strcmp(*l++, os) == 0)
			return 0;
	return 1;
}

static void
cunlink(const char *f)
{
	if (f != NULL)
		unlink(f);
}

static void
dexit(void)
{
	if (!pflag) {
		cunlink(tmp1);
		cunlink(tmp2);
		if (sflag == 0)
			cunlink(tmp3);
		cunlink(tmp4);
		cunlink(tmp5);
		if (tmpdir_ != NULL)
			rmdir(tmpdir_);
	}
	exit(errs ? 1 : 0);
}

static void
sigexit(int s)
{
	(void)s;
	errs++;
	dexit();
}

static int
callsys(const char *f, char **v)
{
	int status;
	pid_t t;

	fflush(stdout);
	if ((t = fork()) == 0) {
		execv(f, v);
		fprintf(stderr, "Can't find %s\n", f);
		_exit(1);
	} else if (t == -1) {
		fprintf(stderr, "Try again\n");
		return 1;
	}
	while (t != wait(&status))
		;
	if (WIFSIGNALED(status) && WTERMSIG(status) != SIGPIPE) {
		if (WTERMSIG(status) != SIGINT)
			fprintf(stderr, "Fatal error in %s\n", f);
		errs++;
		dexit();
	}
	return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static void
findlayout(const char *argv0)
{
	char exe[PATH_MAX];
	ssize_t n = readlink("/proc/self/exe", exe, sizeof exe - 1);
	char *sl;
	const char *e;

	if (n > 0)
		exe[n] = '\0';
	else
		snprintf(exe, sizeof exe, "%s", argv0);
	if ((sl = strrchr(exe, '/')) != NULL) {
		*sl = '\0';
		snprintf(bindir, sizeof bindir, "%s", exe);
	} else
		snprintf(bindir, sizeof bindir, ".");
	if ((e = getenv("CC11_LIB")) != NULL && *e != '\0')
		snprintf(libdir, sizeof libdir, "%s", e);
	else
		snprintf(libdir, sizeof libdir, "%s/../lib", bindir);
	snprintf(pass0, sizeof pass0, "%s/cc0", bindir);
	snprintf(pass1, sizeof pass1, "%s/cc1", bindir);
	snprintf(pass2c, sizeof pass2c, "%s/c2", bindir);
	snprintf(asprog, sizeof asprog, "%s/as", bindir);
	snprintf(ldprog, sizeof ldprog, "%s/ld", bindir);
	snprintf(pref, sizeof pref, "%s/crt0.o", libdir);
}

int
main(int argc, char **argv)
{
	char *t;
	int nc, nl, i, j, f20, nxo;
	char tmpl[PATH_MAX];
	const char *td;

	i = nc = nl = f20 = nxo = 0;
	findlayout(argv[0]);
	while (++i < argc) {
		if (*argv[i] == '-') {
			switch (argv[i][1]) {
			default:
				goto passa;
			case 'S':
				sflag++;
				cflag++;
				break;
			case 'O':
				oflag++;
				break;
			case 'p':
				proflag++;
				snprintf(pref, sizeof pref, "%s/mcrt0.o", libdir);
				break;
			case 'P':
				pflag++;
			case 'c':
				cflag++;
				break;
			case '2':
				if (argv[i][2] == '\0')
					snprintf(pref, sizeof pref, "%s/crt2.o", libdir);
				else {
					snprintf(pref, sizeof pref, "%s/crt20.o", libdir);
					f20 = 1;
				}
				break;
			case 't':
				break;
			}
		} else {
passa:
			t = argv[i];
			if (getsuf(t) == 'c') {
				if (nc >= NAV - 1)
					goto many;
				clist[nc++] = t;
				t = setsuf(copy(t), 'o');
			}
			if (nodup(llist, t)) {
				if (nl >= NAV - 1)
					goto many;
				llist[nl++] = t;
				if (getsuf(t) == 'o')
					nxo++;
			}
		}
	}
	if (nc == 0)
		goto nocom;
	td = getenv("TMPDIR");
	snprintf(tmpl, sizeof tmpl, "%s/ctmXXXXXX", (td && *td) ? td : "/tmp");
	if (pflag == 0) {
		if (mkdtemp(tmpl) == NULL) {
			perror("cc: temporary directory");
			return 1;
		}
		tmpdir_ = copy(tmpl);
		if (signal(SIGINT, SIG_IGN) != SIG_IGN)
			signal(SIGINT, sigexit);
		snprintf(tmpl, sizeof tmpl, "%s/t1", tmpdir_); tmp1 = copy(tmpl);
		snprintf(tmpl, sizeof tmpl, "%s/t2", tmpdir_); tmp2 = copy(tmpl);
		snprintf(tmpl, sizeof tmpl, "%s/t3", tmpdir_); tmp3 = copy(tmpl);
		snprintf(tmpl, sizeof tmpl, "%s/t4", tmpdir_); tmp4 = copy(tmpl);
		if (oflag) {
			snprintf(tmpl, sizeof tmpl, "%s/t5", tmpdir_);
			tmp5 = copy(tmpl);
		}
	}
	for (i = 0; i < nc; i++) {
		if (nc > 1)
			printf("%s:\n", clist[i]);
		av[0] = (char *)"c0";
		if (pflag)
			tmp4 = setsuf(copy(clist[i]), 'i');
		av[1] = (char *)expand(clist[i]);
		if (pflag || exfail)
			continue;
		if (av[1] == NULL) {
			cflag++;
			continue;
		}
		av[2] = tmp1;
		av[3] = tmp2;
		if (proflag) {
			av[4] = (char *)"-P";
			av[5] = NULL;
		} else
			av[4] = NULL;
		if (callsys(pass0, av)) {
			cflag++;
			errs++;
			continue;
		}
		av[0] = (char *)"c1";
		av[1] = tmp1;
		av[2] = tmp2;
		if (sflag)
			tmp3 = setsuf(copy(clist[i]), 's');
		av[3] = tmp3;
		if (oflag)
			av[3] = tmp5;
		av[4] = NULL;
		if (callsys(pass1, av)) {
			cflag++;
			errs++;
			continue;
		}
		if (oflag) {
			av[0] = (char *)"c2";
			av[1] = tmp5;
			av[2] = tmp3;
			av[3] = NULL;
			callsys(pass2c, av);
			unlink(tmp5);
		}
		if (sflag)
			continue;
		av[0] = (char *)"as";
		av[1] = (char *)"-";
		av[2] = tmp3;
		av[3] = NULL;
		cunlink(tmp1);
		cunlink(tmp2);
		cunlink(tmp4);
		if (callsys(asprog, av))
			errs++;
		t = setsuf(copy(clist[i]), 'o');
		cunlink(t);
		if (link("a.out", t) != 0 || unlink("a.out") != 0) {
			if (rename("a.out", t) != 0) {
				printf("move failed: %s\n", t);
				cflag++;
				errs++;
			}
		}
	}
nocom:
	if (cflag == 0 && nl != 0) {
		i = 0;
		av[0] = (char *)"ld";
		av[1] = (char *)"-x";
		av[2] = pref;
		j = 3;
		while (i < nl)
			av[j++] = llist[i++];
		if (f20) {
			static char l2[PATH_MAX + 8];

			snprintf(l2, sizeof l2, "-l2");
			av[j++] = l2;
		} else {
			av[j++] = (char *)"-lc";
			av[j++] = (char *)"-l";
		}
		av[j++] = NULL;
		if (callsys(ldprog, av))
			errs++;
		if (nc == 1 && nxo == 1) {
			t = setsuf(copy(clist[0]), 'o');
			cunlink(t);
		}
	}
	dexit();
	return 0;
many:
	fprintf(stderr, "cc: too many files\n");
	return 1;
}
