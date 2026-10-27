.globl	_args
.comm	_args,0310
.globl	_string
.comm	_string,023420
.globl	_main
.text
_main:
jsr	r5,rsave; 12
cmp	4.(r5),$02
jge	L2
.data
L3:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L2:dec	4.(r5)
add	$02,6.(r5)
mov	$_args,-10.(r5)
mov	$_string,-12.(r5)
clr	-8.(r5)
L4:cmp	-8.(r5),$011
jge	L5
.data
L7:.byte 057,0165,0163,0162,057,0142,0151,0156,057,0
.even
.text
mov	-12.(r5),r0
inc	-12.(r5)
mov	-8.(r5),r1
movb	L7(r1),(r0)
L6:inc	-8.(r5)
jbr	L4
L5:clr	-8.(r5)
L8:cmp	-8.(r5),4.(r5)
jge	L9
mov	-10.(r5),r0
add	$02,-10.(r5)
mov	-12.(r5),(r0)
mov	6.(r5),r0
add	$02,6.(r5)
mov	(r0),-14.(r5)
L11:mov	-12.(r5),r0
inc	-12.(r5)
mov	-14.(r5),r1
inc	-14.(r5)
movb	(r1),(r0)
jeq	L12
jbr	L11
L12:L10:inc	-8.(r5)
jbr	L8
L9:clr	*-10.(r5)
mov	$020,(sp)
jsr	pc,*$_nice
mov	$_args,(sp)
mov	$9.+_string,-(sp)
jsr	pc,*$_execv
tst	(sp)+
mov	$_args,(sp)
mov	$4.+_string,-(sp)
jsr	pc,*$_execv
tst	(sp)+
mov	$_args,(sp)
mov	$_string,-(sp)
jsr	pc,*$_execv
tst	(sp)+
.data
L13:.byte 045,0163,040,0156,0157,0164,040,0146,0157,0165,0156,0144,012,0
.even
.text
mov	$9.+_string,(sp)
mov	$L13,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L1:jmp	rretrn
