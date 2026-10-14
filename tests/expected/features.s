.globl	_x
.data; _x: 5
.globl	_v
.data; _v:1f
1:1
2
3
.globl	_buf
.data; _buf:1f
.bss; 1:.=.+24
.globl	_f
.data; _f:1f
.text; 1:mov r5,-(sp); mov sp,r5
.bss; l1:.=.+2; .even; .text
add	$-12,sp
mov	sp,r0
mov	r0,-(sp)
tst	-(sp)
clr	-2(r5)
l2:cmp	-2(r5),$12
bge	l10000
mov	6(r5),r0
add	-2(r5),r0
cmpb	(r0),$141
bne	l10001
l10000:jmp	l3
l10001:mov	-2(r5),r0
inc	-2(r5)
add	6(r5),r0
movb	(r0),r0
mov	r0,-16(r5)
cmp	-16(r5),$74570
bne	l4
jmp	*L5
br	L6
l4:mov	-16(r5),(r4)+
mov	$3,(r4)
mov	-(r4),r0
mov	-2(r5),r1
bic	$!3,r1
asl	r1
add	-14(r5),r1
add	r0,(r1)
l6:jmp	l2
l3:mov	4(r5),r0
jsr	pc,bswitch; l8
l9:bis	$7,_x
jmp	l7
l10:mov	$-1,_x
l11:tst	4(r5)
ble	l10002
mov	_v,r0
mov	2(r0),r0
br	l10003
l10002:mov	_x,r0
l10003:mov	r0,L1
L7:.data;L8:2; l10
1; l9
L11; 0
.text
.data; l5: 1f; .text; 1:
mov	-14(r5),r0
mov	(r0),r0
add	L1,r0
jmp	retrn
jmp	retrn
