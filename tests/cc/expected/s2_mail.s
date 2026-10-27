.globl	_iobuf
.comm	_iobuf,01006
.globl	_ubuf
.comm	_ubuf,020
.globl	_main
.text
_main:
.bss
L2:.=.+20.
.text
.bss
L3:.=.+40.
.text
.bss
L4:.=.+20.
.text
.bss
L5:.=.+40.
.text
.bss
L6:.=.+40.
.text
jsr	r5,rsave; 22
cmp	4.(r5),$01
jeq	L7
cmp	4.(r5),$02
jne	L10000
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jeq	L7
L10000:.data
L8:.byte 057,0145,0164,0143,057,0160,0141,0163,0163,0167,0144,0
.even
.text
mov	$_iobuf,(sp)
mov	$L8,-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L9
.data
L10:.byte 0103,0141,0156,047,0164,040,0157,0160,0145,0156,040,0160,0141,0163,0163,0167,0157,0162,0144,040,0146,0151,0154,0145,012,0
.even
.text
mov	$L10,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L9:clr	(sp)
jsr	pc,*$_ttyn
mov	r0,-22.(r5)
jeq	L11
.data
L12:.byte 057,0164,0155,0160,057,0165,0164,0155,0160,0
.even
.text
clr	(sp)
mov	$L12,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-20.(r5)
jlt	L13
L14:mov	$020,(sp)
mov	$_ubuf,-(sp)
mov	-20.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
tst	r0
jeq	L15
movb	8.+_ubuf,r0
cmp	-22.(r5),r0
jne	L16
mov	-20.(r5),(sp)
jsr	pc,*$_close
clr	-20.(r5)
L17:cmp	-20.(r5),$010
jge	L18
mov	-20.(r5),r0
movb	_ubuf(r0),r0
cmp	$040,r0
jeq	L18
mov	-20.(r5),r0
mov	-20.(r5),r1
movb	_ubuf(r1),L2(r0)
inc	-20.(r5)
jbr	L17
L18:mov	-20.(r5),r0
clrb	L2(r0)
jbr	L19
L16:jbr	L14
L15:L11:L13:jsr	pc,_getuid
mov	r0,-22.(r5)
L20:mov	$L2,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L10001
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L10001
mov	$L4,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L10001
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L10001
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L10001
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L10001
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jne	L22
L10001:.data
L23:.byte 0127,0150,0157,040,0141,0162,0145,040,0171,0157,0165,077,012,0
.even
.text
mov	$L23,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L22:mov	$L4,(sp)
jsr	pc,*$_atoi
cmp	-22.(r5),r0
jeq	L21
jbr	L20
L21:L19:movb	_iobuf,r0
mov	r0,(sp)
jsr	pc,*$_close
cmp	4.(r5),$02
jle	L24
mov	6.(r5),r0
mov	2.(r0),-20.(r5)
jbr	L25
L24:clr	-20.(r5)
L25:cmp	4.(r5),$02
jle	L10002
mov	$01,r0
jbr	L10003
L10002:clr	r0
L10003:mov	r0,-12.(r5)
.data
L26:.byte 057,0145,0164,0143,057,0160,0141,0163,0163,0167,0144,0
.even
.text
mov	$_iobuf,(sp)
mov	$L26,-(sp)
jsr	pc,*$_fopen
tst	(sp)+
mov	$02,-14.(r5)
L27:mov	$L5,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L28
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L28
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L28
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L28
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L28
mov	$L6,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L28
mov	$L3,(sp)
jsr	pc,*$_getfield
tst	r0
jeq	L28
mov	-12.(r5),-10.(r5)
L29:inc	-10.(r5)
mov	-10.(r5),r0
cmp	4.(r5),r0
jle	L30
mov	$L5,(sp)
mov	-10.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_comp
tst	(sp)+
tst	r0
jeq	L31
mov	$L6,(sp)
mov	$L5,-(sp)
mov	$L2,-(sp)
mov	-20.(r5),-(sp)
jsr	pc,*$_send
add	$06,sp
mov	-10.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
clrb	(r0)
inc	-14.(r5)
mov	-14.(r5),r0
cmp	4.(r5),r0
jgt	L32
jsr	pc,_exit
L32:L31:jbr	L29
L30:jbr	L27
L28:mov	-12.(r5),-10.(r5)
L33:inc	-10.(r5)
mov	-10.(r5),r0
cmp	4.(r5),r0
jle	L34
mov	-10.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
tstb	(r0)
jeq	L35
clrb	L3
mov	$L3,(sp)
mov	-10.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_append
tst	(sp)+
mov	$L3,(sp)
mov	-10.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
mov	$L2,-(sp)
mov	-20.(r5),-(sp)
jsr	pc,*$_send
add	$06,sp
L35:jbr	L33
L34:jsr	pc,_exit
L7:.data
L36:.byte 0155,0141,0151,0154,0142,0157,0170,0
.even
.text
clr	(sp)
mov	$L36,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-18.(r5)
jge	L37
.data
L38:.byte 0116,0157,040,0155,0141,0151,0154,012,0
.even
.text
mov	$L38,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L37:L39:mov	$01000,(sp)
mov	$_iobuf,-(sp)
mov	-18.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-16.(r5)
jeq	L40
mov	-16.(r5),(sp)
mov	$_iobuf,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L39
L40:cmp	4.(r5),$02
jeq	L41
.data
L42:.byte 0123,0141,0166,0145,077,040,0
.even
.text
mov	$L42,(sp)
jsr	pc,*$_printf
mov	$01000,(sp)
mov	$_iobuf,r0
mov	r0,-8.(r5)
mov	r0,-(sp)
clr	-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
jbr	L43
L41:mov	6.(r5),r0
mov	2.(r0),r0
inc	r0
mov	r0,-8.(r5)
L43:cmpb	*-8.(r5),$0171
jne	L44
.data
L45:.byte 0155,0141,0151,0154,0142,0157,0170,0
.even
.text
.data
L46:.byte 0155,0142,0157,0170,0
.even
.text
clr	(sp)
mov	$L46,-(sp)
mov	$L45,-(sp)
jsr	pc,*$_concat
cmp	(sp)+,(sp)+
tst	r0
jeq	L47
.data
L48:.byte 0117,0154,0144,040,0155,0141,0151,0154,040,0151,0156,040,0140,0155,0142,0157,0170,047,012,0
.even
.text
mov	$L48,(sp)
jsr	pc,*$_printf
L47:L44:.data
L49:.byte 0155,0141,0151,0154,0142,0157,0170,0
.even
.text
mov	$L49,(sp)
jsr	pc,*$_unlink
jsr	pc,_exit
L1:jmp	rretrn
.globl	_getfield
.text
_getfield:
jsr	r5,rsave; 6
clr	-8.(r5)
L51:mov	4.(r5),-(sp)
add	-8.(r5),(sp)
mov	$_iobuf,-(sp)
jsr	pc,_getc
tst	(sp)+
movb	r0,*(sp)+
movb	r0,-10.(r5)
jlt	L52
cmpb	-10.(r5),$072
jeq	L10004
cmpb	-10.(r5),$012
jne	L53
L10004:mov	4.(r5),r0
add	-8.(r5),r0
clrb	(r0)
mov	$01,r0
jbr	L50
jbr	L54
L53:inc	-8.(r5)
L54:jbr	L51
L52:clr	r0
jbr	L50
L50:jmp	rretrn
.globl	_send
.text
_send:
.bss
L56:.=.+512.
.text
jsr	r5,rsave; 4
clr	-8.(r5)
L57:mov	-8.(r5),r0
inc	-8.(r5)
add	10.(r5),r0
tstb	(r0)
jeq	L58
jbr	L57
L58:.data
L59:.byte 057,0155,0141,0151,0154,0142,0157,0170,0
.even
.text
mov	10.(r5),(sp)
mov	$L59,-(sp)
jsr	pc,*$_append
tst	(sp)+
clrb	L56
.data
L60:.byte 012,0106,0162,0157,0155,040,0
.even
.text
mov	$L56,(sp)
mov	$L60,-(sp)
jsr	pc,*$_append
tst	(sp)+
mov	$L56,(sp)
mov	6.(r5),-(sp)
jsr	pc,*$_append
tst	(sp)+
.data
L61:.byte 040,0
.even
.text
mov	$L56,(sp)
mov	$L61,-(sp)
jsr	pc,*$_append
tst	(sp)+
mov	$256.+L56,(sp)
jsr	pc,*$_time
mov	$L56,(sp)
mov	$256.+L56,-(sp)
jsr	pc,_ctime
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_append
tst	(sp)+
mov	$L56,(sp)
mov	$01,-(sp)
mov	10.(r5),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_concat
add	$06,sp
tst	r0
jne	L62
.data
L63:.byte 0103,0141,0156,047,0164,040,0163,0145,0156,0144,040,0164,0157,040,0140,045,0163,047,012,0
.even
.text
mov	8.(r5),(sp)
mov	$L63,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L62:mov	10.(r5),r0
add	-8.(r5),r0
clrb	(r0)
L55:jmp	rretrn
.globl	_comp
.text
_comp:
jsr	r5,rsave; 6
clr	-8.(r5)
L65:mov	4.(r5),r0
add	-8.(r5),r0
movb	(r0),r0
movb	r0,-10.(r5)
mov	6.(r5),r1
add	-8.(r5),r1
movb	(r1),r1
cmp	r0,r1
jne	L66
tstb	-10.(r5)
jne	L67
mov	$01,r0
jbr	L64
L67:inc	-8.(r5)
jbr	L65
L66:clr	r0
jbr	L64
L64:jmp	rretrn
.globl	_atoi
.text
_atoi:
jsr	r5,rsave; 6
clr	-8.(r5)
clr	-10.(r5)
L69:mov	4.(r5),r0
add	-10.(r5),r0
tstb	(r0)
jeq	L70
mov	-8.(r5),r1
mul	$012,r1
mov	-10.(r5),r2
inc	-10.(r5)
add	4.(r5),r2
movb	(r2),r2
add	r2,r1
add	$0177720,r1
mov	r1,-8.(r5)
jbr	L69
L70:mov	-8.(r5),r0
jbr	L68
L68:jmp	rretrn
.globl	_append
.text
_append:
jsr	r5,rsave; 6
clr	-8.(r5)
L72:mov	6.(r5),r0
add	-8.(r5),r0
tstb	(r0)
jeq	L73
inc	-8.(r5)
jbr	L72
L73:clr	-10.(r5)
L74:mov	-8.(r5),r0
inc	-8.(r5)
add	6.(r5),r0
mov	-10.(r5),r1
inc	-10.(r5)
add	4.(r5),r1
movb	(r1),(r0)
jeq	L75
jbr	L74
L75:L71:jmp	rretrn
.globl	_concat
.text
_concat:
.bss
L77:.=.+1024.
.text
jsr	r5,rsave; 12
.data
L78:.byte 057,0164,0155,0160,057,0155,0164,0155,00,00,0
.even
.text
mov	$L78,-8.(r5)
mov	-8.(r5),r0
movb	$0141,8.(r0)
L79:mov	-8.(r5),r0
movb	8.(r0),r0
cmp	$0172,r0
jlt	L80
mov	$L77,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jlt	L82
L81:mov	-8.(r5),r0
incb	8.(r0)
jbr	L79
L80:L83:.data
L84:.byte 0143,0141,0156,047,0164,040,0143,0162,0145,0141,0164,040,0164,0155,0160,040,0146,0151,0154,0145,012,0
.even
.text
mov	$L84,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L82:mov	$0400,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-12.(r5)
jlt	L83
tst	4.(r5)
jne	L85
clr	-10.(r5)
jbr	L86
L85:clr	(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-10.(r5)
jge	L87
mov	-12.(r5),(sp)
jsr	pc,*$_close
mov	-8.(r5),(sp)
jsr	pc,*$_unlink
.data
L88:.byte 0143,0141,0156,047,0164,040,0157,0160,0145,0156,040,0140,045,0163,047,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L88,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L87:L86:tst	8.(r5)
jeq	L89
clr	-14.(r5)
L90:mov	-14.(r5),r0
inc	-14.(r5)
add	10.(r5),r0
tstb	(r0)
jeq	L91
jbr	L90
L91:mov	-14.(r5),(sp)
add	$0177777,(sp)
mov	10.(r5),-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L89:L92:mov	$01000,(sp)
mov	$L77,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-14.(r5)
jeq	L93
mov	-14.(r5),(sp)
mov	$L77,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L92
L93:tst	8.(r5)
jeq	L94
.data
L95:.byte 012,0
.even
.text
mov	$01,(sp)
mov	$L95,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L94:tst	-10.(r5)
jeq	L96
mov	-10.(r5),(sp)
jsr	pc,*$_close
L96:mov	$L77,(sp)
mov	6.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jlt	L97
clr	(sp)
mov	6.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-10.(r5)
jge	L98
mov	-12.(r5),(sp)
jsr	pc,*$_close
mov	-8.(r5),(sp)
jsr	pc,*$_unlink
clr	r0
jbr	L76
L98:L99:mov	$01000,(sp)
mov	$L77,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-14.(r5)
jeq	L100
mov	-14.(r5),(sp)
mov	$L77,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L99
L100:mov	-10.(r5),(sp)
jsr	pc,*$_close
L97:mov	-12.(r5),(sp)
jsr	pc,*$_close
clr	(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-12.(r5)
jge	L101
mov	-8.(r5),(sp)
jsr	pc,*$_unlink
jbr	L83
L101:mov	$0666,(sp)
mov	6.(r5),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-10.(r5)
jge	L102
mov	-12.(r5),(sp)
jsr	pc,*$_close
mov	-8.(r5),(sp)
jsr	pc,*$_unlink
clr	r0
jbr	L76
L102:L103:mov	$01000,(sp)
mov	$L77,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-14.(r5)
jeq	L104
mov	-14.(r5),(sp)
mov	$L77,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L103
L104:mov	-10.(r5),(sp)
jsr	pc,*$_close
mov	-12.(r5),(sp)
jsr	pc,*$_close
mov	-8.(r5),(sp)
jsr	pc,*$_unlink
mov	$01,r0
jbr	L76
L76:jmp	rretrn
