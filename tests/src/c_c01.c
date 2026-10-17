

























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















































































































































build(op) {
	register int t1;
	int t2, t3, t;
	struct tnode *p3, *disarray();
	register struct tnode *p1, *p2;
	int d, dope, leftc, cvn, pcvn;

	if (op== 4 ) {
		build( 40 );
		op =  36 ;
	}
	dope = opdope[op];
	if ((dope& 01 )!=0) {
		p2 = chkfun(disarray(*--cp));
		t2 = p2->type;
	}
	p1 = *--cp;
	if (op== 29 ) {
		t1 = length(p1);
		p1->op =  21 ;
		p1->type =  0 ;
		p1->dimp = 0;
		p1->value = t1;
		*cp++ = p1;
		return;
	}
	if (op!= 35 ) {
		p1 = disarray(p1);
		if (op!= 100 )
			p1 = chkfun(p1);
	}
	t1 = p1->type;
	pcvn = 0;
	switch (op) {

	
	case 0:
		*cp++ = p1;
		return;

	
	case  9 :
	case  53 :
	case  54 :
		*cp++ = block(2, op, 0, 0, p1, p2);
		return;

	case  90 :
		if (p2->op!= 8 )
			error("Illegal conditional");
		t = t2;
		goto nocv;

	case  100 :
		if ((t1&030) !=  020 )
			error("Call of non-function");
		*cp++ = block(2, 100 ,decref(t1),p1->dimp,p1,p2);
		return;

	case  36 :
		if (p1->op== 35  || p1->op== 29 ) {
			*cp++ = p1->tr1;
			return;
		}
		if ((t1&030) ==  020 )
			error("Illegal indirection");
		*cp++ = block(1, 36 ,decref(t1),p1->dimp,p1);
		return;

	case  35 :
		if (p1->op== 36 ) {
			p1->tr1->dimp = p1->dimp;
			p1->tr1->type = incref(t1);
			*cp++ = p1->tr1;
			return;
		}
		if (p1->op== 20 ) {
			if (p1->class== 5 )
				op =  29 ;
			*cp++ = block(1,op,incref(t1),p1->dimp,p1);
			return;
		}
		error("Illegal lvalue");
		break;

	case  30 :
	case  31 :
	case  32 :
	case  33 :
		chklval(p1);
		*cp++ = block(2,op,t1,p1->dimp,p1,plength(p1));
		return;

	case  50 :
		*cp++ = p1;
		chkw(p1);
		p1->type =  010 + 4 ;
		build( 36 );
		p1 = *--cp;

	case  39 :
		if (p2->op!= 20  || p2->class!= 4 )
			error("Illegal structure ref");
		*cp++ = p1;
		t = t2;
		if ((t&030) ==  030 ) {
			t = decref(t);
			p2->ssp++;
		}
		setype(p1, t, p2->dimp);
		build( 35 );
		*cp++ = block(1, 21 ,7,0,p2->nloc);
		build( 40 );
		if ((t2&030) !=  030 )
			build( 36 );
		return;
	}
	if ((dope& 02 )!=0)
		chklval(p1);
	if ((dope& 020 )!=0)
		chkw(p1);
	if ((dope& 040 )!=0)
		chkw(p2);
	if ((dope& 01 )==0) {
		if (!fold(op, p1, 0))
			*cp++ = block(1,op,t1,p1->dimp,p1);
		return;
	}
	if (t2==7) {
		t = t1;
		p2->type = 0;	
		t2 = 0;
		goto nocv;
	}
	cvn = cvtab[11*lintyp(t1)+lintyp(t2)];
	leftc = cvn&0100;
	t = leftc? t2:t1;
	if (op== 80  && t1!= 4  && t2!= 4 ) {
		t = t1;
		if (leftc || cvn!=3)		
			goto nocv;
	}
	if (cvn =& 077) {
		if (op== 8  && t1>= 010  && t1==t2)
			goto nocv;
		if (cvn==077) {
			if ((dope& 04 )==0 || t1< 010  || t2< 010 )
		illcv:
				error("Illegal conversion");
			goto nocv;
		}
		if (cvn==2) {			
			t = 0;			
			if ((dope& 04 )!=0) {
				cvn = 0;
				goto nocv;
			}
			if (op!= 41 )
				goto illcv;
			pcvn = cvn;
			cvn = 0;
			goto nocv;
		}
		if (leftc) {
			if ((dope& 010 ) != 0) {
				if (cvn == 3) {	
					leftc = 0;
					cvn = 4;
					t = t1;
					goto rcvt;
				} else
					goto illcv;
			}
			p1 = convert(p1, t, cvn, plength(p2));
		} else {
		rcvt:
			p2 = convert(p2, t, cvn, plength(p1));
		}
nocv:;	}
	if ((dope& 04 )!=0) {
		if (op> 61  && (t1>= 010  || t2>= 010 ))
			op =+ 4;	  
		t = 0;		
	}
	if (fold(op, p1, p2))
		return;
	*cp++ = block(2,op,t,(p1->dimp==0? p2:p1)->dimp,p1,p2);
	if (pcvn) {
		p1 = *--cp;
		*cp++ = convert(p1, 0, pcvn, plength(p1->tr1));
	}
}

convert(p, t, cvn, len)
struct tnode *p;
{
	register int n;

	switch(cvn) {

	
	case 1:
		n =  42 ;
		goto md;

	
	case 2:
		n =  43 ;
	md:
		return(block(2, n, t, 0, p, block(1,  21 , 0, 0, len)));

	
	case 3:
		n =  51 ;
		goto di;

	
	case 4:
		n =  52 ;
	di:
		return(block(1, n, t, 0, p));
	}
	error("C error-- convert");
	return(p);
}

setype(ap, at, adimptr)
struct tnode *ap;
{
	register struct tnode *p;
	register t, dimptr;

	p = ap;
	t = at;
	dimptr = adimptr;
	p->type = t;
	if (dimptr != -1)
		p->dimp = dimptr;
	switch(p->op) {

	case  29 :
	case  35 :
		setype(p->tr1, decref(t), dimptr);
		return;

	case  36 :
		setype(p->tr1, incref(t), dimptr);
		return;

	case  40 :
	case  41 :
		setype(p->tr1, t, dimptr);
	}
}

chkfun(ap)
struct tnode *ap;
{
	register struct tnode *p;
	register int t;

	p = ap;
	if (((t = p->type)&030)== 020 )
		return(block(1, 35 ,incref(t),p->dimp,p));
	return(p);
}

struct tnode *disarray(ap)
struct tnode *ap;
{
	register int t;
	register struct tnode *p;

	p = ap;
	
	if (((t = p->type)&030)!= 030  || p->op== 20 &&p->class== 4 )
		return(p);
	p->ssp++;
	*cp++ = p;
	setype(p, decref(t), -1);
	build( 35 );
	return(*--cp);
}

chkw(p)
struct tnode *p;
{
	register int t;

	if ((t=p->type)> 1  && t< 010 )
		error("Integer operand required");
	return;
}

lintyp(at)
{
	register t;

	t = at;
	if (t< 010 )
		return(t);
	if ((t&0170) == 0150)		
		return(9);
	if ((t&037)==t)
		return((t&07)+5);
	return(10);
}

error(s, p1, p2, p3, p4, p5, p6)
{
	register f;

	nerror++;
	fflush(ascbuf);
	f = ascbuf[0];
	ascbuf[0] = 1;
	printf("%d: ", line);
	printf(s, p1, p2, p3, p4, p5, p6);
	putchar('\n');
	fflush(ascbuf);
	ascbuf[0] = f;
}

block(n, op, t, d, p1,p2,p3)
int *p1, *p2, *p3;
{
	register int *ap, *p;

	ap = &op;
	n =+ 3;
	p = space;
	while(n--)
		pblock(*ap++);
	return(p);
}

pblock(p)
{

	*space++ = p;
	if (--osleft<=0) {
		error("Expression overflow");
		exit(1);
	}
}

chklval(ap)
struct tnode *ap;
{
	register struct tnode *p;

	p = ap;
	if (p->op!= 20  && p->op!= 36 )
		error("Lvalue required");
}

fold(op, ap1, ap2)
struct tnode *ap1, *ap2;
{
	register struct tnode *p1;
	register int v1, v2;

	p1 = ap1;
	if (p1->op!= 21  || (ap2!=0 && ap2->op!= 21 ))
		return(0);
	v1 = p1->value;
	v2 = ap2->value;
	switch (op) {

	case  40 :
		v1 =+ v2;
		break;

	case  41 :
		v1 =- v2;
		break;

	case  42 :
		v1 =* v2;
		break;

	case  43 :
		v1 =/ v2;
		break;

	case  44 :
		v1 =% v2;
		break;

	case  47 :
		v1 =& v2;
		break;

	case  48 :
		v1 =| v2;
		break;

	case  49 :
		v1 =^ v2;
		break;

	case  37 :
		v1 = - v1;
		break;

	case  38 :
		v1 = ~ v1;
		break;

	case  46 :
		v1 =<< v2;
		break;

	case  45 :
		v1 =>> v2;
		break;

	default:
		return(0);
	}
	p1->value = v1;
	*cp++ = p1;
	return(1);
}

conexp()
{
	register struct tnode *t;

	initflg++;
	if (t = tree())
		if (t->op !=  21 )
			error("Constant required");
	initflg--;
	return(t->value);
}
