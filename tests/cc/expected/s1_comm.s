.globl	_one
.comm	_one,02
.globl	_two
.comm	_two,02
.globl	_three
.comm	_three,02
.globl	_ldr
.data
_ldr=.
.text
L1:.byte 0
.even
.data
L1
.text
L2:.byte 011,0
.even
.data
L2
.text
L3:.byte 011,011,0
.even
.data
L3
.globl	_ib1
.comm	_ib1,01006
.globl	_ib2
.comm	_ib2,01006
.globl	_main
.text
_main:
jsr	r5,rsave; 244
cmp	4.(r5),$01
jle	L5
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jne	L6
mov	$01,-8.(r5)
L7:mov	6.(r5),r0
inc	2.(r0)
mov	2.(r0),r0
tstb	(r0)
jeq	L8
mov	6.(r5),r0
movb	*2.(r0),r0
jbr	L10
L11:tst	_one
jne	L12
mov	$01,_one
mov	-8.(r5),r0
dec	-8.(r5)
add	4.+_ldr,r0
movb	$00,(r0)
movb	(r0),r0
movb	r0,*2.+_ldr
L12:jbr	L9
L13:tst	_two
jne	L14
mov	$01,_two
mov	-8.(r5),r0
dec	-8.(r5)
add	4.+_ldr,r0
clrb	(r0)
L14:jbr	L9
L15:mov	$01,_three
jbr	L9
L16:.data
L17:.byte 0111,0154,0154,0145,0147,0141,0154,040,0146,0154,0141,0147,072,040,045,0143,012,0
.even
.text
mov	6.(r5),r0
movb	*2.(r0),r0
mov	r0,(sp)
mov	$L17,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
jbr	L9
L10:sub	$061,r0
cmp	r0,$02
jhi	L16
asl	r0
jmp	*.+4(r0)
L11
L13
L15
/esw
L9:jbr	L7
L8:add	$02,6.(r5)
dec	4.(r5)
L6:L5:cmp	4.(r5),$03
jge	L18
.data
L19:.byte 0101,0162,0147,0143,040,075,040,045,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L19,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L18:mov	$_ib1,(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L20
.data
L21:.byte 0103,0141,0156,047,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	6.(r5),r0
mov	2.(r0),(sp)
mov	$L21,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L20:mov	$_ib2,(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L22
.data
L23:.byte 0103,0141,0156,047,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	6.(r5),r0
mov	4.(r0),(sp)
mov	$L23,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L22:mov	r5,r0
add	$0177650,r0
mov	r0,(sp)
mov	$_ib1,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L24
mov	r5,r0
add	$0177530,r0
mov	r0,(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L25
jsr	pc,_exit
L25:mov	$02,(sp)
mov	r5,r0
add	$0177530,r0
mov	r0,-(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_copy
cmp	(sp)+,(sp)+
L24:mov	r5,r0
add	$0177530,r0
mov	r0,(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L26
mov	$01,(sp)
mov	r5,r0
add	$0177650,r0
mov	r0,-(sp)
mov	$_ib1,-(sp)
jsr	pc,*$_copy
cmp	(sp)+,(sp)+
L26:L27:tst	$01
jeq	L28
mov	r5,r0
add	$0177530,r0
mov	r0,(sp)
mov	r5,r0
add	$0177650,r0
mov	r0,-(sp)
jsr	pc,*$_compare
tst	(sp)+
jbr	L30
L31:mov	$03,(sp)
mov	r5,r0
add	$0177650,r0
mov	r0,-(sp)
jsr	pc,*$_wr
tst	(sp)+
mov	r5,r0
add	$0177650,r0
mov	r0,(sp)
mov	$_ib1,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L32
mov	r5,r0
add	$0177530,r0
mov	r0,(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L33
jsr	pc,_fexit
L33:mov	$02,(sp)
mov	r5,r0
add	$0177530,r0
mov	r0,-(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_copy
cmp	(sp)+,(sp)+
L32:mov	r5,r0
add	$0177530,r0
mov	r0,(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L34
mov	$01,(sp)
mov	r5,r0
add	$0177650,r0
mov	r0,-(sp)
mov	$_ib1,-(sp)
jsr	pc,*$_copy
cmp	(sp)+,(sp)+
L34:jbr	L27
L35:mov	$01,(sp)
mov	r5,r0
add	$0177650,r0
mov	r0,-(sp)
jsr	pc,*$_wr
tst	(sp)+
mov	r5,r0
add	$0177650,r0
mov	r0,(sp)
mov	$_ib1,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L36
mov	$02,(sp)
mov	r5,r0
add	$0177530,r0
mov	r0,-(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_copy
cmp	(sp)+,(sp)+
L36:jbr	L27
L37:mov	$02,(sp)
mov	r5,r0
add	$0177530,r0
mov	r0,-(sp)
jsr	pc,*$_wr
tst	(sp)+
mov	r5,r0
add	$0177530,r0
mov	r0,(sp)
mov	$_ib2,-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L38
mov	$01,(sp)
mov	r5,r0
add	$0177650,r0
mov	r0,-(sp)
mov	$_ib1,-(sp)
jsr	pc,*$_copy
cmp	(sp)+,(sp)+
L38:jbr	L27
jbr	L29
L30:cmp	r0,$02
jhi	L39
asl	r0
jmp	*.+4(r0)
L31
L35
L37
/esw
L29:L39:jbr	L27
L28:L4:jmp	rretrn
.globl	_rd
.text
_rd:
jsr	r5,rsave; 2
dec	6.(r5)
L41:mov	4.(r5),(sp)
jsr	pc,*$_getc
inc	6.(r5)
mov	6.(r5),r1
movb	r0,(r1)
jlt	L42
cmpb	*6.(r5),$012
jne	L43
clrb	*6.(r5)
clr	r0
jbr	L40
L43:jbr	L41
L42:mov	$0177777,r0
jbr	L40
L40:jmp	rretrn
.globl	_wr
.text
_wr:
jsr	r5,rsave; 2
mov	6.(r5),r0
jbr	L46
L47:tst	_one
jne	L44
jbr	L45
L48:tst	_two
jne	L44
jbr	L45
L49:tst	_three
jne	L44
jbr	L45
L46:sub	$01,r0
cmp	r0,$02
jhi	L50
asl	r0
jmp	*.+4(r0)
L47
L48
L49
/esw
L45:L50:.data
L51:.byte 045,0163,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	6.(r5),r0
asl	r0
mov	-2.+_ldr(r0),-(sp)
mov	$L51,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L44:jmp	rretrn
.globl	_copy
.text
_copy:
jsr	r5,rsave; 2
L55:mov	8.(r5),(sp)
mov	6.(r5),-(sp)
jsr	pc,*$_wr
tst	(sp)+
L53:mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_rd
tst	(sp)+
tst	r0
jge	L55
L54:jsr	pc,_flush
jsr	pc,_exit
L52:jmp	rretrn
.globl	_compare
.text
_compare:
jsr	r5,rsave; 2
dec	4.(r5)
mov	4.(r5),r0
mov	r0,r4
dec	6.(r5)
mov	6.(r5),r0
mov	r0,r3
L57:inc	r4
mov	r4,r0
inc	r3
mov	r3,r1
cmpb	(r0),(r1)
jne	L58
tstb	(r4)
jne	L59
clr	r0
jbr	L56
L59:jbr	L57
L58:cmpb	(r4),(r3)
jge	L60
mov	$01,r0
jbr	L56
L60:mov	$02,r0
jbr	L56
L56:jmp	rretrn
.globl	_fexit
.text
_fexit:
jsr	r5,rsave; 2
jsr	pc,_flush
jsr	pc,_exit
L61:jmp	rretrn
