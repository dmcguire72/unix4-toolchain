.globl	_cvtab
.comm	_cvtab,02
.globl	_opdope
.comm	_opdope,02
.globl	_ctab
.comm	_ctab,02
.globl	_symbuf
.comm	_symbuf,010
.globl	_hshused
.comm	_hshused,02
.globl	_hshtab
.comm	_hshtab,06200
.globl	_space
.comm	_space,02
.globl	_cp
.comm	_cp,02
.globl	_cmst
.comm	_cmst,0120
.globl	_isn
.comm	_isn,02
.globl	_swtab
.comm	_swtab,01440
.globl	_swp
.comm	_swp,02
.globl	_contlab
.comm	_contlab,02
.globl	_brklab
.comm	_brklab,02
.globl	_retlab
.comm	_retlab,02
.globl	_deflab
.comm	_deflab,02
.globl	_nauto
.comm	_nauto,02
.globl	_autolen
.comm	_autolen,02
.globl	_peeksym
.comm	_peeksym,02
.globl	_peekc
.comm	_peekc,02
.globl	_eof
.comm	_eof,02
.globl	_line
.comm	_line,02
.globl	_treebase
.comm	_treebase,02
.globl	_debug
.comm	_debug,02
.globl	_defsym
.comm	_defsym,02
.globl	_funcsym
.comm	_funcsym,02
.globl	_xdflg
.comm	_xdflg,02
.globl	_proflg
.comm	_proflg,02
.globl	_csym
.comm	_csym,02
.globl	_cval
.comm	_cval,02
.globl	_fcval
.comm	_fcval,010
.globl	_nchstr
.comm	_nchstr,02
.globl	_nerror
.comm	_nerror,02
.globl	_paraml
.comm	_paraml,02
.globl	_parame
.comm	_parame,02
.globl	_strflg
.comm	_strflg,02
.globl	_osleft
.comm	_osleft,02
.globl	_mosflg
.comm	_mosflg,02
.globl	_initflg
.comm	_initflg,02
.globl	_inhdr
.comm	_inhdr,02
.globl	_dimtab
.comm	_dimtab,0310
.globl	_binbuf
.comm	_binbuf,01006
.globl	_ascbuf
.comm	_ascbuf,01006
.globl	_dimp
.comm	_dimp,02
.globl	_regvar
.comm	_regvar,02
.globl	_decref
.text
_decref:
jsr	r5,rsave; 2
mov	4.(r5),r4
bit	$0177770,r4
jne	L2
.data
L3:.byte 0111,0154,0154,0145,0147,0141,0154,040,0151,0156,0144,0151,0162,0145,0143,0164,0151,0157,0156,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_error
mov	r4,r0
jbr	L1
L2:mov	r4,r0
ash	$-02,r0
bic	$!0177770,r0
mov	r4,r1
bic	$!07,r1
bis	r1,r0
jbr	L1
L1:jmp	rretrn
.globl	_incref
.text
_incref:
jsr	r5,rsave; 2
mov	4.(r5),r0
ash	$02,r0
bic	$!0177743,r0
mov	4.(r5),r1
bic	$!07,r1
bis	r1,r0
bis	$010,r0
jbr	L4
L4:jmp	rretrn
.globl	_cbranch
.text
_cbranch:
jsr	r5,rsave; 2
mov	$02,(sp)
mov	8.(r5),-(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
mov	$0147,-(sp)
mov	$01,-(sp)
jsr	pc,_block
add	$012,sp
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
L5:jmp	rretrn
.globl	_rcexpr
.text
_rcexpr:
jsr	r5,rsave; 2
tst	4.(r5)
jeq	L6
mov	$043,(sp)
jsr	pc,*$_putchar
mov	_space,r0
sub	_treebase,r0
asr	r0
mov	r0,r4
mov	_treebase,r3
mov	$_binbuf,(sp)
mov	r4,-(sp)
jsr	pc,*$_putw
tst	(sp)+
mov	$_binbuf,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_putw
tst	(sp)+
mov	$_binbuf,(sp)
mov	6.(r5),-(sp)
jsr	pc,*$_putw
tst	(sp)+
mov	$_binbuf,(sp)
mov	_line,-(sp)
jsr	pc,*$_putw
tst	(sp)+
L7:mov	r4,r0
dec	r4
tst	r0
jeq	L8
mov	$_binbuf,(sp)
mov	(r3)+,-(sp)
jsr	pc,*$_putw
tst	(sp)+
jbr	L7
L8:L6:jmp	rretrn
.globl	_branch
.text
_branch:
jsr	r5,rsave; 2
.data
L10:.byte 0152,0142,0162,011,0114,045,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L10,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L9:jmp	rretrn
.globl	_label
.text
_label:
jsr	r5,rsave; 2
.data
L12:.byte 0114,045,0144,072,0
.even
.text
mov	4.(r5),(sp)
mov	$L12,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L11:jmp	rretrn
.globl	_plength
.text
_plength:
jsr	r5,rsave; 2
mov	4.(r5),r2
mov	2.(r2),r0
mov	r0,r4
bit	$0177770,r0
jne	L14
mov	$01,r0
jbr	L13
L14:mov	r4,(sp)
jsr	pc,*$_decref
mov	r0,2.(r2)
mov	r2,(sp)
jsr	pc,*$_length
mov	r0,r3
mov	r4,2.(r2)
mov	r3,r0
jbr	L13
L13:jmp	rretrn
.globl	_length
.text
_length:
jsr	r5,rsave; 2
mov	4.(r5),r2
mov	2.(r2),r4
mov	$01,r3
L16:mov	r4,r0
bic	$!030,r0
cmp	$030,r0
jne	L17
mov	r4,(sp)
jsr	pc,*$_decref
mov	r0,r4
movb	4.(r2),r0
bic	$!0377,r0
asl	r0
mov	_dimtab(r0),r3
jbr	L16
L17:mov	r4,r0
bic	$!0177770,r0
cmp	$020,r0
jne	L18
clr	r0
jbr	L15
L18:cmp	r4,$010
jlt	L19
mov	r3,r0
asl	r0
jbr	L15
L19:mov	r4,r0
bic	$!07,r0
jbr	L21
L22:mov	r3,r0
asl	r0
jbr	L15
L23:mov	r3,r0
jbr	L15
L24:mov	r3,r0
ash	$02,r0
jbr	L15
L25:mov	r3,r0
ash	$03,r0
jbr	L15
L26:movb	5.(r2),r1
bic	$!0377,r1
asl	r1
mov	_dimtab(r1),r1
mul	r3,r1
mov	r1,r0
jbr	L15
L27:.data
L28:.byte 0102,0141,0144,040,0163,0164,0162,0165,0143,0164,0165,0162,0145,0
.even
.text
mov	$L28,(sp)
jsr	pc,*$_error
clr	r0
jbr	L15
jbr	L20
L21:cmp	r0,$05
jhi	L29
asl	r0
jmp	*.+4(r0)
L22
L23
L24
L25
L26
L27
/esw
L20:L29:.data
L30:.byte 0103,0157,0155,0160,0151,0154,0145,0162,040,0145,0162,0162,0157,0162,040,050,0154,0145,0156,0147,0164,0150,051,0
.even
.text
mov	$L30,(sp)
jsr	pc,*$_error
L15:jmp	rretrn
.globl	_rlength
.text
_rlength:
jsr	r5,rsave; 2
mov	4.(r5),(sp)
jsr	pc,*$_length
mov	r0,r4
bit	$01,r0
jeq	L32
inc	r4
L32:mov	r4,r0
jbr	L31
L31:jmp	rretrn
.globl	_simplego
.text
_simplego:
jsr	r5,rsave; 2
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$024,r0
jne	L34
jsr	pc,_nextchar
cmp	$073,r0
jne	L34
mov	_csym,r4
tst	(r4)
jne	L35
tst	2.(r4)
jne	L35
mov	$030,2.(r4)
tst	6.(r4)
jne	L36
mov	_isn,r0
inc	_isn
mov	r0,6.(r4)
L36:L35:tst	(r4)
jeq	L10001
cmp	(r4),$07
jne	L37
L10001:cmp	2.(r4),$030
jne	L37
mov	$0177777,_peeksym
mov	6.(r4),r0
jbr	L33
L37:L34:clr	r0
jbr	L33
L33:jmp	rretrn
.globl	_nextchar
.text
_nextchar:
jsr	r5,rsave; 2
L39:mov	_peekc,r0
movb	_ctab(r0),r0
cmp	$0176,r0
jne	L40
jsr	pc,_getchar
mov	r0,_peekc
jbr	L39
L40:mov	_peekc,r0
jbr	L38
L38:jmp	rretrn
.globl	_chconbrk
.text
_chconbrk:
jsr	r5,rsave; 2
tst	4.(r5)
jne	L42
.data
L43:.byte 0102,0162,0145,0141,0153,057,0143,0157,0156,0164,0151,0156,0165,0145,040,0145,0162,0162,0157,0162,0
.even
.text
mov	$L43,(sp)
jsr	pc,*$_error
L42:L41:jmp	rretrn
.globl	_dogoto
.text
_dogoto:
jsr	r5,rsave; 2
jsr	pc,_tree
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
mov	$044,(sp)
jsr	pc,*$_build
sub	$02,_cp
mov	_cp,r0
mov	(r0),r0
mov	r0,r4
mov	r0,(sp)
jsr	pc,*$_chkw
clr	(sp)
mov	r4,-(sp)
clr	-(sp)
clr	-(sp)
mov	$0146,-(sp)
mov	$01,-(sp)
jsr	pc,_block
add	$012,sp
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
L44:jmp	rretrn
.globl	_doret
.text
_doret:
jsr	r5,rsave; 2
jsr	pc,_nextchar
cmp	$073,r0
jeq	L46
clr	(sp)
jsr	pc,_tree
mov	r0,-(sp)
clr	-(sp)
clr	-(sp)
mov	$0156,-(sp)
mov	$01,-(sp)
jsr	pc,_block
add	$012,sp
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
L46:mov	_retlab,(sp)
jsr	pc,*$_branch
L45:jmp	rretrn
.globl	_putchar
.text
_putchar:
jsr	r5,rsave; 2
mov	$_ascbuf,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_putc
tst	(sp)+
L47:jmp	rretrn
