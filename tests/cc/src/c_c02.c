

























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















































































































































extdef() {
	register o, width;
	int type, elsize, nel;
	char *cs;
	register struct hshtab *ds;

	if(((o=symbol())== 0 ) || o== 1 )
		return;
	type = 0;
	xdflg++;
    xxx:
	if (o== 19 ) {
		if (cval== 6 ) {
			o = symbol();
			goto xxx;
		}
		if ((type=cval)> 4 )
			goto syntax;	
		elsize = 0;
		if (type== 4 ) {
			elsize = strdec(&type, 0);
			if ((peeksym=symbol())!= 19 )
				blkhed();
		}
	} else {
		if (o!= 20 )
			goto syntax;
		peeksym = o;
	}
	do {
		defsym = 0;
		strflg = 1;
		decl1( 6 , type&07, 0, elsize);
		if ((ds=defsym)==0)
			return;
		cs = ds->name;
		funcsym = ds;
		*cs =| 0200;		
		printf(".globl	_%.8s\n", cs);
		xdflg = 0;
		type = ds->type;
		if ((type&030)== 020  && ((peeksym=symbol())== 2 
		   || peeksym== 19 )) {
			cfunc(cs);
			return;
		}
		nel = 1;
		while ((ds->type&030)== 030 ) {
			nel = dimtab[ds->ssp&0377];
			ds->type = decref(ds->type);
		}
		width = length(ds);
		if (ds->type== 4 ) {
			nel =* width/2;
			width = 2;
		}
		ds->type = type;
		cinit(cs, type, nel, width);
	} while ((o=symbol())== 9 );
	if (o== 1 )
		return;
syntax:
	error("External definition syntax");
	errflush(o);
	statement(0);
}

cfunc(cs)
char *cs;
{
	register savdimp;

	strflg = 0;
	savdimp = dimp;
	printf(".text\n_%.8s:\n", cs);
	declist( 10 );
	regvar = 5;
	retlab = isn++;
	if ((peeksym = symbol()) !=  2 )
		error("Compound statement required");
	statement(1);
	printf("L%d:jmp	rretrn\n", retlab);
	dimp = savdimp;
}

cinit(cs, type, nel, awidth)
char *cs;
{
	register o, ninit, width;

	width = awidth;
	if ((peeksym=symbol())== 9  || peeksym== 1 ) {
		printf(".comm	_%.8s,0%o\n", cs, (nel*width+1)&~01);
		return;
	}
	ninit = 0;
	printf(".data\n_%.8s=.\n", cs);
	if ((o=symbol())== 2 ) {
		do
			ninit = cinit1(cs, type, width, ninit);
		while ((o=symbol())== 9 );
		if (o!= 3 )
			peeksym = o;
	} else {
		peeksym = o;
		ninit = cinit1(cs, type, width, 0);
	}
	if (ninit<nel)
		printf(".=.+%d.\n", (nel-ninit)*width);
	else
		nel = ninit;
	if (nel>1 && (type&030)!= 030  && (type&07)!= 4 )
		error("Too many initializers");
	if (((nel&width)&01) != 0)
		printf(".even\n");
}

cinit1(cs, type, width, ninit)
char *cs;
{
	float sf;
	register struct tnode *s;

	if ((peeksym=symbol())== 22  && type== 030 + 1 ) {
		peeksym = -1;
		if (ninit)
			bxdec();
		printf(".text\n_%.8s=L%d\n", cs, cval);
		return((nchstr+1) & ~01);
	}
	if (peeksym== 3 )
		return(ninit);
	initflg++;
	s = tree();
	initflg = 0;
	switch(width) {

	case 1:
		printf(".byte ");
		if (s->op !=  21 )
			bxdec();

	case 2:
		if (s->op== 21 ) {
			printf("%d.\n", s->value);
			break;
		}
		rcexpr(block(1, 104 ,0,0,s),  0 );
		break;

	case 4:
		sf = fcval;
		printf("0%o;0%o\n", sf);
		goto flt;

	case 8:
		printf("0%o;0%o;0%o;0%o\n", fcval);
	flt:
		if (s->op== 23  || s->op== 24 )
			break;

	default:
		bxdec();

	}
	return(++ninit);
}

bxdec()
{
	error("Inconsistent external initialization");
}

statement(d)
{
	register o, o1, o2;
	int o3, o4;
	struct tnode *np;

stmt:
	switch(o=symbol()) {

	case  0 :
		error("Unexpected EOF");
	case  1 :
	case  3 :
		return;

	case  2 :
		if(d) {
			if (proflg) {
				o = "jsr\tr5,mrsave;0f;%o\n.bss;0:.=.+2\n.text\n";
			} else
				o = "jsr	r5,rsave; %o\n";
			printf(o, blkhed()-4);
		}
		while (!eof) {
			if ((o=symbol())== 3 )
				return;
			peeksym = o;
			statement(0);
		}
		error("Missing '}'");
		return;

	case  19 :
		switch(cval) {

		case  10 :
			if (o1 = simplegoto())
				branch(o1);
			else 
				dogoto();
			goto semi;

		case  11 :
			doret();
			goto semi;

		case  12 :
			np = pexpr();
			o2 = 0;
			if ((o1=symbol())== 19 ) switch (cval) {
			case  10 :
				if (o2=simplegoto())
					goto simpif;
				cbranch(np, o2=isn++, 0);
				dogoto();
				label(o2);
				goto hardif;

			case  11 :
				if (nextchar()==';') {
					o2 = retlab;
					goto simpif;
				}
				cbranch(np, o1=isn++, 0);
				doret();
				label(o1);
				o2++;
				goto hardif;

			case  17 :
				o2 = brklab;
				goto simpif;

			case  18 :
				o2 = contlab;
			simpif:
				chconbrk(o2);
				cbranch(np, o2, 1);
			hardif:
				if ((o=symbol())!= 1 )
					goto syntax;
				if ((o1=symbol())== 19  && cval== 14 ) 
					goto stmt;
				peeksym = o1;
				return;
			}
			peeksym = o1;
			cbranch(np, o1=isn++, 0);
			statement(0);
			if ((o=symbol())== 19  && cval== 14 ) {
				o2 = isn++;
				branch(o2);
				label(o1);
				statement(0);
				label(o2);
				return;
			}
			peeksym = o;
			label(o1);
			return;

		case  13 :
			o1 = contlab;
			o2 = brklab;
			label(contlab = isn++);
			cbranch(pexpr(), brklab=isn++, 0);
			statement(0);
			branch(contlab);
			label(brklab);
			contlab = o1;
			brklab = o2;
			return;

		case  17 :
			chconbrk(brklab);
			branch(brklab);
			goto semi;

		case  18 :
			chconbrk(contlab);
			branch(contlab);
			goto semi;

		case  19 :
			o1 = contlab;
			o2 = brklab;
			contlab = isn++;
			brklab = isn++;
			label(o3 = isn++);
			statement(0);
			label(contlab);
			contlab = o1;
			if ((o=symbol())== 19  && cval== 13 ) {
				cbranch(tree(), o3, 1);
				label(brklab);
				brklab = o2;
				goto semi;
			}
			goto syntax;

		case  16 :
			o1 = conexp();
			if ((o=symbol())!= 8 )
				goto syntax;
			if (swp==0) {
				error("Case not in switch");
				goto stmt;
			}
			if(swp>=swtab+ 200 ) {
				error("Switch table overflow");
			} else {
				swp->swlab = isn;
				(swp++)->swval = o1;
				label(isn++);
			}
			goto stmt;

		case  15 :
			o1 = brklab;
			brklab = isn++;
			np = pexpr();
			chkw(np);
			rcexpr(block(1, 110 ,0,0,np),  0 );
			pswitch();
			brklab = o1;
			return;

		case  20 :
			if (swp==0)
				error("Default not in switch");
			if ((o=symbol())!= 8 )
				goto syntax;
			label(deflab = isn++);
			goto stmt;

		case  21 :
			o1 = contlab;
			o2 = brklab;
			contlab = isn++;
			brklab = isn++;
			if (o=forstmt())
				goto syntax;
			label(brklab);
			contlab = o1;
			brklab = o2;
			return;
		}

		error("Unknown keyword");
		goto syntax;

	case  20 :
		if (nextchar()==':') {
			peekc = 0;
			o1 = csym;
			if (o1->hclass>0) {
				error("Redefinition");
				goto stmt;
			}
			o1->hclass =  7 ;
			o1->htype =  030 ;
			if (o1->hoffset==0)
				o1->hoffset = isn++;
			label(o1->hoffset);
			if ((peeksym=symbol())== 3 )
				return;
			goto stmt;
		}
	}

	peeksym = o;
	rcexpr(tree(),  1 );

semi:
	if ((o=symbol())== 1 )
		return;
syntax:
	error("Statement syntax");
	errflush(o);
	goto stmt;
}



forstmt()
{
	int l, savxpr[ 150 ];
	int *st, *ss;
	register int *sp1, *sp2, o;

	if ((o=symbol()) !=  6 )
		return(o);
	if ((o=symbol()) !=  1 ) {		
		peeksym = o;
		rcexpr(tree(),  1 );
		if ((o=symbol()) !=  1 )
			return(o);
	}
	label(contlab);
	if ((o=symbol()) !=  1 ) {		
		peeksym = o;
		rcexpr(block(1, 103 ,tree(),brklab,0),  2 );
		if ((o=symbol()) !=  1 )
			return(o);
	}
	if ((peeksym=symbol()) ==  7 ) {	
		peeksym = -1;
		statement(0);
		branch(contlab);
		return(0);
	}
	l = contlab;
	contlab = isn++;
	st = tree();
	if ((o=symbol()) !=  7 )
		return(o);
	ss = space;
	if (space-treebase >  150 ) {
		error("Expression too large");
		space = &treebase[ 150 ];
	}
	sp2 = savxpr;
	for (sp1=treebase; sp1<space;)
		*sp2++ = *sp1++;
	statement(0);
	space = ss;
	sp2 = savxpr;
	for (sp1=treebase; sp1<space;)
		*sp1++ = *sp2++;
	label(contlab);
	rcexpr(st,  1 );
	branch(l);
	return(0);
}

pexpr()
{
	register o, t;

	if ((o=symbol())!= 6 )
		goto syntax;
	t = tree();
	if ((o=symbol())!= 7 )
		goto syntax;
	return(t);
syntax:
	error("Statement syntax");
	errflush(o);
	return(0);
}

pswitch()
{
	int *sswp, swlab;
	register int *swb, *wswp, dl;

	swb = sswp = swp;
	if (swp==0)
		swb = swp = swtab;
	branch(swlab=isn++);
	dl = deflab;
	deflab = 0;
	statement(0);
	branch(brklab);
	label(swlab);
	putchar('#');		
	label(brklab);
	if (!deflab) {
		deflab = isn++;
		label(deflab);
	}
	wswp = swp;
	putw(wswp-swb, binbuf);
	putw(deflab, binbuf);
	putw(4, binbuf);	
	putw(line, binbuf);
	while (swb < wswp)
		putw(*swb++, binbuf);
	deflab = dl;
	swp = sswp;
}

blkhed()
{
	register pl;
	register struct hshtab *cs;

	autolen = 6;
	declist(0);
	pl = 4;
	while(paraml) {
		parame->hoffset = 0;
		cs = paraml;
		paraml = paraml->hoffset;
		if (cs->htype== 2 )
			cs->htype =  3 ;
		cs->hoffset = pl;
		cs->hclass =  5 ;
		if ((cs->htype&030) ==  030 ) {
			cs->htype =- 020;	
			cs->ssp++;		
		}
		pl =+ rlength(cs);
	}
	for (cs=hshtab; cs<hshtab+ 200 ; cs++) {
		if (cs->name[0] == '\0')
			continue;
		
		if (cs->hclass> 1  && (cs->htype&07)== 5 ) {
			cs->lenp = dimtab[cs->lenp&0377]->lenp;
			cs->htype = cs->htype&~07 |  4 ;
		}
		if (cs->hclass ==  9  && dimtab[cs->lenp&0377]==0)
			error("Undefined structure: %.8s", cs->name);
		if (cs->hclass ==  10 )
			error("Not an argument: %.8s", cs->name);
	}
	osleft =  500 ;
	space = treebase;
	rcexpr(block(1,  105 , regvar),  0 );
	return(autolen);
}

blkend() {
	register struct hshtab *cs;

	for (cs=hshtab; cs<hshtab+ 200 ; cs++) {
		if (cs->name[0]) {
			if (cs->hclass==0)
				error("%.8s undefined", cs->name);
			if((cs->name[0]&0200)==0) {	
				cs->name[0] = '\0';
				hshused--;
			}
		}
	}
}

errflush(ao)
{
	register o;

	o = ao;
	while(o> 3 )	
		o = symbol();
	peeksym  = o;
}

declist(skwd)
{
	int o, elsize, ndec;
	register offset, tkw, skw;

	offset = 0;
loop:
	ndec = 0;
	tkw = -1;
	skw = skwd;
	elsize = 0;
	while ((o=symbol())== 19 ) switch (cval) {
	case  5 :
	case  7 :
	case  6 :
	case  8 :
		if (skw)
			error("Conflict in storage class");
		skw = cval;
		ndec++;
		if (tkw<0)
			continue;
		goto list;

	case  4 :
		o = cval;
		elsize = strdec(&o, skw== 4 );
		cval = o;
	case  0 :
	case  1 :
	case  2 :
	case  3 :
		ndec++;
		if (tkw>=0)
			error("Type clash");
		tkw = cval;
		if (skw==0)
			continue;
		goto list;

	default:
		goto brk1;
	}
  brk1:
	peeksym = o;
	if (ndec==0)
		return(offset);
list:
	if (tkw<0)
		tkw =  0 ;
	if (skw==0)
		skw =  5 ;
	offset = declare(skw, tkw, offset, elsize);
	goto loop;
}

strdec(tkwp, mosf)
int *tkwp;
{
	register elsize, o;
	register struct hshtab *ssym;
	struct hshtab *ds;

	mosflg = 1;
	ssym = 0;
	if ((o=symbol())== 20 ) {
		ssym = csym;
		if (ssym->hclass==0) {
			ssym->hclass =  9 ;
			ssym->lenp = dimp;
			chkdim();
			dimtab[dimp++] = 0;
		}
		if (ssym->hclass !=  9 )
			redec();
		mosflg = mosf;
		o = symbol();
	}
	mosflg = 0;
	if (o !=  2 ) {
		if (ssym==0) {
		syntax:
			decsyn(o);
			return(0);
		}
		if (ssym->hclass!= 9 )
			error("Bad structure name");
		if ((elsize = dimtab[ssym->lenp&0377])==0) {
			*tkwp =  5 ;
			elsize = ssym;
		}
		peeksym = o;
	} else {
		ds = defsym;
		mosflg = 0;
		elsize = declist( 4 );
		if (elsize&01)
			elsize++;
		defsym = ds;
		if ((o = symbol()) !=  3 )
			goto syntax;
		if (ssym) {
			if (dimtab[ssym->lenp&0377])
				error("%.8s redeclared", ssym->name);
			dimtab[ssym->lenp&0377] = elsize;
		}
	}
	return(elsize);
}

chkdim()
{
	if (dimp >=  100 ) {
		error("Dimension/struct table overflow");
		exit(1);
	}
}
