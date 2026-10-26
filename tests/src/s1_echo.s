.globl	_main
.text
_main:
jsr	r5,rsave; 4
dec	4.(r5)
mov	$01,-8.(r5)
L2:cmp	-8.(r5),4.(r5)
jgt	L3
.data
L5:.byte 045,0163,045,0143,0
.even
.text
cmp	-8.(r5),4.(r5)
jne	L10000
mov	$012,(sp)
jbr	L10001
L10000:mov	$040,(sp)
L10001:mov	-8.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
mov	$L5,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L4:inc	-8.(r5)
jbr	L2
L3:L1:jmp	rretrn
