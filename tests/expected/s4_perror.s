.globl	_errno
.comm	_errno,02
.globl	_sys_nerr
.comm	_sys_nerr,02
.globl	_sys_errl
.comm	_sys_errl,02
.globl	_perror
.text
_perror:
jsr	r5,rsave; 4
.data
L2:.byte 0125,0156,0153,0156,0157,0167,0156,040,0145,0162,0162,0157,0162,0
.even
.text
mov	$L2,-8.(r5)
cmp	_errno,_sys_nerr
jge	L3
mov	_errno,r0
asl	r0
mov	_sys_errl(r0),-8.(r5)
L3:tstb	*4.(r5)
jne	L4
.data
L5:.byte 045,0163,0
.even
.text
mov	-8.(r5),(sp)
mov	$L5,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L6
L4:.data
L7:.byte 045,0163,072,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	4.(r5),-(sp)
mov	$L7,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L6:L1:jmp	rretrn
