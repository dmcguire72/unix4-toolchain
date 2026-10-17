.globl	_ab
.comm	_ab,03720
.globl	_ava
.comm	_ava,0620
.globl	_av
.data
_av=.
_ava
.globl	_string
.data
_string=.
_ab
.globl	_errno
.comm	_errno,02
.globl	_main
.text
_main:
jsr	r5,rsave; 112
cmp	4.(r5),$03
jge	L2
.data
L3:.byte 0101,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$012,(sp)
mov	$L3,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L1
L2:clr	-18.(r5)
add	$02,_av
clr	14.+-34.(r5)
L4:add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),r0
mov	r0,-76.(r5)
mov	r0,-78.(r5)
L5:mov	-76.(r5),r0
inc	-76.(r5)
movb	(r0),r0
mov	r0,-12.(r5)
jeq	L6
cmp	-12.(r5),$052
jeq	L7
cmp	-12.(r5),$077
jeq	L7
cmp	-12.(r5),$0133
jeq	L7
jbr	L5
L6:mov	-78.(r5),(sp)
jsr	pc,*$_copy
mov	-18.(r5),r1
inc	-18.(r5)
asl	r1
add	_av,r1
mov	r0,(r1)
dec	4.(r5)
mov	4.(r5),r0
cmp	$02,r0
jle	L4
jbr	L8
L7:dec	-76.(r5)
mov	-76.(r5),r0
movb	(r0),r0
cmp	$057,r0
jne	L9
clrb	*-76.(r5)
.data
L10:.byte 057,0
.even
.text
clr	(sp)
cmp	-76.(r5),-78.(r5)
jne	L10000
mov	$L10,-(sp)
jbr	L10001
L10000:mov	-78.(r5),-(sp)
L10001:jsr	pc,*$_open
tst	(sp)+
mov	r0,-16.(r5)
jlt	L11
mov	-76.(r5),r0
inc	-76.(r5)
movb	$057,(r0)
jbr	L12
L9:cmp	-76.(r5),-78.(r5)
jne	L7
.data
L13:.byte 056,0
.even
.text
clr	(sp)
mov	$L13,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-16.(r5)
jge	L12
L11:.data
L14:.byte 0116,0157,040,0144,0151,0162,0145,0143,0164,0157,0162,0171,012,0
.even
.text
mov	$015,(sp)
mov	$L14,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L1
L12:mov	-18.(r5),-10.(r5)
L15:L16:mov	$02,(sp)
mov	r5,r0
add	$0177762,r0
mov	r0,-(sp)
mov	-16.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
tst	r0
jle	L17
mov	$016,(sp)
mov	r5,r0
add	$0177736,r0
mov	r0,-(sp)
mov	-16.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
tst	-14.(r5)
jeq	L15
mov	-76.(r5),(sp)
mov	r5,r0
add	$0177736,r0
mov	r0,-(sp)
jsr	pc,*$_match
tst	(sp)+
tst	r0
jeq	L18
movb	*-76.(r5),r0
mov	r0,-12.(r5)
clrb	*-76.(r5)
mov	r5,r0
add	$0177736,r0
mov	r0,(sp)
mov	-78.(r5),-(sp)
jsr	pc,*$_cat
tst	(sp)+
mov	-18.(r5),r1
inc	-18.(r5)
asl	r1
add	_av,r1
mov	r0,(r1)
movb	-12.(r5),*-76.(r5)
L18:jbr	L16
L17:mov	-16.(r5),(sp)
jsr	pc,*$_close
mov	-10.(r5),-8.(r5)
L19:mov	-18.(r5),r0
add	$0177777,r0
cmp	-8.(r5),r0
jge	L20
mov	-8.(r5),-10.(r5)
L21:inc	-10.(r5)
mov	-10.(r5),r0
cmp	-18.(r5),r0
jle	L22
mov	-10.(r5),r0
asl	r0
add	_av,r0
mov	(r0),(sp)
mov	-8.(r5),r0
asl	r0
add	_av,r0
mov	(r0),-(sp)
jsr	pc,*$_compar
tst	(sp)+
tst	r0
jeq	L23
mov	-8.(r5),r0
asl	r0
add	_av,r0
mov	(r0),-12.(r5)
mov	-8.(r5),r0
asl	r0
add	_av,r0
mov	-10.(r5),r1
asl	r1
add	_av,r1
mov	(r1),(r0)
mov	-10.(r5),r0
asl	r0
add	_av,r0
mov	-12.(r5),(r0)
L23:jbr	L21
L22:inc	-8.(r5)
jbr	L19
L20:dec	4.(r5)
mov	4.(r5),r0
cmp	$02,r0
jle	L4
L8:cmp	-18.(r5),$01
jgt	L24
.data
L25:.byte 0116,0157,040,0155,0141,0164,0143,0150,012,0
.even
.text
mov	$011,(sp)
mov	$L25,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L1
L24:mov	-18.(r5),r0
asl	r0
add	_av,r0
clr	(r0)
mov	_av,(sp)
jsr	pc,*$_strip
mov	_av,(sp)
mov	*_av,-(sp)
jsr	pc,*$_execv
tst	(sp)+
cmp	_errno,$07
jeq	L26
.data
L27:.byte 057,0142,0151,0156,057,0
.even
.text
mov	*_av,(sp)
mov	$L27,-(sp)
jsr	pc,*$_cat
tst	(sp)+
mov	r0,-8.(r5)
mov	_av,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_execv
tst	(sp)+
cmp	_errno,$07
jeq	L26
.data
L28:.byte 057,0165,0163,0162,0
.even
.text
mov	-8.(r5),(sp)
mov	$L28,-(sp)
jsr	pc,*$_cat
tst	(sp)+
mov	r0,-8.(r5)
mov	_av,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_execv
tst	(sp)+
cmp	_errno,$02
jeq	L29
mov	-8.(r5),*_av
.data
L30:.byte 057,0142,0151,0156,057,0163,0150,0
.even
.text
sub	$02,_av
mov	_av,r0
mov	$L30,(r0)
mov	_av,(sp)
mov	*_av,-(sp)
jsr	pc,*$_execv
tst	(sp)+
L29:.data
L31:.byte 0103,0157,0155,0155,0141,0156,0144,040,0156,0157,0164,040,0146,0157,0165,0156,0144,012,0
.even
.text
mov	$022,(sp)
mov	$L31,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L1
L26:.data
L32:.byte 0101,0162,0147,040,0154,0151,0163,0164,040,0164,0157,0157,040,0154,0157,0156,0147,012,0
.even
.text
mov	$022,(sp)
mov	$L32,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L1:jmp	rretrn
.globl	_match
.text
_match:
jsr	r5,rsave; 2
cmpb	*4.(r5),$056
jne	L34
cmpb	*6.(r5),$056
jeq	L34
clr	r0
jbr	L33
L34:mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_amatch
tst	(sp)+
jbr	L33
L33:jmp	rretrn
.globl	_amatch
.text
_amatch:
jsr	r5,rsave; 14
movb	*4.(r5),r0
mov	r0,-16.(r5)
mov	$077777,-14.(r5)
movb	*6.(r5),r0
mov	r0,-8.(r5)
jbr	L37
L38:clr	-12.(r5)
L39:inc	6.(r5)
mov	6.(r5),r0
movb	(r0),r0
mov	r0,-10.(r5)
jeq	L40
mov	-10.(r5),r0
jbr	L42
L43:tst	-12.(r5)
jeq	L44
inc	6.(r5)
mov	6.(r5),(sp)
inc	4.(r5)
mov	4.(r5),-(sp)
jsr	pc,*$_amatch
tst	(sp)+
jbr	L35
L44:clr	r0
jbr	L35
L45:mov	6.(r5),r0
movb	1.(r0),r0
mov	r0,-10.(r5)
cmp	-16.(r5),r0
jle	L10002
clr	r0
jbr	L10003
L10002:mov	$01,r0
L10003:cmp	-14.(r5),-16.(r5)
jle	L10004
clr	r1
jbr	L10005
L10004:mov	$01,r1
L10005:com	r1
bic	r1,r0
bis	r0,-12.(r5)
jbr	L41
L42:mov	$L10007,r1
mov	r0,L10008
L10009:cmp	r0,(r1)+
jne	L10009
jmp	*L10008-L10007(r1)
.data
L10007:55
135
L10008:..
L45
L43
L46
.text
/esw
L41:L46:mov	-10.(r5),r0
mov	r0,-14.(r5)
cmp	-16.(r5),r0
jne	L47
inc	-12.(r5)
L47:jbr	L39
L40:clr	r0
jbr	L35
L48:L49:tst	-16.(r5)
jeq	L50
inc	6.(r5)
mov	6.(r5),(sp)
inc	4.(r5)
mov	4.(r5),-(sp)
jsr	pc,*$_amatch
tst	(sp)+
jbr	L35
L50:clr	r0
jbr	L35
L51:inc	6.(r5)
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_umatch
tst	(sp)+
jbr	L35
L52:mov	-16.(r5),r0
beq	1f
clr	r0
br	2f
1:	mov	$1,r0
2:
jbr	L35
jbr	L36
L37:mov	$L10011,r1
mov	r0,L10012
L10013:cmp	r0,(r1)+
jne	L10013
jmp	*L10012-L10011(r1)
.data
L10011:0
52
77
133
L10012:..
L52
L51
L48
L38
L53
.text
/esw
L36:L53:cmp	-8.(r5),-16.(r5)
jeq	L49
clr	r0
jbr	L35
L35:jmp	rretrn
.globl	_umatch
.text
_umatch:
jsr	r5,rsave; 2
tstb	*6.(r5)
jne	L55
mov	$01,r0
jbr	L54
L55:L56:tstb	*4.(r5)
jeq	L57
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
inc	4.(r5)
jsr	pc,*$_amatch
tst	(sp)+
tst	r0
jeq	L58
mov	$01,r0
jbr	L54
L58:jbr	L56
L57:clr	r0
jbr	L54
L54:jmp	rretrn
.globl	_compar
.text
_compar:
jsr	r5,rsave; 6
L60:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
mov	r0,-8.(r5)
jne	L61
clr	r0
jbr	L59
L61:mov	6.(r5),r0
inc	6.(r5)
movb	(r0),r0
mov	r0,-10.(r5)
jne	L62
mov	$01,r0
jbr	L59
L62:cmp	-8.(r5),-10.(r5)
jeq	L60
cmp	-8.(r5),-10.(r5)
jgt	L10014
clr	r0
jbr	L10015
L10014:mov	$01,r0
L10015:jbr	L59
L59:jmp	rretrn
.globl	_copy
.text
_copy:
jsr	r5,rsave; 4
mov	_string,-8.(r5)
L64:mov	_string,r0
inc	_string
mov	4.(r5),r1
inc	4.(r5)
movb	(r1),(r0)
jeq	L65
jbr	L64
L65:mov	-8.(r5),r0
jbr	L63
L63:jmp	rretrn
.globl	_cat
.text
_cat:
jsr	r5,rsave; 4
mov	_string,-8.(r5)
L67:mov	_string,r0
inc	_string
mov	4.(r5),r1
inc	4.(r5)
movb	(r1),(r0)
jeq	L68
jbr	L67
L68:dec	_string
L69:mov	_string,r0
inc	_string
mov	6.(r5),r1
inc	6.(r5)
movb	(r1),(r0)
jeq	L70
jbr	L69
L70:mov	-8.(r5),r0
jbr	L66
L66:jmp	rretrn
.globl	_strip
.text
_strip:
jsr	r5,rsave; 2
mov	4.(r5),r4
L72:mov	(r4)+,r3
jeq	L73
L74:tstb	(r3)
jeq	L75
bicb	$!0177,(r3)+
jbr	L74
L75:jbr	L72
L73:L71:jmp	rretrn
