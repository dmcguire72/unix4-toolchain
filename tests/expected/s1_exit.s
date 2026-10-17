.globl	_main
.text
_main:
jsr	r5,rsave; 2
mov	$02,(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L1:jmp	rretrn
