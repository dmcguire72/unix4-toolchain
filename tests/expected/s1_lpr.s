.globl	_tfname
.comm	_tfname,02
.globl	_nact
.comm	_nact,02
.globl	_tff
.comm	_tff,02
.globl	_first
.comm	_first,02
.globl	_main
.text
_main:
jsr	r5,rsave; 16
clr	-16.(r5)
.data
L2:.byte 057,0165,0163,0162,057,0154,0160,0144,057,0164,0146,0170,0170,0170,0
.even
.text
mov	$L2,(sp)
jsr	pc,*$_ranname
mov	r0,_tfname
tst	_tfname
jeq	L3
mov	$0666,(sp)
mov	_tfname,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,_tff
jbr	L4
L3:mov	$0177777,_tff
L4:tst	_tff
jge	L5
.data
L6:.byte 0103,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,0145,040,0151,0156,040,057,0165,0163,0162,057,0154,0160,0144,012,0
.even
.text
mov	$L6,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L5:jsr	pc,_ident
cmp	4.(r5),$01
jne	L7
clr	(sp)
jsr	pc,*$_copy
L7:L8:dec	4.(r5)
jeq	L9
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),-8.(r5)
movb	*-8.(r5),r0
mov	r0,-12.(r5)
cmp	-12.(r5),$053
jeq	L10000
cmp	-12.(r5),$055
jne	L10
L10000:mov	-12.(r5),-18.(r5)
inc	-8.(r5)
mov	-8.(r5),r0
tstb	(r0)
jne	L11
mov	-18.(r5),-16.(r5)
jbr	L8
L11:jbr	L12
L10:mov	-16.(r5),-18.(r5)
L12:cmp	-18.(r5),$053
jeq	L13
cmpb	*-8.(r5),$057
jne	L14
cmp	-18.(r5),$055
jeq	L14
mov	-8.(r5),(sp)
mov	$0106,-(sp)
jsr	pc,*$_card
tst	(sp)+
inc	_nact
jbr	L8
L14:.data
L15:.byte 057,0165,0163,0162,057,0154,0160,0144,057,0154,0146,0170,0170,0170,0
.even
.text
mov	$L15,(sp)
jsr	pc,*$_ranname
mov	r0,-14.(r5)
tst	-14.(r5)
jeq	L16
mov	-14.(r5),(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_link
tst	(sp)+
tst	r0
jne	L13
mov	-14.(r5),(sp)
mov	$0106,-(sp)
jsr	pc,*$_card
tst	(sp)+
mov	-14.(r5),(sp)
mov	$0125,-(sp)
jsr	pc,*$_card
tst	(sp)+
inc	_nact
jbr	L17
L16:L13:clr	(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-14.(r5)
tst	-14.(r5)
jge	L18
.data
L19:.byte 0103,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L19,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L8
L18:mov	-14.(r5),(sp)
jsr	pc,*$_copy
mov	-14.(r5),(sp)
jsr	pc,*$_close
L17:cmp	-18.(r5),$055
jne	L20
mov	-8.(r5),(sp)
jsr	pc,*$_unlink
mov	r0,-14.(r5)
tst	-14.(r5)
jge	L21
.data
L22:.byte 0103,0141,0156,0156,0157,0164,040,0162,0145,0155,0157,0166,0145,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L22,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L21:L20:jbr	L8
L9:tst	_nact
jeq	L23
.data
L24:.byte 057,0165,0163,0162,057,0154,0160,0144,057,0144,0146,0170,0170,0170,0
.even
.text
mov	$L24,(sp)
jsr	pc,*$_ranname
mov	r0,-14.(r5)
tst	-14.(r5)
jeq	L25
mov	-14.(r5),(sp)
mov	_tfname,-(sp)
jsr	pc,*$_link
tst	(sp)+
jbr	L26
L25:.data
L27:.byte 0103,0141,0156,0156,0157,0164,040,0162,0145,0156,0141,0155,0145,040,0151,0156,040,057,0165,0163,0162,057,0154,0160,0144,012,0
.even
.text
mov	$L27,(sp)
jsr	pc,*$_printf
L26:mov	_tfname,(sp)
jsr	pc,*$_unlink
.data
L28:.byte 057,0145,0164,0143,057,0154,0160,0144,0
.even
.text
.data
L29:.byte 0154,0160,0144,0
.even
.text
clr	(sp)
mov	$L29,-(sp)
mov	$L28,-(sp)
jsr	pc,*$_execl
cmp	(sp)+,(sp)+
L23:mov	_tfname,(sp)
jsr	pc,*$_unlink
L1:jmp	rretrn
.globl	_copy
.text
_copy:
.bss
L31:.=.+512.
.text
jsr	r5,rsave; 14
.data
L32:.byte 057,0165,0163,0162,057,0154,0160,0144,057,0143,0146,0170,0170,0170,0
.even
.text
mov	$L32,(sp)
jsr	pc,*$_ranname
mov	r0,-8.(r5)
tst	-8.(r5)
jeq	L33
mov	$0666,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-10.(r5)
jbr	L34
L33:mov	$0177777,-10.(r5)
L34:tst	-10.(r5)
jge	L35
.data
L36:.byte 0103,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,0145,040,0151,0156,040,057,0165,0163,0162,057,0154,0160,0144,012,0
.even
.text
mov	$L36,(sp)
jsr	pc,*$_printf
jbr	L30
L35:clr	-16.(r5)
clr	-14.(r5)
L37:mov	$01000,(sp)
mov	$L31,-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-12.(r5)
jle	L38
mov	-12.(r5),(sp)
mov	$L31,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
add	-12.(r5),-16.(r5)
cmp	-16.(r5),$01000
jlt	L39
sub	$01000,-16.(r5)
inc	-14.(r5)
cmp	-14.(r5),$0620
jle	L40
.data
L41:.byte 0103,0157,0160,0171,040,0146,0151,0154,0145,040,0151,0163,040,0164,0157,0157,040,0154,0141,0162,0147,0145,012,0
.even
.text
mov	$L41,(sp)
jsr	pc,*$_printf
jbr	L38
L40:L39:jbr	L37
L38:mov	-10.(r5),(sp)
jsr	pc,*$_close
mov	-8.(r5),(sp)
mov	$0106,-(sp)
jsr	pc,*$_card
tst	(sp)+
mov	-8.(r5),(sp)
mov	$0125,-(sp)
jsr	pc,*$_card
tst	(sp)+
inc	_nact
L30:jmp	rretrn
.globl	_card
.text
_card:
.bss
L43:.=.+512.
.text
jsr	r5,rsave; 10
mov	$L43,-8.(r5)
mov	6.(r5),-10.(r5)
clr	-12.(r5)
mov	-8.(r5),r0
inc	-8.(r5)
movb	4.(r5),(r0)
L44:mov	-10.(r5),r0
inc	-10.(r5)
movb	(r0),r0
mov	r0,4.(r5)
jeq	L45
mov	-8.(r5),r0
inc	-8.(r5)
movb	4.(r5),(r0)
inc	-12.(r5)
jbr	L44
L45:mov	-8.(r5),r0
inc	-8.(r5)
movb	$012,(r0)
mov	-12.(r5),(sp)
add	$02,(sp)
mov	$L43,-(sp)
mov	_tff,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L42:jmp	rretrn
.globl	_ident
.text
_ident:
.bss
L47:.=.+100.
.text
.bss
L48:.=.+100.
.text
jsr	r5,rsave; 14
mov	$L47,-16.(r5)
mov	$L47,(sp)
jsr	pc,_getuid
mov	r0,-(sp)
jsr	pc,*$_getpw
tst	(sp)+
tst	r0
jeq	L49
.data
L50:.byte 0160,0144,0160,072,072,072,072,0147,0157,0153,072,0
.even
.text
mov	$L50,-16.(r5)
L49:clr	-12.(r5)
L51:.data
L53:.byte 044,011,0151,0144,0145,0156,0164,011,0
.even
.text
mov	-12.(r5),r0
movb	L53(r0),r0
mov	r0,-8.(r5)
jeq	L52
mov	-12.(r5),r0
inc	-12.(r5)
movb	-8.(r5),L48(r0)
jbr	L51
L52:clr	-10.(r5)
mov	$05,-14.(r5)
L54:dec	-14.(r5)
jeq	L55
L56:mov	-10.(r5),r0
inc	-10.(r5)
add	-16.(r5),r0
movb	(r0),r0
cmp	$072,r0
jeq	L57
jbr	L56
L57:jbr	L54
L55:L58:mov	-10.(r5),r0
inc	-10.(r5)
add	-16.(r5),r0
movb	(r0),r0
mov	r0,-8.(r5)
cmp	$072,r0
jeq	L59
mov	-12.(r5),r0
inc	-12.(r5)
movb	-8.(r5),L48(r0)
jbr	L58
L59:mov	-12.(r5),r0
inc	-12.(r5)
movb	$054,L48(r0)
clr	-10.(r5)
L60:mov	-10.(r5),r0
inc	-10.(r5)
add	-16.(r5),r0
movb	(r0),r0
mov	r0,-8.(r5)
cmp	$072,r0
jeq	L61
mov	-12.(r5),r0
inc	-12.(r5)
movb	-8.(r5),L48(r0)
jbr	L60
L61:mov	-12.(r5),r0
inc	-12.(r5)
clrb	L48(r0)
mov	$L48,(sp)
mov	$0114,-(sp)
jsr	pc,*$_card
tst	(sp)+
L46:jmp	rretrn
.globl	_ranname
.text
_ranname:
.bss
L63:.=.+40.
.text
jsr	r5,rsave; 2
L64:jsr	pc,_ranc
mov	4.(r5),r1
movb	r0,11.(r1)
jsr	pc,_ranc
mov	4.(r5),r1
movb	r0,12.(r1)
jsr	pc,_ranc
mov	4.(r5),r1
movb	r0,13.(r1)
mov	$L63,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jeq	L65
mov	4.(r5),r0
jbr	L62
L65:jbr	L64
L62:jmp	rretrn
.globl	_ranc
.text
_ranc:
jsr	r5,rsave; 10
tst	_first
jne	L67
mov	r5,r0
add	$0177766,r0
mov	r0,(sp)
jsr	pc,*$_time
mov	2.+-10.(r5),(sp)
jsr	pc,*$_srand
inc	_first
L67:jsr	pc,_rand
mov	r0,-12.(r5)
mov	-12.(r5),r1
ash	$-013,r1
sxt	r0
div	$012,r0
add	$060,r1
mov	r1,r0
jbr	L66
L66:jmp	rretrn
