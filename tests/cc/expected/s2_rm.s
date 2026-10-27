.globl	_main
.text
_main:
jsr	r5,rsave; 10
clr	-10.(r5)
clr	-12.(r5)
L2:dec	4.(r5)
jle	L3
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),-8.(r5)
cmpb	*-8.(r5),$055
jne	L4
mov	-8.(r5),r0
movb	1.(r0),r0
cmp	$0146,r0
jne	L5
inc	-10.(r5)
jbr	L2
L5:mov	-8.(r5),r0
movb	1.(r0),r0
cmp	$0162,r0
jne	L6
inc	-12.(r5)
jbr	L2
L6:L4:mov	-12.(r5),(sp)
mov	-10.(r5),-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_rm
cmp	(sp)+,(sp)+
jbr	L2
L3:L1:jmp	rretrn
.globl	_rm
.text
_rm:
jsr	r5,rsave; 60
mov	r5,r0
add	$0177720,r0
mov	r0,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jeq	L8
.data
L9:.byte 045,0163,072,040,0156,0157,0156,040,0145,0170,0151,0163,0164,0145,0156,0164,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L9,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L7
L8:mov	4.+-48.(r5),r0
bic	$!060000,r0
cmp	$040000,r0
jne	L10
tst	8.(r5)
jeq	L11
jsr	pc,_fork
mov	r0,-50.(r5)
tst	-50.(r5)
jge	L12
.data
L13:.byte 045,0163,072,040,0164,0162,0171,040,0141,0147,0141,0151,0156,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L13,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L7
L12:tst	-50.(r5)
jeq	L14
L15:jsr	pc,_wait
cmp	-50.(r5),r0
jeq	L16
jbr	L15
L16:jbr	L7
L14:mov	4.(r5),(sp)
jsr	pc,*$_chdir
tst	r0
jeq	L17
.data
L18:.byte 045,0163,072,040,0143,0141,0156,0156,0157,0164,040,0143,0150,0144,0151,0162,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L18,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L17:clr	-8.(r5)
.data
L19:.byte 057,0145,0164,0143,057,0147,0154,0157,0142,0
.even
.text
.data
L20:.byte 0147,0154,0157,0142,0
.even
.text
.data
L21:.byte 0162,0155,0
.even
.text
.data
L22:.byte 055,0162,0
.even
.text
.data
L23:.byte 055,0146,0
.even
.text
.data
L24:.byte 052,0
.even
.text
.data
L25:.byte 052,0
.even
.text
clr	(sp)
tst	6.(r5)
jeq	L10000
mov	$L25,-(sp)
jbr	L10001
L10000:mov	-8.(r5),-(sp)
L10001:tst	6.(r5)
jeq	L10002
mov	$L23,-(sp)
jbr	L10003
L10002:mov	$L24,-(sp)
L10003:mov	$L22,-(sp)
mov	$L21,-(sp)
mov	$L20,-(sp)
mov	$L19,-(sp)
jsr	pc,*$_execl
add	$014,sp
.data
L26:.byte 045,0163,072,040,0156,0157,040,0147,0154,0157,0142,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L26,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L11:.data
L27:.byte 045,0163,072,040,0144,0151,0162,0145,0143,0164,0157,0162,0171,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L27,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L7
L10:tst	6.(r5)
jne	L28
jsr	pc,_getuid
movb	7.+-48.(r5),r1
cmp	r0,r1
jne	L29
mov	$0200,-52.(r5)
jbr	L30
L29:mov	$02,-52.(r5)
L30:bit	-52.(r5),4.+-48.(r5)
jne	L31
.data
L32:.byte 045,0163,072,040,045,0157,040,0155,0157,0144,0145,040,0
.even
.text
mov	4.+-48.(r5),(sp)
mov	4.(r5),-(sp)
mov	$L32,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_getchar
mov	r0,-52.(r5)
mov	r0,-50.(r5)
mov	-52.(r5),-50.(r5)
L33:cmp	-52.(r5),$012
jeq	L34
tst	-52.(r5)
jeq	L34
jsr	pc,_getchar
mov	r0,-52.(r5)
jbr	L33
L34:cmp	-50.(r5),$0171
jne	L7
L31:L28:mov	4.(r5),(sp)
jsr	pc,*$_unlink
tst	r0
jeq	L35
.data
L36:.byte 045,0163,072,040,0156,0157,0164,040,0162,0145,0155,0157,0166,0145,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L36,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L35:L7:jmp	rretrn
.globl	_putchar
.text
_putchar:
jsr	r5,rsave; 2
mov	$01,(sp)
mov	r5,r0
add	$04,r0
mov	r0,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L37:jmp	rretrn
.globl	_getchar
.text
_getchar:
jsr	r5,rsave; 4
mov	$01,(sp)
mov	r5,r0
add	$0177770,r0
mov	r0,-(sp)
clr	-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01,r0
jeq	L39
clr	r0
jbr	L38
L39:movb	-8.(r5),r0
jbr	L38
L38:jmp	rretrn
