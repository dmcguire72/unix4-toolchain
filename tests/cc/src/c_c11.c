







struct tnode {
	int op;
	int type;
	int degree;
	struct tnode *tr1, *tr2;
};

struct bnode {
	int bop;
	struct tnode *btree;
	int lbl;
	int cond;
};

struct tname {
	int nop;
	int ntype;
	int elsize;
	int class;
	int offset;
	int nloc;
};

struct tconst {
	int cop;
	int ctype;
	int cdeg;
	int value;
};

struct optab {
	char tabdeg1;
	char tabtyp1;
	char tabdeg2;
	char tabtyp2;
	char *tabstring;
};

struct table {
	int tabop;
	struct optab *tabp;
};

struct instab {
	int iop;
	char *str1;
	char *str2;
};

struct swtab {
	int swlab;
	int swval;
};

char	maprel[];
char	notrel[];
int	nreg;
int	isn;
int	namsiz;
int	line;
char	binbuf[518];
char	ascbuf[518];
char	outbuf[518];
int	nerror;
struct	table	cctab[];
struct	table	efftab[];
struct	table	regtab[];
struct	table	sptab[];
struct	table	lsptab[];
struct	instab	instab[];
int	opdope[];
int	nstack;
int	nfloat;
struct tconst czero, cone, fczero;























































































































max(a, b)
{
	if (a>b)
		return(a);
	return(b);
}

degree(at)
struct tnode *at;
{
	register struct tnode *t;

	if ((t=at)==0 || t->op==0)
		return(0);
	if (t->op ==  21 )
		return(-3);
	if (t->op ==  35 )
		return(-2);
	if (t->op ==  29 )
		return(-1);
	if ((opdope[t->op] &  0400 ) != 0) {
		if (t->type== 1 )
			return(1);
		return(0);
	}
	return(t->degree);
}

pname(ap, flag)
struct tnode *ap;
{
	register i;
	register struct tnode *p;

	p = ap;
loop:
	switch(p->op) {

	case  24 :
	case  21 :
		printf("$0%o", p->value);
		return;

	case  23 :
		printf("L%d", p->value);
		return;

	casename:
	case  20 :
		if (i = p->offset)
			printf("%d.+", i);
		switch(p->class) {

		case  5 :
			printf("%d.(r5)", p->nloc);
			return;

		case  6 :
			printf("_%.8s", &(p->nloc));
			return;

		case  4 :
			error("Illegal structure reference");
			printf("$0");
			return;

		case  8 :
			if (i)
				error("Bad reg. reference");
			printf("r%d", p->nloc);
			return;

		}
		
		printf("L%d", p->nloc);
		return;

	case  35 :
		putchar('$');
		p = p->tr1;
		goto loop;

	case  36 :
		p = p->tr1;
		
		if (p->op== 40 ) {
			if (p->tr2->op== 35 ) {
				pname(p->tr2->tr1, 0);
				printf("(r%d)", p->tr1->nloc);
			return;
		}
			printf("%d.(r%d)", p->tr2->value, p->tr1->nloc);
			return;
		} else if (p->op== 32 ) {
			printf("(r%d)%c", p->tr1->nloc, flag?0:'+');
			return;
		} else if (p->op== 31 ) {
			printf("%c(r%d)", flag?0:'-', p->tr1->nloc);
			return;
		} else if (p->op== 20  && p->class== 8 ) {
			printf("(r%d)", p->nloc);
			return;
		}
		putchar('*');
		goto loop;

	}
	error("pname called illegally");
}

xdcalc(ap, nrleft)
struct tnode *ap;
{
	register struct tnode *p;
	register d;

	p = ap;
	d = dcalc(p, nrleft);
	if (d<20 && p->type== 1 ) {
		if (nrleft>=1)
			d = 20;
		else
			d = 24;
	}
	return(d);
}

dcalc(ap, nrleft)
struct tnode *ap;
{
	register struct tnode *p, *p1;

	if ((p=ap)==0)
		return(0);
	switch (p->op) {

	case  20 :
	case  35 :
		return(12);

	case  21 :
	case  24 :
		return(p->value==0? 4:(p->value==1?5:8));

	case  23 :
		return(12);

	case  36 :
		p1 = p->tr1;
		if (p1->op== 20  || p1->op== 21 )
			return(12);
		if (p1->op== 36 )
			p1 = p1->tr1;
		if (p1->op== 40  && p1->tr1->op== 20 
		 && p1->tr1->class== 8  && p1->tr2->op== 21 )
			return(12);
		if (p1->op== 40  && p1->tr2->op== 35  && p1->tr1->op== 20 
		 && p1->tr2->type==( 010 + 1 ) && p1->tr1->class== 8 )
			return(12);
		if ((p1->op== 32  || p1->op== 31 ) &&
		     p1->tr1->type!=( 4 + 010 ))
			p1 = p1->tr1;
		 if (p1->op== 20  && p1->class== 8 )
			return(12);
	}
	return(p->degree<=nrleft? 20: 24);
}

notcompat(ap, ast)
struct tnode *ap;
{
	register at, st;
	register struct tnode *p;

	p = ap;
	at = p->type;
	st = ast;
	if ((at&07)== 4 )
		at =& 077770;	
	if (st==0)		
		return(at>1 & at<=07);
	if (st==1)		
		return(at>0 & at<=07);
	st =- 2;
	if ((at&077740) != 0)
		at = 020;
	if ((at&077770) != 0)
		at = at&07 | 020;
	if (st==2 && at==3)
		at = 2;
	if (p->op== 20  && p->class== 8  && at== 0  && st== 1 )
		return(0);
	return(st != at);
}

prins(op, c) {
	register struct instab *insp;
	register char *ip;

	for (insp=instab; insp->op != 0; insp++) {
		if (insp->op == op) {
			ip = c? insp->str2: insp->str1;
			if (ip==0)
				break;
			printf("%s", ip);
			return;
		}
	}
	error("No match' for op %d", op);
}

collcon(ap)
struct tnode *ap;
{
	register op;
	register struct tnode *p;

	p = ap;
	if(p->op== 40 ) {
		op = p->tr2->op;
		if (op== 21  || op== 35  || op== 29 )
			return(1);
	}
	return(0);
}

isfloat(at)
struct tnode *at;
{
	register struct tnode *t;

	t = at;
	if ((opdope[t->op]& 04 )!=0)
		t = t->tr1;
	if (t->type== 2  || t->type== 3 ) {
		nfloat = 1;
		return('f');
	}
	return(0);
}

oddreg(t, areg)
struct tnode *t;
{
	register reg;

	reg = areg;
	if (!isfloat(t))
		switch(t->op) {
		case  43 :
		case  44 :
		case  73 :
		case  74 :
			reg++;

		case  42 :
		case  72 :
			return(reg|1);
		}
	return(reg);
}

arlength(t)
{
	if (t>= 010 )
		return(2);
	switch(t) {

	case  0 :
	case  1 :
		return(2);

	case  2 :
	case  3 :
		return(8);
	}
	return(1024);
}

pswitch(afp, alp, deflab)
struct swtab *afp, *alp;
{
	int tlab, ncase, i, j, tabs, worst, best, range;
	register struct swtab *swp, *fp, *lp;
	int poctab[ 200 ];

	fp = afp;
	lp = alp;
	if (fp==lp) {
		printf("jbr	L%d\n", deflab);
		return;
	}
	tlab = isn++;
	if (sort(fp, lp))
		return;
	ncase = lp-fp;
	lp--;
	range = lp->swval - fp->swval;
	
	if (range>0 && range <= 3*ncase) {
		if (fp->swval)
			printf("sub	$0%o,r0\n", fp->swval);
		printf("cmp	r0,$0%o\n", range);
		printf("jhi	L%d\n", deflab);
		printf("asl	r0\n");
		printf("jmp	*.+4(r0)\n");
		for (i=fp->swval; i<=lp->swval; i++) {
			if (i==fp->swval) {
				printf("L%d\n", fp->swlab);
				fp++;
			} else
				printf("L%d\n", deflab);
		}
		goto esw;
	}
	
	if (ncase<8) {
		i = isn++;
		j = isn++;
		printf("mov	$L%d,r1\n", i);
		printf("mov	r0,L%d\n", j);
		printf("L%d:cmp	r0,(r1)+\n", isn);
		printf("jne	L%d\n", isn++);
		printf("jmp	*L%d-L%d(r1)\n", j, i);
		printf(".data\nL%d:", i);
		for (; fp<=lp; fp++)
			printf("%o\n", fp->swval);
		printf("L%d:..\n", j);
		for (fp = afp; fp<=lp; fp++)
			printf("L%d\n", fp->swlab);
		printf("L%d\n.text\n", deflab);
		goto esw;
	}
	
	best = 077777;
	for (i=ncase/4; i<=ncase/2; i++) {
		for (j=0; j<i; j++)
			poctab[j] = 0;
		for (swp=fp; swp<=lp; swp++)
			poctab[lrem(0, swp->swval, i)]++;
		worst = 0;
		for (j=0; j<i; j++)
			if (poctab[j]>worst)
				worst = poctab[j];
		if (i*worst < best) {
			tabs = i;
			best = i*worst;
		}
	}
	printf("jsr	r2,hsw; %o; L%d\n", tabs, isn);
	printf("jmp	*L%d-L%d(r1)\n", isn+tabs+1, isn+1);
	printf(".data\nL%d:", isn++);
	for (i=0; i<=tabs; i++)
		printf("L%d\n", isn+i);
	for (i=0; i<tabs; i++) {
		printf("L%d:..\n", isn++);
		for (swp=fp; swp<=lp; swp++)
			if (lrem(0, swp->swval, tabs) == i)
				printf("%o\n", ldiv(0, swp->swval, tabs));
	}
	printf("L%d:", isn++);
	for (i=0; i<tabs; i++) {
		printf("L%d\n", deflab);
		for (swp=fp; swp<=lp; swp++)
			if (lrem(0, swp->swval, tabs) == i)
				printf("L%d\n", swp->swlab);
	}
	printf(".text\n");
esw:
	printf("/esw\n");
}

sort(afp, alp)
struct swtab *afp, *alp;
{
	register struct swtab *cp, *fp, *lp;
	int intch, t;

	fp = afp;
	lp = alp;
	while (fp < --lp) {
		intch = 0;
		for (cp=fp; cp<lp; cp++) {
			if (cp->swval == cp[1].swval) {
				error("Duplicate case (%d)", cp->swval);
				return(1);
			}
			if (cp->swval > cp[1].swval) {
				intch++;
				t = cp->swval;
				cp->swval = cp[1].swval;
				cp[1].swval = t;
				t = cp->swlab;
				cp->swlab = cp[1].swlab;
				cp[1].swlab = t;
			}
		}
		if (intch==0)
			break;
	}
	return(0);
}

ispow2(atree)
{
	register int d;
	register struct tnode *tree;

	tree = atree;
	if (!isfloat(tree) && tree->tr2->op== 21 ) {
		d = tree->tr2->value;
		if (d>0 && (d&(d-1))==0)
			return(d);
	}
	return(0);
}

pow2(atree)
struct tnode *atree;
{
	register int d, i;
	register struct tnode *tree;

	tree = atree;
	if (d = ispow2(tree)) {
		for (i=0; (d=>>1)!=0; i++);
		tree->tr2->value = i;
		d = tree->op;
		tree->op = d== 42 ?  46 :
			  (d== 43 ?  45 :
			  (d== 72 ?  76 :  75 ));
	}
}

cbranch(atree, albl, cond, areg)
struct tnode *atree;
{
	int l1, opfix;
	register lbl, reg;
	register struct tnode *tree;
	struct tnode lbuf;

	lbl = albl;
	reg = areg;
	if ((tree=atree)==0)
		return;
	switch(tree->op) {

	case  53 :
		if (cond) {
			cbranch(tree->tr1, l1=isn++, 0, reg);
			cbranch(tree->tr2, lbl, 1, reg);
			label(l1);
		} else {
			cbranch(tree->tr1, lbl, 0, reg);
			cbranch(tree->tr2, lbl, 0, reg);
		}
		return;

	case  54 :
		if (cond) {
			cbranch(tree->tr1, lbl, 1, reg);
			cbranch(tree->tr2, lbl, 1, reg);
		} else {
			cbranch(tree->tr1, l1=isn++, 1, reg);
			cbranch(tree->tr2, lbl, 0, reg);
			label(l1);
		}
		return;

	case  34 :
		cbranch(tree->tr1, lbl, !cond, reg);
		return;

	case  9 :
		rcexpr(tree->tr1, efftab, reg);
		tree = tree->tr2;
		break;
	}
	opfix = 0;
	if ((opdope[tree->op]& 04 )==0) {
		lbuf.op =  61 ;
		lbuf.type = tree->type;
		lbuf.degree = tree->degree;
		lbuf.tr1 = tree;
		lbuf.tr2 = &czero;
		if (isfloat(tree))
			lbuf.tr2 = &fczero;
		tree = &lbuf;
	}
 	if (tree->tr2->op== 21  && tree->tr2->value==0)
		opfix = 200;
	rcexpr(tree, cctab, reg);
	if (isfloat(tree))
		printf("cfcc\n");
	branch(lbl, tree->op+opfix, !cond);
}

branch(lbl, aop, c)
{
	register op;

	if(op=aop)
		prins(op,c);
	else
		printf("jbr");
	printf("\tL%d\n", lbl);
}

label(l)
{
	printf("L%d:", l);
}

popstk(a)
{
	switch(a) {

	case 0:
		return;

	case 2:
		printf("tst	(sp)+\n");
		return;

	case 4:
		printf("cmp	(sp)+,(sp)+\n");
		return;
	}
	printf("add	$0%o,sp\n", a);
}

error(s, p1, p2, p3, p4, p5, p6)
{
	register f;

	nerror++;
	fflush(outbuf);
	f = outbuf[0];
	outbuf[0] = 1;
	printf("%d: ", line);
	printf(s, p1, p2, p3, p4, p5, p6);
	putchar('\n');
	fflush(outbuf);
	outbuf[0] = f;
}

