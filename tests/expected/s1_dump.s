.globl	_dargv
.data
_dargv=.
0.
.text
L1:.byte 0151,0
.even
.data
L1
.text
L2:.byte 057,0144,0145,0166,057,0162,0160,060,0
.even
.data
L2
0.
.text
L3:.byte 057,0145,0164,0143,057,0144,0144,0141,0164,0145,0
.even
.data
.globl	_dfile
.data
_dfile=.
L3
.text
L4:.byte 057,0144,0145,0166,057,0155,0164,060,0
.even
.data
.globl	_ofile
.data
_ofile=.
L4
.globl	_sblock
.comm	_sblock,01004
.globl	_talist
.comm	_talist,02
.globl	_fi
.comm	_fi,02
.globl	_buf
.comm	_buf,01000
.globl	_dbuf
.comm	_dbuf,01000
.globl	_ibuf
.comm	_ibuf,01000
.globl	_date
.comm	_date,04
.globl	_ddate
.comm	_ddate,04
.globl	_fo
.comm	_fo,02
.globl	_pher
.comm	_pher,02
.globl	_dflg
.comm	_dflg,02
.globl	_tsize
.data
_tsize=.
15000.
.globl	_main
.text
_main:
jsr	r5,rsave; 14
mov	$_date,(sp)
jsr	pc,*$_time
cmp	4.(r5),$01
jne	L6
mov	$_dargv,6.(r5)
mov	$01,4.(r5)
L7:mov	4.(r5),r0
asl	r0
tst	_dargv(r0)
jeq	L8
L9:inc	4.(r5)
jbr	L7
L8:L6:dec	4.(r5)
add	$02,6.(r5)
mov	*6.(r5),-8.(r5)
L10:tstb	*-8.(r5)
jeq	L11
mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
jbr	L13
L14:.data
L15:.byte 0142,0141,0144,040,0143,0150,0141,0162,0141,0143,0164,0145,0162,040,0151,0156,040,0153,0145,0171,012,0
.even
.text
mov	$L15,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L16:clr	(sp)
mov	_dfile,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-12.(r5)
tst	-12.(r5)
jlt	L17
mov	$04,(sp)
mov	$_ddate,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	-12.(r5),(sp)
jsr	pc,*$_close
L17:jbr	L10
L18:mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
mov	r0,r1
mul	$011,r1
mov	r1,_tsize
add	$02,6.(r5)
dec	4.(r5)
jbr	L10
L19:inc	_dflg
jbr	L10
L20:clr	r0
mov	r0,2.+_ddate
mov	r0,_ddate
jbr	L10
L21:mov	$052140,-12.(r5)
jbr	L22
L23:mov	$01604,-12.(r5)
jbr	L22
L22:mov	_date,_ddate
mov	2.+_date,2.+_ddate
mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
ash	$02,r0
mov	r0,-10.(r5)
add	$02,6.(r5)
dec	4.(r5)
L24:tst	-10.(r5)
jeq	L25
cmp	-12.(r5),2.+_ddate
jlos	L26
dec	_ddate
L26:sub	-12.(r5),2.+_ddate
dec	-10.(r5)
jbr	L24
L25:jbr	L10
jbr	L12
L13:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:60
144
150
151
163
165
L10002:..
L20
L21
L23
L16
L18
L19
L14
.text
/esw
L12:jbr	L10
L11:cmp	4.(r5),$01
jgt	L27
.data
L28:.byte 0156,0157,040,0146,0151,0154,0145,040,0163,0171,0163,0164,0145,0155,040,0163,0160,0145,0143,0151,0146,0151,0145,0144,012,0
.even
.text
mov	$L28,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L27:.data
L29:.byte 045,0163,072,012,0
.even
.text
mov	6.(r5),r0
mov	2.(r0),(sp)
mov	$L29,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
tst	_fi
jge	L30
.data
L31:.byte 0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	6.(r5),r0
mov	2.(r0),(sp)
mov	$L31,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L30:.data
L32:.byte 0151,0156,0143,0162,0145,0155,0145,0156,0164,0141,0154,040,0144,0165,0155,0160,040,0146,0162,0157,0155,012,0
.even
.text
mov	$L32,(sp)
jsr	pc,*$_printf
mov	$_ddate,(sp)
jsr	pc,*$_pdate
jsr	pc,_sync
mov	$_sblock,(sp)
mov	$01,-(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	_sblock,r0
ash	$05,r0
mov	r0,(sp)
clr	-(sp)
jsr	pc,*$_size
tst	(sp)+
ash	$011,r0
mov	r0,(sp)
jsr	pc,*$_sbrk
mov	r0,_talist
mov	_talist,r4
clr	-14.(r5)
clr	-16.(r5)
clr	-12.(r5)
L33:cmp	-12.(r5),_sblock
jhis	L34
mov	$_buf,(sp)
mov	-12.(r5),-(sp)
add	$02,(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	$_buf,r3
L36:cmp	r3,$512.+_buf
jhis	L37
tst	(r3)
jeq	L39
tstb	2.(r3)
jeq	L39
mov	6.(r3),(sp)
mov	r3,r0
movb	5.(r0),r0
mov	r0,-(sp)
jsr	pc,*$_size
tst	(sp)+
mov	r0,-10.(r5)
cmp	28.(r3),_ddate
jlo	L39
cmp	28.(r3),_ddate
jne	L10004
cmp	30.(r3),2.+_ddate
jlo	L39
L10004:inc	-14.(r5)
add	-10.(r5),-16.(r5)
mov	-10.(r5),r0
inc	r0
mov	r0,(r4)
L39:add	$02,r4
L38:add	$040,r3
jbr	L36
L37:L35:inc	-12.(r5)
jbr	L33
L34:.data
L40:.byte 045,0154,040,0146,0151,0154,0145,0163,012,045,0154,040,0142,0154,0157,0143,0153,0163,012,0
.even
.text
mov	-16.(r5),(sp)
mov	-14.(r5),-(sp)
mov	$L40,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_otape
mov	$_buf,r4
mov	r4,(sp)
jsr	pc,*$_clrbuf
mov	_sblock,(r4)+
mov	2.+_sblock,(r4)+
mov	_date,(r4)+
mov	2.+_date,(r4)+
mov	_ddate,(r4)+
mov	2.+_ddate,(r4)+
mov	_tsize,(r4)+
mov	$_buf,(sp)
jsr	pc,*$_swrite
mov	_sblock,r0
ash	$05,r0
mov	r0,(sp)
clr	-(sp)
jsr	pc,*$_size
tst	(sp)+
mov	r0,-12.(r5)
mov	_talist,r4
L41:mov	-12.(r5),r0
dec	-12.(r5)
tst	r0
jeq	L42
mov	r4,(sp)
jsr	pc,*$_bwrite
add	$01000,r4
jbr	L41
L42:mov	_talist,r4
clr	-12.(r5)
L43:cmp	-12.(r5),_sblock
jhis	L44
mov	$_buf,(sp)
mov	-12.(r5),-(sp)
add	$02,(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	$_buf,r3
L46:cmp	r3,$512.+_buf
jhis	L47
tst	(r4)
jeq	L49
mov	(r4),(sp)
add	$0177777,(sp)
mov	r3,-(sp)
jsr	pc,*$_dump
tst	(sp)+
L49:add	$02,r4
L48:add	$040,r3
jbr	L46
L47:L45:inc	-12.(r5)
jbr	L43
L44:.data
L50:.byte 045,0154,040,0160,0150,0141,0163,0145,040,0145,0162,0162,0157,0162,0163,012,0
.even
.text
mov	_pher,(sp)
mov	$L50,-(sp)
jsr	pc,*$_printf
tst	(sp)+
tst	_dflg
jeq	L51
mov	$01,(sp)
mov	_dfile,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-12.(r5)
tst	-12.(r5)
jlt	L52
mov	$04,(sp)
mov	$_date,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
.data
L53:.byte 0144,0141,0164,0145,040,0165,0160,0144,0141,0164,0145,0144,012,0
.even
.text
mov	$L53,(sp)
jsr	pc,*$_printf
mov	$_date,(sp)
jsr	pc,*$_pdate
L52:L51:L5:jmp	rretrn
.globl	_pdate
.text
_pdate:
jsr	r5,rsave; 2
tst	*4.(r5)
jne	L55
mov	4.(r5),r0
tst	2.(r0)
jne	L55
.data
L56:.byte 0164,0150,0145,040,0145,0160,0157,0143,0150,012,0
.even
.text
mov	$L56,(sp)
jsr	pc,*$_printf
jbr	L57
L55:mov	4.(r5),(sp)
jsr	pc,*$_ctime
mov	r0,(sp)
jsr	pc,*$_printf
L57:L54:jmp	rretrn
.globl	_dump
.text
_dump:
jsr	r5,rsave; 2
mov	$_dbuf,r4
mov	4.(r5),r3
mov	r4,(sp)
jsr	pc,*$_clrbuf
L59:mov	4.(r5),r0
add	$040,r0
cmp	r3,r0
jhis	L60
mov	(r3)+,(r4)+
jbr	L59
L60:mov	$_dbuf,(sp)
jsr	pc,*$_swrite
bit	$020000,*4.(r5)
jeq	L61
tst	6.(r5)
jeq	L62
.data
L63:.byte 0163,0160,0145,0143,0151,0141,0154,012,0
.even
.text
mov	$L63,(sp)
jsr	pc,*$_printf
L62:jbr	L58
L61:mov	4.(r5),r0
add	$010,r0
mov	r0,r4
L64:mov	4.(r5),r0
add	$030,r0
cmp	r4,r0
jhis	L65
tst	(r4)
jeq	L66
bit	$010000,*4.(r5)
jeq	L67
mov	$_ibuf,(sp)
mov	(r4),-(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	$_ibuf,r3
L68:cmp	r3,$512.+_ibuf
jhis	L69
tst	(r3)
jeq	L70
dec	6.(r5)
jlt	L71
mov	$_dbuf,(sp)
mov	(r3),-(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	$_dbuf,(sp)
jsr	pc,*$_bwrite
L70:add	$02,r3
jbr	L68
L69:jbr	L72
L67:dec	6.(r5)
jlt	L71
mov	$_dbuf,(sp)
mov	(r4),-(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	$_dbuf,(sp)
jsr	pc,*$_bwrite
L72:L66:add	$02,r4
jbr	L64
L65:tst	6.(r5)
jne	L71
jbr	L58
L71:mov	$_dbuf,(sp)
jsr	pc,*$_clrbuf
L73:dec	6.(r5)
jlt	L74
mov	$_dbuf,(sp)
jsr	pc,*$_bwrite
jbr	L73
L74:inc	_pher
L58:jmp	rretrn
.globl	_bread
.text
_bread:
jsr	r5,rsave; 2
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L76
.data
L77:.byte 0162,0145,0141,0144,040,0145,0162,0162,0157,0162,040,045,0154,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L77,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L76:L75:jmp	rretrn
.globl	_clrbuf
.text
_clrbuf:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	$0400,r4
L79:mov	r4,r0
dec	r4
tst	r0
jeq	L80
clr	(r3)+
jbr	L79
L80:L78:jmp	rretrn
.globl	_swrite
.text
_swrite:
jsr	r5,rsave; 2
mov	$0377,r4
clr	r3
mov	4.(r5),r2
L82:mov	r4,r0
dec	r4
tst	r0
jeq	L83
add	(r2)+,r3
jbr	L82
L83:mov	$031415,r0
sub	r3,r0
mov	r0,(r2)
mov	4.(r5),(sp)
jsr	pc,*$_bwrite
L81:jmp	rretrn
.globl	_bwrite
.text
_bwrite:
.bss
L85:.=.+2.
.text
jsr	r5,rsave; 2
mov	L85,r0
inc	L85
cmp	_tsize,r0
jhis	L86
.data
L87:.byte 0143,0150,0141,0156,0147,0145,040,0164,0141,0160,0145,0163,012,0
.even
.text
mov	$L87,(sp)
jsr	pc,*$_printf
mov	_fo,(sp)
jsr	pc,*$_close
L88:jsr	pc,_getchar
mov	r0,L85
cmp	$012,r0
jeq	L89
tst	L85
jne	L90
jsr	pc,_exit
L90:jbr	L88
L89:clr	L85
jsr	pc,_otape
L86:mov	$01000,(sp)
mov	4.(r5),-(sp)
mov	_fo,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L91
.data
L92:.byte 0167,0162,0151,0164,0145,040,0145,0162,0162,0157,0162,012,0
.even
.text
mov	$L92,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L91:L84:jmp	rretrn
.globl	_number
.text
_number:
jsr	r5,rsave; 2
clr	r4
L94:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r3
jeq	L95
cmp	r3,$060
jlt	L94
cmp	r3,$071
jgt	L94
mov	r4,r1
mul	$012,r1
add	r3,r1
add	$0177720,r1
mov	r1,r4
jbr	L94
L95:mov	r4,r0
jbr	L93
L93:jmp	rretrn
.globl	_size
.text
_size:
jsr	r5,rsave; 2
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
bic	$!0377,(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
tst	_ldivr
jeq	L97
inc	r4
L97:mov	r4,r0
jbr	L96
L96:jmp	rretrn
.globl	_otape
.text
_otape:
jsr	r5,rsave; 2
mov	$01,(sp)
mov	_ofile,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fo
tst	_fo
jge	L99
.data
L100:.byte 0143,0141,0156,040,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	_ofile,(sp)
mov	$L100,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L99:L98:jmp	rretrn
