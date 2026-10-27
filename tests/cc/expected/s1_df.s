.globl	_dargv
.data
_dargv=.
0.
.text
L1:.byte 057,0144,0145,0166,057,0162,0153,062,0
.even
.data
L1
.text
L2:.byte 057,0144,0145,0166,057,0162,0160,060,0
.even
.data
L2
0.
.globl	_sblock
.comm	_sblock,01004
.globl	_fi
.comm	_fi,02
.globl	_main
.text
_main:
jsr	r5,rsave; 4
cmp	4.(r5),$01
jgt	L4
mov	$01,4.(r5)
L5:mov	4.(r5),r0
asl	r0
tst	_dargv(r0)
jeq	L6
L7:inc	4.(r5)
jbr	L5
L6:mov	$_dargv,6.(r5)
L4:mov	$01,-8.(r5)
L8:cmp	-8.(r5),4.(r5)
jge	L9
cmp	4.(r5),$01
jle	L11
.data
L12:.byte 045,0163,040,0
.even
.text
mov	-8.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),(sp)
mov	$L12,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L11:mov	-8.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),(sp)
jsr	pc,*$_dfree
L10:inc	-8.(r5)
jbr	L8
L9:L3:jmp	rretrn
.globl	_dfree
.text
_dfree:
jsr	r5,rsave; 4
clr	(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
tst	_fi
jge	L14
.data
L15:.byte 0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L15,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L13
L14:jsr	pc,_sync
mov	$_sblock,(sp)
mov	$01,-(sp)
jsr	pc,*$_bread
tst	(sp)+
clr	-8.(r5)
L16:jsr	pc,_alloc
tst	r0
jeq	L17
inc	-8.(r5)
jbr	L16
L17:.data
L18:.byte 045,0154,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L18,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	_fi,(sp)
jsr	pc,*$_close
L13:jmp	rretrn
.globl	_alloc
.text
_alloc:
jsr	r5,rsave; 1006
dec	4.+_sblock
mov	4.+_sblock,r0
mov	r0,-10.(r5)
tst	-10.(r5)
jlt	L10000
cmp	-10.(r5),$0144
jlt	L20
L10000:.data
L21:.byte 0142,0141,0144,040,0146,0162,0145,0145,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L21,(sp)
jsr	pc,*$_printf
clr	r0
jbr	L19
L20:mov	-10.(r5),r0
asl	r0
mov	6.+_sblock(r0),-8.(r5)
tst	-8.(r5)
jne	L22
clr	r0
jbr	L19
L22:mov	_sblock,r0
add	$02,r0
cmp	-8.(r5),r0
jlo	L10001
cmp	-8.(r5),2.+_sblock
jlo	L23
L10001:.data
L24:.byte 0142,0141,0144,040,0146,0162,0145,0145,040,0142,0154,0157,0143,0153,040,050,045,0154,051,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L24,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	r0
jbr	L19
L23:tst	4.+_sblock
jgt	L25
mov	r5,r0
add	$0176766,r0
mov	r0,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	-522.(r5),4.+_sblock
clr	-10.(r5)
L26:cmp	-10.(r5),$0144
jge	L27
mov	-10.(r5),r0
asl	r0
mov	-10.(r5),r1
asl	r1
add	r5,r1
mov	-520.(r1),6.+_sblock(r0)
L28:inc	-10.(r5)
jbr	L26
L27:L25:mov	-8.(r5),r0
jbr	L19
L19:jmp	rretrn
.globl	_bread
.text
_bread:
jsr	r5,rsave; 4
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-8.(r5)
cmp	$01000,r0
jeq	L30
.data
L31:.byte 0162,0145,0141,0144,040,0145,0162,0162,0157,0162,040,045,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L31,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L32:.byte 0143,0157,0165,0156,0164,040,075,040,045,0144,073,040,0145,0162,0162,0156,0157,040,075,040,045,0144,012,0
.even
.text
mov	_errno,(sp)
mov	-8.(r5),-(sp)
mov	$L32,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_exit
L30:L29:jmp	rretrn
