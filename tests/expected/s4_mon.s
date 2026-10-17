.globl	_monitor
.text
_monitor:
.bss
L2:.=.+2.
.text
.bss
L3:.=.+2.
.text
jsr	r5,rsave; 2
tst	4.(r5)
jne	L4
clr	(sp)
clr	-(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_profil
add	$06,sp
.data
L5:.byte 0155,0157,0156,056,0157,0165,0164,0
.even
.text
mov	$0666,(sp)
mov	$L5,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,r4
mov	L3,r0
asl	r0
mov	r0,(sp)
mov	L2,-(sp)
mov	r4,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r4,(sp)
jsr	pc,*$_close
jbr	L1
L4:jsr	pc,_nargs
cmp	$04,r0
jlt	L6
clr	12.(r5)
L6:mov	10.(r5),L3
mov	4.(r5),*8.(r5)
mov	8.(r5),r0
mov	6.(r5),2.(r0)
mov	8.(r5),r0
mov	12.(r5),4.(r0)
mov	8.(r5),L2
mov	12.(r5),r1
inc	r1
mul	$06,r1
add	r1,8.(r5)
mov	12.(r5),r1
inc	r1
mul	$03,r1
sub	r1,10.(r5)
tst	10.(r5)
jle	L1
mov	6.(r5),r0
sub	4.(r5),r0
asr	r0
bic	$!077777,r0
mov	r0,r4
cmp	10.(r5),r4
jhis	L7
mov	r4,r0
asl	r0
mov	r0,(sp)
clr	-(sp)
mov	10.(r5),-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
jbr	L8
L7:mov	$077777,r4
L8:mov	r4,r0
asl	r0
mov	r0,(sp)
mov	4.(r5),-(sp)
mov	10.(r5),r0
asl	r0
mov	r0,-(sp)
mov	8.(r5),-(sp)
jsr	pc,*$_profil
add	$06,sp
L1:jmp	rretrn
