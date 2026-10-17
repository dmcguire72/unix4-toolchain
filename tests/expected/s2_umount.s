.globl	_mtab
.comm	_mtab,02000
.globl	_main
.text
_main:
jsr	r5,rsave; 4
.data
L2:.byte 057,0145,0164,0143,057,0155,0164,0141,0142,0
.even
.text
clr	(sp)
mov	$L2,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-8.(r5)
mov	$02000,(sp)
mov	$_mtab,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	4.(r5),$02
jeq	L3
.data
L4:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L4,(sp)
jsr	pc,*$_printf
jbr	L1
L3:mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_umount
tst	r0
jge	L5
.data
L6:.byte 0165,0155,0157,0165,0156,0164,0
.even
.text
mov	$L6,(sp)
jsr	pc,*$_perror
jbr	L1
L5:mov	6.(r5),r0
mov	2.(r0),r3
L7:tstb	(r3)+
jeq	L8
jbr	L7
L8:dec	r3
L9:cmpb	-(r3),$057
jne	L10
clrb	(r3)
jbr	L9
L10:L11:mov	6.(r5),r0
cmp	2.(r0),r3
jhis	L12
cmpb	-(r3),$057
jeq	L12
jbr	L11
L12:cmpb	(r3),$057
jne	L13
inc	r3
L13:mov	6.(r5),r0
mov	r3,2.(r0)
mov	$_mtab,r4
L14:cmp	r4,$1024.+_mtab
jhis	L15
mov	6.(r5),r0
mov	2.(r0),r3
mov	r4,r0
add	$040,r0
mov	r0,r2
L17:cmpb	(r3)+,(r2)
jne	L18
tstb	(r2)+
jne	L19
mov	r4,r3
L20:mov	r4,r0
add	$0100,r0
cmp	r3,r0
jhis	L21
clrb	(r3)+
jbr	L20
L21:mov	$1024.+_mtab,r4
L22:sub	$0100,r4
mov	r4,r0
tstb	(r0)
jne	L23
jbr	L22
L23:.data
L24:.byte 057,0145,0164,0143,057,0155,0164,0141,0142,0
.even
.text
mov	$0644,(sp)
mov	$L24,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-8.(r5)
mov	r4,r0
sub	$_mtab,r0
ash	$-06,r0
inc	r0
ash	$06,r0
mov	r0,(sp)
mov	$_mtab,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L1
L19:jbr	L17
L18:L16:add	$0100,r4
jbr	L14
L15:.data
L25:.byte 045,0163,040,0156,0157,0164,040,0151,0156,040,0155,0157,0165,0156,0164,040,0164,0141,0142,0154,0145,012,0
.even
.text
mov	6.(r5),r0
mov	2.(r0),(sp)
mov	$L25,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L1:jmp	rretrn
