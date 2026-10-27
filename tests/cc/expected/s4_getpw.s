.globl	_getpw
.text
_getpw:
.bss
L2:.=.+2.
.text
jsr	r5,rsave; 1010
tst	L2
jne	L3
.data
L4:.byte 057,0145,0164,0143,057,0160,0141,0163,0163,0167,0144,0
.even
.text
mov	r5,r0
add	$0176764,r0
mov	r0,(sp)
mov	$L4,-(sp)
jsr	pc,*$_fopen
tst	(sp)+
mov	-524.(r5),L2
jbr	L5
L3:mov	L2,-524.(r5)
L5:tst	L2
jge	L6
mov	$01,r0
jbr	L1
L6:clr	(sp)
clr	-(sp)
mov	L2,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
clr	2.+-524.(r5)
clr	4.+-524.(r5)
L7:mov	6.(r5),r2
L9:mov	r5,r0
add	$0176764,r0
mov	r0,(sp)
jsr	pc,*$_getc
mov	r0,r3
cmp	$012,r0
jeq	L10
tst	r3
jne	L11
mov	$01,r0
jbr	L1
L11:movb	r3,(r2)+
jbr	L9
L10:clrb	(r2)+
mov	6.(r5),r2
mov	$03,r4
L12:dec	r4
jeq	L13
L14:movb	(r2)+,r0
mov	r0,r3
cmp	$072,r0
jeq	L15
cmp	r3,$012
jne	L16
mov	$01,r0
jbr	L1
L16:jbr	L14
L15:jbr	L12
L13:L17:movb	(r2)+,r0
mov	r0,r3
cmp	$072,r0
jeq	L18
cmp	r3,$060
jlt	L17
cmp	r3,$071
jgt	L17
mov	r4,r1
mul	$012,r1
add	r3,r1
add	$0177720,r1
mov	r1,r4
jbr	L17
L18:cmp	r4,4.(r5)
jne	L19
clr	r0
jbr	L1
L19:jbr	L7
L8:mov	$01,r0
jbr	L1
L1:jmp	rretrn
