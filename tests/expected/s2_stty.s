.globl	_arg
.comm	_arg,02
.globl	_mode
.comm	_mode,06
.globl	_main
.text
_main:
jsr	r5,rsave; 4
mov	$_mode,(sp)
mov	$01,-(sp)
jsr	pc,*$_gtty
tst	(sp)+
L2:dec	4.(r5)
jle	L3
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),_arg
.data
L4:.byte 0145,0166,0145,0156,0
.even
.text
mov	$L4,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L5
mov	$0200,(sp)
jsr	pc,*$_set
L5:.data
L6:.byte 055,0145,0166,0145,0156,0
.even
.text
mov	$L6,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L7
mov	$0200,(sp)
jsr	pc,*$_reset
L7:.data
L8:.byte 0157,0144,0144,0
.even
.text
mov	$L8,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L9
mov	$0100,(sp)
jsr	pc,*$_set
L9:.data
L10:.byte 055,0157,0144,0144,0
.even
.text
mov	$L10,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L11
mov	$0100,(sp)
jsr	pc,*$_reset
L11:.data
L12:.byte 0162,0141,0167,0
.even
.text
mov	$L12,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L13
mov	$040,(sp)
jsr	pc,*$_set
L13:.data
L14:.byte 055,0162,0141,0167,0
.even
.text
.data
L15:.byte 0143,0157,0157,0153,0145,0144,0
.even
.text
mov	$L14,(sp)
jsr	pc,*$_eq
tst	r0
jne	L10000
mov	$L15,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L16
L10000:mov	$040,(sp)
jsr	pc,*$_reset
L16:.data
L17:.byte 055,0156,0154,0
.even
.text
mov	$L17,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L18
mov	$020,(sp)
jsr	pc,*$_set
L18:.data
L19:.byte 0156,0154,0
.even
.text
mov	$L19,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L20
mov	$020,(sp)
jsr	pc,*$_reset
L20:.data
L21:.byte 0145,0143,0150,0157,0
.even
.text
mov	$L21,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L22
mov	$010,(sp)
jsr	pc,*$_set
L22:.data
L23:.byte 055,0145,0143,0150,0157,0
.even
.text
mov	$L23,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L24
mov	$010,(sp)
jsr	pc,*$_reset
L24:.data
L25:.byte 0154,0143,0141,0163,0145,0
.even
.text
mov	$L25,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L26
mov	$04,(sp)
jsr	pc,*$_set
L26:.data
L27:.byte 055,0154,0143,0141,0163,0145,0
.even
.text
mov	$L27,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L28
mov	$04,(sp)
jsr	pc,*$_reset
L28:.data
L29:.byte 055,0164,0141,0142,0163,0
.even
.text
mov	$L29,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L30
mov	$02,(sp)
jsr	pc,*$_set
L30:.data
L31:.byte 0164,0141,0142,0163,0
.even
.text
mov	$L31,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L32
mov	$02,(sp)
jsr	pc,*$_reset
L32:.data
L33:.byte 055,0144,0145,0154,0141,0171,0
.even
.text
mov	$L33,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L34
mov	$01,(sp)
jsr	pc,*$_set
L34:.data
L35:.byte 0144,0145,0154,0141,0171,0
.even
.text
mov	$L35,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L36
mov	$01,(sp)
jsr	pc,*$_reset
L36:.data
L37:.byte 0164,0144,0145,0154,0141,0171,0
.even
.text
mov	$L37,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L38
mov	$010000,(sp)
jsr	pc,*$_reset
L38:.data
L39:.byte 055,0164,0144,0145,0154,0141,0171,0
.even
.text
mov	$L39,(sp)
jsr	pc,*$_eq
tst	r0
jeq	L40
mov	$010000,(sp)
jsr	pc,*$_set
L40:tst	_arg
jeq	L41
.data
L42:.byte 0165,0156,0153,0156,0157,0167,0156,040,0155,0157,0144,0145,072,040,045,0163,012,0
.even
.text
mov	_arg,(sp)
mov	$L42,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L41:jbr	L2
L3:mov	$_mode,(sp)
mov	$01,-(sp)
jsr	pc,*$_stty
tst	(sp)+
L1:jmp	rretrn
.globl	_eq
.text
_eq:
jsr	r5,rsave; 4
tst	_arg
jne	L44
clr	r0
jbr	L43
L44:clr	-8.(r5)
L45:mov	_arg,r0
add	-8.(r5),r0
mov	4.(r5),r1
add	-8.(r5),r1
cmpb	(r0),(r1)
jeq	L46
clr	r0
jbr	L43
L46:mov	-8.(r5),r0
inc	-8.(r5)
add	_arg,r0
tstb	(r0)
jne	L45
clr	_arg
mov	$01,r0
jbr	L43
L43:jmp	rretrn
.globl	_set
.text
_set:
jsr	r5,rsave; 2
bis	4.(r5),4.+_mode
L47:jmp	rretrn
.globl	_reset
.text
_reset:
jsr	r5,rsave; 2
mov	4.(r5),r0
com	r0
com	r0
bic	r0,4.+_mode
L48:jmp	rretrn
