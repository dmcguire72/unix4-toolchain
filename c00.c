/* C compiler

Copyright 1972 Bell Telephone Laboratories, Inc. 

*/
#include "c0.h"

int	peeksym	= -1;
int	peekc;
int	eof;
int	line	= 1;
struct symbol *csym;
int16_t	cval;
int	ctyp;
int	isn	= 1;
int	nreg	= 4;
int	ncpw	= 2;
int	nerror;
int	swp	= -1;
int	swtab[SWSIZ];
int	contlab, brklab, deflab;
int	stack;
struct symbol *paraml, *parame;
const char *tmpfil;
FILE	*fin, *fout;

static char symbuf[NAMSIZ];
int hshused;
struct symbol hshtab[HSHSIZ];

struct tnode *cmst[CMSIZ], **cp;

const int maprel[10] = { 60, 61, 64, 65, 62, 63, 68, 69, 66, 67 };

static void
init(const char *s, int t)
{
	int i;
	char *sp;
	struct symbol *np;

	i = NAMSIZ;
	sp = symbuf;
	while (i--)
		if ((*sp++ = *s++) == '\0')
			--s;
	np = lookup();
	np->sclass = SC_KEYWORD;
	np->type = t;
}

int
main(int argc, char **argv)
{
	if (argc < 4) {
		error("Arg count");
		fprintf(stderr, "usage: %s source output tmpfile\n", argv[0]);
		exit(1);
	}
	if ((fin = fopen(argv[1], "rb")) == NULL) {
		error("Can't find %s", argv[1]);
		exit(1);
	}
	if ((fout = fopen(argv[2], "wb")) == NULL) {
		error("Can't create %s", argv[2]);
		exit(1);
	}
	tmpfil = argv[3];
	init("int", K_INT);
	init("char", K_CHAR);
	init("float", K_FLOAT);
	init("double", K_DOUBLE);
/*	init("long", 4);  */
	init("auto", K_AUTO);
	init("extern", K_EXTERN);
	init("static", K_STATIC);
	init("goto", K_GOTO);
	init("return", K_RETURN);
	init("if", K_IF);
	init("while", K_WHILE);
	init("else", K_ELSE);
	init("switch", K_SWITCH);
	init("case", K_CASE);
	init("break", K_BREAK);
	init("continue", K_CONTINUE);
	init("do", K_DO);
	init("default", K_DEFAULT);
	while (!eof) {
		extdef();
		blkend();
	}
	flush();
	flshw();
	exit(nerror != 0);
}

struct symbol *
lookup(void)
{
	int i, j;
	uint16_t h;
	struct symbol *np;

	h = 0;
	for (j = 0; j < NAMSIZ; j += 2)
		h += (uint8_t)symbuf[j] | (uint8_t)symbuf[j + 1] << 8;
	i = (int16_t)h;
	if (i < 0)
		i = -i;
	i %= HSHSIZ;
	while (hshtab[i].name[0] != '\0' || hshtab[i].name[1] != '\0') {
		np = &hshtab[i];
		if (memcmp(np->name, symbuf, NAMSIZ) == 0)
			return np;
		if (++i >= HSHSIZ)
			i = 0;
	}

	if (hshused >= HSHSIZ - 1) {
		error("Symbol table overflow");
		exit(1);
	}
	hshused++;
	np = &hshtab[i];
	np->sclass = np->type = np->off = np->dim = 0;
	np->plink = NULL;
	memcpy(np->name, symbuf, NAMSIZ);
	return np;
}

static int cclass(int c);
static int getstr(void);
static int getcc(void);
static int mapch(int c);

int
symbol(void)
{
	int b, c;
	char *sp;

	if (peeksym >= 0) {
		c = peeksym;
		peeksym = -1;
		return c;
	}
	if (peekc) {
		c = peekc;
		peekc = 0;
	} else if (eof)
		return 0;
	else
		c = rdchar();
loop:
	switch (cclass(c)) {

	case C_NL:
		line++;

	case C_SPACE:
		c = rdchar();
		goto loop;

	case C_EOF:
		eof++;
		return 0;

	case CH_PLUS:
		return subseq(c, 40, 30);

	case CH_MINUS:
		return subseq(c, 41, 31);

	case CH_EQ:
		if (subseq(' ', 0, 1))
			return 80;
		c = symbol();
		if (c >= 40 && c <= 49)
			return c + 30;
		if (c == 80)
			return 60;
		peeksym = c;
		return 80;

	case CH_LT:
		if (subseq(c, 0, 1))
			return 46;
		return subseq('=', 63, 62);

	case CH_GT:
		if (subseq(c, 0, 1))
			return 45;
		return subseq('=', 65, 64);

	case CH_NOT:
		return subseq('=', 34, 61);

	case CH_SLASH:
		if (subseq('*', 1, 0))
			return 43;
com:
		c = rdchar();
com1:
		if (c == '\0') {
			eof++;
			error("Nonterminated comment");
			return 0;
		}
		if (c == '\n')
			line++;
		if (c != '*')
			goto com;
		c = rdchar();
		if (c != '/')
			goto com1;
		c = rdchar();
		goto loop;

	case C_DIGIT:
		cval = 0;
		if (c == '0')
			b = 8;
		else
			b = 10;
		while (cclass(c) == C_DIGIT) {
			cval = (int16_t)(cval * b + c - '0');
			c = rdchar();
		}
		peekc = c;
		return T_CON;

	case C_STRQ:
		return getstr();

	case C_CHRQ:
		return getcc();

	case C_LETTER:
		sp = symbuf;
		while (cclass(c) == C_LETTER || cclass(c) == C_DIGIT) {
			if (sp < symbuf + NAMSIZ)
				*sp++ = c;
			c = rdchar();
		}
		while (sp < symbuf + NAMSIZ)
			*sp++ = '\0';
		peekc = c;
		csym = lookup();
		if (csym->sclass == SC_KEYWORD) {
			cval = csym->type;
			return T_KEYW;
		}
		return T_NAME;

	case C_UNKN:
		error("Unknown character");
		c = rdchar();
		goto loop;

	}
	return ctab[c];
}

static int
cclass(int c)
{
	return c >= 0 && c < 128 ? ctab[c] : C_UNKN;
}

int
subseq(int c, int a, int b)
{
	if (!peekc)
		peekc = rdchar();
	if (peekc != c)
		return a;
	peekc = 0;
	return b;
}

static int
getstr(void)
{
	int c;

	oprintf(".data;L%d:.byte ", cval = isn++);
	while ((c = mapch('"')) >= 0)
		oprintf("%o,", c);
	oprintf("0;.even;.text\n");
	return T_STRING;
}

static int
getcc(void)
{
	int c, cc;

	cval = 0;
	cc = 0;
	while ((c = mapch('\'')) >= 0) {
		if (cc++ < ncpw)
			cval = (int16_t)(cval | (c & 0377) << 8 * (cc - 1));
	}
	if (cc > ncpw)
		error("Long character constant");
	return T_CON;
}

static int
mapch(int c)
{
	int a;

	if ((a = rdchar()) == c)
		return -1;
	switch (a) {

	case '\n':
	case 0:
		error("Nonterminated string");
		peekc = a;
		return -1;

	case '\\':
		switch (a = rdchar()) {

		case 't':
			return '\t';

		case 'n':
			return '\n';

		case '0':
			return '\0';

		case 'r':
			return '\r';

		case '\n':
			line++;
			return '\n';
		}

	}
	return a;
}

struct tnode *
tree(void)
{
	int opst[OPSIZ + 1], prst[OPSIZ + 1];
	int *op, *pp;
	int andflg, o, p, ps, os;

	space = 0;
	op = opst;
	pp = prst;
	cp = cmst;
	*op = 200;
	*pp = 06;
	andflg = 0;

advanc:
	switch (o = symbol()) {

	case T_NAME:
		if (csym->sclass == SC_UNDEF) {
			if ((peeksym = symbol()) == T_LPAREN)
				csym->sclass = SC_EXTERN;
			else if (csym->off == 0)
				csym->off = isn++;
		}
		*cp++ = nameblock(csym);
		goto tand;

	case T_CON:
	case21:
		*cp++ = conblock(T_CON, ctyp, cval);
		goto tand;

	case T_STRING:
		*cp++ = conblock(T_STRING, 17, cval);

tand:
		if (cp >= cmst + CMSIZ) {
			error("Expression overflow");
			exit(1);
		}
		if (andflg)
			goto syntax;
		andflg = 1;
		goto advanc;

	case O_INCBEF:
	case O_DECBEF:
		if (andflg)
			o += 2;
		goto oponst;

	case O_NOT:
		if (andflg)
			goto syntax;
		goto oponst;

	case O_MINUS:
		if (!andflg) {
			peeksym = symbol();
			if (peeksym == T_CON) {
				peeksym = -1;
				cval = (int16_t)-cval;
				goto case21;
			}
			o = O_NEG;
		}
		andflg = 0;
		goto oponst;

	case O_AND:
	case O_TIMES:
		if (andflg)
			andflg = 0;
		else if (o == O_AND)
			o = O_ADDR;
		else
			o = O_IND;
		goto oponst;

	case T_LPAREN:
		if (andflg) {
			o = symbol();
			if (o == T_RPAREN)
				o = O_MCALL;
			else {
				peeksym = o;
				o = O_CALL;
				andflg = 0;
			}
		}
		goto oponst;

	case T_RBRACK:
	case T_RPAREN:
		if (!andflg)
			goto syntax;
		goto oponst;
	}

	if (!andflg)
		goto syntax;
	andflg = 0;

oponst:
	p = D_PREC(opdope[o]);
opon1:
	ps = *pp;
	if (p > ps || (p == ps && (opdope[o] & D_RASSOC) != 0)) {
		switch (o) {

		case T_LPAREN:
		case T_LBRACK:
		case O_CALL:
			p = 04;
		}
		if (op >= opst + OPSIZ) {
			error("expression overflow");
			exit(1);
		}
		*++op = o;
		*++pp = p;
		goto advanc;
	}
	--pp;
	switch (os = *op--) {

	case 200:
		peeksym = o;
		return *--cp;

	case O_CALL:
		if (o != T_RPAREN)
			goto syntax;
		build(os);
		goto advanc;

	case O_MCALL:
		*cp++ = NULL;
		os = O_CALL;
		goto fbuild;

	case T_LPAREN:
		if (o != T_RPAREN)
			goto syntax;
		goto advanc;

	case T_LBRACK:
		if (o != T_RBRACK)
			goto syntax;
		build(4);
		goto advanc;
	}
fbuild:
	build(os);
	goto opon1;

syntax:
	error("Expression syntax");
	errflush(o);
	return NULL;
}

void
declare(int kw)
{
	int o;

	while ((o = symbol()) == T_NAME) {
		if (kw >= 5) {
			if (csym->sclass > 0)
				error("%p redeclared", csym->name);
			csym->sclass = kw;
		} else {
			if ((csym->type & 017) != 0)
				error("%p redeclared", csym->name);
			csym->type |= kw;
			if (csym->sclass == SC_UNDEF)
				csym->sclass = SC_SORTLESS;
		}
		while ((o = symbol()) == T_LBRACK) {
			if ((o = symbol()) == T_CON) {
				if (csym->type >= 020)
					error("Bad vector");
				csym->dim = cval;
				o = symbol();
			}
			if (o != T_RBRACK)
				goto syntax;
			csym->type += 020;
		}
		if (kw == K_PARAM) {
			csym->sclass = SC_PARAMDECL;
			if (paraml == NULL)
				paraml = csym;
			else {
				parame->plink = csym;
				parame->sclass = SC_PLINKED;
			}
			parame = csym;
		}
		if (o != T_COMMA)
			break;
	}
	if ((o == T_SEMI && kw != K_PARAM) || (o == T_RPAREN && kw == K_PARAM))
		return;
syntax:
	error("Declaration syntax");
	errflush(o);
}
