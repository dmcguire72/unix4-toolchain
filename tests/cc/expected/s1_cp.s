.globl	_main
.text
_main:
jsr	r5,rsave; 1024
clr	-528.(r5)
cmp	4.(r5),$04
jne	L2
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jne	L2
mov	6.(r5),r0
mov	2.(r0),r0
movb	1.(r0),r0
cmp	$0164,r0
jne	L2
dec	4.(r5)
add	$02,6.(r5)
mov	$01,-528.(r5)
L2:cmp	4.(r5),$03
jeq	L3
.data
L4:.byte 0125,0163,0141,0147,0145,072,040,0143,0160,040,0157,0154,0144,0146,0151,0154,0145,040,0156,0145,0167,0146,0151,0154,0145,012,0
.even
.text
mov	$032,(sp)
mov	$L4,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L3:clr	(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-520.(r5)
jge	L5
.data
L6:.byte 0103,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,0157,0154,0144,040,0146,0151,0154,0145,056,012,0
.even
.text
mov	$026,(sp)
mov	$L6,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L5:mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	-520.(r5),-(sp)
jsr	pc,*$_fstat
tst	(sp)+
mov	4.+-518.(r5),-536.(r5)
mov	-536.(r5),(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-522.(r5)
jge	L7
mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_stat
tst	(sp)+
mov	4.+-518.(r5),r0
bic	$!060000,r0
cmp	$040000,r0
jne	L8
mov	6.(r5),r0
mov	2.(r0),-530.(r5)
mov	6.(r5),r0
mov	4.(r0),-532.(r5)
mov	r5,r0
add	$0176772,r0
mov	r0,-534.(r5)
L9:mov	-534.(r5),r0
inc	-534.(r5)
mov	-532.(r5),r1
inc	-532.(r5)
movb	(r1),(r0)
jeq	L10
jbr	L9
L10:mov	-534.(r5),r0
movb	$057,-1.(r0)
mov	-534.(r5),-532.(r5)
L11:mov	-530.(r5),r0
inc	-530.(r5)
movb	(r0),*-534.(r5)
jeq	L12
mov	-534.(r5),r0
inc	-534.(r5)
movb	(r0),r0
cmp	$057,r0
jne	L13
mov	-532.(r5),-534.(r5)
L13:jbr	L11
L12:mov	-536.(r5),(sp)
mov	r5,r0
add	$0176772,r0
mov	r0,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-522.(r5)
jge	L14
.data
L15:.byte 0103,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,040,0156,0145,0167,040,0146,0151,0154,0145,056,012,0
.even
.text
mov	$027,(sp)
mov	$L15,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L14:jbr	L16
L8:.data
L17:.byte 0103,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,040,0156,0145,0167,040,0146,0151,0154,0145,056,012,0
.even
.text
mov	$027,(sp)
mov	$L17,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L16:L7:L18:mov	$01000,(sp)
mov	r5,r0
add	$0176772,r0
mov	r0,-(sp)
mov	-520.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-524.(r5)
jeq	L19
tst	-524.(r5)
jge	L20
.data
L21:.byte 0122,0145,0141,0144,040,0145,0162,0162,0157,0162,012,0
.even
.text
mov	$013,(sp)
mov	$L21,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
jbr	L22
L20:mov	-524.(r5),(sp)
mov	r5,r0
add	$0176772,r0
mov	r0,-(sp)
mov	-522.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
cmp	-524.(r5),r0
jeq	L23
.data
L24:.byte 0127,0162,0151,0164,0145,040,0145,0162,0162,0157,0162,056,012,0
.even
.text
mov	$015,(sp)
mov	$L24,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L23:L22:inc	-526.(r5)
jbr	L18
L19:tst	-528.(r5)
jeq	L25
mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	$06,-(sp)
mov	-526.(r5),-(sp)
jsr	pc,*$_conf
cmp	(sp)+,(sp)+
mov	$012,6.+-518.(r5)
mov	$07,(sp)
mov	r5,r0
add	$0176772,r0
mov	r0,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L25:clr	(sp)
jsr	pc,*$_exit
L1:jmp	rretrn
.globl	_conf
.text
_conf:
jsr	r5,rsave; 6
mov	6.(r5),-8.(r5)
L27:mov	-8.(r5),r0
dec	-8.(r5)
tst	r0
jeq	L28
mov	8.(r5),r0
add	-8.(r5),r0
movb	$040,(r0)
jbr	L27
L28:mov	4.(r5),r1
sxt	r0
div	$012,r0
mov	r0,-10.(r5)
jeq	L10000
mov	8.(r5),(sp)
dec	6.(r5)
mov	6.(r5),-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_conf
cmp	(sp)+,(sp)+
jbr	L10001
L10000:dec	6.(r5)
mov	6.(r5),r0
L10001:add	8.(r5),r0
mov	4.(r5),r3
sxt	r2
div	$012,r2
add	$060,r3
movb	r3,(r0)
inc	6.(r5)
mov	6.(r5),r0
jbr	L26
L26:jmp	rretrn
