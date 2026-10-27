




























































struct node {
	char	op;
	char	subop;
	struct	node	*forw;
	struct	node	*back;
	struct	node	*ref;
	int	labno;
	char	*code;
	int	refc;
};

struct {
	int	combop;
};

struct optab {
	char	*opstring;
	int	opcode;
} optab[] {
	"jbr",	 1 ,
	"jeq",	 2  |  0 <<8,
	"jne",	 2  |  1 <<8,
	"jle",	 2  |  2 <<8,
	"jge",	 2  |  3 <<8,
	"jlt",	 2  |  4 <<8,
	"jgt",	 2  |  5 <<8,
	"jlo",	 2  |  6 <<8,
	"jhi",	 2  |  7 <<8,
	"jlos",	 2  |  8 <<8,
	"jhis",	 2  |  9 <<8,
	"jmp",	 3 ,
	"/esw",	 6 ,
	"/for",  8 ,
	".globl", 7 ,
	"mov",	 10 ,
	"clr",	 11 ,
	"com",	 12 ,
	"inc",	 13 ,
	"dec",	 14 ,
	"neg",	 15 ,
	"tst",	 16 ,
	"asr",	 17 ,
	"asl",	 18 ,
	"sxt",	 19 ,
	"cmp",	 20 ,
	"add",	 21 ,
	"sub",	 22 ,
	"bit",	 23 ,
	"bic",	 24 ,
	"bis",	 25 ,
	"mul",	 26 ,
	"ash",	 28 ,
	"xor",	 29 ,
	".text", 30 ,
	".data", 31 ,
	".bss",	 32 ,
	".even", 33 ,
	"movf",	 34 ,
	"movof", 35 ,
	"movfo", 36 ,
	"addf",	 37 ,
	"subf",	 38 ,
	"divf",	 39 ,
	"mulf",	 40 ,
	"clrf",	 41 ,
	"cmpf",	 42 ,
	"negf",	 43 ,
	"tstf",	 44 ,
	"cfcc",	 45 ,
	0,	0
};

char	line[512];
struct	node	first;
char	*curlp;
int	nbrbr;
int	nsaddr;
int	redunm;
int	iaftbr;
int	njp1;
int	nrlab;
int	nxjump;
int	ncmot;
int	nrevbr;

int	nchange;
int	isn	20000;

int	debug;
char	*lasta;
char	*lastr;
char	*firstr;

char	revbr[] {  1 ,  0 ,  5 ,  4 ,  3 ,  2 ,  9 ,  8 ,  7 ,  6  };

char	regs[12][20];







struct optab *ophash[ 57 ];
struct { int int;};
struct { char lbyte; };

main(argc, argv)
char **argv;
{
	register int niter, maxiter, isend;
	extern end;
	extern fin, fout;
	int nflag;

	if (argc>1 && argv[1][0]=='+') {
		argc--;
		argv++;
		debug++;
	}
	if (argc>1 && argv[1][0]=='-') {
		argc--;
		argv++;
		nflag++;
	}
	if (argc>1) {
		if ((fin = open(argv[1], 0)) < 0) {
			printf("C2: can't find %s\n", argv[1]);
			exit(1);
		}
	} else
		fin = dup(0);
	if (argc>2) {
		if ((fout = creat(argv[2], 0666)) < 0) {
			fout = 1;
			printf("C2: can't create %s\n", argv[2]);
			exit(1);
		}
	} else
		fout = dup(1);
	lasta = firstr = lastr = sbrk(2);
	maxiter = 0;
	opsetup();
	do {
		isend = input();
		movedat();
		niter = 0;
		do {
			refcount();
			do {
				iterate();
				clearreg();
				niter++;
			} while (nchange);
			rmove();
		} while (jumpsw());
		output();
		if (niter > maxiter)
			maxiter = niter;
		lasta = firstr;
	} while (isend);
	flush();
	fout = 2;
	if (nflag) {
		printf("%d iterations\n", maxiter);
		printf("%d jumps to jumps\n", nbrbr);
		printf("%d inst. after jumps\n", iaftbr);
		printf("%d jumps to .+2\n", njp1);
		printf("%d redundant labels\n", nrlab);
		printf("%d cross-jumps\n", nxjump);
		printf("%d code motions\n", ncmot);
		printf("%d branches reversed\n", nrevbr);
		printf("%d redundant moves\n", redunm);
		printf("%d simplified addresses\n", nsaddr);
		printf("%dK core\n", ((lastr+01777)>>10)&077);
		flush();
	}
}

input()
{
	register struct node *p, *lastp;
	register int op;

	lastp = &first;
	while ((op = getline()) >= 0) {
		switch (op.op) {
	
		case  4 :
			p = alloc(sizeof first);
			if (line[0] == 'L') {
				p->combop =  4 ;
				p->labno = getnum(line+1);
				p->code = 0;
			} else {
				p->combop =  5 ;
				p->labno = 0;
				p->code = copy(line);
			}
			break;
	
		case  1 :
		case  2 :
		case  3 :
		case  9 :
			p = alloc(sizeof first);
			p->combop = op;
			if (*curlp=='L' && (p->labno = getnum(curlp+1)))
				p->code = 0;
			else {
				p->labno = 0;
				p->code = copy(curlp);
			}
			break;

		default:
			p = alloc(sizeof first);
			p->combop = op;
			p->labno = 0;
			p->code = copy(curlp);
			break;

		}
		p->forw = 0;
		p->back = lastp;
		lastp->forw = p;
		lastp = p;
		p->ref = 0;
		if (op== 7 )
			return(1);
	}
	lastp->forw = 0;
	return(0);
}

getline()
{
	register char *lp;
	register c;

	lp = line;
	while (c = getchar()) {
		if (c==':') {
			*lp++ = 0;
			return( 4 );
		}
		if (c=='\n') {
			*lp++ = 0;
			return(oplook());
		}
		*lp++ = c;
	}
	return(-1);
}

getnum(ap)
char *ap;
{
	register char *p;
	register n, c;

	p = ap;
	n = 0;
	while ((c = *p++) >= '0' && c <= '9')
		n = n*10 + c - '0';
	if (*--p != 0)
		return(0);
	return(n);
}

output()
{
	register struct node *t;
	register struct optab *op;
	register int byte;

	t = &first;
	while (t = t->forw) switch (t->op) {

	case  4 :
		printf("L%d:", t->labno);
		continue;

	case  5 :
		printf("%s:", t->code);
		continue;

	default:
		if ((byte = t->subop) ==  100 )
			t->subop = 0;
		for (op = optab; op->opstring!=0; op++) 
			if (op->opcode == t->combop) {
				printf("%s", op->opstring);
				if (byte== 100 )
					printf("b");
				break;
			}
		if (t->code)
			printf("\t%s\n", t->code);
		else if (t->op== 1  || t->op== 2 )
			printf("\tL%d\n", t->labno);
		else
			printf("\n");
		continue;

	case  9 :
		printf("L%d\n", t->labno);
		continue;

	case 0:
		if (t->code)
			printf("%s", t->code);
		printf("\n");
		continue;
	}
}

copy(ap)
char *ap;
{
	register char *p, *np;
	char *onp;
	register n;
	int na;

	na = nargs();
	p = ap;
	n = 0;
	if (*p==0)
		return(0);
	do
		n++;
	while (*p++);
	if (na>1) {
		p = (&ap)[1];
		while (*p++)
			n++;
	}
	onp = np = alloc(n);
	p = ap;
	while (*np++ = *p++);
	if (na>1) {
		p = (&ap)[1];
		np--;
		while (*np++ = *p++);
	}
	return(onp);
}

opsetup()
{
	register struct optab *optp, **ophp;
	register int *p;

	for (optp = optab; p = optp->opstring; optp++) {
		ophp = &ophash[((p[0]+p[1].lbyte)&077777) %  57 ];
		while (*ophp++)
			if (ophp > &ophash[ 57 ])
				ophp = ophash;
		*--ophp = optp;
	}
}

oplook()
{
	register struct optab *optp;
	register char *lp, *op;
	static char tmpop[32];
	struct optab **ophp;

	op = tmpop;
	for (lp = line; *lp && *lp!=' ' && *lp!='\t';)
		*op++ = *lp++;
	*op++ = 0;
	while (*lp=='\t' || *lp==' ')
		lp++;
	curlp = lp;
	ophp = &ophash[((tmpop[0].int+tmpop[2])&077777) %  57 ];
	while (optp = *ophp) {
		op = optp->opstring;
		lp = tmpop;
		while (*lp == *op++)
			if (*lp++ == 0)
				return(optp->opcode);
		if (*lp++=='b' && *lp++==0 && *--op==0)
			return(optp->opcode + ( 100 <<8));
		ophp++;
		if (ophp >= &ophash[ 57 ])
			ophp = ophash;
	}
	if (line[0]=='L') {
		lp = &line[1];
		while (*lp)
			if (*lp<'0' || *lp++>'9')
				return(0);
		curlp = line;
		return( 9 );
	}
	curlp = line;
	return(0);
}

refcount()
{
	register struct node *p, *lp;
	static struct node *labhash[ 127 ];
	register struct node **hp;

	for (hp = labhash; hp < &labhash[ 127 ];)
		*hp++ = 0;
	for (p = first.forw; p!=0; p = p->forw)
		if (p->op== 4 ) {
			labhash[p->labno %  127 ] = p;
			p->refc = 0;
		}
	for (p = first.forw; p!=0; p = p->forw) {
		if (p->op== 1  || p->op== 2  || p->op== 9 ) {
			p->ref = 0;
			lp = labhash[p->labno %  127 ];
			if (lp && p->labno == lp->labno) {
				p->ref = lp;
				lp->refc++;
				continue;
			}
			for (lp=first.forw; lp; lp = lp->forw) {
				if (lp->op== 4  && p->labno==lp->labno) {
					p->ref = lp;
					lp->refc++;
					break;
				}
			}
		}
	}
	for (p = first.forw; p!=0; p = p->forw)
		if (p->refc==0)
			p->refc++;
}

iterate()
{
	register struct node *p, *rp;

	nchange = 0;
	for (p = first.forw; p!=0; p = p->forw) {
		if ((p->op== 1 ||p->op== 2 ||p->op== 9 ) && p->ref) {
			rp = nonlab(p->ref);
			if (rp->op== 1  && rp->labno) {
				nbrbr++;
				p->labno = rp->labno;
				decref(p->ref);
				rp->ref->refc++;
				p->ref = rp->ref;
				nchange++;
			}
		}
		if (p->op== 1 ) {
			while (p->forw && p->forw->op!= 4 ) {
				nchange++;
				iaftbr++;
				if (p->forw->ref)
					decref(p->forw->ref);
				p->forw = p->forw->forw;
				p->forw->back = p;
			}
			rp = p->forw;
			while (rp && rp->op== 4 ) {
				if (p->ref == rp) {
					p->back->forw = p->forw;
					p->forw->back = p->back;
					p = p->back;
					decref(rp);
					nchange++;
					njp1++;
					break;
				}
				rp = rp->forw;
			}
			xjump(p);
			p = codemove(p);
		}
	}
}

xjump(ap)
{
	register int *p1, *p2, *p3;
	int nxj;

	nxj = 0;
	p1 = ap;
	if ((p2 = p1->ref) == 0)
		return(0);
	for (;;) {
		while ((p1 = p1->back) && p1->op== 4 );
		while ((p2 = p2->back) && p2->op== 4 );
		if (!equop(p1, p2))
			return(nxj);
		p3 = alloc(sizeof first);
		p3->combop =  4 ;
		p3->labno = isn;
		p3->ref = 0;
		p3->code = 0;
		p3->refc = 1;
		p3->back = p2->back;
		p3->forw = p2;
		p2->back->forw = p3;
		p2->back = p3;
		p1->combop =  1 ;
		p1->ref = p3;
		p1->labno = isn++;
		p1->code = 0;
		nxj++;
		nxjump++;
		nchange++;
	}
}

codemove(ap)
struct node *ap;
{
	register struct node *p1, *p2, *p3;

	p1 = ap;
	if (p1->op!= 1  || (p2 = p1->ref)==0)
		return(p1);
	while (p2->op ==  4 )
		if ((p2 = p2->back) == 0)
			return(p1);
	if (p2->op!= 1  && p2->op!= 6  && p2->op!= 8 )
		return(p1);
	p2 = p2->forw;
	p3 = p1->ref;
	while (p3) {
		if (p3->op== 1  || p3->op== 6 ) {
			if (p1 == p3)
				return(p1);
			ncmot++;
			nchange++;
			if (p2->back->op== 8 ) {
				p2->back->op =  1 ;
				p2->back->labno = p2->labno;
				p2->back->ref = p2;
				p2->refc++;
				p2->back->code = 0;
			}
			p1->back->forw = p2;
			p1->forw->back = p3;
			p2->back->forw = p3->forw;
			p3->forw->back = p2->back;
			p2->back = p1->back;
			p3->forw = p1->forw;
			decref(p1->ref);
			return(p2);
		} else
			p3 = p3->forw;
	}
	return(p1);
}

rmove()
{
	register struct node *p;
	register char *cp;
	register int r;
	int r1, flt;

	for (p=first.forw; p!=0; p = p->forw) {
	if (debug) {
		for (r=0; r<2* 5 ; r++)
			if (regs[r][0])
				printf("%d: %s\n", r, regs[r]);
		printf("-\n");
	}
	flt = 0;
	switch (p->op) {

	case  34 :
	case  36 :
	case  35 :
		flt =  5 ;

	case  10 :
		dualop(p);
		if ((r = findrand(regs[ 10 ], flt)) >= 0) {
			if (r == flt+isreg(regs[ 11 ]) && p->forw->op!= 2 ) {
				p->forw->back = p->back;
				p->back->forw = p->forw;
				redunm++;
				continue;
			}
		}
		repladdr(p, 0, flt);
		r = isreg(regs[ 10 ]);
		r1 = isreg(regs[ 11 ]);
		dest(regs[ 11 ], flt);
		if (r >= 0)
			if (r1 >= 0)
				savereg(r1+flt, regs[r+flt]);
			else
				savereg(r+flt, regs[ 11 ]);
		else
			if (r1 >= 0)
				savereg(r1+flt, regs[ 10 ]);
		source(regs[ 10 ]);
		continue;

	case  37 :
	case  38 :
	case  39 :
	case  40 :
		flt =  5 ;

	case  21 :
	case  22 :
	case  24 :
	case  25 :
	case  26 :
	case  27 :
	case  28 :
		dualop(p);
		repladdr(p, 0, flt);
		source(regs[ 10 ]);
		dest(regs[ 11 ], flt);
		if (p->op== 27  && (r = isreg(regs[ 11 ])>=0))
			regs[r+1][0] = 0;
		continue;

	case  41 :
	case  43 :
		flt =  5 ;

	case  11 :
	case  12 :
	case  13 :
	case  14 :
	case  15 :
	case  17 :
	case  18 :
	case  19 :
		singop(p);
		dest(regs[ 10 ], flt);
		continue;

	case  44 :
		flt =  5 ;

	case  16 :
		singop(p);
		repladdr(p, 0, flt);
		source(regs[ 10 ]);
		continue;

	case  42 :
		flt =  5 ;

	case  20 :
	case  23 :
		dualop(p);
		source(regs[ 10 ]);
		source(regs[ 11 ]);
		repladdr(p, 1, flt);
		continue;

	case  2 :
	case  45 :
		continue;

	default:
		clearreg();
	}
	}
}

jumpsw()
{
	register struct node *p, *p1;
	register t;
	int nj;

	t = 0;
	nj = 0;
	for (p=first.forw; p!=0; p = p->forw)
		p->refc = ++t;
	for (p=first.forw; p!=0; p = p1) {
		p1 = p->forw;
		if (p->op ==  2  && p1->op== 1  && p->ref && p1->ref
		 && abs(p->refc - p->ref->refc) > abs(p1->refc - p1->ref->refc)) {
			p->subop = revbr[p->subop];
			t = p1->ref;
			p1->ref = p->ref;
			p->ref = t;
			t = p1->labno;
			p1->labno = p->labno;
			p->labno = t;
			nrevbr++;
			nj++;
		}
	}
	return(nj);
}

abs(x)
{
	return(x<0? -x: x);
}

equop(ap1, p2)
struct node *ap1, *p2;
{
	register char *cp1, *cp2;
	register struct node *p1;

	p1 = ap1;
	if (p1->combop != p2->combop)
		return(0);
	if (p1->op>0 && p1->op< 10 )
		return(0);
	cp1 = p1->code;
	cp2 = p2->code;
	if (cp1==0 && cp2==0)
		return(1);
	if (cp1==0 || cp2==0)
		return(0);
	while (*cp1 == *cp2++)
		if (*cp1++ == 0)
			return(1);
	return(0);
}

decref(ap)
{
	register struct node *p;

	p = ap;
	if (--p->refc <= 0) {
		nrlab++;
		p->back->forw = p->forw;
		p->forw->back = p->back;
	}
}

nonlab(ap)
struct node *ap;
{
	register struct node *p;

	p = ap;
	while (p && p->op== 4 )
		p = p->forw;
	return(p);
}

alloc(an)
{
	register int n;
	register char *p;

	n = an;
	n++;
	n =& ~01;
	if (lasta+n >= lastr) {
		if (sbrk(2000) == -1) {
			write(2, "Out of space\n", 14);
			exit(1);
		}
		lastr =+ 2000;
	}
	p = lasta;
	lasta =+ n;
	return(p);
}

clearreg()
{
	register int i;

	for (i=0; i<2* 5 ; i++)
		regs[i][0] = '\0';
}

savereg(ai, as)
char *as;
{
	register char *p, *s;

	p = regs[ai];
	s = as;
	if (source(s))
		return;
	while (*p++ = *s)
		if (*s++ == ',')
			break;
	*--p = '\0';
}

dest(as, flt)
char *as;
{
	register char *s;
	register int i;

	s = as;
	if ((i = isreg(s)) >= 0)
		regs[i+flt][0] = 0;
	while ((i = findrand(s, flt)) >= 0)
		regs[i][0] = 0;
	while (*s) {
		if ((*s=='(' && (*(s+1)!='r' || *(s+2)!='5')) || *s++=='*') {
			for (i=flt; i<flt+ 5 ; i++)
				if (regs[i][0] != '$')
					regs[i][0] = 0;
			return;
		}
	}
}

singop(ap)
struct node *ap;
{
	register char *p1, *p2;

	p1 = ap->code;
	p2 = regs[ 10 ];
	while (*p2++ = *p1++);
	regs[ 11 ][0] = 0;
}


dualop(ap)
struct node *ap;
{
	register char *p1, *p2;
	register struct node *p;

	p = ap;
	p1 = p->code;
	p2 = regs[ 10 ];
	while (*p1 && *p1!=',')
		*p2++ = *p1++;
	*p2++ = 0;
	p2 = regs[ 11 ];
	*p2 = 0;
	if (*p1++ !=',')
		return;
	while (*p2++ = *p1++);
}

findrand(as, flt)
char *as;
{
	register char *s;
	register int i;
	register char *p;

	for (i = flt; i< 5 +flt; i++) {
		p = regs[i];
		s = as;
		while (*s == *p++)
			if (*s++ == 0)
				return(i);
	}
	return(-1);
}

isreg(as)
char *as;
{
	register char *s;

	s = as;
	if (s[0]=='r' && s[1]>='0' && s[1]<='4' && s[2]==0)
		return(s[1]-'0');
	return(-1);
}

check()
{
	register struct node *p, *lp;

	lp = &first;
	for (p=first.forw; p!=0; p = p->forw) {
		if (p->back != lp)
			abort();
		lp = p;
	}
}

source(ap)
char *ap;
{
	register char *p1, *p2;

	p1 = ap;
	p2 = p1;
	if (*p1==0)
		return(0);
	while (*p2++);
	if (*p1=='-' && *(p1+1)=='('
	 || *p1=='*' && *(p1+1)=='-' && *(p1+2)=='('
	 || *(p2-2)=='+') {
		while (*p1 && *p1++!='r');
		if (*p1>='0' && *p1<='4')
			regs[*p1 - '0'][0] = 0;
		return(1);
	}
	return(0);
}

repladdr(p, f, flt)
struct node *p;
{
	register r;
	int r1;
	register char *p1, *p2;
	static char rt1[50], rt2[50];

	if (f)
		r1 = findrand(regs[ 11 ], flt);
	else
		r1 = -1;
	r = findrand(regs[ 10 ], flt);
	if (r1 >=  5 )
		r1 =-  5 ;
	if (r >=  5 )
		r =-  5 ;
	if (r>=0 || r1>=0) {
		p2 = regs[ 10 ];
		for (p1 = rt1; *p1++ = *p2++;);
		if (regs[ 11 ][0]) {
			p1 = rt2;
			*p1++ = ',';
			for (p2 = regs[ 11 ]; *p1++ = *p2++;);
		} else
			rt2[0] = 0;
		if (r>=0) {
			rt1[0] = 'r';
			rt1[1] = r + '0';
			rt1[2] = 0;
			nsaddr++;
		}
		if (r1>=0) {
			rt2[1] = 'r';
			rt2[2] = r1 + '0';
			rt2[3] = 0;
			nsaddr++;
		}
		p->code = copy(rt1, rt2);
	}
}

movedat()
{
	register struct node *p1, *p2;
	struct node *p3;
	register seg;
	struct node data;
	struct node *datp;

	if (first.forw == 0)
		return;
	datp = &data;
	for (p1 = first.forw; p1!=0; p1 = p1->forw) {
		if (p1->op ==  31 ) {
			p2 = p1->forw;
			while (p2 && p2->op!= 30 )
				p2 = p2->forw;
			if (p2==0)
				break;
			p3 = p1->back;
			p1->back->forw = p2->forw;
			p2->forw->back = p3;
			p2->forw = 0;
			datp->forw = p1;
			p1->back = datp;
			p1 = p3;
			datp = p2;
		}
	}
	if (data.forw) {
		datp->forw = first.forw;
		first.forw->back = datp;
		data.forw->back = &first;
		first.forw = data.forw;
	}
	seg = -1;
	for (p1 = first.forw; p1!=0; p1 = p1->forw) {
		if (p1->op== 30 |p1->op== 31 ||p1->op== 32 ) {
			if (p1->op == seg || p1->forw&&p1->forw->op==seg) {
				p1->back->forw = p1->forw;
				p1->forw->back = p1->back;
				p1 = p1->back;
				continue;
			}
			seg = p1->op;
		}
	}
}
