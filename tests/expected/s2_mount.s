.globl	_mtab
.comm	_mtab,02000
.globl	_main
.text
_main:
jsr	r5,rsave; 6
.data
L2:.byte 057,0145,0164,0143,057,0155,0164,0141,0142,0
.even
.text
clr	(sp)
mov	$L2,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-10.(r5)
mov	$02000,(sp)
mov	$_mtab,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	4.(r5),$01
jne	L3
mov	$_mtab,r3
L4:cmp	r3,$1024.+_mtab
jhis	L5
tstb	(r3)
jeq	L7
.data
L8:.byte 045,0163,040,0157,0156,040,045,0163,012,0
.even
.text
mov	r3,(sp)
mov	r3,-(sp)
add	$040,(sp)
mov	$L8,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L7:L6:add	$0100,r3
jbr	L4
L5:jbr	L1
L3:cmp	4.(r5),$03
jge	L9
.data
L10:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L10,(sp)
jsr	pc,*$_printf
jbr	L1
L9:clr	r4
cmp	4.(r5),$03
jle	L11
inc	r4
L11:mov	r4,(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_mount
cmp	(sp)+,(sp)+
tst	r0
jge	L12
.data
L13:.byte 0155,0157,0165,0156,0164,0
.even
.text
mov	$L13,(sp)
jsr	pc,*$_perror
jbr	L1
L12:mov	6.(r5),r0
mov	2.(r0),r2
L14:tstb	(r2)+
jeq	L15
jbr	L14
L15:dec	r2
L16:cmpb	-(r2),$057
jne	L17
clrb	(r2)
jbr	L16
L17:L18:mov	6.(r5),r0
cmp	2.(r0),r2
jhis	L19
cmpb	-(r2),$057
jeq	L19
jbr	L18
L19:cmpb	(r2),$057
jne	L20
inc	r2
L20:mov	6.(r5),r0
mov	r2,2.(r0)
mov	$_mtab,r3
L21:cmp	r3,$1024.+_mtab
jhis	L22
tstb	(r3)
jne	L24
mov	r3,r0
add	$040,r0
mov	r0,r2
L25:mov	r3,r0
add	$077,r0
cmp	r2,r0
jhis	L26
mov	6.(r5),r0
mov	2.(r0),-(sp)
inc	2.(r0)
mov	(sp)+,r0
movb	(r0),(r2)+
jne	L27
mov	6.(r5),r0
dec	2.(r0)
L27:jbr	L25
L26:mov	r3,r2
L28:mov	r3,r0
add	$037,r0
cmp	r2,r0
jhis	L29
mov	6.(r5),r0
mov	4.(r0),-(sp)
inc	4.(r0)
mov	(sp)+,r0
movb	(r0),(r2)+
jne	L30
mov	6.(r5),r0
dec	4.(r0)
L30:jbr	L28
L29:mov	$1024.+_mtab,r3
L31:sub	$0100,r3
mov	r3,r0
tstb	(r0)
jne	L32
jbr	L31
L32:.data
L33:.byte 057,0145,0164,0143,057,0155,0164,0141,0142,0
.even
.text
mov	$0644,(sp)
mov	$L33,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-10.(r5)
mov	r3,r0
sub	$_mtab,r0
ash	$-06,r0
inc	r0
ash	$06,r0
mov	r0,(sp)
mov	$_mtab,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L1
L24:L23:add	$0100,r3
jbr	L21
L22:L1:jmp	rretrn
