/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#include "c0.h"

static void pswitch(void);
static void blkhed(void);

void
function(void)
{
	oprintf(".text; 1:mov r5,-(sp); mov sp,r5\n");
	declare(K_PARAM);
	declist();
	statement(1);
	retseq();
}

void
extdef(void)
{
	int o, c;
	const char *cs;
	const char *s;

	if ((o = symbol()) == T_EOF || o == T_SEMI)
		return;
	if (o != T_NAME)
		goto syntax;
	csym->sclass = SC_EXTERN;
	cs = csym->name;
	oprintf(".globl\t%p\n", cs);
	s = ".data; %p:1f\n";
	switch (o = symbol()) {

	case T_LPAREN:
		oprintf(s, cs);
		function();
		return;

	case T_CON:
		oprintf(".data; %p: %o\n", cs, cval);
		if ((o = symbol()) != T_SEMI)
			goto syntax;
		return;

	case T_SEMI:
		oprintf(".bss; %p: .=.+2\n", cs);
		return;

	case T_LBRACK:
		c = 0;
		if ((o = symbol()) == T_CON) {
			c = (int16_t)(cval << 1);
			o = symbol();
		}
		if (o != T_RBRACK)
			goto syntax;
		oprintf(s, cs);
		if ((o = symbol()) == T_SEMI) {
			oprintf(".bss; 1:.=.+%o\n", c);
			return;
		}
		oprintf("1:");
		while (o == T_CON) {
			oprintf("%o\n", cval);
			c -= 2;
			if ((o = symbol()) == T_SEMI)
				goto done;
			if (o != T_COMMA)
				goto syntax;
			else
				o = symbol();
		}
		goto syntax;
	done:
		if (c > 0)
			oprintf(".=.+%o\n", c);
		return;

	case T_EOF:
		return;
	}

syntax:
	error("External definition syntax");
	errflush(o);
	statement(0);
}

void
statement(int d)
{
	int o, o1, o2, o3;
	struct tnode *np;

stmt:
	switch (o = symbol()) {

	case T_EOF:
		error("Unexpected EOF");

	case T_SEMI:
	case T_RBRACE:
		return;

	case T_LBRACE: {
		if (d)
			blkhed();
		while (!eof) {
			if ((o = symbol()) == T_RBRACE)
				goto bend;
			peeksym = o;
			statement(0);
		}
		error("Missing '}'");
	bend:
		return;
	}

	case T_KEYW:
		switch (cval) {

		case K_GOTO:
			np = block(O_GOTO, 0, 0, tree(), NULL);
			rcexpr(np, TAB_REG);
			goto semi;

		case K_RETURN:
			if ((peeksym = symbol()) == T_LPAREN)
				rcexpr(pexpr(), TAB_REG);
			retseq();
			goto semi;

		case K_IF:
			o1 = isn++;
			jumpc(pexpr(), o1, 0);
			statement(0);
			if ((o = symbol()) == T_KEYW && cval == K_ELSE) {
				o2 = isn++;
				(easystmt() ? branch : jump)(o2);
				label(o1);
				statement(0);
				label(o2);
				return;
			}
			peeksym = o;
			label(o1);
			return;

		case K_WHILE:
			o1 = contlab;
			o2 = brklab;
			label(contlab = isn++);
			brklab = isn++;
			jumpc(pexpr(), brklab, 0);
			o3 = easystmt();
			statement(0);
			(o3 ? branch : jump)(contlab);
			label(brklab);
			contlab = o1;
			brklab = o2;
			return;

		case K_BREAK:
			if (brklab == 0)
				error("Nothing to break from");
			jump(brklab);
			goto semi;

		case K_CONTINUE:
			if (contlab == 0)
				error("Nothing to continue");
			jump(contlab);
			goto semi;

		case K_DO:
			o1 = contlab;
			o2 = brklab;
			contlab = isn++;
			brklab = isn++;
			label(o3 = isn++);
			statement(0);
			label(contlab);
			contlab = o1;
			if ((o = symbol()) == T_KEYW && cval == K_WHILE) {
				jumpc(tree(), o3, 1);
				label(brklab);
				brklab = o2;
				goto semi;
			}
			goto syntax;

		case K_CASE:
			if ((o = symbol()) != T_CON)
				goto syntax;
			if ((o = symbol()) != T_COLON)
				goto syntax;
			if (swp < 0) {
				error("Case not in switch");
				goto stmt;
			}
			if (swp + 2 > SWSIZ) {
				error("Switch table overflow");
			} else {
				swtab[swp++] = isn;
				swtab[swp++] = cval;
				label(isn++);
			}
			goto stmt;

		case K_SWITCH:
			o1 = brklab;
			brklab = isn++;
			np = pexpr();
			if (np != NULL && np->type > 1 && np->type < 16)
				error("Integer required");
			rcexpr(np, TAB_REG);
			pswitch();
			brklab = o1;
			return;

		case K_DEFAULT:
			if (swp < 0)
				error("Default not in switch");
			if ((o = symbol()) != T_COLON)
				goto syntax;
			deflab = isn++;
			label(deflab);
			goto stmt;
		}

		error("Unknown keyword");
		goto syntax;

	case T_NAME:
		if (peekc == ':') {
			peekc = 0;
			if (csym->sclass > 0) {
				error("Redefinition");
				goto stmt;
			}
			csym->sclass = SC_LABEL;
			csym->type = 020;
			if (csym->off == 0)
				csym->off = isn++;
			slabel();
			goto stmt;
		}
	}

	peeksym = o;
	rcexpr(tree(), TAB_EFF);
	goto semi;

semi:
	if ((o = symbol()) != T_SEMI)
		goto syntax;
	return;

syntax:
	error("Statement syntax");
	errflush(o);
	goto stmt;
}

struct tnode *
pexpr(void)
{
	int o;
	struct tnode *t;

	if ((o = symbol()) != T_LPAREN)
		goto syntax;
	t = tree();
	if ((o = symbol()) != T_RPAREN)
		goto syntax;
	return t;
syntax:
	error("Statement syntax");
	errflush(o);
	return NULL;
}

static void
pswitch(void)
{
	int sswp, dl, cv, swlab;

	sswp = swp;
	if (swp < 0)
		swp = 0;
	swlab = isn++;
	oprintf("jsr\tpc,bswitch; l%d\n", swlab);
	dl = deflab;
	deflab = 0;
	statement(0);
	if (!deflab) {
		deflab = isn++;
		label(deflab);
	}
	oprintf("L%d:.data;L%d:", brklab, swlab);
	while (swp > sswp && swp > 0) {
		cv = swtab[--swp];
		oprintf("%o; l%d\n", cv, swtab[--swp]);
	}
	oprintf("L%d; 0\n.text\n", deflab);
	deflab = dl;
	swp = sswp;
}

static void
blkhed(void)
{
	int al, pl, hl;
	struct symbol *cs;

	declist();
	stack = al = -2;
	pl = 4;
	while (paraml) {
		parame->plink = NULL;
		cs = paraml;
		paraml = cs->plink;
		cs->off = pl;
		cs->sclass = SC_PARAM;
		pl += rlength(cs->type);
	}

	for (hl = 0; hl < HSHSIZ; hl++) {
		cs = &hshtab[hl];
		if (cs->name[0] == '\0' && cs->name[1] == '\0')
			continue;
		switch (cs->sclass) {

		case SC_SORTLESS:
			cs->sclass = SC_AUTO;

		case SC_AUTO:
			if (cs->dim) {
				al -= (cs->dim * length(cs->type - 020) + 1) & 077776;
				setstk(al);
				defvec(al);
			}
			cs->off = al;
			al -= rlength(cs->type);
			break;

		case SC_PARAM:
			cs->sclass = SC_AUTO;
			break;

		case SC_STATIC:
			cs->off = isn++;
			defstat(cs);
			break;
		}
	}
	setstk(al);
}

void
blkend(void)
{
	int i;
	struct symbol *s;

	for (i = 0; i < HSHSIZ; i++) {
		s = &hshtab[i];
		if (s->name[0] == '\0' && s->name[1] == '\0')
			continue;
		if (s->sclass == SC_UNDEF)
			error("%p undefined", s->name);
		if (s->sclass != SC_KEYWORD) {
			hshused--;
			s->name[0] = s->name[1] = '\0';
		}
	}
}

void
errflush(int o)
{
	while (o > 3)
		o = symbol();
	peeksym = o;
}

void
declist(void)
{
	int o;

	while ((o = symbol()) == T_KEYW && cval < 10)
		declare(cval);
	peeksym = o;
}

int
easystmt(void)
{
	if ((peeksym = symbol()) == T_NAME)
		return peekc != ':';
	if (peeksym == T_KEYW) {
		switch (cval) {

		case K_GOTO:
		case K_RETURN:
		case K_BREAK:
		case K_CONTINUE:
			return 1;
		}
		return 0;
	}
	return peeksym != T_LBRACE;
}

void
branch(int lab)
{
	oprintf("br\tL%d\n", lab);
}
