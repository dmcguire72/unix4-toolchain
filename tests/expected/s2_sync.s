.globl	_main
.text
_main:
jsr	r5,rsave; 2
jsr	pc,_sync
L1:jmp	rretrn
