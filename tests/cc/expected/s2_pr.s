.globl	_ncol
.data
_ncol=.
1.
.globl	_header
.comm	_header,02
.globl	_col
.comm	_col,02
.globl	_icol
.comm	_icol,02
.globl	_file
.comm	_file,02
.globl	_bufp
.comm	_bufp,02
.globl	_bufs
.data
_bufs=.
5120.
.globl	_buffer
.comm	_buffer,012000
.globl	_line
.comm	_line,02
.globl	_colp
.comm	_colp,0220
.globl	_peekc
.comm	_peekc,02
.globl	_fpage
.comm	_fpage,02
.globl	_page
.comm	_page,02
.globl	_colw
.comm	_colw,02
.globl	_nspace
.comm	_nspace,02
.globl	_width
.data
_width=.
72.
.globl	_length
.data
_length=.
66.
.globl	_ntflg
.comm	_ntflg,02
.globl	_tty
.comm	_tty,02
.globl	_mode
.comm	_mode,02
.globl	_main
.text
_main:
jsr	r5,rsave; 4
.data
L2:.byte 057,0144,0145,0166,057,0164,0164,0171,0170,0
.even
.text
mov	$L2,_tty
mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
mov	$01,(sp)
jsr	pc,*$_close
mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
bit	$01,r0
jne	L3
mov	$_onintr,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L3:jsr	pc,_fixtty
clr	-8.(r5)
L4:cmp	4.(r5),$01
jle	L5
add	$02,6.(r5)
mov	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L7
inc	*6.(r5)
mov	*6.(r5),r0
movb	(r0),r0
jbr	L9
L10:cmp	4.(r5),$02
jlt	L11
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),_header
dec	4.(r5)
L11:jbr	L6
L12:inc	_ntflg
jbr	L6
L13:inc	*6.(r5)
mov	*6.(r5),(sp)
jsr	pc,*$_getn
mov	r0,_length
jbr	L6
L14:inc	*6.(r5)
mov	*6.(r5),(sp)
jsr	pc,*$_getn
mov	r0,_width
jbr	L6
L15:mov	*6.(r5),(sp)
jsr	pc,*$_getn
mov	r0,_ncol
jbr	L6
jbr	L8
L9:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:150
154
164
167
L10002:..
L10
L13
L12
L14
L15
.text
/esw
L8:jbr	L16
L7:mov	6.(r5),r0
movb	*(r0),r0
cmp	$053,r0
jne	L17
inc	*6.(r5)
mov	*6.(r5),(sp)
jsr	pc,*$_getn
mov	r0,_fpage
jbr	L18
L17:mov	*6.(r5),(sp)
jsr	pc,*$_print
inc	-8.(r5)
L18:L16:L6:dec	4.(r5)
jbr	L4
L5:tst	-8.(r5)
jne	L19
clr	(sp)
jsr	pc,*$_print
L19:jsr	pc,_flush
jsr	pc,_onintr
L1:jmp	rretrn
.globl	_onintr
.text
_onintr:
jsr	r5,rsave; 2
mov	_mode,(sp)
mov	_tty,-(sp)
jsr	pc,*$_chmod
tst	(sp)+
clr	(sp)
jsr	pc,*$_exit
L20:jmp	rretrn
.globl	_fixtty
.text
_fixtty:
jsr	r5,rsave; 46
mov	_fout,(sp)
jsr	pc,*$_ttyn
mov	_tty,r1
movb	r0,8.(r1)
mov	r5,r0
add	$0177726,r0
mov	r0,(sp)
mov	_fout,-(sp)
jsr	pc,*$_fstat
tst	(sp)+
mov	4.+-42.(r5),r0
bic	$!0777,r0
mov	r0,_mode
mov	$0600,(sp)
mov	_tty,-(sp)
jsr	pc,*$_chmod
tst	(sp)+
L21:jmp	rretrn
.globl	_print
.text
_print:
jsr	r5,rsave; 46
cmp	_length,$012
jgt	L23
mov	$0102,_length
L23:tst	_width
jgt	L24
mov	$0110,_width
L24:cmp	_ncol,$0110
jgt	L10004
cmp	_ncol,_width
jle	L25
L10004:.data
L26:.byte 0126,0145,0162,0171,040,0146,0165,0156,0156,0171,056,012,0
.even
.text
mov	$014,(sp)
mov	$L26,-(sp)
mov	_fout,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L25:mov	_width,r1
sxt	r0
div	_ncol,r0
mov	r0,_colw
mov	_ncol,r4
mov	_header,r3
dec	_ncol
jge	L27
clr	_ncol
L27:tst	4.(r5)
jeq	L28
clr	(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_file
tst	_file
jlt	L22
mov	r5,r0
add	$0177726,r0
mov	r0,(sp)
mov	_file,-(sp)
jsr	pc,*$_fstat
tst	(sp)+
jbr	L29
L28:clr	_file
mov	r5,r0
add	$0177766,r0
mov	r0,(sp)
jsr	pc,*$_time
L29:tst	_header
jne	L30
mov	4.(r5),_header
L30:mov	r5,r0
add	$0177766,r0
mov	r0,(sp)
jsr	pc,*$_ctime
mov	r0,r2
clrb	16.(r2)
mov	$01,_page
clr	_icol
mov	_ncol,r0
asl	r0
mov	$_buffer,r1
mov	r1,_bufp
mov	r1,_colp(r0)
jsr	pc,_nexbuf
L31:mov	_ncol,(sp)
jsr	pc,*$_tgetc
tst	r0
jeq	L32
mov	_ncol,r0
asl	r0
dec	_colp(r0)
mov	_ncol,r0
asl	r0
cmp	_colp(r0),$_buffer
jhis	L33
mov	_ncol,r0
asl	r0
mov	_bufs,r1
add	$_buffer,r1
mov	r1,_colp(r0)
L33:clr	_line
tst	_ntflg
jne	L34
.data
L35:.byte 012,012,0
.even
.text
mov	$L35,(sp)
jsr	pc,*$_puts
mov	r2,(sp)
add	$04,(sp)
jsr	pc,*$_puts
.data
L36:.byte 040,040,0
.even
.text
mov	$L36,(sp)
jsr	pc,*$_puts
mov	_header,(sp)
jsr	pc,*$_puts
.data
L37:.byte 040,0120,0141,0147,0145,040,0
.even
.text
mov	$L37,(sp)
jsr	pc,*$_puts
mov	_page,(sp)
jsr	pc,*$_putd
.data
L38:.byte 012,012,012,0
.even
.text
mov	$L38,(sp)
jsr	pc,*$_puts
L34:jsr	pc,_putpage
tst	_ntflg
jne	L39
L40:cmp	_line,_length
jge	L41
mov	$012,(sp)
jsr	pc,*$_put
jbr	L40
L41:L39:inc	_page
jbr	L31
L32:mov	_file,(sp)
jsr	pc,*$_close
mov	r4,_ncol
mov	r3,_header
L22:jmp	rretrn
.globl	_putpage
.text
_putpage:
jsr	r5,rsave; 4
tst	_ncol
jne	L43
L44:mov	_length,r0
add	$0177773,r0
cmp	_line,r0
jge	L45
L46:clr	(sp)
jsr	pc,*$_tgetc
mov	r0,r2
jeq	L47
cmp	r2,$012
jeq	L47
mov	r2,(sp)
jsr	pc,*$_putcp
jbr	L46
L47:mov	$012,(sp)
jsr	pc,*$_putcp
inc	_line
jbr	L44
L45:jbr	L42
L43:mov	_ncol,r0
asl	r0
mov	_colp(r0),_colp
mov	$01,r3
L48:cmp	r3,_ncol
jgt	L49
mov	r3,r0
asl	r0
mov	r3,r1
asl	r1
mov	-2.+_colp(r1),_colp(r0)
mov	$012,-8.(r5)
L51:cmp	-8.(r5),_length
jge	L52
L54:mov	r3,(sp)
jsr	pc,*$_tgetc
mov	r0,r2
cmp	$012,r0
jeq	L55
tst	r2
jeq	L55
jbr	L54
L55:L53:inc	-8.(r5)
jbr	L51
L52:L50:inc	r3
jbr	L48
L49:L56:mov	_length,r0
add	$0177773,r0
cmp	_line,r0
jge	L57
mov	_colw,r4
clr	r3
L58:cmp	r3,_ncol
jge	L59
L61:mov	r3,(sp)
jsr	pc,*$_getc
mov	r0,r2
jeq	L62
cmp	r2,$012
jeq	L62
cmp	_col,r4
jge	L63
mov	r2,(sp)
jsr	pc,*$_put
L63:jbr	L61
L62:L64:cmp	_col,r4
jge	L65
mov	$040,(sp)
jsr	pc,*$_put
jbr	L64
L65:add	_colw,r4
L60:inc	r3
jbr	L58
L59:L66:mov	_ncol,(sp)
jsr	pc,*$_getc
mov	r0,r2
jeq	L67
cmp	r2,$012
jeq	L67
mov	r2,(sp)
jsr	pc,*$_put
jbr	L66
L67:mov	$012,(sp)
jsr	pc,*$_put
jbr	L56
L57:L68:mov	_length,r0
add	$0177773,r0
cmp	_line,r0
jge	L69
mov	$012,(sp)
jsr	pc,*$_put
jbr	L68
L69:L42:jmp	rretrn
.globl	_nexbuf
.text
_nexbuf:
jsr	r5,rsave; 2
mov	_bufp,r3
mov	_bufs,r0
add	$_buffer,r0
sub	r3,r0
mov	r0,r4
cmp	r4,$01000
jle	L71
mov	$01000,r4
L71:mov	r4,(sp)
mov	r3,-(sp)
mov	_file,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,r4
jgt	L72
movb	$0376,(r3)
jbr	L73
L72:add	r4,r3
mov	_bufs,r0
add	$_buffer,r0
cmp	r3,r0
jlo	L74
mov	$_buffer,r3
L74:movb	$0375,(r3)
L73:mov	r3,_bufp
L70:jmp	rretrn
.globl	_tgetc
.text
_tgetc:
jsr	r5,rsave; 2
mov	4.(r5),r2
L76:mov	r2,r0
asl	r0
add	$_colp,r0
mov	r0,r4
movb	*(r0),r0
bic	$!0377,r0
mov	r0,r3
cmp	r3,$0375
jne	L77
jsr	pc,_nexbuf
movb	*(r4),r0
bic	$!0377,r0
mov	r0,r3
L77:cmp	r3,$0376
jne	L78
clr	r0
jbr	L75
L78:inc	(r4)
mov	r4,r0
mov	_bufs,r1
add	$_buffer,r1
cmp	(r0),r1
jlo	L79
mov	$_buffer,(r4)
L79:tst	r3
jeq	L76
mov	r3,r0
jbr	L75
L75:jmp	rretrn
.globl	_getc
.text
_getc:
jsr	r5,rsave; 2
tst	_peekc
jeq	L81
mov	_peekc,r4
clr	_peekc
jbr	L82
L81:mov	4.(r5),(sp)
jsr	pc,*$_tgetc
mov	r0,r4
L82:mov	r4,r0
jbr	L84
L85:inc	_icol
bit	$07,_icol
jeq	L86
mov	$011,_peekc
L86:mov	$040,r0
jbr	L80
L87:clr	_icol
jbr	L83
L88:L89:dec	_icol
jbr	L83
jbr	L83
L84:mov	$L10006,r1
mov	r0,L10007
L10008:cmp	r0,(r1)+
jne	L10008
jmp	*L10007-L10006(r1)
.data
L10006:10
11
12
33
L10007:..
L88
L85
L87
L89
L90
.text
/esw
L83:L90:cmp	r4,$040
jlt	L91
inc	_icol
L91:mov	r4,r0
jbr	L80
L80:jmp	rretrn
.globl	_puts
.text
_puts:
jsr	r5,rsave; 2
mov	4.(r5),r3
jeq	L92
L93:movb	(r3)+,r4
jeq	L94
mov	r4,(sp)
jsr	pc,*$_put
jbr	L93
L94:L92:jmp	rretrn
.globl	_putd
.text
_putd:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	r3,r1
sxt	r0
div	$012,r0
mov	r0,r4
jeq	L96
mov	r4,(sp)
jsr	pc,*$_putd
L96:mov	r3,r1
sxt	r0
div	$012,r0
mov	r1,(sp)
add	$060,(sp)
jsr	pc,*$_put
L95:jmp	rretrn
.globl	_put
.text
_put:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	r3,r0
jbr	L99
L100:inc	_nspace
inc	_col
jbr	L97
L101:clr	_col
clr	_nspace
inc	_line
jbr	L98
L102:L103:dec	_col
jge	L104
clr	_col
L104:dec	_nspace
jge	L105
clr	_nspace
L105:jbr	L98
L99:mov	$L10010,r1
mov	r0,L10011
L10012:cmp	r0,(r1)+
jne	L10012
jmp	*L10011-L10010(r1)
.data
L10010:10
12
33
40
L10011:..
L102
L101
L103
L100
L106
.text
/esw
L98:L106:L107:tst	_nspace
jeq	L108
cmp	_nspace,$02
jle	L109
mov	_col,r0
sub	_nspace,r0
bis	$07,r0
mov	r0,r4
cmp	_col,r0
jle	L109
mov	_col,r0
sub	r4,r0
add	$0177777,r0
mov	r0,_nspace
mov	$011,(sp)
jsr	pc,*$_putcp
jbr	L110
L109:dec	_nspace
mov	$040,(sp)
jsr	pc,*$_putcp
L110:jbr	L107
L108:cmp	r3,$040
jlt	L111
inc	_col
L111:mov	r3,(sp)
jsr	pc,*$_putcp
L97:jmp	rretrn
.globl	_getn
.text
_getn:
jsr	r5,rsave; 2
mov	4.(r5),r2
clr	r4
L113:movb	(r2)+,r0
mov	r0,r3
cmp	$060,r0
jgt	L114
cmp	r3,$071
jgt	L114
mov	r4,r1
mul	$012,r1
add	r3,r1
add	$0177720,r1
mov	r1,r4
jbr	L113
L114:mov	r4,r0
jbr	L112
L112:jmp	rretrn
.globl	_putcp
.text
_putcp:
jsr	r5,rsave; 2
cmp	_page,_fpage
jlt	L116
mov	4.(r5),(sp)
jsr	pc,*$_putchar
L116:L115:jmp	rretrn
