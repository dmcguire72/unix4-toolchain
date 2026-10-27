.globl	_main
.text
_main:
jsr	r5,rsave; 10
clr	-10.(r5)
cmp	4.(r5),$02
jge	L2
.data
L3:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L2:mov	6.(r5),r0
mov	2.(r0),-12.(r5)
L4:mov	-12.(r5),r0
inc	-12.(r5)
movb	(r0),r0
mov	r0,-8.(r5)
jeq	L5
cmp	-8.(r5),$060
jlt	L10000
cmp	-8.(r5),$071
jle	L6
L10000:.data
L7:.byte 0142,0141,0144,040,0143,0150,0141,0162,0141,0143,0164,0145,0162,012,0
.even
.text
mov	$L7,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L6:mov	-10.(r5),r1
mul	$012,r1
add	-8.(r5),r1
add	$0177720,r1
mov	r1,-10.(r5)
jbr	L4
L5:mov	-10.(r5),(sp)
jsr	pc,*$_sleep
L1:jmp	rretrn
