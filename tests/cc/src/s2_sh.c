




























char	*dolp;
char	**dolv;
int	dolc;
char	*promp;
char	*linep;
char	*elinep;
char	**argp;
char	**eargp;
int	*treep;
char	peekc;
char	gflg;
char	error;
char	acctf;
char	uid;
char	setintr;

char	*mesg[] {
	0,
	"Hangup",
	0,
	"Quit",
	"Illegal instruction",
	"Trace/BPT trap",
	"IOT trap",
	"EMT trap",
	"Floating exception",
	"Killed",
	"Bus error",
	"Memory fault",
	"Bad system call"
};

struct stime {
	int proct[2];
	int cputim[2];
	int systim[2];
} timeb;

main(c, av)
int c;
char **av;
{
	register f;
	register char *acname, **v;

	close(2);
	if((f=dup(1)) != 2)
		close(f);
	v = av;
	acname = "/usr/adm/sh_acct";
	promp = "% ";
	if(((uid = getuid())&0377) == 0) {
		promp = "# ";
		acname = "/usr/adm/su_acct";
	}
	acctf = open(acname, 1);
	if(c > 1) {
		promp = 0;
		close(0);
		f = open(v[1], 0);
		if(f < 0) {
			prs(v[1]);
			err(": cannot open");
		}
	}
	if(**v == '-') {
		setintr++;
		signal( 3 , 1);
		signal( 2 , 1);
	}
	dolv = v+1;
	dolc = c-1;

loop:
	if(promp != 0)
		prs(promp);
	peekc = getc();
	main1();
	goto loop;
}

main1()
{
	char line[ 1000 ];
	char *args[ 50 ];
	int trebuf[ 50 ];
	register char c, *cp;
	register *t;

	argp = args;
	eargp = args+ 50 -5;
	linep = line;
	elinep = line+ 1000 -5;
	error = 0;
	gflg = 0;
	do {
		cp = linep;
		word();
	} while(*cp != '\n');
	treep = trebuf;
	if(gflg == 0) {
		if(error == 0)
			t = syntax(args, argp);
		if(error != 0)
			err("syntax error"); else
			execute(t);
	}
}

word()
{
	register char c, c1;

	*argp++ = linep;

loop:
	switch(c = getc()) {

	case ' ':
	case '\t':
		goto loop;

	case '\'':
	case '"':
		c1 = c;
		while((c=readc()) != c1) {
			if(c == '\n') {
				error++;
				peekc = c;
				return;
			}
			*linep++ = c| 0200 ;
		}
		goto pack;

	case '&':
	case ';':
	case '<':
	case '>':
	case '(':
	case ')':
	case '|':
	case '^':
	case '\n':
		*linep++ = c;
		*linep++ = '\0';
		return;
	}

	peekc = c;

pack:
	for(;;) {
		c = getc();
		if(any(c, " '\"\t;&<>()|^\n")) {
			peekc = c;
			if(any(c, "\"'"))
				goto loop;
			*linep++ = '\0';
			return;
		}
		*linep++ = c;
	}
}

tree(n)
int n;
{
	register *t;

	t = treep;
	treep =+ n;
	return(t);
}

getc()
{
	register char c;

	if(peekc) {
		c = peekc;
		peekc = 0;
		return(c);
	}
	if(argp > eargp) {
		argp =- 10;
		while((c=getc()) != '\n');
		argp =+ 10;
		err("Too many args");
		gflg++;
		return(c);
	}
	if(linep > elinep) {
		linep =- 10;
		while((c=getc()) != '\n');
		linep =+ 10;
		err("Too many characters");
		gflg++;
		return(c);
	}
getd:
	if(dolp) {
		c = *dolp++;
		if(c != '\0')
			return(c);
		dolp = 0;
	}
	c = readc();
	if(c == '\\') {
		c = readc();
		if(c == '\n')
			return(' ');
		return(c| 0200 );
	}
	if(c == '$') {
		c = getc();
		if(c>='0' && c<='9') {
			if(c-'0' < dolc)
				dolp = dolv[c-'0'];
			goto getd;
		}
	}
	return(c&0177);
}

readc()
{
	int c;

	if(read(0, &c, 1) != 1)
		exit();
	return(c&0377);
}







syntax(p1, p2)
char **p1, **p2;
{

	while(p1 != p2) {
		if(any(**p1, ";&\n"))
			p1++; else
			return(syn1(p1, p2));
	}
	return(0);
}








syn1(p1, p2)
char **p1, **p2;
{
	register char **p;
	register *t, *t1;
	int l;

	l = 0;
	for(p=p1; p!=p2; p++)
	switch(**p) {

	case '(':
		l++;
		continue;

	case ')':
		l--;
		if(l < 0)
			error++;
		continue;

	case '&':
	case ';':
	case '\n':
		if(l == 0) {
			t = tree(4);
			t[ 0 ] =  4 ;
			t[ 1 ] = syn2(p1, p);
			t[ 3 ] = 0;
			if(**p == '&') {
				t1 = t[ 1 ];
				t1[ 3 ] =|  1 | 32 ;
			}
			t[ 2 ] = syntax(p+1, p2);
			return(t);
		}
	}
	if(l == 0)
		return(syn2(p1, p2));
	error++;
}







syn2(p1, p2)
char **p1, **p2;
{
	char **p;
	int l, *t;

	l = 0;
	for(p=p1; p!=p2; p++)
	switch(**p) {

	case '(':
		l++;
		continue;

	case ')':
		l--;
		continue;

	case '|':
	case '^':
		if(l == 0) {
			t = tree(4);
			t[ 0 ] =  3 ;
			t[ 1 ] = syn3(p1, p);
			t[ 2 ] = syn2(p+1, p2);
			t[ 3 ] = 0;
			return(t);
		}
	}
	return(syn3(p1, p2));
}







syn3(p1, p2)
char **p1, **p2;
{
	register char **p;
	char **lp, **rp;
	register *t;
	int b[100], n, l, i, o, c, flg;

	flg = 0;
	if(**p2 == ')')
		flg =|  16 ;
	lp = 0;
	rp = 0;
	i = 0;
	o = 0;
	n = 0;
	l = 0;
	for(p=p1; p!=p2; p++)
	switch(c = **p) {

	case '(':
		if(l == 0) {
			if(lp != 0)
				error++;
			lp = p+1;
		}
		l++;
		continue;

	case ')':
		l--;
		if(l == 0)
			rp = p;
		continue;

	case '>':
		p++;
		if(p!=p2 && **p=='>')
			flg =|  2 ; else
			p--;

	case '<':
		if(l == 0) {
			p++;
			if(p == p2) {
				error++;
				p--;
			}
			if(any(**p, "<>("))
				error++;
			if(c == '<') {
				if(i != 0)
					error++;
				i = *p;
				continue;
			}
			if(o != 0)
				error++;
			o = *p;
		}
		continue;

	default:
		if(l == 0)
			b[n++] = *p;
	}
	if(lp != 0) {
		if(n != 0)
			error++;
		t = tree(5);
		t[ 0 ] =  2 ;
		t[ 4 ] = syn1(lp, rp);
		goto out;
	}
	if(n == 0)
		error++;
	b[n++] = 0;
	t = tree(n+5);
	t[ 0 ] =  1 ;
	for(l=0; l<n; l++)
		t[l+ 5 ] = b[l];
out:
	t[ 3 ] = flg;
	t[ 1 ] = i;
	t[ 2 ] = o;
	return(t);
}

scan(t, f)
int *t;
int (*f)();
{
	register char *p, c;

	t =+  5 ;
	while(p = *t++)
		while(c = *p)
			*p++ = (*f)(c);
}

tglob(c)
int c;
{

	if(any(c, "[?*"))
		gflg = 1;
	return(c);
}

trim(c)
int c;
{

	return(c&0177);
}

execute(t, pf1, pf2)
int *t, *pf1, *pf2;
{
	int i, f, pv[2];
	register *t1;
	register char *cp1, *cp2;

	if(t != 0)
	switch(t[ 0 ]) {

	case  1 :
		cp1 = t[ 5 ];
		if(equal(cp1, "chdir")) {
			if(t[ 5 +1] != 0) {
				if(chdir(t[ 5 +1]) < 0)
					err("chdir: bad directory");
			} else
				err("chdir: arg count");
			return;
		}
		if(equal(cp1, "shift")) {
			if(dolc < 1) {
				prs("shift: no args\n");
				return;
			}
			dolv[1] = dolv[0];
			dolv++;
			dolc--;
			return;
		}
		if(equal(cp1, "login")) {
			if(promp != 0) {
				close(acctf);
				execv("/bin/login", t+ 5 );
			}
			prs("login: cannot execute\n");
			return;
		}
		if(equal(cp1, "wait")) {
			pwait(-1, 0);
			return;
		}
		if(equal(cp1, ":"))
			return;

	case  2 :
		f = t[ 3 ];
		i = 0;
		if((f& 16 ) == 0)
			i = fork();
		if(i == -1) {
			err("try again");
			return;
		}
		if(i != 0) {
			if((f& 4 ) != 0) {
				close(pf1[0]);
				close(pf1[1]);
			}
			if((f& 1 ) != 0) {
				prn(i);
				prs("\n");
				return;
			}
			if((f& 8 ) == 0)
				pwait(i, t);
			return;
		}
		if(t[ 1 ] != 0) {
			close(0);
			i = open(t[ 1 ], 0);
			if(i < 0) {
				prs(t[ 1 ]);
				err(": cannot open");
				exit();
			}
		}
		if(t[ 2 ] != 0) {
			if((f& 2 ) != 0) {
				i = open(t[ 2 ], 1);
				if(i >= 0) {
					seek(i, 0, 2);
					goto f1;
				}
			}
			i = creat(t[ 2 ], 0666);
			if(i < 0) {
				prs(t[ 2 ]);
				err(": cannot create");
				exit();
			}
		f1:
			close(1);
			dup(i);
			close(i);
		}
		if((f& 4 ) != 0) {
			close(0);
			dup(pf1[0]);
			close(pf1[0]);
			close(pf1[1]);
		}
		if((f& 8 ) != 0) {
			close(1);
			dup(pf2[1]);
			close(pf2[0]);
			close(pf2[1]);
		}
		if((f& 32 )!=0 && t[ 1 ]==0 && (f& 4 )==0) {
			close(0);
			open("/dev/null", 0);
		}
		if((f& 32 ) == 0 && setintr) {
			signal( 2 , 0);
			signal( 3 , 0);
		}
		if(t[ 0 ] ==  2 ) {
			if(t1 = t[ 4 ])
				t1[ 3 ] =| f& 32 ;
			execute(t1);
			exit();
		}
		close(acctf);
		gflg = 0;
		scan(t, &tglob);
		if(gflg) {
			t[ 4 ] = "/etc/glob";
			execv(t[ 4 ], t+ 4 );
			prs("glob: cannot execute\n");
			exit();
		}
		scan(t, &trim);
		execv(t[ 5 ], t+ 5 );
		cp1 = linep;
		cp2 = "/usr/bin/";
		while(*cp1 = *cp2++)
			cp1++;
		cp2 = t[ 5 ];
		while(*cp1++ = *cp2++);
		execv(linep+4, t+ 5 );
		execv(linep, t+ 5 );
		if(runcom()) {
			t[ 4 ] = "/bin/sh";
			t[ 5 ] = linep;
			execv(t[ 4 ], t+ 4 );
		}
		prs(t[ 5 ]);
		err(": not found");
		exit();

	case  3 :
		f = t[ 3 ];
		pipe(pv);
		t1 = t[ 1 ];
		t1[ 3 ] =|  8  | (f&( 4 | 32 ));
		execute(t1, pf1, pv);
		t1 = t[ 2 ];
		t1[ 3 ] =|  4  | (f&( 8 | 32 | 1 ));
		execute(t1, pv, pf2);
		return;

	case  4 :
		f = t[ 3 ]& 32 ;
		if(t1 = t[ 1 ])
			t1[ 3 ] =| f;
		execute(t1);
		if(t1 = t[ 2 ])
			t1[ 3 ] =| f;
		execute(t1);
		return;

	}
}

err(s)
char *s;
{

	prs(s);
	prs("\n");
	if(promp == 0) {
		seek(0, 0, 2);
		exit();
	}
}

prs(s)
char *s;
{

	while(*s)
		putc(*s++);
}

putc(c)
{

	write(2, &c, 1);
}

prn(n)
int n;
{
	register a;

	if(a=ldiv(0,n,10))
		prn(a);
	putc(lrem(0,n,10)+'0');
}

any(c, s)
int c;
char *s;
{

	while(*s)
		if(*s++ == c)
			return(1);
	return(0);
}

equal(s1, s2)
char *s1, *s2;
{

	while(*s1++ == *s2)
		if(*s2++ == '\0')
			return(1);
	return(0);
}

pwait(i, t)
int i, *t;
{
	register p, e;
	int s;

	if(i != 0)
	for(;;) {
		times(&timeb);
		time(timeb.proct);
		p = wait(&s);
		if(p == -1)
			break;
		e = s&0177;
		if(mesg[e] != 0) {
			if(p != i) {
				prn(p);
				prs(": ");
			}
			prs(mesg[e]);
			if(s&0200)
				prs(" -- Core dumped");
		}
		if(e != 0)
			err("");
		if(i == p) {
			acct(t);
			break;
		} else
			acct(0);
	}
}

acct(t)
int *t;
{
	if(t == 0)
		enacct("**gok"); else
	if(*t ==  2 )
		enacct("()"); else
	enacct(t[ 5 ]);
}

enacct(s)
char *s;
{
	struct stime timbuf;
	struct {
		char cname[14];
		int realt;
		int bcput;
		int bsyst;
	} tbuf;
	register i;
	register char *np;

	times(&timbuf);
	time(timbuf.proct);
	tbuf.realt = timbuf.proct[1] -
		timeb.proct[1];
	tbuf.bcput = timbuf.cputim[1] -
		timeb.cputim[1];
	tbuf.bsyst = timbuf.systim[1] -
		timeb.systim[1];
	do {
		np = s;
		while (*s != '\0' && *s != '/')
			s++;
	} while (*s++ != '\0');
	for (i=0; i<13; i++) {
		tbuf.cname[i] = *np;
		if (*np)
			np++;
	}
	tbuf.cname[13] = uid;
	seek(acctf, 0, 2);
	write(acctf, &tbuf, 20);
}

runcom()
{
	int buf[40];

	if(stat(linep, buf) < 0)
		return(0);
	if((buf[2]&060111) != 0)
		return(0);
	return(1);
}
