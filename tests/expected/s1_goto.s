.globl	_offset
.data
_offset=.
0.
.globl	_main
.text
_main:
jsr	r5,rsave; 102
cmp	4.(r5),$02
jlt	L10000
clr	(sp)
jsr	pc,*$_ttyn
cmp	$0170,r0
jeq	L2
L10000:.data
L3:.byte 0147,0157,0164,0157,040,0145,0162,0162,0157,0162,012,0
.even
.text
mov	$013,(sp)
mov	$L3,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$02,(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
jbr	L1
L2:clr	(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
clr	(sp)
jsr	pc,*$_dup
mov	r0,_fin
L4:mov	r5,r0
add	$0177672,r0
mov	r0,(sp)
jsr	pc,*$_getlin
tst	r0
jeq	L5
.data
L6:.byte 0154,0141,0142,0145,0154,040,0156,0157,0164,040,0146,0157,0165,0156,0144,012,0
.even
.text
mov	$020,(sp)
mov	$L6,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L1
L5:mov	6.(r5),r0
mov	2.(r0),(sp)
mov	r5,r0
add	$0177672,r0
mov	r0,-(sp)
jsr	pc,*$_compar
tst	(sp)+
tst	r0
jne	L4
clr	(sp)
mov	_offset,-(sp)
clr	-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L1:jmp	rretrn
.globl	_getlin
.text
_getlin:
jsr	r5,rsave; 6
clr	-10.(r5)
L8:jsr	pc,_getc
mov	r0,-8.(r5)
jne	L9
mov	$01,r0
jbr	L7
L9:cmp	-8.(r5),$072
jeq	L10
L11:cmp	-8.(r5),$012
jeq	L12
tst	-8.(r5)
jeq	L12
jsr	pc,_getc
mov	r0,-8.(r5)
jbr	L11
L12:jbr	L8
L10:L13:jsr	pc,_getc
mov	r0,-8.(r5)
cmp	$040,r0
jne	L14
jbr	L13
L14:L15:cmp	-8.(r5),$040
jeq	L16
cmp	-8.(r5),$012
jeq	L16
tst	-8.(r5)
jeq	L16
mov	-10.(r5),r0
inc	-10.(r5)
add	4.(r5),r0
movb	-8.(r5),(r0)
jsr	pc,_getc
mov	r0,-8.(r5)
jbr	L15
L16:L17:cmp	-8.(r5),$012
jeq	L18
jsr	pc,_getc
mov	r0,-8.(r5)
jbr	L17
L18:mov	4.(r5),r0
add	-10.(r5),r0
clrb	(r0)
clr	r0
jbr	L7
L7:jmp	rretrn
.globl	_compar
.text
_compar:
jsr	r5,rsave; 6
clr	-10.(r5)
L20:mov	4.(r5),r0
add	-10.(r5),r0
mov	6.(r5),r1
add	-10.(r5),r1
cmpb	(r0),(r1)
jeq	L21
mov	$01,r0
jbr	L19
L21:mov	-10.(r5),r0
inc	-10.(r5)
add	4.(r5),r0
tstb	(r0)
jne	L22
clr	r0
jbr	L19
L22:jbr	L20
L19:jmp	rretrn
.globl	_getc
.text
_getc:
jsr	r5,rsave; 2
inc	_offset
jsr	pc,_getchar
jbr	L23
L23:jmp	rretrn
