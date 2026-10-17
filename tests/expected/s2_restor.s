.globl	_dargv
.data
_dargv=.
0.
.text
L1:.byte 0164,0
.even
.data
L1
0.
.text
L2:.byte 057,0144,0145,0166,057,0155,0164,060,0
.even
.data
.globl	_ifile
.data
_ifile=.
L2
.globl	_ofile
.comm	_ofile,02
.globl	_sblock
.comm	_sblock,01004
.globl	_isize
.comm	_isize,02
.globl	_talist
.comm	_talist,02
.globl	_fi
.comm	_fi,02
.globl	_buf
.comm	_buf,01000
.globl	_dbuf
.comm	_dbuf,01000
.globl	_cbuf
.comm	_cbuf,01000
.globl	_date
.comm	_date,04
.globl	_ddate
.comm	_ddate,04
.globl	_fo
.comm	_fo,02
.globl	_pher
.comm	_pher,02
.globl	_tsize
.data
_tsize=.
15000.
.globl	_iflg
.comm	_iflg,02
.globl	_wflg
.comm	_wflg,02
.globl	_file
.comm	_file,012
.globl	_ilist
.comm	_ilist,0310
.globl	_main
.text
_main:
jsr	r5,rsave; 14
cmp	4.(r5),$01
jne	L4
mov	$_dargv,6.(r5)
mov	$01,4.(r5)
L5:mov	4.(r5),r0
asl	r0
tst	_dargv(r0)
jeq	L6
L7:inc	4.(r5)
jbr	L5
L6:L4:dec	4.(r5)
add	$02,6.(r5)
mov	*6.(r5),-8.(r5)
L8:tstb	*-8.(r5)
jeq	L9
mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
jbr	L11
L12:.data
L13:.byte 0142,0141,0144,040,0143,0150,0141,0162,0141,0143,0164,0145,0145,0162,040,0151,0156,040,0153,0145,0171,012,0
.even
.text
mov	$L13,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L14:L15:L16:mov	-8.(r5),r0
movb	-1.(r0),r0
mov	r0,-12.(r5)
jbr	L8
L17:inc	_iflg
jbr	L8
L18:inc	_wflg
jbr	L8
jbr	L10
L11:sub	$0151,r0
cmp	r0,$017
jhi	L12
asl	r0
jmp	*.+4(r0)
L17
L12
L12
L12
L12
L12
L12
L12
L12
L15
L12
L14
L12
L12
L18
L16
/esw
L10:jbr	L8
L9:jsr	pc,_otape
clr	(sp)
mov	$_buf,-(sp)
jsr	pc,*$_sread
tst	(sp)+
mov	$_buf,r4
mov	(r4)+,_isize
mov	(r4)+,r0
mov	(r4)+,_date
mov	(r4)+,2.+_date
mov	(r4)+,_ddate
mov	(r4)+,2.+_ddate
mov	(r4)+,_tsize
mov	_isize,r0
ash	$05,r0
mov	r0,(sp)
clr	-(sp)
jsr	pc,*$_size
tst	(sp)+
mov	r0,-10.(r5)
mov	-10.(r5),r0
ash	$011,r0
mov	r0,(sp)
jsr	pc,*$_sbrk
mov	r0,_talist
mov	_talist,r4
L19:mov	-10.(r5),r0
dec	-10.(r5)
tst	r0
jeq	L20
clr	(sp)
mov	r4,-(sp)
jsr	pc,*$_tread
tst	(sp)+
add	$01000,r4
jbr	L19
L20:mov	-12.(r5),r0
jbr	L22
L23:clr	-12.(r5)
mov	$_ddate,(sp)
jsr	pc,*$_pdate
mov	$_date,(sp)
jsr	pc,*$_pdate
mov	_talist,r4
clr	-10.(r5)
L24:mov	_isize,r0
ash	$04,r0
cmp	-10.(r5),r0
jge	L25
tst	(r4)+
jne	L27
tst	-12.(r5)
jeq	L26
cmp	-10.(r5),-12.(r5)
jne	L28
.data
L29:.byte 045,0154,012,0
.even
.text
mov	-10.(r5),(sp)
mov	$L29,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L30
L28:.data
L31:.byte 045,0154,055,045,0154,012,0
.even
.text
mov	-10.(r5),(sp)
mov	-12.(r5),-(sp)
mov	$L31,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L30:clr	-12.(r5)
jbr	L32
L27:tst	-12.(r5)
jne	L33
mov	-10.(r5),r0
inc	r0
mov	r0,-12.(r5)
L33:L32:L26:inc	-10.(r5)
jbr	L24
L25:tst	-12.(r5)
jeq	L34
.data
L35:.byte 045,0154,055,012,0
.even
.text
mov	-12.(r5),(sp)
mov	$L35,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L34:jsr	pc,_exit
L36:cmp	4.(r5),$01
jgt	L37
.data
L38:.byte 0156,0157,040,0146,0151,0154,0145,0163,0171,0163,0164,0145,0155,040,0156,0141,0155,0145,012,0
.even
.text
mov	$L38,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L37:mov	6.(r5),r0
mov	2.(r0),_ofile
mov	$02,(sp)
mov	_ofile,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fo
tst	_fo
jge	L39
.data
L40:.byte 0143,0141,0156,040,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	_ofile,(sp)
mov	$L40,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L39:.data
L41:.byte 0154,0141,0163,0164,040,0143,0150,0141,0156,0143,0145,040,0142,0145,0146,0157,0162,0145,040,0163,0143,0162,0151,0142,0142,0154,0151,0156,0147,040,0157,0156,040,045,0163,012,0
.even
.text
mov	_ofile,(sp)
mov	$L41,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_getchar
mov	$_sblock,(sp)
mov	$01,-(sp)
jsr	pc,*$_dread
tst	(sp)+
mov	_talist,r4
clr	-10.(r5)
L42:cmp	-10.(r5),_isize
jge	L43
mov	$_buf,(sp)
mov	-10.(r5),-(sp)
add	$02,(sp)
jsr	pc,*$_dread
tst	(sp)+
mov	$_buf,r2
L45:cmp	r2,$512.+_buf
jhis	L46
mov	(r4)+,-14.(r5)
tst	-14.(r5)
jeq	L47
cmp	-10.(r5),_sblock
jlos	L48
.data
L49:.byte 0151,0163,0151,0172,0145,040,0164,0157,0157,040,0163,0155,0141,0154,0154,012,0
.even
.text
mov	$L49,(sp)
jsr	pc,*$_printf
jbr	L47
L48:clr	(sp)
mov	$_dbuf,-(sp)
jsr	pc,*$_sread
tst	(sp)+
mov	r2,(sp)
jsr	pc,*$_dealoc
mov	$_dbuf,-16.(r5)
mov	r2,r3
L50:mov	r2,r0
add	$040,r0
cmp	r3,r0
jhis	L51
mov	-16.(r5),r0
add	$02,-16.(r5)
mov	(r0),(r3)+
jbr	L50
L51:mov	-14.(r5),(sp)
add	$0177777,(sp)
mov	r2,-(sp)
jsr	pc,*$_restor
tst	(sp)+
L47:add	$040,r2
jbr	L45
L46:mov	$_buf,(sp)
mov	-10.(r5),-(sp)
add	$02,(sp)
jsr	pc,*$_dwrite
tst	(sp)+
L44:inc	-10.(r5)
jbr	L42
L43:mov	$_sblock,(sp)
mov	$01,-(sp)
jsr	pc,*$_dwrite
tst	(sp)+
jsr	pc,_exit
L52:mov	$_ilist,r4
L53:cmp	4.(r5),$01
jle	L54
mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
mov	r0,(r4)+
add	$02,6.(r5)
dec	4.(r5)
jbr	L53
L54:mov	_talist,r4
clr	-10.(r5)
L55:mov	_isize,r0
ash	$04,r0
cmp	-10.(r5),r0
jge	L56
tst	_ilist
jeq	L58
clr	-14.(r5)
L59:mov	-14.(r5),r0
asl	r0
tst	_ilist(r0)
jeq	L60
mov	-14.(r5),r0
asl	r0
mov	-10.(r5),r1
inc	r1
cmp	_ilist(r0),r1
jeq	L62
L61:inc	-14.(r5)
jbr	L59
L60:mov	(r4)+,-14.(r5)
jbr	L63
L58:L62:mov	(r4)+,-14.(r5)
tst	-14.(r5)
jeq	L57
dec	-14.(r5)
clr	(sp)
mov	$_buf,-(sp)
jsr	pc,*$_tread
tst	(sp)+
mov	$_buf,r2
mov	-10.(r5),(sp)
inc	(sp)
jsr	pc,*$_dwait
mov	r0,_fo
tst	_fo
jge	L64
L63:L65:mov	-14.(r5),r0
dec	-14.(r5)
tst	r0
jeq	L66
mov	$01,(sp)
mov	$_dbuf,-(sp)
jsr	pc,*$_tread
tst	(sp)+
jbr	L65
L66:jbr	L57
L64:L67:mov	-14.(r5),r0
dec	-14.(r5)
tst	r0
jeq	L68
clr	(sp)
mov	$_dbuf,-(sp)
jsr	pc,*$_tread
tst	(sp)+
mov	$01000,-12.(r5)
tstb	5.(r2)
jne	L69
cmp	6.(r2),$01000
jhis	L69
mov	6.(r2),-12.(r5)
L69:mov	-12.(r5),(sp)
mov	$_dbuf,-(sp)
mov	_fo,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
cmp	6.(r2),-12.(r5)
jhis	L70
decb	5.(r2)
L70:sub	-12.(r5),6.(r2)
jbr	L67
L68:mov	_fo,(sp)
jsr	pc,*$_close
mov	(r2),(sp)
mov	$_file,-(sp)
jsr	pc,*$_chmod
tst	(sp)+
mov	r2,r0
movb	3.(r0),r0
mov	r0,(sp)
mov	$_file,-(sp)
jsr	pc,*$_chown
tst	(sp)+
L57:inc	-10.(r5)
jbr	L55
L56:jsr	pc,_exit
jbr	L21
L22:sub	$0162,r0
cmp	r0,$06
jhi	L71
asl	r0
jmp	*.+4(r0)
L36
L71
L23
L71
L71
L71
L52
/esw
L21:L71:L3:jmp	rretrn
.globl	_dealoc
.text
_dealoc:
jsr	r5,rsave; 1002
mov	4.(r5),r4
bit	$020000,(r4)
jne	L72
clr	r3
L73:cmp	r3,$010
jge	L74
mov	r3,r0
asl	r0
add	r4,r0
tst	8.(r0)
jeq	L76
bit	$010000,(r4)
jeq	L77
mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	r3,r0
asl	r0
add	r4,r0
mov	8.(r0),-(sp)
jsr	pc,*$_dread
tst	(sp)+
clr	r2
L78:cmp	r2,$0400
jge	L79
mov	r2,r0
asl	r0
add	r5,r0
tst	-518.(r0)
jeq	L81
mov	r2,r0
asl	r0
add	r5,r0
mov	-518.(r0),(sp)
jsr	pc,*$_free
L81:L80:inc	r2
jbr	L78
L79:L77:mov	r3,r0
asl	r0
add	r4,r0
mov	8.(r0),(sp)
jsr	pc,*$_free
L76:L75:inc	r3
jbr	L73
L74:L72:jmp	rretrn
.globl	_restor
.text
_restor:
jsr	r5,rsave; 2
mov	4.(r5),r4
bit	$020000,(r4)
jne	L82
clr	r3
L83:cmp	r3,$010
jge	L84
mov	r3,r0
asl	r0
add	r4,r0
clr	8.(r0)
L85:inc	r3
jbr	L83
L84:cmp	6.(r5),$010
jgt	L86
clr	r3
L87:cmp	r3,6.(r5)
jge	L88
jsr	pc,_rcop
mov	r3,r1
asl	r1
add	r4,r1
mov	r0,8.(r1)
L89:inc	r3
jbr	L87
L88:bic	$!0167777,(r4)
jbr	L82
L86:clr	r2
L90:cmp	6.(r5),$0400
jlt	L91
clr	r3
L92:cmp	r3,$0400
jge	L93
jsr	pc,_rcop
mov	r3,r1
asl	r1
mov	r0,_dbuf(r1)
L94:inc	r3
jbr	L92
L93:jsr	pc,_alloc
mov	r2,r1
asl	r1
add	r4,r1
mov	r0,8.(r1)
mov	$_dbuf,(sp)
mov	r2,r0
inc	r2
asl	r0
add	r4,r0
mov	8.(r0),-(sp)
jsr	pc,*$_dwrite
tst	(sp)+
sub	$0400,6.(r5)
jbr	L90
L91:tst	6.(r5)
jeq	L95
clr	r3
L96:cmp	r3,$0400
jge	L97
mov	r3,r0
asl	r0
clr	_dbuf(r0)
L98:inc	r3
jbr	L96
L97:clr	r3
L99:cmp	r3,6.(r5)
jge	L100
jsr	pc,_rcop
mov	r3,r1
asl	r1
mov	r0,_dbuf(r1)
L101:inc	r3
jbr	L99
L100:jsr	pc,_alloc
mov	r2,r1
asl	r1
add	r4,r1
mov	r0,8.(r1)
mov	$_dbuf,(sp)
mov	r2,r0
asl	r0
add	r4,r0
mov	8.(r0),-(sp)
jsr	pc,*$_dwrite
tst	(sp)+
L95:bis	$010000,(r4)
L82:jmp	rretrn
.globl	_rcop
.text
_rcop:
jsr	r5,rsave; 2
jsr	pc,_alloc
mov	r0,r4
clr	(sp)
mov	$_cbuf,-(sp)
jsr	pc,*$_tread
tst	(sp)+
mov	$_cbuf,(sp)
mov	r4,-(sp)
jsr	pc,*$_dwrite
tst	(sp)+
mov	r4,r0
jbr	L102
L102:jmp	rretrn
.globl	_pdate
.text
_pdate:
jsr	r5,rsave; 2
tst	*4.(r5)
jne	L104
mov	4.(r5),r0
tst	2.(r0)
jne	L104
.data
L105:.byte 0164,0150,0145,040,0145,0160,0157,0143,0150,012,0
.even
.text
mov	$L105,(sp)
jsr	pc,*$_printf
jbr	L106
L104:mov	4.(r5),(sp)
jsr	pc,*$_ctime
mov	r0,(sp)
jsr	pc,*$_printf
L106:L103:jmp	rretrn
.globl	_dread
.text
_dread:
jsr	r5,rsave; 2
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fo,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fo,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L108
.data
L109:.byte 0162,0145,0141,0144,040,0145,0162,0162,0157,0162,040,045,0154,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L109,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L108:L107:jmp	rretrn
.globl	_dwrite
.text
_dwrite:
jsr	r5,rsave; 2
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fo,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fo,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L111
.data
L112:.byte 0167,0162,0151,0164,0145,040,0145,0162,0162,0157,0162,040,045,0154,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L112,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L111:L110:jmp	rretrn
.globl	_sread
.text
_sread:
jsr	r5,rsave; 2
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_tread
tst	(sp)+
tst	6.(r5)
jne	L113
mov	$0400,r4
clr	r3
mov	4.(r5),r2
L114:mov	r4,r0
dec	r4
tst	r0
jeq	L115
add	(r2)+,r3
jbr	L114
L115:cmp	r3,$031415
jeq	L116
.data
L117:.byte 0143,0150,0145,0143,0153,0163,0165,0155,040,0145,0162,0162,0157,0162,012,0
.even
.text
mov	$L117,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L116:L113:jmp	rretrn
.globl	_tread
.text
_tread:
.bss
L119:.=.+2.
.text
jsr	r5,rsave; 2
mov	L119,r0
inc	L119
cmp	_tsize,r0
jhis	L120
.data
L121:.byte 0143,0150,0141,0156,0147,0145,040,0164,0141,0160,0145,0163,012,0
.even
.text
mov	$L121,(sp)
jsr	pc,*$_printf
mov	_fi,(sp)
jsr	pc,*$_close
L122:jsr	pc,_getchar
mov	r0,L119
cmp	$012,r0
jeq	L123
tst	L119
jne	L124
jsr	pc,_exit
L124:jbr	L122
L123:clr	L119
jsr	pc,_otape
L120:tst	6.(r5)
jeq	L125
mov	$04,(sp)
mov	$01,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
jbr	L126
L125:mov	$01000,(sp)
mov	4.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L127
.data
L128:.byte 0162,0145,0141,0144,040,0145,0162,0162,0157,0162,012,0
.even
.text
mov	$L128,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L127:L126:L118:jmp	rretrn
.globl	_number
.text
_number:
jsr	r5,rsave; 2
clr	r4
L130:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r3
jeq	L131
cmp	r3,$060
jlt	L130
cmp	r3,$071
jgt	L130
mov	r4,r1
mul	$012,r1
add	r3,r1
add	$0177720,r1
mov	r1,r4
jbr	L130
L131:mov	r4,r0
jbr	L129
L129:jmp	rretrn
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
jeq	L133
inc	r4
L133:mov	r4,r0
jbr	L132
L132:jmp	rretrn
.globl	_otape
.text
_otape:
jsr	r5,rsave; 2
clr	(sp)
mov	_ifile,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
tst	_fi
jge	L135
.data
L136:.byte 0143,0141,0156,040,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	_ifile,(sp)
mov	$L136,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L135:L134:jmp	rretrn
.globl	_dwait
.text
_dwait:
jsr	r5,rsave; 2
mov	$_file,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_dconv
tst	(sp)+
L138:tst	_wflg
jeq	L139
.data
L140:.byte 045,0163,040,0
.even
.text
mov	$_file,(sp)
mov	$L140,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_getchar
mov	r0,r4
cmp	r4,$0170
jne	L141
jsr	pc,_exit
L141:cmp	r4,$012
jne	L142
mov	$0177777,r0
jbr	L137
L142:cmp	r4,$0171
jne	L143
jsr	pc,_getchar
mov	r0,r4
cmp	r4,$012
jeq	L144
L143:L145:jsr	pc,_getchar
mov	r0,r4
cmp	$012,r0
jeq	L146
tst	r4
jne	L147
jsr	pc,_exit
L147:jbr	L145
L146:jbr	L138
L144:L139:mov	$0666,(sp)
mov	$_file,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,r4
mov	r4,r0
jbr	L137
L137:jmp	rretrn
.globl	_dconv
.text
_dconv:
jsr	r5,rsave; 2
mov	$012,(sp)
mov	4.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
jeq	L149
mov	6.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_dconv
tst	(sp)+
mov	r0,6.(r5)
L149:mov	$012,(sp)
mov	4.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_lrem
cmp	(sp)+,(sp)+
add	$060,r0
mov	6.(r5),r1
inc	6.(r5)
movb	r0,(r1)
clrb	*6.(r5)
mov	6.(r5),r0
jbr	L148
L148:jmp	rretrn
.globl	_alloc
.text
_alloc:
jsr	r5,rsave; 2
dec	4.+_sblock
mov	4.+_sblock,r0
mov	r0,r3
tst	r3
jlt	L10002
cmp	r3,$0144
jlt	L151
L10002:.data
L152:.byte 0142,0141,0144,040,0146,0162,0145,0145,0142,0154,0157,0143,0153,012,0
.even
.text
mov	$L152,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L151:mov	r3,r0
asl	r0
mov	6.+_sblock(r0),r4
tst	r4
jne	L153
.data
L154:.byte 0157,0165,0164,040,0157,0146,040,0146,0162,0145,0145,0154,0151,0163,0164,012,0
.even
.text
mov	$L154,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L153:tst	4.+_sblock
jgt	L155
mov	$_cbuf,(sp)
mov	r4,-(sp)
jsr	pc,*$_dread
tst	(sp)+
mov	_cbuf,4.+_sblock
clr	r3
L156:cmp	r3,$0144
jge	L157
mov	r3,r0
asl	r0
mov	r3,r1
asl	r1
mov	2.+_cbuf(r1),6.+_sblock(r0)
L158:inc	r3
jbr	L156
L157:L155:mov	r4,r0
jbr	L150
L150:jmp	rretrn
.globl	_free
.text
_free:
jsr	r5,rsave; 2
cmp	4.+_sblock,$0144
jlt	L160
mov	4.+_sblock,_cbuf
clr	r4
L161:cmp	r4,$0144
jge	L162
mov	r4,r0
asl	r0
mov	r4,r1
asl	r1
mov	6.+_sblock(r1),2.+_cbuf(r0)
L163:inc	r4
jbr	L161
L162:clr	4.+_sblock
mov	$_cbuf,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_dwrite
tst	(sp)+
L160:mov	4.+_sblock,r0
inc	4.+_sblock
asl	r0
mov	4.(r5),6.+_sblock(r0)
L159:jmp	rretrn
