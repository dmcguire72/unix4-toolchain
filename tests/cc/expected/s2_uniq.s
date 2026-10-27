.globl	_fields
.data
_fields=.
0.
.globl	_letters
.data
_letters=.
0.
.globl	_linec
.comm	_linec,02
.globl	_mode
.comm	_mode,02
.globl	_uniq
.comm	_uniq,02
.globl	_main
.text
_main:
.bss
L2:.=.+1000.
.text
.bss
L3:.=.+1000.
.text
jsr	r5,rsave; 4
L4:cmp	4.(r5),$01
jle	L5
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jne	L6
mov	6.(r5),r0
mov	2.(r0),r0
movb	1.(r0),r0
cmp	$071,r0
jlt	L7
mov	6.(r5),r0
mov	2.(r0),(sp)
inc	(sp)
jsr	pc,*$_conv
mov	r0,_fields
jbr	L8
L7:mov	6.(r5),r0
mov	2.(r0),r0
movb	1.(r0),r0
mov	r0,_mode
L8:dec	4.(r5)
add	$02,6.(r5)
jbr	L4
L6:mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$053,r0
jne	L9
mov	6.(r5),r0
mov	2.(r0),(sp)
inc	(sp)
jsr	pc,*$_conv
mov	r0,_letters
dec	4.(r5)
add	$02,6.(r5)
jbr	L4
L9:clr	(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jge	L10
.data
L11:.byte 0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	6.(r5),r0
mov	2.(r0),(sp)
mov	$L11,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L10:mov	-8.(r5),_fin
jbr	L12
L5:clr	(sp)
jsr	pc,*$_dup
mov	r0,_fin
L12:cmp	4.(r5),$02
jle	L13
mov	$0666,(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jge	L14
.data
L15:.byte 0143,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,0145,040,045,0163,012,0
.even
.text
mov	6.(r5),r0
mov	4.(r0),(sp)
mov	$L15,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L14:mov	-8.(r5),_fout
jbr	L16
L13:mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
L16:clr	(sp)
jsr	pc,*$_close
mov	$01,(sp)
jsr	pc,*$_close
mov	$L2,(sp)
jsr	pc,*$_gline
L17:inc	_linec
mov	$L3,(sp)
jsr	pc,*$_gline
tst	r0
jeq	L18
mov	$L2,(sp)
jsr	pc,*$_pline
jsr	pc,_flush
jsr	pc,_exit
L18:mov	$L3,(sp)
mov	$L2,-(sp)
jsr	pc,*$_equal
tst	(sp)+
tst	r0
jne	L17
mov	$L2,(sp)
jsr	pc,*$_pline
clr	_linec
L19:inc	_linec
mov	$L2,(sp)
jsr	pc,*$_gline
tst	r0
jeq	L20
mov	$L3,(sp)
jsr	pc,*$_pline
jsr	pc,_flush
jsr	pc,_exit
L20:mov	$L3,(sp)
mov	$L2,-(sp)
jsr	pc,*$_equal
tst	(sp)+
tst	r0
jne	L19
mov	$L3,(sp)
jsr	pc,*$_pline
clr	_linec
jbr	L17
L1:jmp	rretrn
.globl	_gline
.text
_gline:
jsr	r5,rsave; 4
L22:jsr	pc,_getchar
mov	r0,-8.(r5)
cmp	$012,r0
jeq	L23
tst	-8.(r5)
jne	L24
mov	$01,r0
jbr	L21
L24:mov	4.(r5),r0
inc	4.(r5)
movb	-8.(r5),(r0)
jbr	L22
L23:clrb	*4.(r5)
clr	r0
jbr	L21
L21:jmp	rretrn
.globl	_pline
.text
_pline:
jsr	r5,rsave; 4
mov	_mode,r0
jbr	L27
L28:tst	_uniq
jeq	L29
clr	_uniq
jbr	L25
L29:jbr	L26
L30:tst	_uniq
jne	L26
jbr	L25
L31:.data
L32:.byte 045,064,0144,040,0
.even
.text
mov	_linec,(sp)
mov	$L32,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L26
L27:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:143
144
165
L10002:..
L31
L30
L28
L33
.text
/esw
L26:L33:clr	_uniq
L34:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
mov	r0,-8.(r5)
jeq	L35
mov	-8.(r5),(sp)
jsr	pc,*$_putchar
jbr	L34
L35:mov	$012,(sp)
jsr	pc,*$_putchar
L25:jmp	rretrn
.globl	_equal
.text
_equal:
jsr	r5,rsave; 4
mov	4.(r5),(sp)
jsr	pc,*$_skip
mov	r0,4.(r5)
mov	6.(r5),(sp)
jsr	pc,*$_skip
mov	r0,6.(r5)
L37:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
mov	r0,-8.(r5)
jeq	L38
mov	6.(r5),r0
inc	6.(r5)
movb	(r0),r0
cmp	-8.(r5),r0
jeq	L39
clr	r0
jbr	L36
L39:jbr	L37
L38:tstb	*6.(r5)
jeq	L40
clr	r0
jbr	L36
L40:inc	_uniq
mov	$01,r0
jbr	L36
L36:jmp	rretrn
.globl	_skip
.text
_skip:
jsr	r5,rsave; 6
clr	r0
mov	r0,-10.(r5)
mov	r0,-8.(r5)
L42:mov	-8.(r5),r0
inc	-8.(r5)
cmp	_fields,r0
jle	L43
L44:cmpb	*4.(r5),$040
jeq	L10004
cmpb	*4.(r5),$011
jne	L45
L10004:inc	4.(r5)
jbr	L44
L45:L46:cmpb	*4.(r5),$040
jeq	L47
cmpb	*4.(r5),$011
jeq	L47
tstb	*4.(r5)
jne	L48
mov	4.(r5),r0
jbr	L41
L48:inc	4.(r5)
jbr	L46
L47:jbr	L42
L43:L49:mov	-10.(r5),r0
inc	-10.(r5)
cmp	_letters,r0
jle	L50
tstb	*4.(r5)
jne	L51
mov	4.(r5),r0
jbr	L41
L51:inc	4.(r5)
jbr	L49
L50:mov	4.(r5),r0
jbr	L41
L41:jmp	rretrn
.globl	_conv
.text
_conv:
jsr	r5,rsave; 6
clr	-10.(r5)
L53:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
add	$0177720,r0
mov	r0,-8.(r5)
tst	-8.(r5)
jlt	L54
cmp	-8.(r5),$011
jgt	L54
mov	-10.(r5),r1
mul	$012,r1
add	-8.(r5),r1
mov	r1,-10.(r5)
jbr	L53
L54:mov	-10.(r5),r0
jbr	L52
L52:jmp	rretrn
