.globl	_main
.text
_main:
jsr	r5,rsave; 10
cmp	4.(r5),$05
jeq	L2
.data
L3:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_printf
jbr	L4
L2:mov	6.(r5),r0
movb	*4.(r0),r0
cmp	$0142,r0
jne	L5
mov	$0160666,-8.(r5)
jbr	L6
L5:mov	6.(r5),r0
movb	*4.(r0),r0
cmp	$0143,r0
jne	L7
mov	$0120666,-8.(r5)
jbr	L8
L7:jbr	L4
L8:L6:mov	6.(r5),r0
mov	6.(r0),(sp)
jsr	pc,*$_number
mov	r0,-10.(r5)
tst	-10.(r5)
jlt	L4
mov	6.(r5),r0
mov	8.(r0),(sp)
jsr	pc,*$_number
mov	r0,-12.(r5)
tst	-12.(r5)
jlt	L4
mov	-10.(r5),r0
ash	$010,r0
mov	r0,(sp)
bis	-12.(r5),(sp)
mov	-8.(r5),-(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_mknod
cmp	(sp)+,(sp)+
tst	r0
jge	L9
.data
L10:.byte 0155,0153,0156,0157,0144,0
.even
.text
mov	$L10,(sp)
jsr	pc,*$_perror
L9:jsr	pc,_exit
L4:.data
L11:.byte 0165,0163,0141,0147,0145,072,040,0155,0153,0156,0157,0144,040,0156,0141,0155,0145,040,0142,057,0143,040,0155,0141,0152,0157,0162,040,0155,0151,0156,0157,0162,012,0
.even
.text
mov	$L11,(sp)
jsr	pc,*$_printf
L1:jmp	rretrn
.globl	_number
.text
_number:
jsr	r5,rsave; 6
clr	-8.(r5)
L13:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
mov	r0,-10.(r5)
jeq	L14
cmp	-10.(r5),$060
jlt	L10000
cmp	-10.(r5),$071
jle	L15
L10000:mov	$0177777,r0
jbr	L12
L15:mov	-8.(r5),r1
mul	$012,r1
add	-10.(r5),r1
add	$0177720,r1
mov	r1,-8.(r5)
jbr	L13
L14:mov	-8.(r5),r0
jbr	L12
L12:jmp	rretrn
