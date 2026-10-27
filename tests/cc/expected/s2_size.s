.globl	_main
.text
_main:
jsr	r5,rsave; 30
mov	4.(r5),-26.(r5)
cmp	-26.(r5),$01
jne	L2
.data
L3:.byte 0141,056,0157,0165,0164,0
.even
.text
mov	$L3,*6.(r5)
inc	-26.(r5)
sub	$02,6.(r5)
L2:L4:dec	-26.(r5)
jeq	L5
add	$02,6.(r5)
clr	(sp)
mov	*6.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-24.(r5)
jge	L6
.data
L7:.byte 045,0163,040,0156,0157,0164,040,0146,0157,0165,0156,0144,012,0
.even
.text
mov	*6.(r5),(sp)
mov	$L7,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L4
L6:mov	$020,(sp)
mov	r5,r0
add	$0177752,r0
mov	r0,-(sp)
mov	-24.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	-22.(r5),$0410
jeq	L8
cmp	-22.(r5),$0407
jeq	L8
.data
L9:.byte 0102,0141,0144,040,0146,0157,0162,0155,0141,0164,072,040,045,0163,012,0
.even
.text
mov	*6.(r5),(sp)
mov	$L9,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	-24.(r5),(sp)
jsr	pc,*$_close
jbr	L4
L8:cmp	4.(r5),$02
jle	L10
.data
L11:.byte 045,0163,072,040,0
.even
.text
mov	*6.(r5),(sp)
mov	$L11,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L10:.data
L12:.byte 045,0154,053,045,0154,053,045,0154,075,0
.even
.text
mov	6.+-22.(r5),(sp)
mov	4.+-22.(r5),-(sp)
mov	2.+-22.(r5),-(sp)
mov	$L12,-(sp)
jsr	pc,*$_printf
add	$06,sp
mov	2.+-22.(r5),r0
add	4.+-22.(r5),r0
add	6.+-22.(r5),r0
mov	r0,-28.(r5)
.data
L13:.byte 045,0154,040,050,045,0157,051,012,0
.even
.text
mov	-28.(r5),(sp)
mov	-28.(r5),-(sp)
mov	$L13,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
mov	-24.(r5),(sp)
jsr	pc,*$_close
jbr	L4
L5:L1:jmp	rretrn
