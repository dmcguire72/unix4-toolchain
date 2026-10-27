.globl	_linect
.comm	_linect,04
.globl	_alphct
.comm	_alphct,04
.globl	_punct
.comm	_punct,04
.globl	_wordct
.comm	_wordct,04
.globl	_ctlct
.comm	_ctlct,04
.globl	_roffflg
.data
_roffflg=.
0.
.globl	_codeflg
.data
_codeflg=.
0.
.text
L1:.byte 055,0154,0167,0
.even
.data
.globl	_code
.data
_code=.
L1
.globl	_class
.data
_class=.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 2.
.byte 16.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 1.
.byte 2.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 8.
.byte 4.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 8.
.byte 4.
.byte 4.
.byte 4.
.byte 4.
.byte 1.
.globl	_buf
.comm	_buf,01006
.globl	_main
.text
_main:
jsr	r5,rsave; 6
dec	4.(r5)
mov	4.(r5),r0
cmp	$01,r0
jle	L10000
clr	r0
jbr	L10001
L10000:mov	$01,r0
L10001:add	$02,6.(r5)
mov	6.(r5),r1
movb	*(r1),r1
cmp	$055,r1
jeq	L10002
clr	r1
jbr	L10003
L10002:mov	$01,r1
L10003:bit	r1,r0
jeq	L3
mov	$01,-8.(r5)
L4:mov	*6.(r5),r0
add	-8.(r5),r0
movb	(r0),r4
jeq	L5
cmp	r4,$0162
jne	L7
mov	$01,_roffflg
jbr	L8
L7:mov	$01,_codeflg
L8:L6:inc	-8.(r5)
jbr	L4
L5:tst	_codeflg
jeq	L9
mov	*6.(r5),_code
L9:dec	4.(r5)
add	$02,6.(r5)
L3:clr	-8.(r5)
L12:tst	4.(r5)
jgt	L13
clrb	_buf
jbr	L14
L13:mov	$_buf,(sp)
mov	-8.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L15
.data
L16:.byte 045,0163,072,040,0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,012,0
.even
.text
mov	-8.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),(sp)
mov	$L16,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L15:L14:jsr	pc,_count
mov	$01,-10.(r5)
L17:mov	_code,r0
add	-10.(r5),r0
tstb	(r0)
jeq	L18
mov	_code,r0
add	-10.(r5),r0
movb	(r0),r0
jbr	L21
L22:mov	$_linect,(sp)
jsr	pc,*$_putd
jbr	L20
L23:mov	$_alphct,(sp)
jsr	pc,*$_putd
jbr	L20
L24:mov	$_punct,(sp)
jsr	pc,*$_putd
jbr	L20
L25:mov	$_wordct,(sp)
jsr	pc,*$_putd
jbr	L20
L26:mov	$_ctlct,(sp)
jsr	pc,*$_putd
jbr	L20
jbr	L20
L21:mov	$L10005,r1
mov	r0,L10006
L10007:cmp	r0,(r1)+
jne	L10007
jmp	*L10006-L10005(r1)
.data
L10005:141
143
154
160
167
L10006:..
L23
L26
L22
L24
L25
L27
.text
/esw
L20:L27:L19:inc	-10.(r5)
jbr	L17
L18:.data
L28:.byte 045,0163,012,0
.even
.text
.data
L29:.byte 0
.even
.text
tst	4.(r5)
jgt	L10008
mov	$L29,(sp)
jbr	L10009
L10008:mov	-8.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),(sp)
L10009:mov	$L28,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L10:inc	-8.(r5)
mov	-8.(r5),r0
cmp	4.(r5),r0
jgt	L12
L11:L2:jmp	rretrn
.globl	_count
.text
_count:
jsr	r5,rsave; 2
mov	$01,r4
clr	r3
mov	$_linect,(sp)
jsr	pc,*$_clear
mov	$_alphct,(sp)
jsr	pc,*$_clear
mov	$_punct,(sp)
jsr	pc,*$_clear
mov	$_wordct,(sp)
jsr	pc,*$_clear
mov	$_ctlct,(sp)
jsr	pc,*$_clear
L31:mov	$_buf,(sp)
jsr	pc,*$_getc
mov	r0,r2
jlt	L32
bic	$!0177,r2
tst	r4
jeq	L33
tst	_roffflg
jeq	L34
cmp	r2,$056
jeq	L10010
cmp	r2,$047
jne	L34
L10010:mov	$_ctlct,(sp)
jsr	pc,*$_inc
L35:mov	$_buf,(sp)
jsr	pc,*$_getc
tst	r0
jge	L10011
clr	r0
jbr	L10012
L10011:mov	$01,r0
L10012:mov	r0,r2
jeq	L36
cmp	r2,$012
jeq	L36
jbr	L35
L36:jbr	L31
jbr	L37
L34:clr	r4
mov	$_linect,(sp)
jsr	pc,*$_inc
L37:L33:tst	r2
jlt	L31
movb	_class(r2),r2
bit	$01,r2
jne	L31
bit	$014,r2
jeq	L38
cmp	r3,r2
jeq	L31
tst	r3
jne	L39
mov	$_wordct,(sp)
jsr	pc,*$_inc
L39:mov	r2,r3
bit	$010,r2
jeq	L40
mov	$_alphct,(sp)
jsr	pc,*$_inc
jbr	L41
L40:mov	$_punct,(sp)
jsr	pc,*$_inc
L41:jbr	L31
L38:bit	$022,r2
jeq	L42
clr	r3
bit	$020,r2
jeq	L43
mov	$01,r4
L43:L42:jbr	L31
L32:L30:jmp	rretrn
.globl	_clear
.text
_clear:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	$00,2.(r0)
mov	2.(r0),r0
mov	r0,*4.(r5)
L44:jmp	rretrn
.globl	_inc
.text
_inc:
jsr	r5,rsave; 2
inc	*4.(r5)
tst	*4.(r5)
jne	L46
mov	4.(r5),r0
inc	2.(r0)
L46:L45:jmp	rretrn
.globl	_putd
.text
_putd:
jsr	r5,rsave; 2
.data
L48:.byte 045,067,0163,040,0
.even
.text
mov	*4.(r5),(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_locv
tst	(sp)+
mov	r0,(sp)
mov	$L48,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L47:jmp	rretrn
