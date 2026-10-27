.globl	_ap
.comm	_ap,02
.globl	_ac
.comm	_ac,02
.globl	_av
.comm	_av,02
.globl	_main
.text
_main:
jsr	r5,rsave; 2
mov	4.(r5),r0
asl	r0
add	6.(r5),r0
clr	(r0)
mov	4.(r5),_ac
mov	6.(r5),_av
mov	$01,_ap
cmp	4.(r5),$02
jlt	L1
jsr	pc,_exp
tst	r0
jeq	L2
clr	(sp)
jsr	pc,*$_doex
tst	r0
jeq	L3
.data
L4:.byte 0156,0157,040,0143,0157,0155,0155,0141,0156,0144,012,0
.even
.text
mov	$013,(sp)
mov	$L4,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$02,(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L3:L2:L1:jmp	rretrn
.globl	_nxtarg
.text
_nxtarg:
jsr	r5,rsave; 2
cmp	_ap,_ac
jle	L6
clr	r0
jbr	L5
L6:mov	_ap,r0
inc	_ap
asl	r0
add	_av,r0
mov	(r0),r0
jbr	L5
L5:jmp	rretrn
.globl	_exp
.text
_exp:
jsr	r5,rsave; 4
jsr	pc,_e1
mov	r0,-8.(r5)
.data
L8:.byte 055,0157,0
.even
.text
mov	$L8,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L9
jsr	pc,_exp
bis	-8.(r5),r0
jbr	L7
L9:dec	_ap
mov	-8.(r5),r0
jbr	L7
L7:jmp	rretrn
.globl	_e1
.text
_e1:
jsr	r5,rsave; 4
jsr	pc,_e2
mov	r0,-8.(r5)
.data
L11:.byte 055,0141,0
.even
.text
mov	$L11,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L12
jsr	pc,_e1
mov	-8.(r5),r1
com	r1
bic	r1,r0
jbr	L10
L12:dec	_ap
mov	-8.(r5),r0
jbr	L10
L10:jmp	rretrn
.globl	_e2
.text
_e2:
jsr	r5,rsave; 2
.data
L14:.byte 041,0
.even
.text
mov	$L14,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L15
jsr	pc,_e3
tst	r0
beq	1f
clr	r0
br	2f
1:	mov	$1,r0
2:
jbr	L13
L15:dec	_ap
jsr	pc,_e3
jbr	L13
L13:jmp	rretrn
.globl	_e3
.text
_e3:
jsr	r5,rsave; 6
clr	-10.(r5)
jsr	pc,_nxtarg
mov	r0,r4
jeq	L17
.data
L18:.byte 050,0
.even
.text
mov	$L18,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L19
jsr	pc,_exp
mov	r0,-8.(r5)
.data
L20:.byte 051,0
.even
.text
mov	$L20,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L17
mov	-8.(r5),r0
jbr	L16
L19:.data
L21:.byte 055,0162,0
.even
.text
mov	$L21,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L22
clr	(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_tio
tst	(sp)+
jbr	L16
L22:.data
L23:.byte 055,0167,0
.even
.text
mov	$L23,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L24
mov	$01,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_tio
tst	(sp)+
jbr	L16
L24:.data
L25:.byte 055,0143,0
.even
.text
mov	$L25,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L26
jsr	pc,_nxtarg
mov	r0,(sp)
jsr	pc,*$_tcreat
jbr	L16
L26:.data
L27:.byte 0173,0
.even
.text
mov	$L27,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L28
jsr	pc,_fork
tst	r0
jeq	L29
mov	r5,r0
add	$0177766,r0
mov	r0,(sp)
jsr	pc,*$_wait
jbr	L30
L29:mov	$01,(sp)
jsr	pc,*$_doex
jbr	L17
L30:L31:.data
L33:.byte 0175,0
.even
.text
jsr	pc,_nxtarg
mov	r0,r4
jeq	L32
mov	$L33,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jne	L32
jbr	L31
L32:tst	-10.(r5)
jeq	L10000
clr	r0
jbr	L10001
L10000:mov	$01,r0
L10001:jbr	L16
L28:jsr	pc,_nxtarg
mov	r0,-8.(r5)
tst	-8.(r5)
jeq	L17
.data
L34:.byte 075,0
.even
.text
mov	$L34,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L35
jsr	pc,_nxtarg
mov	r0,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
jbr	L16
L35:.data
L36:.byte 041,075,0
.even
.text
mov	$L36,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jeq	L37
jsr	pc,_nxtarg
mov	r0,(sp)
mov	r4,-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
beq	1f
clr	r0
br	2f
1:	mov	$1,r0
2:
jbr	L16
L37:L17:.data
L38:.byte 0151,0146,040,0145,0162,0162,0157,0162,012,0
.even
.text
mov	$011,(sp)
mov	$L38,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$011,(sp)
jsr	pc,*$_exit
L16:jmp	rretrn
.globl	_tio
.text
_tio:
jsr	r5,rsave; 2
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,4.(r5)
tst	4.(r5)
jlt	L40
mov	4.(r5),(sp)
jsr	pc,*$_close
mov	$01,r0
jbr	L39
L40:clr	r0
jbr	L39
L39:jmp	rretrn
.globl	_tcreat
.text
_tcreat:
jsr	r5,rsave; 2
mov	$01,r0
jbr	L41
L41:jmp	rretrn
.globl	_eq
.text
_eq:
jsr	r5,rsave; 2
clr	r4
L43:mov	4.(r5),r0
add	r4,r0
mov	6.(r5),r1
add	r4,r1
cmpb	(r0),(r1)
jeq	L44
clr	r0
jbr	L42
L44:mov	r4,r0
inc	r4
add	4.(r5),r0
tstb	(r0)
jne	L45
mov	$01,r0
jbr	L42
L45:jbr	L43
L42:jmp	rretrn
.globl	_doex
.text
_doex:
jsr	r5,rsave; 152
clr	r4
L47:jsr	pc,_nxtarg
mov	r0,-110.(r5)
jeq	L48
.data
L49:.byte 0175,0
.even
.text
tst	4.(r5)
jeq	L10002
mov	$L49,(sp)
mov	-110.(r5),-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jne	L48
L10002:mov	r4,r0
inc	r4
asl	r0
add	r5,r0
mov	-110.(r5),-106.(r0)
jbr	L47
L48:.data
L50:.byte 0175,0
.even
.text
tst	4.(r5)
jeq	L51
mov	$L50,(sp)
mov	-110.(r5),-(sp)
jsr	pc,*$_eq
tst	(sp)+
tst	r0
jne	L51
mov	$011,r0
jbr	L46
L51:mov	r4,r0
asl	r0
add	r5,r0
clr	-106.(r0)
tst	r4
jne	L52
mov	4.(r5),r0
jbr	L46
L52:mov	r4,(sp)
mov	r5,r0
add	$0177626,r0
mov	r0,-(sp)
mov	-106.(r5),-(sp)
jsr	pc,*$_execv
cmp	(sp)+,(sp)+
clr	r3
.data
L53:.byte 057,0165,0163,0162,057,0142,0151,0156,057,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0
.even
.text
mov	$L53,-108.(r5)
L54:mov	-106.(r5),r0
add	r3,r0
movb	(r0),r2
jeq	L55
mov	r3,r0
inc	r3
add	-108.(r5),r0
movb	r2,9.(r0)
jbr	L54
L55:mov	-108.(r5),r0
add	r3,r0
clrb	9.(r0)
mov	r4,(sp)
mov	r5,r0
add	$0177626,r0
mov	r0,-(sp)
mov	-108.(r5),-(sp)
add	$04,(sp)
jsr	pc,*$_execv
cmp	(sp)+,(sp)+
mov	r4,(sp)
mov	r5,r0
add	$0177626,r0
mov	r0,-(sp)
mov	-108.(r5),-(sp)
jsr	pc,*$_execv
cmp	(sp)+,(sp)+
mov	$01,r0
jbr	L46
L46:jmp	rretrn
