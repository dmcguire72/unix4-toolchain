.globl	_fcore
.comm	_fcore,02
.globl	_fsym
.comm	_fsym,02
.globl	_symoff
.comm	_symoff,02
.globl	_lp
.comm	_lp,02
.globl	_errflg
.comm	_errflg,02
.globl	_symlen
.comm	_symlen,02
.globl	_symct
.comm	_symct,02
.globl	_symbol
.comm	_symbol,010
.globl	_symflg
.comm	_symflg,02
.globl	_symval
.comm	_symval,02
.globl	_ssymbol
.comm	_ssymbol,010
.globl	_ssymflg
.comm	_ssymflg,02
.globl	_ssymval
.comm	_ssymval,02
.globl	_line
.comm	_line,0200
.globl	_regbuf
.comm	_regbuf,02000
.globl	_uuusp
.comm	_uuusp,02
.globl	_usize
.comm	_usize,02
.globl	_uregs
.comm	_uregs,02
.globl	_textsize
.comm	_textsize,02
.globl	_pc
.data
_pc=.
-2.
.globl	_r5
.data
_r5=.
-9.
.globl	_dot
.comm	_dot,02
.globl	_tdot
.comm	_tdot,02
.globl	_dotinc
.data
_dotinc=.
2.
.globl	_lastcom
.data
_lastcom=.
47.
.globl	_main
.text
_main:
jsr	r5,rsave; 6
cmp	4.(r5),$02
jge	L2
.data
L3:.byte 0143,0157,0162,0145,0
.even
.text
mov	$L3,-8.(r5)
jbr	L4
L2:mov	6.(r5),r0
mov	2.(r0),-8.(r5)
L4:cmp	4.(r5),$03
jge	L5
.data
L6:.byte 0141,056,0157,0165,0164,0
.even
.text
mov	$L6,-10.(r5)
jbr	L7
L5:mov	6.(r5),r0
mov	4.(r0),-10.(r5)
L7:clr	(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fcore
jge	L8
.data
L9:.byte 045,0163,040,0156,0157,0164,040,0146,0157,0165,0156,0144,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L9,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L1
L8:clr	(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fsym
jge	L10
.data
L11:.byte 045,0163,040,0156,0157,0164,040,0146,0157,0165,0156,0144,012,0
.even
.text
mov	-10.(r5),(sp)
mov	$L11,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L1
L10:mov	$020,(sp)
mov	$_regbuf,-(sp)
mov	_fsym,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	_regbuf,$0410
jne	L12
mov	2.+_regbuf,_textsize
jbr	L13
L12:cmp	_regbuf,$0407
jeq	L14
.data
L15:.byte 0102,0141,0144,040,0146,0157,0162,0155,0141,0164,072,040,045,0163,012,0
.even
.text
mov	-10.(r5),(sp)
mov	$L15,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L1
L14:L13:mov	2.+_regbuf,r0
add	4.+_regbuf,r0
mov	r0,_symoff
mov	8.+_regbuf,_symlen
cmp	14.+_regbuf,$01
jeq	L16
asl	_symoff
L16:add	$020,_symoff
mov	$_regbuf,(sp)
mov	_fcore,-(sp)
jsr	pc,*$_fstat
tst	(sp)+
mov	10.+_regbuf,r0
add	$0176000,r0
mov	r0,_usize
mov	$02000,(sp)
mov	$_regbuf,-(sp)
mov	_fcore,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	$1012.+_regbuf,_uuusp
mov	$1024.+_regbuf,_uregs
jsr	pc,_setexit
L17:tst	_errflg
jeq	L18
.data
L19:.byte 077,012,0
.even
.text
mov	$L19,(sp)
jsr	pc,*$_printf
clr	_errflg
L18:mov	$_line,_lp
L20:jsr	pc,_getchar
movb	r0,*_lp
cmp	$012,r0
jeq	L21
mov	_lp,r0
inc	_lp
tstb	(r0)
jeq	L1
jbr	L20
L21:mov	$_line,_lp
jsr	pc,_command
jbr	L17
L1:jmp	rretrn
.globl	_command
.text
_command:
jsr	r5,rsave; 6
jsr	pc,_expr
mov	r0,-8.(r5)
tst	_errflg
jne	L22
jsr	pc,_getcnt
mov	r0,-10.(r5)
cmpb	*_lp,$012
jne	L23
tst	-8.(r5)
jne	L24
add	_dotinc,_dot
L24:jbr	L25
L23:mov	_lp,r0
inc	_lp
movb	(r0),r0
mov	r0,_lastcom
L25:cmpb	*_lp,$012
jeq	L26
inc	_errflg
jbr	L22
L26:tst	-8.(r5)
jeq	L27
mov	_tdot,_dot
L27:L28:tst	-10.(r5)
jeq	L29
jsr	pc,_scommand
dec	-10.(r5)
jeq	L30
add	_dotinc,_dot
L30:jbr	L28
L29:L22:jmp	rretrn
.globl	_scommand
.text
_scommand:
jsr	r5,rsave; 6
mov	_lastcom,r0
jbr	L33
L34:.data
L35:.byte 045,0157,012,0
.even
.text
mov	_dot,(sp)
jsr	pc,*$_cget
mov	r0,(sp)
mov	$L35,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$02,_dotinc
jbr	L31
L36:.data
L37:.byte 045,0157,012,0
.even
.text
mov	_dot,(sp)
mov	$L37,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L31
L38:mov	_dot,(sp)
jsr	pc,*$_cget
mov	r0,(sp)
bic	$!0377,(sp)
jsr	pc,*$_printc
mov	$012,(sp)
jsr	pc,*$_putchar
mov	$01,_dotinc
jbr	L31
L39:mov	_dot,(sp)
jsr	pc,*$_cget
mov	r0,-8.(r5)
L40:mov	-8.(r5),(sp)
inc	-8.(r5)
jsr	pc,*$_cget
bic	$!0377,r0
mov	r0,-10.(r5)
jeq	L41
mov	-10.(r5),(sp)
jsr	pc,*$_printc
jbr	L40
L41:mov	$012,(sp)
jsr	pc,*$_putchar
jbr	L31
L42:mov	_dot,(sp)
jsr	pc,*$_cget
mov	r0,(sp)
jsr	pc,*$_vallook
tst	_errflg
jeq	L43
jsr	pc,_reset
L43:clr	-10.(r5)
L44:cmp	-10.(r5),$010
jge	L45
.data
L47:.byte 045,0143,0
.even
.text
mov	-10.(r5),r0
movb	_ssymbol(r0),r0
mov	r0,(sp)
mov	$L47,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L46:inc	-10.(r5)
jbr	L44
L45:jbr	L31
L48:jsr	pc,_printtra
jbr	L31
jbr	L32
L33:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:42
44
46
47
57
75
L10002:..
L39
L48
L42
L38
L34
L36
L49
.text
/esw
L32:L49:L31:jmp	rretrn
.globl	_getcnt
.text
_getcnt:
jsr	r5,rsave; 6
cmpb	*_lp,$054
jeq	L51
mov	$01,r0
jbr	L50
L51:inc	_lp
mov	_tdot,-8.(r5)
jsr	pc,_expr
tst	r0
jne	L52
mov	-8.(r5),_tdot
mov	$01,r0
jbr	L50
L52:mov	_tdot,-10.(r5)
mov	-8.(r5),_tdot
mov	-10.(r5),r0
jbr	L50
L50:jmp	rretrn
.globl	_cget
.text
_cget:
jsr	r5,rsave; 4
mov	4.(r5),(sp)
jsr	pc,*$_get
mov	r0,-8.(r5)
tst	_errflg
jeq	L54
jsr	pc,_reset
L54:mov	-8.(r5),r0
jbr	L53
L53:jmp	rretrn
.globl	_printc
.text
_printc:
jsr	r5,rsave; 2
cmp	4.(r5),$040
jlt	L10004
cmp	4.(r5),$0175
jle	L56
L10004:.data
L57:.byte 0134,045,0157,0
.even
.text
mov	4.(r5),(sp)
mov	$L57,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L58
L56:.data
L59:.byte 045,0143,0
.even
.text
mov	4.(r5),(sp)
mov	$L59,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L58:L55:jmp	rretrn
.globl	_expr
.text
_expr:
jsr	r5,rsave; 32
clr	_tdot
clr	-26.(r5)
mov	$053,-28.(r5)
clr	_ssymval
clr	-24.(r5)
L61:cmpb	*_lp,$0141
jlt	L10006
cmpb	*_lp,$0172
jle	L10005
L10006:cmpb	*_lp,$0137
jne	L62
L10005:clr	-18.(r5)
mov	-18.(r5),r0
inc	-18.(r5)
add	r5,r0
movb	$0137,-16.(r0)
inc	-26.(r5)
L63:cmpb	*_lp,$0141
jlt	L10008
cmpb	*_lp,$0172
jle	L10007
L10008:cmpb	*_lp,$060
jlt	L10009
cmpb	*_lp,$071
jle	L10007
L10009:cmpb	*_lp,$0137
jne	L64
L10007:cmp	-18.(r5),$010
jge	L65
mov	-18.(r5),r0
inc	-18.(r5)
add	r5,r0
movb	*_lp,-16.(r0)
L65:inc	_lp
jbr	L63
L64:L66:cmp	-18.(r5),$010
jge	L67
mov	-18.(r5),r0
inc	-18.(r5)
add	r5,r0
clrb	-16.(r0)
jbr	L66
L67:mov	r5,r0
add	$0177760,r0
mov	r0,(sp)
jsr	pc,*$_symlook
tst	r0
jne	L68
inc	_errflg
jsr	pc,_reset
L68:jbr	L61
L62:cmpb	*_lp,$060
jlt	L69
cmpb	*_lp,$071
jgt	L69
inc	-26.(r5)
clr	_ssymval
cmpb	*_lp,$060
jne	L70
mov	$010,-30.(r5)
jbr	L71
L70:mov	$012,-30.(r5)
L71:L72:cmpb	*_lp,$060
jlt	L73
cmpb	*_lp,$071
jgt	L73
mov	_ssymval,r1
mul	-30.(r5),r1
mov	r1,_ssymval
mov	_lp,r0
inc	_lp
movb	(r0),r0
add	$0177720,r0
add	r0,_ssymval
jbr	L72
L73:jbr	L61
L69:movb	*_lp,r0
jbr	L75
L76:inc	-24.(r5)
L77:L78:mov	-28.(r5),r0
jbr	L80
L81:add	_ssymval,_tdot
jbr	L82
L83:sub	_ssymval,_tdot
L82:tst	-24.(r5)
jeq	L84
mov	-26.(r5),r0
jbr	L60
L84:mov	_lp,r0
inc	_lp
movb	(r0),r0
mov	r0,-28.(r5)
jbr	L79
L80:sub	$053,r0
cmp	r0,$02
jhi	L85
asl	r0
jmp	*.+4(r0)
L81
L85
L83
/esw
L79:L85:jbr	L61
L86:L87:inc	_lp
jbr	L61
L88:inc	_lp
mov	_ssymval,-20.(r5)
mov	_tdot,-22.(r5)
jsr	pc,_expr
tst	r0
jne	L89
clr	_tdot
L89:mov	_tdot,r0
asl	r0
mov	r0,(sp)
add	-20.(r5),(sp)
jsr	pc,*$_get
mov	r0,_ssymval
tst	_errflg
jeq	L90
jsr	pc,_reset
L90:mov	-22.(r5),_tdot
cmpb	*_lp,$0135
jne	L91
inc	_lp
L91:jbr	L61
jbr	L74
L75:mov	$L10012,r1
mov	r0,L10013
L10014:cmp	r0,(r1)+
jne	L10014
jmp	*L10013-L10012(r1)
.data
L10012:11
40
53
55
133
L10013:..
L87
L86
L77
L78
L88
L76
.text
/esw
L74:L60:jmp	rretrn
.globl	_printtra
.text
_printtra:
jsr	r5,rsave; 14
mov	_pc,r0
asl	r0
add	_uregs,r0
mov	(r0),-8.(r5)
mov	_r5,r0
asl	r0
add	_uregs,r0
mov	(r0),-10.(r5)
.data
L93:.byte 0163,0141,0166,0162,065,0
.even
.text
mov	$L93,(sp)
jsr	pc,*$_symlook
tst	r0
jeq	L94
mov	_ssymval,(sp)
jsr	pc,*$_get
mov	r0,-12.(r5)
jeq	L95
mov	-12.(r5),-10.(r5)
L95:L94:L96:tst	_errflg
jne	L97
mov	-10.(r5),(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_findrout
tst	(sp)+
mov	r0,-12.(r5)
clr	-16.(r5)
L98:cmp	-16.(r5),$010
jge	L99
.data
L101:.byte 045,0143,0
.even
.text
mov	-16.(r5),r0
movb	_ssymbol(r0),r0
mov	r0,(sp)
mov	$L101,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L100:inc	-16.(r5)
jbr	L98
L99:.data
L102:.byte 050,0
.even
.text
mov	$L102,(sp)
jsr	pc,*$_printf
dec	-12.(r5)
jlt	L103
.data
L104:.byte 045,0157,0
.even
.text
mov	-10.(r5),(sp)
add	$04,(sp)
jsr	pc,*$_get
mov	r0,(sp)
mov	$L104,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L103:mov	-10.(r5),r0
add	$04,r0
mov	r0,-14.(r5)
L105:dec	-12.(r5)
jlt	L106
.data
L107:.byte 054,045,0157,0
.even
.text
add	$02,-14.(r5)
mov	-14.(r5),r0
mov	r0,(sp)
jsr	pc,*$_get
mov	r0,(sp)
mov	$L107,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L105
L106:.data
L108:.byte 051,012,0
.even
.text
mov	$L108,(sp)
jsr	pc,*$_printf
mov	-10.(r5),(sp)
add	$02,(sp)
jsr	pc,*$_get
mov	r0,-8.(r5)
mov	-10.(r5),(sp)
jsr	pc,*$_get
mov	r0,-10.(r5)
jeq	L97
jbr	L96
L97:L92:jmp	rretrn
.globl	_findrout
.text
_findrout:
jsr	r5,rsave; 10
mov	6.(r5),(sp)
add	$02,(sp)
jsr	pc,*$_get
mov	r0,-8.(r5)
mov	-8.(r5),(sp)
add	$0177774,(sp)
jsr	pc,*$_get
mov	r0,-10.(r5)
cmp	$04737,r0
jne	L110
mov	$01,-12.(r5)
jbr	L111
L110:mov	-10.(r5),r0
bic	$!0177700,r0
cmp	$04700,r0
jne	L112
clr	-12.(r5)
jbr	L113
L112:inc	_errflg
clr	r0
jbr	L109
L113:L111:mov	-8.(r5),(sp)
add	$0177776,(sp)
jsr	pc,*$_get
mov	r0,(sp)
cmp	-10.(r5),$04767
jne	L10015
mov	-8.(r5),r0
jbr	L10016
L10015:clr	r0
L10016:add	r0,(sp)
jsr	pc,*$_vallook
mov	-8.(r5),(sp)
jsr	pc,*$_get
mov	r0,-10.(r5)
cmp	-10.(r5),$05726
jne	L114
mov	-12.(r5),r0
inc	r0
jbr	L109
L114:cmp	-10.(r5),$022626
jne	L115
mov	-12.(r5),r0
add	$02,r0
jbr	L109
L115:cmp	-10.(r5),$062706
jne	L116
mov	-8.(r5),(sp)
add	$02,(sp)
jsr	pc,*$_get
asr	r0
add	-12.(r5),r0
jbr	L109
L116:mov	-12.(r5),r0
jbr	L109
L109:jmp	rretrn
.globl	_symlook
.text
_symlook:
jsr	r5,rsave; 2
jsr	pc,_symset
L118:jsr	pc,_symget
tst	r0
jeq	L119
mov	4.(r5),(sp)
mov	$_symbol,-(sp)
jsr	pc,*$_eqstr
tst	(sp)+
tst	r0
jeq	L120
jsr	pc,_savsym
mov	$01,r0
jbr	L117
L120:jbr	L118
L119:clr	r0
jbr	L117
L117:jmp	rretrn
.globl	_eqstr
.text
_eqstr:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	6.(r5),r3
mov	r4,r0
add	$010,r0
mov	r0,r2
L122:cmp	r4,r2
jhis	L123
cmp	(r4)+,(r3)+
jeq	L124
clr	r0
jbr	L121
L124:jbr	L122
L123:mov	$01,r0
jbr	L121
L121:jmp	rretrn
.globl	_vallook
.text
_vallook:
jsr	r5,rsave; 2
jsr	pc,_symset
L126:jsr	pc,_symget
tst	r0
jeq	L127
cmp	_symval,4.(r5)
jne	L128
mov	_symflg,r0
bic	$!037,r0
cmp	$02,r0
jne	L128
mov	$0137,(sp)
jsr	pc,*$_savsym
jbr	L125
L128:jbr	L126
L127:inc	_errflg
L125:jmp	rretrn
.globl	_get
.text
_get:
jsr	r5,rsave; 4
clr	-8.(r5)
cmp	4.(r5),_textsize
jhis	L130
clr	(sp)
mov	4.(r5),-(sp)
add	$020,(sp)
mov	_fsym,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$02,(sp)
mov	r5,r0
add	$0177770,r0
mov	r0,-(sp)
mov	_fsym,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$02,r0
jeq	L131
inc	_errflg
L131:mov	-8.(r5),r0
jbr	L129
L130:cmp	*_uuusp,4.(r5)
jhi	L132
add	_usize,4.(r5)
jbr	L133
L132:mov	_textsize,r0
add	$017777,r0
bic	$!0160000,r0
sub	r0,4.(r5)
L133:clr	(sp)
mov	4.(r5),-(sp)
add	$02000,(sp)
mov	_fcore,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$02,(sp)
mov	r5,r0
add	$0177770,r0
mov	r0,-(sp)
mov	_fcore,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$02,r0
jle	L134
inc	_errflg
L134:mov	-8.(r5),r0
jbr	L129
L129:jmp	rretrn
.globl	_symset
.text
_symset:
jsr	r5,rsave; 2
mov	_symlen,_symct
clr	(sp)
mov	_symoff,-(sp)
mov	_fsym,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L135:jmp	rretrn
.globl	_symget
.text
_symget:
jsr	r5,rsave; 2
sub	$014,_symct
jge	L137
clr	r0
jbr	L136
L137:mov	$014,(sp)
mov	$_symbol,-(sp)
mov	_fsym,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$014,r0
jeq	L10017
clr	r0
jbr	L10018
L10017:mov	$01,r0
L10018:jbr	L136
L136:jmp	rretrn
.globl	_savsym
.text
_savsym:
jsr	r5,rsave; 10
mov	$_symbol,-10.(r5)
mov	$_ssymbol,-12.(r5)
L139:cmp	-10.(r5),$8.+_symbol
jhis	L140
mov	-10.(r5),r0
inc	-10.(r5)
movb	(r0),r0
mov	r0,-8.(r5)
jeq	L140
cmp	-8.(r5),4.(r5)
jeq	L139
mov	-12.(r5),r0
inc	-12.(r5)
movb	-8.(r5),(r0)
jbr	L139
L140:L141:cmp	-12.(r5),$8.+_ssymbol
jhis	L142
mov	-12.(r5),r0
inc	-12.(r5)
clrb	(r0)
jbr	L141
L142:mov	_symflg,_ssymflg
mov	_symval,_ssymval
L138:jmp	rretrn
