.globl	_rin
.text
_rin:
jsr	r5,rsave; 2
mov	$01,r2
clr	r4
L2:jsr	pc,_getchar
mov	r0,r3
tst	r3
jne	L3
jsr	pc,_exit
L3:cmp	r3,$055
jne	L4
mov	r2,r0
neg	r0
mov	r0,r2
jbr	L2
L4:cmp	r3,$060
jlt	L5
cmp	r3,$071
jgt	L5
mov	r4,r1
mul	$012,r1
add	r3,r1
add	$0177720,r1
mov	r1,r4
jbr	L2
L5:mov	r4,r1
mul	r2,r1
mov	r1,r0
jbr	L1
L1:jmp	rretrn
