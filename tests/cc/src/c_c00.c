

























struct tnode {
	int op;
	int type;
	int dimp;
	struct tnode *tr1, *tr2;
};

struct {
	int fop;
	int ftype;
	char ssp;		
	char lenp;		
};

struct tname {
	int nop;
	int ntype;
	int ndimp;
	int class;
	int offset;
	int nloc;
};


struct tconst {
	int cop;
	int ctype;
	int cdimp;
	int value;
};

struct hshtab {
	int hclass;
	int htype;
	int hdimp;
	int hoffset;
	char name[ 8 ];
};

struct swtab {
	int swlab;
	int swval;
};

char	cvtab[];
int	opdope[];
char	ctab[];
char	symbuf[ 8 ];
int	hshused;
struct	hshtab	hshtab[ 200 ];
int	*space;
int	*cp;
int	cmst[ 40 ];
int	isn;
struct	swtab	swtab[ 200 ];
struct	swtab	*swp;
int	contlab;
int	brklab;
int	retlab;
int	deflab;
int	nauto;
int	autolen;
int	peeksym;
int	peekc;
int	eof;
int	line;
int	*treebase;
int	debug;
struct	hshtab	*defsym;
struct	hshtab	*funcsym;
int	xdflg;
int	proflg;
struct	hshtab	*csym;
int	cval;
double	fcval;
int	nchstr;
int	nerror;
struct	hshtab	*paraml;
struct	hshtab	*parame;
int	strflg;
int	osleft;
int	mosflg;
int	initflg;
int	inhdr;
int	dimtab[ 100 ];
char	binbuf[518];
char	ascbuf[518];
int	dimp;
int	regvar;















































































































































int	isn 1;
int	peeksym -1;
int	line 1;
int	debug 0;
int	dimp	0;

struct kwtab {
	char	*kwname;
	int	kwval;
} kwtab[]
{
	"int",		 0 ,
	"char",		 1 ,
	"float",	 2 ,
	"double",	 3 ,
	"struct",	 4 ,
	"auto",		 5 ,
	"extern",	 6 ,
	"static",	 7 ,
	"register",	 8 ,
	"goto",		 10 ,
	"return",	 11 ,
	"if",		 12 ,
	"while",	 13 ,
	"else",		 14 ,
	"switch",	 15 ,
	"case",		 16 ,
	"break",	 17 ,
	"continue",	 18 ,
	"do",		 19 ,
	"default",	 20 ,
	"for",		 21 ,
	"sizeof",	 29 ,
	0,		0,
};

main(argc, argv)
char *argv[];
{
	extern fin;
	int treespace[ 500 ];
	register char *sp, *np;
	register struct kwtab *ip;

	if(argc<4) {
		error("Arg count");
		exit(1);
	}
	if((fin=open(argv[1],0))<0) {
		error("Can't find %s", argv[1]);
		exit(1);
	}
	if (fcreat(argv[2], ascbuf)<0 || fcreat(argv[3], binbuf)<0) {
		error("Can't create temp");
		exit(1);
	}
	if (argc>4)
		proflg++;
	xdflg++;
	for (ip=kwtab; (np = ip->kwname); ip++) {
		for (sp = symbuf; sp<symbuf+ 8 ;)
			if ((*sp++ = *np++) == '\0')
				np--;
		np = lookup();
		np->hclass =  1 ;
		np->htype = ip->kwval;
	}
	xdflg = 0;
	treebase = treespace+10;
	putw(treebase, binbuf);
	while(!eof) {
		extdef();
		blkend();
	}
	fflush(ascbuf);
	fflush(binbuf);
	exit(nerror!=0);
}

struct hshtab *lookup()
{
	int ihash;
	register struct hshtab *rp;
	register char *sp, *np;

	ihash = 0;
	for (sp=symbuf; sp<symbuf+ 8 ;)
		ihash =+ *sp++ & 0177;
	rp = &hshtab[ihash% 200 ];
	while (*(np = rp->name)) {
		for (sp=symbuf; sp<symbuf+ 8 ;)
			if ((*np++&0177) != *sp++)
				goto no;
		return(rp);
	no:
		if (++rp >= &hshtab[ 200 ])
			rp = hshtab;
	}
	if(++hshused >=  200 ) {
		error("Symbol table overflow");
		exit(1);
	}
	rp->hclass = 0;
	rp->htype = 0;
	rp->hoffset = 0;
	rp->dimp = 0;
	sp = symbuf;
	if (xdflg)
		*sp =| 0200;
	for (np=rp->name; sp<symbuf+ 8 ;)
		*np++ = *sp++;
	return(rp);
}

symbol() {
	register c;
	register char *sp;

	if (peeksym>=0) {
		c = peeksym;
		peeksym = -1;
		if (c== 20 )
			mosflg = 0;
		return(c);
	}
	if (peekc) {
		c = peekc;
		peekc = 0;
	} else
		if (eof)
			return( 0 );
		else
			c = getchar();
loop:
	switch(ctab[c]) {

	case  119 :		
		inhdr = 1;
		c = getchar();
		goto loop;

	case  125 :
		if (!inhdr)
			line++;
		inhdr = 0;

	case  126 :
		c = getchar();
		goto loop;

	case  0 :
		eof++;
		return(0);

	case  40 :
		return(subseq(c, 40 , 30 ));

	case  41 :
		return(subseq(c,subseq('>', 41 , 50 ), 31 ));

	case  80 :
		if (subseq(' ',0,1)) return( 80 );
		c = symbol();
		if (c>= 40  && c<= 49 ) {
			if (peekc==0)
				peekc = getchar();
			if (ctab[peekc] !=  126  
			 && (c== 41  || c== 47  || c== 42 )) {
				error("Warning: assignment understood");
				nerror--;
			}
			return(c+30);
		}
		if (c== 80 )
			return( 60 );
		peeksym = c;
		return( 80 );

	case  63 :
		if (subseq(c,0,1)) return( 46 );
		return(subseq('=', 63 , 62 ));

	case  65 :
		if (subseq(c,0,1)) return( 45 );
		return(subseq('=', 65 , 64 ));

	case  34 :
		return(subseq('=', 34 , 61 ));

	case  43 :
		if (subseq('*',1,0))
			return( 43 );
com:
		c = getchar();
com1:
		switch(c) {
		case '\0':
			eof++;
			error("Nonterminated comment");
			return(0);
		case '\n':
			if (!inhdr)
				line++;
			inhdr = 0;
			goto com;
		case 001:		
			inhdr++;
		default:
			goto com;
		case '*':
			c = getchar();
			if (c!='/')
				goto com1;
		}
		c = getchar();
		goto loop;

	case  120 :
	case  124 :
		peekc = c;
		if ((c=getnum(c=='0'?8:10)) ==  23 )
			cval = isn++;
		return(c);

	case  122 :
		return(getstr());

	case  121 :
		return(getcc());

	case  123 :
		sp = symbuf;
		if (mosflg) {
			*sp++ = '.';
			mosflg = 0;
		}
		while(ctab[c]== 123  || ctab[c]== 124 ) {
			if (sp<symbuf+ 8 ) *sp++ = c;
			c = getchar();
		}
		while(sp<symbuf+ 8 )
			*sp++ = '\0';
		peekc = c;
		csym = lookup();
		if (csym->hclass== 1 ) {
			if (csym->htype== 29 )
				return( 29 );
			cval = csym->htype;
			return( 19 );
		}
		return( 20 );

	case  47 :
		return(subseq('&',  47 ,  53 ));

	case  48 :
		return(subseq('|',  48 ,  54 ));

	case  127 :
		error("Unknown character");
		c = getchar();
		goto loop;

	}
	return(ctab[c]);
}

subseq(c,a,b) {
	if (!peekc)
		peekc = getchar();
	if (peekc != c)
		return(a);
	peekc = 0;
	return(b);
}

getstr() {
	register int c;
	register char *t, *d;

	nchstr = 1;
	t = ".text";
	d = ".data";
	printf("%s\nL%d:.byte ", (strflg?t:d), cval=isn++);
	while((c=mapch('"')) >= 0) {
		printf("0%o,", c);
		nchstr++;
	}
	printf("0\n.even\n%s\n", (strflg?d:t));
	return( 22 );
}

getcc()
{
	register int c, cc;
	register char *ccp;

	cval = 0;
	ccp = &cval;
	cc = 0;
	while((c=mapch('\'')) >= 0)
		if(cc++ <  2 )
			*ccp++ = c;
	if(cc> 2 )
		error("Long character constant");
	return( 21 );
}

mapch(ac)
{
	register int a, c, n;
	static mpeek;

	c = ac;
	if (mpeek) {
		a = mpeek;
		mpeek = 0;
	} else
		a = getchar();
loop:
	if (a==c)
		return(-1);
	switch(a) {

	case '\n':
	case '\0':
		error("Nonterminated string");
		peekc = a;
		return(-1);

	case '\\':
		switch (a=getchar()) {

		case 't':
			return('\t');

		case 'n':
			return('\n');

		case 'b':
			return('\b');

		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			n = 0;
			while (n<0100 && '0'<=a && a<='7') {
				n =<< 3;
				n =+ a-'0';
				a = getchar();
			}
			mpeek = a;
			return(n);

		case 'r':
			return('\r');

		case '\n':
			if (!inhdr)
				line++;
			inhdr = 0;
			a = getchar();
			goto loop;
		}
	}
	return(a);
}

tree()
{


	int *op, opst[ 20 ], *pp, prst[ 20 ];
	register int andflg, o;
	register struct hshtab *cs;
	int p, ps, os, *np;

	osleft =  500 ;
	space = treebase;
	op = opst;
	pp = prst;
	cp = cmst;
	*op =  200 ;
	*pp = 06;
	andflg = 0;

advanc:
	switch (o=symbol()) {

	case  20 :
		cs = csym;
		if (cs->hclass==0 && cs->htype==0)
			if(nextchar()=='(') {
				
				cs->hclass =  6 ;
				cs->htype =  020 ;
			} else if (initflg)
				cs->hclass =  6 ;
			else {
				
				cs->htype =  030 ;
				if (cs->hoffset==0)
					cs->hoffset = isn++;
			}
		*cp++ = block(2, 20 ,cs->htype,cs->hdimp,
		    cs->hclass,0);
		if (cs->hclass== 6 ) {
			np = cs->name;
			for (o=0; o<4; o++) {
				pblock(*np);
				if (((*np++)&~0177) == 0)
					break;
			}
		} else
			pblock(cs->hoffset);
		goto tand;

	case  23 :
		if (!initflg)
			printf(".data\nL%d:0%o;0%o;0%o;0%o\n.text\n",cval,fcval);

	case  21 :
	case  24 :
		*cp++ = block(1,o,(o== 21 ? 0 : 3 ),0,cval);
		goto tand;

	
	case  22 :
		*cp++ = block(3,  20 ,  030 + 1 ,0, 7 ,0,cval);

tand:
		if(cp>=cmst+ 40 ) {
			error("Expression overflow");
			exit(1);
		}
		if (andflg)
			goto syntax;
		andflg = 1;
		goto advanc;

	case  30 :
	case  31 :
		if (andflg)
			o =+ 2;
		goto oponst;

	case  38 :
	case  34 :
	case  29 :
		if (andflg)
			goto syntax;
		goto oponst;

	case  41 :
		if (!andflg)  {
			if ((peeksym=symbol())== 23 ) {
				fcval = - fcval;
				goto advanc;
			}
			if (peeksym== 24 ) {
				fcval = - fcval;
				cval =^ 0100000;
				goto advanc;
			}
			o =  37 ;
		}
		andflg = 0;
		goto oponst;

	case  47 :
	case  42 :
		if (andflg)
			andflg = 0; else
			if(o== 47 )
				o =  35 ;
			else
				o =  36 ;
		goto oponst;

	case  6 :
		if (andflg) {
			o = symbol();
			if (o== 7 )
				o =  101 ;
			else {
				peeksym = o;
				o =  100 ;
				andflg = 0;
			}
		}
		goto oponst;

	case  5 :
	case  7 :
		if (!andflg)
			goto syntax;
		goto oponst;

	case  39 :
	case  50 :
		mosflg++;
		break;

	}
	
	if (!andflg)
		goto syntax;
	andflg = 0;

oponst:
	p = (opdope[o]>>9) & 077;
	if ((o== 9  || o== 8 ) && initflg)
		p = 05;
opon1:
	ps = *pp;
	if (p>ps || p==ps && (opdope[o]& 0200 )!=0) {
		switch (o) {

		case  32 :
		case  33 :
			p = 37;
			break;
		case  6 :
		case  4 :
		case  100 :
			p = 04;
		}
		if(op>=opst+ 20 ) {
			error("expression overflow");
			exit(1);
		}
		*++op = o;
		*++pp = p;
		goto advanc;
	}
	--pp;
	switch (os = *op--) {

	case  200 :
		peeksym = o;
		build(0);		
		return(*--cp);

	case  100 :
		if (o!= 7 )
			goto syntax;
		build(os);
		goto advanc;

	case  101 :
		*cp++ = block(0,0,0,0);	
		os =  100 ;
		goto fbuild;

	case  6 :
		if (o!= 7 )
			goto syntax;
		goto advanc;

	case  4 :
		if (o!= 5 )
			goto syntax;
		build( 4 );
		goto advanc;
	}
fbuild:
	build(os);
	goto opon1;

syntax:
	error("Expression syntax");
	errflush(o);
	return(0);
}

declare(askw, tkw, offset, elsize)
{
	register int o;
	register int skw;

	skw = askw;
	do {
		offset =+ decl1(skw, tkw, offset, elsize);
		if (xdflg && skw!= 4 )
			return;
	} while ((o=symbol()) ==  9 );
	if (o== 1  || o== 7  && skw== 11 )
		return(offset);
	decsyn(o);
}

decl1(askw, tkw, offset, elsize)
{
	int t1, chkoff;
	register int type, skw;
	register struct hshtab *dsym;

	skw = askw;
	chkoff = 0;
	mosflg = skw== 4 ;
	if ((peeksym=symbol())== 1  || peeksym== 7 )
		return(0);
	if ((t1=getype()) < 0)
		goto syntax;
	type = 0;
	do
		type = type<<2 | (t1 & 030);
	while (((t1=>>2) & 030)!=0);
	type =| tkw;
	dsym = defsym;
	if (!(dsym->hclass==0
	   || (skw== 10  && dsym->hclass== 11 )
	   || (skw== 6  && dsym->hclass== 6  && dsym->htype==type)))
		if (skw== 4  && dsym->hclass== 4  && dsym->htype==type)
			chkoff = 1;
		else {
			redec();
			goto syntax;
		}
	dsym->htype = type;
	if (skw)
		dsym->hclass = skw;
	if (skw== 11 ) {
		if (paraml==0)
			paraml = dsym;
		else
			parame->hoffset = dsym;
		parame = dsym;
	}
	if (elsize && ((type&07)== 5  || (type&07)== 4 )) {
		dsym->lenp = dimp;
		chkdim();
		dimtab[dimp++] = elsize;
	}
	elsize = 0;
	if (skw== 4 ) {
		elsize = length(dsym);
		if ((offset&1)!=0 && elsize!=1) {
			offset++;
			elsize++;
		}
		if (chkoff && dsym->hoffset != offset)
			redec();
		dsym->hoffset = offset;
	}
	if ((dsym->htype&030)== 020 ) {
		if (dsym->hclass!= 6  && dsym->hclass!= 5 )
			error("Bad function");
		dsym->hclass =  6 ;
	}
	if (dsym->hclass== 5 ) {
		autolen =+ rlength(dsym);
		dsym->hoffset = -autolen;
	} else if (dsym->hclass== 7 ) {
		dsym->hoffset = isn;
		printf(".bss\nL%d:.=.+%d.\n.text\n", isn++, rlength(dsym));
	} else if (dsym->hclass== 8 ) {
		if ((type&07)> 1  && (type&030)==0
		 || (type&030)> 010  || regvar<3)
			error("Bad register %o", type);
		dsym->hoffset = --regvar;
	}
syntax:
	return(elsize);
}

getype()
{
	register int o, type;
	register struct hshtab *ds;

	switch(o=symbol()) {

	case  42 :
		return(getype()<<2 |  010 );

	case  6 :
		type = getype();
		if ((o=symbol()) !=  7 )
			goto syntax;
		goto getf;

	case  20 :
		defsym = ds = csym;
		type = 0;
		ds->ssp = dimp;
	getf:
		switch(o=symbol()) {

		case  6 :
			if (xdflg) {
				xdflg = 0;
				ds = defsym;
				declare( 11 , 0, 0, 0);
				defsym = ds;
				xdflg++;
			} else
				if ((o=symbol()) !=  7 )
					goto syntax;
			type = type<<2 |  020 ;
			goto getf;

		case  4 :
			if ((o=symbol()) !=  5 ) {
				peeksym = o;
				cval = conexp();
				for (o=ds->ssp&0377; o<dimp; o++)
					dimtab[o] =* cval;
				dimtab[dimp++] = cval;
				if ((o=symbol())!= 5 )
					goto syntax;
			} else
				dimtab[dimp++] = 1;
			type = type<<2 |  030 ;
			goto getf;
		}
		peeksym = o;
		return(type);
	}
syntax:
	decsyn(o);
	return(-1);
}

decsyn(o)
{
	error("Declaration syntax");
	errflush(o);
}

redec()
{
	error("%.8s redeclared", defsym->name);
}

