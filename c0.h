/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#ifndef C0_H
#define C0_H

#include "cc.h"

#define HSHSIZ	100
#define CMSIZ	40
#define OPSIZ	20
#define SWSIZ	120	

enum {
	C_EOF	= 0,
	C_STRQ	= 122,
	C_CHRQ	= 121,
	C_LETTER= 123,
	C_DIGIT	= 124,
	C_NL	= 125,
	C_SPACE	= 126,
	C_UNKN	= 127
};

enum {
	CH_PLUS	= 40,
	CH_MINUS= 41,
	CH_STAR	= 42,
	CH_SLASH= 43,
	CH_PCT	= 44,
	CH_LT	= 63,
	CH_GT	= 65,
	CH_NOT	= 34,
	CH_EQ	= 80
};

enum {
	K_INT = 0, K_CHAR = 1, K_FLOAT = 2, K_DOUBLE = 3,
	K_AUTO = 5, K_EXTERN = 6, K_STATIC = 7, K_PARAM = 8,
	K_GOTO = 10, K_RETURN = 11, K_IF = 12, K_WHILE = 13, K_ELSE = 14,
	K_SWITCH = 15, K_CASE = 16, K_BREAK = 17, K_CONTINUE = 18,
	K_DO = 19, K_DEFAULT = 20
};

#define D_BINARY	01
#define D_LVAL		02
#define D_REL		04
#define D_ASSIGN	010
#define D_WORDL		020
#define D_WORDR		040
#define D_COMM		0100
#define D_RASSOC	0200
#define D_PREC(d)	(((d) >> 9) & 077)

struct symbol {
	int16_t sclass;
	int16_t type;
	int16_t off;
	int16_t dim;
	char name[NAMSIZ];
	struct symbol *plink;
};

extern const unsigned char ctab[128];
extern const unsigned char cvtab[];

extern int peeksym, peekc, eof;
extern int16_t cval;
extern int ctyp;
extern int isn;
extern struct symbol *csym;
extern int nreg, ncpw;
extern int swp;
extern int contlab, brklab, deflab;
extern int stack;
extern struct symbol *paraml, *parame;
extern struct symbol hshtab[HSHSIZ];
extern int hshused;
extern struct tnode *cmst[CMSIZ], **cp;
extern const int maprel[10];
extern int swtab[SWSIZ];
extern struct tnode ospace[TSIZE];
extern int space;

int	symbol(void);
struct tnode *tree(void);
void	declare(int kw);
struct symbol *lookup(void);
int	subseq(int c, int a, int b);

void	build(int op);
struct tnode *convert(struct tnode *p, int t, int d, int cvn);
void	chkw(struct tnode *p);
int	lintyp(int t);
struct tnode *block(int op, int t, int d, struct tnode *p1, struct tnode *p2);
struct tnode *conblock(int op, int t, int val);
struct tnode *nameblock(struct symbol *s);
void	chklval(struct tnode *p);
int	max(int a, int b);

void	extdef(void);
void	blkend(void);
void	errflush(int o);
void	function(void);
void	statement(int d);
struct tnode *pexpr(void);
void	declist(void);
int	easystmt(void);
void	branch(int lab);

void	jumpc(struct tnode *tree, int lbl, int cond);
void	rcexpr(struct tnode *tree, int table);
void	retseq(void);
void	slabel(void);
void	setstk(int a);
void	defvec(int a);
void	defstat(struct symbol *s);

#endif
