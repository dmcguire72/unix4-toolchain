.globl	_main
.text
_main:
jsr	r5,rsave; 4
cmp	4.(r5),$03
jge	L2
.data
L3:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_printf
mov	$01,(sp)
jsr	pc,*$_exit
L2:mov	6.(r5),r0
mov	2.(r0),r2
clr	r3
clr	r3
L4:tstb	(r2)
jeq	L5
cmpb	(r2),$060
jlt	L10000
cmpb	(r2),$067
jle	L7
L10000:.data
L8:.byte 0142,0141,0144,040,0155,0157,0144,0145,012,0
.even
.text
mov	$L8,(sp)
jsr	pc,*$_printf
mov	$01,(sp)
jsr	pc,*$_exit
L7:mov	r3,r0
ash	$03,r0
movb	(r2),r1
add	$0177720,r1
bis	r1,r0
mov	r0,r3
L6:inc	r2
jbr	L4
L5:mov	$02,r4
L9:cmp	r4,4.(r5)
jge	L10
mov	r3,(sp)
mov	r4,r0
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_chmod
tst	(sp)+
tst	r0
jge	L12
inc	-8.(r5)
mov	r4,r0
asl	r0
add	6.(r5),r0
mov	(r0),(sp)
jsr	pc,*$_perror
L12:L11:inc	r4
jbr	L9
L10:mov	-8.(r5),(sp)
jsr	pc,*$_exit
L1:jmp	rretrn
