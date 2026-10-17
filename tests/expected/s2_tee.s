.globl	_open
.data
_open=.
1.
.=.+16.
.globl	_n
.data
_n=.
1.
.globl	_t
.data
_t=.
0.
.globl	_in
.comm	_in,01000
.globl	_out
.comm	_out,01000
.globl	_main
.text
_main:
jsr	r5,rsave; 46
mov	r5,r0
add	$0177726,r0
mov	r0,(sp)
mov	$01,-(sp)
jsr	pc,*$_fstat
tst	(sp)+
mov	4.+-42.(r5),r0
bic	$!060000,r0
cmp	$020000,r0
jeq	L10000
clr	r0
jbr	L10001
L10000:mov	$01,r0
L10001:mov	r0,_t
L2:mov	4.(r5),r0
dec	4.(r5)
cmp	$01,r0
jge	L3
mov	$0666,(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	_n,r1
inc	_n
asl	r1
mov	r0,_open(r1)
mov	r5,r0
add	$0177726,r0
mov	r0,(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jlt	L4
mov	4.+-42.(r5),r0
bic	$!060000,r0
cmp	$020000,r0
jne	L5
inc	_t
L5:L4:add	$02,6.(r5)
jbr	L2
L3:clr	r0
mov	r0,r3
mov	r0,r4
L6:clr	r2
L8:cmp	r2,$01000
jge	L9
cmp	r4,r3
jlt	L10
tst	_t
jle	L10002
tst	r2
jgt	L9
L10002:mov	$01000,(sp)
mov	$_in,-(sp)
clr	-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,r3
clr	r4
tst	r3
jgt	L11
mov	r2,(sp)
jsr	pc,*$_stash
jbr	L1
L11:L10:mov	r2,r0
inc	r2
mov	r4,r1
inc	r4
movb	_in(r1),_out(r0)
jbr	L8
L9:mov	r2,(sp)
jsr	pc,*$_stash
jbr	L6
L7:L1:jmp	rretrn
.globl	_stash
.text
_stash:
jsr	r5,rsave; 10
tst	_t
jeq	L10003
mov	$012,r0
jbr	L10004
L10003:mov	4.(r5),r0
L10004:mov	r0,-12.(r5)
clr	-10.(r5)
L13:cmp	-10.(r5),4.(r5)
jge	L14
clr	-8.(r5)
L16:cmp	-8.(r5),_n
jge	L17
mov	4.(r5),r0
sub	-10.(r5),r0
cmp	-12.(r5),r0
jge	L10005
mov	-12.(r5),(sp)
jbr	L10006
L10005:mov	4.(r5),(sp)
sub	-10.(r5),(sp)
L10006:mov	-10.(r5),-(sp)
add	$_out,(sp)
mov	-8.(r5),r0
asl	r0
mov	_open(r0),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L18:inc	-8.(r5)
jbr	L16
L17:L15:add	-12.(r5),-10.(r5)
jbr	L13
L14:L12:jmp	rretrn
