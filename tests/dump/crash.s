/ test program: main -> f -> g, then odd-address word store
.globl _main
.globl _f
.globl _g
.text
_main:
	mov	r5,-(sp)
	mov	sp,r5
	mov	$1234,-(sp)
	jsr	pc,_f
	tst	(sp)+
	mov	(sp)+,r5
	rts	pc
_f:
	mov	r5,-(sp)
	mov	sp,r5
	mov	4(r5),-(sp)
	jsr	pc,_g
	tst	(sp)+
	mov	(sp)+,r5
	rts	pc
_g:
	mov	r5,-(sp)
	mov	sp,r5
	mov	$0123456,r0
	mov	$1,r1
	mov	$0177777,r2
	mov	$3,r3
	mov	$4,r4
	clr	-(sp)
	mov	$42,*$077777		/ odd address -> bus error
	mov	(sp)+,r5
	rts	pc
.data
msg:	<hello dump\n\0>
	.even
.bss
buf:	.=.+100
