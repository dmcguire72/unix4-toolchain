.globl	_dflag
.data
_dflag=.
0.
.globl	_sflag
.data
_sflag=.
0.
.globl	_cflag
.data
_cflag=.
0.
.globl	_save
.data
_save=.
0.
.globl	_code
.comm	_code,0400
.globl	_squeez
.comm	_squeez,0400
.globl	_vect
.comm	_vect,0400
.globl	_string1
.comm	_string1,010
.globl	_string2
.comm	_string2,010
.globl	_inbuf
.comm	_inbuf,01006
.globl	_main
.text
_main:
jsr	r5,rsave; 14
clr	r0
mov	r0,_string2
mov	r0,_string1
clr	r0
mov	r0,2.+_string2
mov	r0,2.+_string1
clr	r0
mov	r0,4.+_string2
mov	r0,4.+_string1
.data
L2:.byte 0
.even
.text
mov	$L2,r0
mov	r0,6.+_string2
mov	r0,6.+_string1
dec	4.(r5)
jle	L3
add	$02,6.(r5)
mov	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L4
L5:inc	*6.(r5)
mov	*6.(r5),r0
tstb	(r0)
jeq	L6
mov	6.(r5),r0
movb	*(r0),r0
jbr	L8
L9:inc	_cflag
jbr	L5
L10:inc	_dflag
jbr	L5
L11:inc	_sflag
jbr	L5
jbr	L7
L8:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:143
144
163
L10002:..
L9
L10
L11
L12
.text
/esw
L7:L12:jbr	L5
L6:dec	4.(r5)
add	$02,6.(r5)
L4:L3:tst	4.(r5)
jle	L13
mov	*6.(r5),6.+_string1
L13:cmp	4.(r5),$01
jle	L14
mov	6.(r5),r0
mov	2.(r0),6.+_string2
L14:clr	-8.(r5)
L15:cmp	-8.(r5),$0400
jge	L16
mov	-8.(r5),r0
mov	-8.(r5),r1
movb	$00,_vect(r1)
movb	_vect(r1),r1
movb	r1,_code(r0)
L17:inc	-8.(r5)
jbr	L15
L16:tst	_cflag
jeq	L18
L19:mov	$_string1,(sp)
jsr	pc,*$_next
mov	r0,-12.(r5)
jeq	L20
mov	-12.(r5),r0
bic	$!0377,r0
movb	$01,_vect(r0)
jbr	L19
L20:clr	-10.(r5)
mov	$01,-8.(r5)
L21:cmp	-8.(r5),$0400
jge	L22
mov	-8.(r5),r0
tstb	_vect(r0)
jne	L24
mov	-10.(r5),r0
inc	-10.(r5)
movb	-8.(r5),_vect(r0)
L24:L23:inc	-8.(r5)
jbr	L21
L22:mov	-10.(r5),r0
clrb	_vect(r0)
mov	$_vect,-16.(r5)
L18:clr	-8.(r5)
L25:cmp	-8.(r5),$0400
jge	L26
mov	-8.(r5),r0
clrb	_squeez(r0)
L27:inc	-8.(r5)
jbr	L25
L26:L28:tst	_cflag
jeq	L30
mov	-16.(r5),r0
inc	-16.(r5)
movb	(r0),r0
mov	r0,-12.(r5)
jbr	L31
L30:mov	$_string1,(sp)
jsr	pc,*$_next
mov	r0,-12.(r5)
L31:tst	-12.(r5)
jeq	L29
mov	$_string2,(sp)
jsr	pc,*$_next
mov	r0,-14.(r5)
tst	-14.(r5)
jne	L32
mov	-12.(r5),-14.(r5)
L32:mov	-12.(r5),r0
bic	$!0377,r0
movb	-14.(r5),_code(r0)
mov	-14.(r5),r0
bic	$!0377,r0
movb	$01,_squeez(r0)
jbr	L28
L29:L33:mov	$_string2,(sp)
jsr	pc,*$_next
mov	r0,-14.(r5)
jeq	L34
mov	-14.(r5),r0
bic	$!0377,r0
movb	$01,_squeez(r0)
jbr	L33
L34:movb	$01,_squeez
clr	-8.(r5)
L35:cmp	-8.(r5),$0400
jge	L36
mov	-8.(r5),r0
tstb	_code(r0)
jne	L38
mov	-8.(r5),r0
movb	-8.(r5),_code(r0)
jbr	L39
L38:tst	_dflag
jeq	L40
mov	-8.(r5),r0
clrb	_code(r0)
L40:L39:L37:inc	-8.(r5)
jbr	L35
L36:clr	_inbuf
mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
mov	$01,(sp)
jsr	pc,*$_close
L41:mov	$_inbuf,(sp)
jsr	pc,*$_getc
mov	r0,-12.(r5)
jlt	L42
tst	-12.(r5)
jeq	L41
mov	-12.(r5),r0
bic	$!0377,r0
movb	_code(r0),r0
bic	$!0377,r0
mov	r0,-12.(r5)
jeq	L43
tst	_sflag
jeq	L10004
cmp	-12.(r5),_save
jne	L10004
mov	-12.(r5),r0
bic	$!0377,r0
tstb	_squeez(r0)
jne	L44
L10004:mov	-12.(r5),r0
mov	r0,_save
mov	r0,(sp)
jsr	pc,*$_putchar
L44:L43:jbr	L41
L42:jsr	pc,_flush
L1:jmp	rretrn
.globl	_next
.text
_next:
jsr	r5,rsave; 14
mov	4.(r5),r0
dec	4.(r0)
jle	L46
mov	*4.(r5),r0
jbr	L45
L46:mov	4.(r5),r0
mov	4.(r5),r1
cmp	(r0),2.(r1)
jge	L47
inc	*4.(r5)
mov	*4.(r5),r0
jbr	L45
L47:mov	4.(r5),r0
movb	*6.(r0),r0
cmp	$0133,r0
jne	L48
mov	4.(r5),(sp)
jsr	pc,*$_nextc
mov	4.(r5),(sp)
jsr	pc,*$_nextc
mov	r0,-8.(r5)
mov	r0,*4.(r5)
mov	4.(r5),r0
clr	2.(r0)
mov	4.(r5),(sp)
jsr	pc,*$_nextc
jbr	L50
L51:mov	4.(r5),(sp)
jsr	pc,*$_nextc
mov	r0,-10.(r5)
cmp	-10.(r5),-8.(r5)
jlt	L52
mov	4.(r5),r0
mov	6.(r0),-(sp)
inc	6.(r0)
mov	(sp)+,r0
movb	(r0),r0
cmp	$0135,r0
jne	L52
mov	4.(r5),r0
mov	-10.(r5),2.(r0)
mov	-8.(r5),r0
jbr	L45
L53:mov	4.(r5),r0
movb	*6.(r0),r0
cmp	$060,r0
jne	L10005
mov	$010,r0
jbr	L10006
L10005:mov	$012,r0
L10006:mov	r0,-16.(r5)
clr	-14.(r5)
L54:mov	4.(r5),r0
movb	*6.(r0),r0
mov	r0,-12.(r5)
cmp	$060,r0
jgt	L55
mov	-16.(r5),r0
add	$060,r0
cmp	-12.(r5),r0
jge	L55
mov	-16.(r5),r1
mul	-14.(r5),r1
add	-12.(r5),r1
add	$0177720,r1
mov	r1,-14.(r5)
mov	4.(r5),r0
inc	6.(r0)
jbr	L54
L55:mov	4.(r5),r0
mov	6.(r0),-(sp)
inc	6.(r0)
mov	(sp)+,r0
movb	(r0),r0
cmp	$0135,r0
jne	L52
tst	-14.(r5)
jne	L56
mov	$01750,-14.(r5)
L56:mov	4.(r5),r0
mov	-14.(r5),4.(r0)
mov	-8.(r5),r0
jbr	L45
L57:L52:.data
L58:.byte 0102,0141,0144,040,0163,0164,0162,0151,0156,0147,012,0
.even
.text
mov	$013,(sp)
mov	$L58,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
jbr	L49
L50:sub	$052,r0
cmp	r0,$03
jhi	L57
asl	r0
jmp	*.+4(r0)
L53
L57
L57
L51
/esw
L49:L48:mov	4.(r5),(sp)
jsr	pc,*$_nextc
jbr	L45
L45:jmp	rretrn
.globl	_nextc
.text
_nextc:
jsr	r5,rsave; 10
mov	4.(r5),r0
mov	6.(r0),-(sp)
inc	6.(r0)
mov	(sp)+,r0
movb	(r0),r0
mov	r0,-8.(r5)
cmp	-8.(r5),$0134
jne	L60
clr	r0
mov	r0,-12.(r5)
mov	r0,-10.(r5)
L61:cmp	-10.(r5),$03
jge	L62
mov	4.(r5),r0
movb	*6.(r0),r0
mov	r0,-8.(r5)
cmp	$060,r0
jgt	L62
cmp	-8.(r5),$067
jgt	L62
mov	-12.(r5),r0
ash	$03,r0
add	-8.(r5),r0
add	$0177720,r0
mov	r0,-12.(r5)
inc	-10.(r5)
mov	4.(r5),r0
inc	6.(r0)
jbr	L61
L62:tst	-10.(r5)
jle	L63
mov	-12.(r5),-8.(r5)
jbr	L64
L63:mov	4.(r5),r0
mov	6.(r0),-(sp)
inc	6.(r0)
mov	(sp)+,r0
movb	(r0),r0
mov	r0,-8.(r5)
L64:L60:tst	-8.(r5)
jne	L65
mov	4.(r5),r0
dec	6.(r0)
mov	6.(r0),r0
clrb	(r0)
L65:mov	-8.(r5),r0
bic	$!0377,r0
jbr	L59
L59:jmp	rretrn
