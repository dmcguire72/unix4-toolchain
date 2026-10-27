.globl	_proc
.comm	_proc,01750
.globl	_partab
.comm	_partab,02
.globl	_u
.comm	_u,0362
.globl	_nl
.comm	_nl,074
.globl	_proc
.comm	_proc,024
.globl	_u
.comm	_u,0362
.globl	_lflg
.comm	_lflg,02
.globl	_kflg
.comm	_kflg,02
.globl	_xflg
.comm	_xflg,02
.globl	_tflg
.comm	_tflg,02
.globl	_aflg
.comm	_aflg,02
.globl	_mem
.comm	_mem,02
.globl	_swap
.comm	_swap,02
.globl	_stbuf
.comm	_stbuf,01002
.globl	_coref
.comm	_coref,02
.globl	_main
.text
_main:
jsr	r5,rsave; 22
cmp	4.(r5),$01
jle	L2
mov	6.(r5),r0
mov	2.(r0),-20.(r5)
L3:tstb	*-20.(r5)
jeq	L4
mov	-20.(r5),r0
inc	-20.(r5)
movb	(r0),r0
jbr	L6
L7:inc	_aflg
jbr	L5
L8:inc	_tflg
jbr	L5
L9:inc	_xflg
jbr	L5
L10:inc	_lflg
jbr	L5
L11:inc	_kflg
jbr	L5
jbr	L5
L6:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:141
153
154
164
170
L10002:..
L7
L11
L10
L8
L9
L12
.text
/esw
L5:L12:jbr	L3
L4:L2:.data
L13:.byte 0137,0160,0162,0157,0143,0
.even
.text
mov	$L13,(sp)
mov	$_nl,-(sp)
jsr	pc,*$_setup
tst	(sp)+
.data
L14:.byte 0137,0144,0143,061,061,0
.even
.text
mov	$L14,(sp)
mov	$12.+_nl,-(sp)
jsr	pc,*$_setup
tst	(sp)+
.data
L15:.byte 0137,0153,0154,061,061,0
.even
.text
mov	$L15,(sp)
mov	$24.+_nl,-(sp)
jsr	pc,*$_setup
tst	(sp)+
.data
L16:.byte 0137,0144,0150,061,061,0
.even
.text
mov	$L16,(sp)
mov	$36.+_nl,-(sp)
jsr	pc,*$_setup
tst	(sp)+
.data
L17:.byte 057,0165,0156,0151,0170,0
.even
.text
mov	$_nl,(sp)
mov	$L17,-(sp)
jsr	pc,*$_nlist
tst	(sp)+
tst	8.+_nl
jne	L18
.data
L19:.byte 0116,0157,040,0156,0141,0155,0145,0154,0151,0163,0164,012,0
.even
.text
mov	$L19,(sp)
jsr	pc,*$_printf
jbr	L1
L18:.data
L20:.byte 057,0144,0145,0166,057,0155,0145,0155,0
.even
.text
mov	$L20,_coref
tst	_kflg
jeq	L21
.data
L22:.byte 057,0144,0145,0166,057,0162,0153,060,0
.even
.text
mov	$L22,_coref
L21:clr	(sp)
mov	_coref,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-22.(r5)
jge	L23
.data
L24:.byte 0116,0157,040,0155,0145,0155,012,0
.even
.text
mov	$L24,(sp)
jsr	pc,*$_printf
jbr	L1
L23:clr	(sp)
mov	_coref,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_mem
.data
L25:.byte 057,0144,0145,0166,057,0162,0146,060,0
.even
.text
clr	(sp)
mov	$L25,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_swap
mov	$20.+_proc,-10.(r5)
mov	$_proc,-12.(r5)
sub	-12.(r5),-10.(r5)
clr	(sp)
mov	10.+_nl,-(sp)
mov	-22.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
clr	(sp)
jsr	pc,*$_ttyn
mov	r0,-18.(r5)
clr	-14.(r5)
L26:cmp	-14.(r5),$062
jge	L27
mov	-10.(r5),(sp)
mov	$_proc,-(sp)
mov	-22.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
tstb	_proc
jeq	L28
mov	6.+_proc,(sp)
jsr	pc,*$_ttyc
mov	r0,-16.(r5)
cmp	$077,r0
jne	L10004
tst	_xflg
jeq	L28
L10004:cmp	-18.(r5),-16.(r5)
jeq	L10005
tst	_aflg
jeq	L28
L10005:tst	_lflg
jeq	L29
.data
L30:.byte 045,063,0157,045,065,0144,045,065,0144,0
.even
.text
movb	5.+_proc,r0
mov	r0,(sp)
movb	2.+_proc,r0
mov	r0,-(sp)
movb	1.+_proc,r0
ash	$03,r0
mov	r0,-(sp)
bisb	_proc,(sp)
mov	$L30,-(sp)
jsr	pc,*$_printf
add	$06,sp
L29:tst	_aflg
jeq	L31
.data
L32:.byte 045,062,0143,0
.even
.text
mov	-16.(r5),(sp)
mov	$L32,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L31:.data
L33:.byte 045,066,0144,0
.even
.text
mov	8.+_proc,(sp)
mov	$L33,-(sp)
jsr	pc,*$_printf
tst	(sp)+
tst	_lflg
jeq	L34
.data
L35:.byte 045,064,0144,0
.even
.text
mov	14.+_proc,r0
add	$07,r0
ash	$-03,r0
mov	r0,(sp)
mov	$L35,-(sp)
jsr	pc,*$_printf
tst	(sp)+
tst	16.+_proc
jeq	L36
.data
L37:.byte 045,070,0157,0
.even
.text
mov	16.+_proc,(sp)
mov	$L37,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L38
L36:.data
L39:.byte 040,040,040,040,040,040,040,040,0
.even
.text
mov	$L39,(sp)
jsr	pc,*$_printf
L38:L34:jsr	pc,_prcom
.data
L40:.byte 012,0
.even
.text
mov	$L40,(sp)
jsr	pc,*$_printf
L28:inc	-14.(r5)
jbr	L26
L27:L1:jmp	rretrn
.globl	_ttyc
.text
_ttyc:
jsr	r5,rsave; 4
mov	4.(r5),r1
sub	22.+_nl,r1
sxt	r0
div	$036,r0
mov	r0,-8.(r5)
tst	-8.(r5)
jlt	L42
cmp	-8.(r5),$012
jge	L42
cmp	-8.(r5),$010
jge	L43
mov	-8.(r5),r0
add	$060,r0
jbr	L41
L43:mov	-8.(r5),r0
add	$0131,r0
jbr	L41
L42:mov	4.(r5),r1
sub	34.+_nl,r1
sxt	r0
div	$036,r0
mov	r0,-8.(r5)
tst	-8.(r5)
jlt	L44
cmp	-8.(r5),$04
jge	L44
mov	-8.(r5),r0
add	$070,r0
jbr	L41
L44:mov	4.(r5),r1
sub	46.+_nl,r1
sxt	r0
div	$036,r0
mov	r0,-8.(r5)
tst	-8.(r5)
jlt	L45
cmp	-8.(r5),$020
jge	L45
mov	-8.(r5),r0
add	$0146,r0
jbr	L41
L45:mov	$077,r0
jbr	L41
L41:jmp	rretrn
.globl	_setup
.text
_setup:
jsr	r5,rsave; 2
L47:mov	4.(r5),r0
inc	4.(r5)
mov	6.(r5),r1
inc	6.(r5)
movb	(r1),(r0)
jeq	L48
jbr	L47
L48:L46:jmp	rretrn
.globl	_prcom
.text
_prcom:
jsr	r5,rsave; 20
clr	-8.(r5)
clr	-10.(r5)
bitb	$01,1.+_proc
jeq	L50
mov	12.+_proc,-10.(r5)
mov	_mem,-12.(r5)
jbr	L51
L50:mov	12.+_proc,-8.(r5)
mov	_swap,-12.(r5)
L51:mov	14.+_proc,r0
add	$0177770,r0
add	r0,-10.(r5)
mov	-10.(r5),r0
ash	$-03,r0
add	r0,-8.(r5)
mov	-10.(r5),r0
bic	$!07,r0
ash	$06,r0
mov	r0,-10.(r5)
mov	$03,(sp)
mov	-8.(r5),-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01,(sp)
mov	-10.(r5),-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	$_stbuf,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L52
clr	r0
jbr	L49
L52:mov	$512.+_stbuf,-14.(r5)
L53:cmp	-14.(r5),$_stbuf
jlos	L54
sub	$02,-14.(r5)
mov	-14.(r5),r0
cmp	(r0),$0177777
jne	L55
mov	-14.(r5),r0
add	$02,r0
mov	r0,-16.(r5)
tstb	*-16.(r5)
jne	L56
inc	-16.(r5)
L56:mov	-16.(r5),-18.(r5)
L57:cmp	-18.(r5),$512.+_stbuf
jhis	L58
movb	*-18.(r5),r0
mov	r0,-20.(r5)
tst	-20.(r5)
jne	L60
movb	$040,*-18.(r5)
jbr	L61
L60:cmp	-20.(r5),$040
jlt	L10006
cmp	-20.(r5),$0176
jle	L62
L10006:movb	$077,*-18.(r5)
L62:L61:L59:inc	-18.(r5)
jbr	L57
L58:.data
L63:.byte 040,045,056,063,062,0163,040,0
.even
.text
mov	-16.(r5),(sp)
mov	$L63,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$01,r0
jbr	L49
L55:jbr	L53
L54:clr	r0
jbr	L49
L49:jmp	rretrn
