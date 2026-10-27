

























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















































































































































decref(at)
{
	register t;

	t = at;
	if ((t & ~07) == 0) {
		error("Illegal indirection");
		return(t);
	}
	return((t>>2) & ~07 | t&07);
}

incref(t)
{
	return((t<<2)&~034 | (t&07) |  010 );
}

cbranch(tree, lbl, cond)
struct tnode *tree;
{
	rcexpr(block(1, 103 ,tree,lbl,cond), 2 );
}

rcexpr(tree, table)
int table;
struct tnode *tree;
{
	register c, *sp;

	if (tree == 0)
		return;
	putchar('#');
	c = space-treebase;
	sp = treebase;
	putw(c, binbuf);
	putw(tree, binbuf);
	putw(table, binbuf);
	putw(line, binbuf);
	while(c--)
		putw(*sp++, binbuf);
}

branch(lab) {
	printf("jbr\tL%d\n", lab);
}

label(l) {
	printf("L%d:", l);
}

plength(ap)
struct tname *ap;
{
	register t, l;
	register struct tname *p;

	p = ap;
	if (((t=p->ntype)&~07) == 0)		
		return(1);
	p->ntype = decref(t);
	l = length(p);
	p->ntype = t;
	return(l);
}

length(acs)
struct tnode *acs;
{
	register t, n;
	register struct tnode *cs;

	cs = acs;
	t = cs->type;
	n = 1;
	while ((t&030) ==  030 ) {
		t = decref(t);
		n = dimtab[cs->ssp&0377];
	}
	if ((t&~07)== 020 )
		return(0);
	if (t>= 010 )
		return(2*n);
	switch(t&07) {

	case  0 :
		return(2*n);

	case  1 :
		return(n);

	case  2 :
		return(4*n);

	case  3 :
		return(8*n);

	case  4 :
		return(n * dimtab[cs->lenp&0377]);

	case  5 :
		error("Bad structure");
		return(0);
	}
	error("Compiler error (length)");
}

rlength(cs)
struct tnode *cs;
{
	register int l;

	if (((l=length(cs))&01) != 0)
		l++;
	return(l);
}

simplegoto()
{
	register struct hshtab *csp;

	if ((peeksym=symbol())== 20  && nextchar()==';') {
		csp = csym;
		if (csp->hclass==0 && csp->htype==0) {
			csp->htype =  030 ;
			if (csp->hoffset==0)
				csp->hoffset = isn++;
		}
		if ((csp->hclass==0||csp->hclass== 7 )
		 &&  csp->htype== 030 ) {
			peeksym = -1;
			return(csp->hoffset);
		}
	}
	return(0);
}

nextchar()
{
	while (ctab[peekc]== 126 )
		peekc = getchar();
	return(peekc);
}

chconbrk(l)
{
	if (l==0)
		error("Break/continue error");
}

dogoto()
{
	register struct tnode *np;

	*cp++ = tree();
	build( 36 );
	chkw(np = *--cp);
	rcexpr(block(1, 102 ,0,0,np),  0 );
}

doret()
{
	if (nextchar() != ';')
		rcexpr(block(1,  110 , 0, 0, tree()),  0 );
	branch(retlab);
}

putchar(c)
{
	putc(c, ascbuf);
}
