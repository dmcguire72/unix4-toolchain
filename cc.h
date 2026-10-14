/*
 * Copyright 1972 Bell Telephone Laboratories, Inc.
 */
#ifndef CC_H
#define CC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMSIZ	8
#define TSIZE	300

enum {
	T_EOF	= 0,
	T_SEMI	= 1,	/* ;  */
	T_LBRACE= 2,	/* {  */
	T_RBRACE= 3,	/* }  */
	T_LBRACK= 4,	/* [  */
	T_RBRACK= 5,	/* ]  */
	T_LPAREN= 6,	/* (  */
	T_RPAREN= 7,	/* )  */
	T_COLON	= 8,	/* :  */
	T_COMMA	= 9,	/* ,  */
	T_KEYW	= 19,	/* keyword */
	T_NAME	= 20,	/* identifier */
	T_CON	= 21,	/* short constant */
	T_STRING= 22,	/* string constant */
	O_INCBEF= 30,	/* ++x */
	O_DECBEF= 31,	/* --x */
	O_INCAFT= 32,	/* x++ */
	O_DECAFT= 33,	/* x-- */
	O_NOT	= 34,	/* !   */
	O_ADDR	= 35,	/* &x  (unary) */
	O_IND	= 36,	/* *x  (unary) */
	O_NEG	= 37,	/* -x  (unary) */
	O_COM	= 38,	/* ~x  */
	O_ADDRA	= 29,
	O_PLUS	= 40,
	O_MINUS	= 41,
	O_TIMES	= 42,
	O_DIV	= 43,
	O_MOD	= 44,
	O_RSH	= 45,
	O_LSH	= 46,
	O_AND	= 47,
	O_OR	= 48,
	O_XOR	= 49,
	O_EQ	= 60,
	O_NE	= 61,
	O_LE	= 62,
	O_LT	= 63,
	O_GE	= 64,
	O_GT	= 65,
	O_ASPLUS= 70,	/* =+  ... =^ are 70..79 */
	O_ASSIGN= 80,	/* =   */
	O_QUEST	= 90,	/* ?   */
	O_CALL	= 100,
	O_MCALL	= 101,	/* call, after args have been pushed (pass 2) */
	O_GOTO	= 102,
	O_JUMPC	= 103,	/* conditional jump, long form */
	O_CBR	= 104	/* conditional jump, short (br) form */
};

enum {
	SC_UNDEF	= 0,
	SC_KEYWORD	= 1,
	SC_LABEL	= 2,
	SC_AUTO		= 5,
	SC_EXTERN	= 6,
	SC_STATIC	= 7,
	SC_PARAM	= 10,
	SC_PARAMDECL	= -1,
	SC_SORTLESS	= -2,
	SC_PLINKED	= 99
};

enum { TAB_REG, TAB_EFF, TAB_CC, TAB_SP };

struct tnode {
	int16_t op;
	int16_t type;
	int16_t degree;
	union {
		struct {
			struct tnode *p1, *p2;
		};
		struct {
			int16_t sclass;
			union {
				int16_t off;
				char name[NAMSIZ];
			};
		};
		int16_t cval;
	};
};

extern FILE *fin, *fout;
extern const char *tmpfil;
extern int line;
extern int nerror;

int	rdchar(void);
void	wrchar(int c);
void	flush(void);
void	oprintf(const char *fmt, ...);
void	error(const char *fmt, ...);
int	length(int t);
int	rlength(int t);
void	jump(int lab);
void	label(int l);
void	putwrd(int w);
int	getwrd(void);
void	flshw(void);

extern const int opdope[];

#endif
