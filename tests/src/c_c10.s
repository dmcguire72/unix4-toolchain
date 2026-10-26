.globl	_maprel
.comm	_maprel,02
.globl	_notrel
.comm	_notrel,02
.globl	_nreg
.comm	_nreg,02
.globl	_isn
.comm	_isn,02
.globl	_namsiz
.comm	_namsiz,02
.globl	_line
.comm	_line,02
.globl	_binbuf
.comm	_binbuf,01006
.globl	_ascbuf
.comm	_ascbuf,01006
.globl	_outbuf
.comm	_outbuf,01006
.globl	_nerror
.comm	_nerror,02
.globl	_cctab
.comm	_cctab,04
.globl	_efftab
.comm	_efftab,04
.globl	_regtab
.comm	_regtab,04
.globl	_sptab
.comm	_sptab,04
.globl	_lsptab
.comm	_lsptab,04
.globl	_instab
.comm	_instab,06
.globl	_opdope
.comm	_opdope,02
.globl	_nstack
.comm	_nstack,02
.globl	_nfloat
.comm	_nfloat,02
.globl	_czero
.comm	_czero,010
.globl	_cone
.comm	_cone,010
.globl	_fczero
.comm	_fczero,010
.globl	_maprel
.data
_maprel=.
.byte 60.
.byte 61.
.byte 64.
.byte 65.
.byte 62.
.byte 63.
.byte 68.
.byte 69.
.byte 66.
.byte 67.
.globl	_notrel
.data
_notrel=.
.byte 61.
.byte 60.
.byte 65.
.byte 64.
.byte 63.
.byte 62.
.byte 69.
.byte 68.
.byte 67.
.byte 66.
.globl	_czero
.data
_czero=.
21.
0.
0.
0.
.globl	_cone
.data
_cone=.
21.
0.
0.
1.
.globl	_fczero
.data
_fczero=.
24.
3.
0.
0.
.globl	_tabtab
.data
_tabtab=.
_regtab
_efftab
_cctab
_sptab
0.
.globl	_nreg
.data
_nreg=.
3.
.globl	_isn
.data
_isn=.
10000.
.globl	_namsiz
.data
_namsiz=.
8.
.globl	_treebase
.comm	_treebase,02
.globl	_main
.text
_main:
jsr	r5,rsave; 1754
cmp	4.(r5),$04
jge	L2
.data
L3:.byte 0101,0162,0147,040,0143,0157,0165,0156,0164,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L2:mov	$_ascbuf,(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jlt	L10000
mov	$_binbuf,(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L4
L10000:.data
L5:.byte 0115,0151,0163,0163,0151,0156,0147,040,0164,0145,0155,0160,040,0146,0151,0154,0145,0
.even
.text
mov	$L5,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L4:mov	$_outbuf,(sp)
mov	6.(r5),r0
mov	6.(r0),-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jge	L6
.data
L7:.byte 0103,0141,0156,047,0164,040,0143,0162,0145,0141,0164,0145,040,045,0163,0
.even
.text
mov	6.(r5),r0
mov	6.(r0),(sp)
mov	$L7,-(sp)
jsr	pc,*$_error
tst	(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L6:mov	$_binbuf,(sp)
jsr	pc,*$_getw
mov	r0,_treebase
mov	_treebase,r0
mov	r5,r1
add	$0176022,r1
cmp	r0,r1
jhis	L8
.data
L9:.byte 0124,0162,0145,0145,040,0163,0160,0141,0143,0145,040,0142,0157,0164,0143,0150,0
.even
.text
mov	$L9,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L8:L10:mov	$_ascbuf,(sp)
jsr	pc,*$_getc
mov	r0,r3
jle	L11
cmp	r3,$043
jne	L12
mov	_treebase,r4
mov	$_binbuf,(sp)
jsr	pc,*$_getw
mov	r0,r3
mov	$_binbuf,(sp)
jsr	pc,*$_getw
mov	r0,r2
mov	$_binbuf,(sp)
jsr	pc,*$_getw
asl	r0
mov	_tabtab(r0),-1008.(r5)
mov	$_binbuf,(sp)
jsr	pc,*$_getw
mov	r0,_line
L13:dec	r3
jlt	L14
mov	$_binbuf,(sp)
jsr	pc,*$_getw
mov	r0,(r4)+
jbr	L13
L14:tst	-1008.(r5)
jne	L15
mov	r2,(sp)
mov	r4,-(sp)
mov	_treebase,-(sp)
jsr	pc,*$_pswitch
cmp	(sp)+,(sp)+
jbr	L16
L15:mov	r2,(sp)
jsr	pc,*$_optim
mov	r0,r2
clr	_nstack
clr	(sp)
mov	-1008.(r5),-(sp)
mov	r2,-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
L16:jbr	L17
L12:mov	r3,(sp)
jsr	pc,*$_putchar
L17:jbr	L10
L11:tst	_nfloat
jeq	L18
.data
L19:.byte 056,0147,0154,0157,0142,0154,011,0146,0154,0164,0165,0163,0145,0144,012,0
.even
.text
mov	$L19,(sp)
jsr	pc,*$_printf
L18:mov	$_outbuf,(sp)
jsr	pc,*$_fflush
tst	_nerror
jne	L10001
clr	(sp)
jbr	L10002
L10001:mov	$01,(sp)
L10002:jsr	pc,*$_exit
L1:jmp	rretrn
.globl	_match
.text
_match:
jsr	r5,rsave; 20
mov	4.(r5),r3
jne	L21
clr	r0
jbr	L20
L21:cmp	6.(r5),$_lsptab
jne	L22
mov	$_sptab,6.(r5)
L22:mov	(r3),-8.(r5)
mov	-8.(r5),r0
asl	r0
mov	_opdope(r0),-18.(r5)
bit	$0400,-18.(r5)
jne	L23
mov	6.(r3),r4
jbr	L24
L23:mov	r3,r4
L24:mov	2.(r4),-14.(r5)
mov	8.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_dcalc
tst	(sp)+
mov	r0,-10.(r5)
bit	$01,-18.(r5)
jeq	L25
mov	8.(r3),-20.(r5)
mov	-20.(r5),r0
mov	2.(r0),-16.(r5)
mov	8.(r5),(sp)
mov	-20.(r5),-(sp)
jsr	pc,*$_dcalc
tst	(sp)+
mov	r0,-12.(r5)
L25:L26:cmp	*6.(r5),-8.(r5)
jeq	L27
tst	*6.(r5)
jne	L29
clr	r0
jbr	L20
L29:L28:add	$04,6.(r5)
jbr	L26
L27:mov	6.(r5),r0
mov	2.(r0),r2
L30:tstb	(r2)
jeq	L31
movb	(r2),r0
bic	$!077,r0
cmp	-10.(r5),r0
jgt	L32
cmpb	(r2),$0100
jlt	L10003
cmp	(r4),$044
jne	L32
L10003:mov	r2,r0
movb	1.(r0),r0
mov	r0,(sp)
mov	r4,-(sp)
jsr	pc,*$_notcompa
tst	(sp)+
tst	r0
jeq	L33
jbr	L32
L33:mov	-8.(r5),r0
asl	r0
bit	$01,_opdope(r0)
jeq	L34
tst	-20.(r5)
jeq	L34
movb	2.(r2),r0
bic	$!077,r0
cmp	-12.(r5),r0
jgt	L32
cmpb	2.(r2),$0100
jlt	L10004
cmp	*-20.(r5),$044
jne	L32
L10004:mov	r2,r0
movb	3.(r0),r0
mov	r0,(sp)
mov	-20.(r5),-(sp)
jsr	pc,*$_notcompa
tst	(sp)+
tst	r0
jne	L32
L34:mov	r2,r0
jbr	L20
L32:add	$06,r2
jbr	L30
L31:clr	r0
jbr	L20
L20:jmp	rretrn
.globl	_rcexpr
.text
_rcexpr:
jsr	r5,rsave; 4
mov	6.(r5),r2
mov	4.(r5),r3
jne	L36
clr	r0
jbr	L35
L36:mov	(r3),r0
jbr	L38
L39:mov	2.(r3),r0
add	$0177777,r0
mov	r0,_nreg
jbr	L35
L40:clr	(sp)
mov	6.(r3),-(sp)
mov	4.(r3),-(sp)
mov	2.(r3),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
clr	r0
jbr	L35
L41:cmp	*6.(r3),$043
jne	L42
mov	6.(r3),r0
mov	6.(r0),6.(r3)
L42:cmp	*6.(r3),$024
jeq	L43
cmp	*6.(r3),$025
jeq	L43
.data
L44:.byte 0111,0154,0154,0145,0147,0141,0154,040,0151,0156,0151,0164,0151,0141,0154,0151,0172,0141,0164,0151,0157,0156,0
.even
.text
mov	$L44,(sp)
jsr	pc,*$_error
jbr	L45
L43:mov	_nreg,(sp)
mov	$_regtab,-(sp)
mov	r3,-(sp)
jsr	pc,*$_cexpr
cmp	(sp)+,(sp)+
L45:clr	r0
jbr	L35
L46:mov	*6.(r3),r0
asl	r0
bit	$04,_opdope(r0)
jeq	L47
mov	6.(r3),r3
mov	(r3),r0
movb	-60.+_notrel(r0),r0
mov	r0,(r3)
L47:jbr	L37
L48:mov	8.(r5),(sp)
mov	r2,-(sp)
mov	6.(r3),-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	r0,r4
jeq	L49
.data
L50:.byte 0155,0157,0166,045,0143,011,0162,045,0144,054,0162,060,012,0
.even
.text
mov	r4,(sp)
mov	6.(r3),-(sp)
jsr	pc,_isfloat
tst	(sp)+
mov	r0,-(sp)
mov	$L50,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L49:clr	r0
jbr	L35
L51:mov	8.(r5),(sp)
mov	$_efftab,-(sp)
mov	6.(r3),-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	8.(r3),r3
jbr	L37
L52:L53:L54:L55:mov	r3,(sp)
jsr	pc,*$_pow2
jbr	L37
L38:jsr	r2,hsw; 2; L10006
jmp	*L10009-L10007(r1)
.data
L10006:L10007
L10008
L10009
L10007:..
21
25
44
64
67
L10008:..
4
25
44
63
64
L10009:L56
L46
L52
L54
L41
L48
L56
L51
L53
L55
L40
L39
.text
/esw
L37:L56:mov	8.(r5),(sp)
mov	r2,-(sp)
mov	r3,-(sp)
jsr	pc,*$_cexpr
cmp	(sp)+,(sp)+
mov	r0,r4
jlt	L57
mov	r4,r0
jbr	L35
L57:cmp	r2,$_regtab
jeq	L58
mov	8.(r5),(sp)
mov	$_regtab,-(sp)
mov	r3,-(sp)
jsr	pc,*$_cexpr
cmp	(sp)+,(sp)+
mov	r0,r4
jlt	L59
mov	r3,(sp)
jsr	pc,*$_isfloat
mov	r0,-8.(r5)
cmp	r2,$_sptab
jeq	L10010
cmp	r2,$_lsptab
jne	L60
L10010:.data
L61:.byte 0155,0157,0166,045,0143,011,0162,045,0144,054,045,0143,050,0163,0160,051,012,0
.even
.text
cmp	r2,$_sptab
jne	L10011
mov	$055,(sp)
jbr	L10012
L10011:clr	(sp)
L10012:mov	r4,-(sp)
mov	-8.(r5),-(sp)
mov	$L61,-(sp)
jsr	pc,*$_printf
add	$06,sp
inc	_nstack
L60:cmp	r2,$_cctab
jne	L62
.data
L63:.byte 0164,0163,0164,045,0143,011,0162,045,0144,012,0
.even
.text
mov	r4,(sp)
mov	-8.(r5),-(sp)
mov	$L63,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L62:clr	r0
jbr	L35
L59:L58:.data
L64:.byte 0116,0157,040,0155,0141,0164,0143,0150,040,0146,0157,0162,040,0157,0160,040,045,0144,0
.even
.text
mov	(r3),(sp)
mov	$L64,-(sp)
jsr	pc,*$_error
tst	(sp)+
L35:jmp	rretrn
.globl	_cexpr
.text
_cexpr:
jsr	r5,rsave; 32
mov	4.(r5),r2
mov	8.(r5),-18.(r5)
mov	8.(r2),r3
mov	(r2),r0
mov	r0,-8.(r5)
cmp	$0144,r0
jne	L66
clr	-10.(r5)
clr	-26.(r5)
clr	-28.(r5)
cmp	*6.(r2),$024
jeq	L67
inc	-26.(r5)
inc	_nstack
L67:tst	(r3)
jeq	L68
L69:cmp	(r3),$011
jne	L70
mov	r5,r0
add	$0177744,r0
mov	r0,(sp)
mov	8.(r3),-(sp)
jsr	pc,*$_comarg
tst	(sp)+
add	r0,-10.(r5)
mov	6.(r3),r3
inc	-26.(r5)
jbr	L69
L70:mov	r5,r0
add	$0177744,r0
mov	r0,(sp)
mov	r3,-(sp)
jsr	pc,*$_comarg
tst	(sp)+
add	r0,-10.(r5)
inc	-26.(r5)
L68:mov	$0145,(r2)
mov	-10.(r5),4.(r2)
L66:mov	-8.(r5),r0
asl	r0
bit	$04,_opdope(r0)
jne	L10013
cmp	-8.(r5),$065
jeq	L10013
cmp	-8.(r5),$066
jne	L71
L10013:cmp	6.(r5),$_cctab
jeq	L71
mov	-18.(r5),(sp)
mov	$01,-(sp)
mov	_isn,r0
inc	_isn
mov	r0,-8.(r5)
mov	r0,-(sp)
mov	r2,-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	-18.(r5),(sp)
mov	6.(r5),-(sp)
mov	$_czero,-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
clr	(sp)
mov	_isn,-(sp)
jsr	pc,*$_branch
tst	(sp)+
mov	-8.(r5),(sp)
jsr	pc,*$_label
mov	-18.(r5),(sp)
mov	6.(r5),-(sp)
mov	$_cone,-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	_isn,(sp)
inc	_isn
jsr	pc,*$_label
mov	-18.(r5),r0
jbr	L65
L71:cmp	-8.(r5),$0132
jne	L72
cmp	6.(r5),$_cctab
jne	L73
mov	$0177777,r0
jbr	L65
L73:mov	-18.(r5),(sp)
clr	-(sp)
mov	_isn,r0
inc	_isn
mov	r0,-8.(r5)
mov	r0,-(sp)
mov	6.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	_nstack,-24.(r5)
mov	-18.(r5),(sp)
mov	6.(r5),-(sp)
mov	6.(r3),-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	r0,-22.(r5)
mov	-24.(r5),_nstack
clr	(sp)
mov	_isn,r0
inc	_isn
mov	r0,-10.(r5)
mov	r0,-(sp)
jsr	pc,*$_branch
tst	(sp)+
mov	-8.(r5),(sp)
jsr	pc,*$_label
mov	-22.(r5),(sp)
mov	6.(r5),-(sp)
mov	8.(r3),-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	r0,-18.(r5)
cmp	-22.(r5),-18.(r5)
jeq	L74
.data
L75:.byte 0155,0157,0166,045,0143,011,0162,045,0144,054,0162,045,0144,012,0
.even
.text
mov	-22.(r5),(sp)
mov	-18.(r5),-(sp)
mov	r2,-(sp)
jsr	pc,_isfloat
tst	(sp)+
mov	r0,-(sp)
mov	$L75,-(sp)
jsr	pc,*$_printf
add	$06,sp
L74:mov	-22.(r5),-18.(r5)
mov	-10.(r5),(sp)
jsr	pc,*$_label
jbr	L76
L72:cmp	-8.(r5),$043
jne	L77
cmp	*6.(r2),$024
jne	L77
mov	6.(r2),r0
cmp	6.(r0),$010
jne	L77
.data
L78:.byte 0111,0154,0154,0145,0147,0141,0154,040,0165,0163,0145,040,0157,0146,040,0162,0145,0147,0151,0163,0164,0145,0162,0
.even
.text
mov	$L78,(sp)
jsr	pc,*$_error
L77:mov	-18.(r5),(sp)
mov	r2,-(sp)
jsr	pc,*$_oddreg
tst	(sp)+
mov	r0,-18.(r5)
mov	-18.(r5),r0
inc	r0
mov	r0,-20.(r5)
mov	-18.(r5),(sp)
mov	6.(r5),-(sp)
mov	r2,-(sp)
jsr	pc,*$_chkleaf
cmp	(sp)+,(sp)+
tst	r0
jge	L76
mov	_nreg,(sp)
sub	-18.(r5),(sp)
mov	6.(r5),-(sp)
mov	r2,-(sp)
jsr	pc,*$_match
cmp	(sp)+,(sp)+
mov	r0,-30.(r5)
jne	L79
mov	$0177777,r0
jbr	L65
L79:mov	-30.(r5),r0
mov	4.(r0),-16.(r5)
mov	6.(r2),r3
clr	-14.(r5)
mov	(r2),r0
asl	r0
bit	$01,_opdope(r0)
jeq	L80
mov	8.(r2),-14.(r5)
L80:L81:mov	-16.(r5),r0
inc	-16.(r5)
movb	(r0),r0
mov	r0,-8.(r5)
jbr	L83
L84:cmp	(r2),$0145
jne	L85
mov	4.(r2),(sp)
jsr	pc,*$_popstk
clr	-18.(r5)
sub	-26.(r5),_nstack
L85:L76:mov	r2,(sp)
jsr	pc,*$_isfloat
tst	r0
jne	L86
cmp	(r2),$053
jeq	L10014
cmp	(r2),$0111
jne	L87
L10014:dec	-18.(r5)
L87:L86:mov	-18.(r5),r0
jbr	L65
L88:mov	r3,r4
jbr	L89
L90:mov	-14.(r5),r4
jbr	L89
L91:mov	r2,r4
L89:clr	-8.(r5)
cmpb	*-16.(r5),$047
jne	L92
inc	-8.(r5)
inc	-16.(r5)
L92:mov	-8.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_pname
tst	(sp)+
jbr	L81
L93:movb	*-16.(r5),r0
mov	r0,-8.(r5)
cmp	$047,r0
jne	L94
inc	-16.(r5)
jbr	L95
L94:clr	-8.(r5)
L95:mov	-8.(r5),(sp)
mov	(r2),-(sp)
jsr	pc,*$_prins
tst	(sp)+
jbr	L81
L96:mov	(r2),r0
asl	r0
bit	$0400,_opdope(r0)
jeq	L97
mov	r2,r4
jbr	L98
L97:mov	r3,r4
L98:jbr	L99
L100:mov	r2,r4
jbr	L101
L102:mov	-14.(r5),r4
L99:cmp	2.(r4),$01
jne	L103
mov	$0142,(sp)
jsr	pc,*$_putchar
L103:L101:mov	r4,(sp)
jsr	pc,*$_isfloat
tst	r0
jeq	L104
mov	$0146,(sp)
jsr	pc,*$_putchar
L104:jbr	L81
L105:cmp	2.(r3),$01
jeq	L10015
mov	-14.(r5),r0
cmp	2.(r0),$01
jne	L106
L10015:mov	$0142,(sp)
jsr	pc,*$_putchar
L106:mov	r2,r4
jbr	L101
L107:mov	6.(r3),r4
jbr	L108
L109:mov	-14.(r5),r0
mov	6.(r0),r4
L108:.data
L110:.byte 060,045,0157,0
.even
.text
mov	r4,(sp)
mov	$L110,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L81
L111:mov	r3,r4
mov	$01,-24.(r5)
jbr	L112
L113:mov	-14.(r5),r4
mov	$02,-24.(r5)
jbr	L112
L114:mov	r2,r4
mov	$04,-24.(r5)
L112:mov	$_regtab,-12.(r5)
mov	-16.(r5),r0
inc	-16.(r5)
movb	(r0),r0
add	$0177677,r0
mov	r0,-8.(r5)
bit	$02,-8.(r5)
jeq	L115
mov	$_sptab,-12.(r5)
L115:bit	$04,-8.(r5)
jeq	L116
cmp	(r4),$040
jeq	L117
cmp	(r4),$041
jeq	L117
mov	_nreg,(sp)
sub	-18.(r5),(sp)
mov	$_efftab,-(sp)
mov	r4,-(sp)
jsr	pc,*$_match
cmp	(sp)+,(sp)+
tst	r0
jeq	L117
mov	$_efftab,-12.(r5)
jbr	L118
L117:mov	$_cctab,-12.(r5)
L118:L116:bit	$01,-8.(r5)
jeq	L119
mov	6.(r4),r4
mov	r4,(sp)
jsr	pc,*$_collcon
tst	r0
jeq	L120
cmp	-12.(r5),$_sptab
jeq	L120
cmp	*8.(r4),$035
jne	L121
bis	$010,-24.(r5)
L121:mov	6.(r4),r4
L120:L119:cmp	6.(r5),$_lsptab
jne	L122
cmp	-12.(r5),$_sptab
jne	L122
mov	$_lsptab,-12.(r5)
L122:bit	$010,-8.(r5)
jeq	L123
mov	-20.(r5),-10.(r5)
jbr	L124
L123:mov	(r4),r0
asl	r0
bit	$0400,_opdope(r0)
jne	L10016
cmp	4.(r4),$02
jge	L125
L10016:mov	-18.(r5),-10.(r5)
jbr	L126
L125:mov	8.(r5),-10.(r5)
L126:L124:mov	-10.(r5),(sp)
mov	-12.(r5),-(sp)
mov	r4,-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	r0,-22.(r5)
bit	$010,-24.(r5)
jeq	L127
.data
L128:.byte 0141,0144,0144,011,0162,065,054,0162,045,0144,012,0
.even
.text
mov	-22.(r5),(sp)
mov	$L128,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L127:bit	$010,-8.(r5)
jeq	L129
mov	-22.(r5),-20.(r5)
jbr	L130
L129:cmp	-22.(r5),-18.(r5)
jeq	L131
cmp	-12.(r5),$_regtab
jne	L131
clr	(sp)
mov	r2,-(sp)
jsr	pc,*$_oddreg
tst	(sp)+
tst	r0
jne	L132
bit	$04,-24.(r5)
jne	L10017
bit	$01,-24.(r5)
jeq	L10018
mov	_nreg,(sp)
sub	-22.(r5),(sp)
add	$0177777,(sp)
mov	-14.(r5),-(sp)
jsr	pc,*$_xdcalc
tst	(sp)+
mov	-30.(r5),r1
movb	2.(r1),r1
bic	$!077,r1
cmp	r0,r1
jle	L10017
L10018:bit	$02,-24.(r5)
jeq	L132
mov	_nreg,(sp)
sub	-22.(r5),(sp)
add	$0177777,(sp)
mov	r3,-(sp)
jsr	pc,*$_xdcalc
tst	(sp)+
movb	*-30.(r5),r1
bic	$!077,r1
cmp	r0,r1
jgt	L132
L10017:mov	-22.(r5),-18.(r5)
mov	-22.(r5),r0
inc	r0
mov	r0,-20.(r5)
jbr	L133
L132:.data
L134:.byte 0155,0157,0166,045,0143,011,0162,045,0144,054,0162,045,0144,012,0
.even
.text
mov	-18.(r5),(sp)
mov	-22.(r5),-(sp)
mov	r2,-(sp)
jsr	pc,_isfloat
tst	(sp)+
mov	r0,-(sp)
mov	$L134,-(sp)
jsr	pc,*$_printf
add	$06,sp
L133:L131:L130:jbr	L81
L135:mov	-18.(r5),-10.(r5)
cmpb	*-16.(r5),$055
jne	L136
inc	-16.(r5)
dec	-10.(r5)
L136:jbr	L137
L138:mov	-20.(r5),-10.(r5)
L137:cmp	-10.(r5),_nreg
jle	L139
.data
L140:.byte 0122,0145,0147,0151,0163,0164,0145,0162,040,0157,0166,0145,0162,0146,0154,0157,0167,072,040,0163,0151,0155,0160,0154,0151,0146,0171,040,0145,0170,0160,0162,0145,0163,0163,0151,0157,0156,0
.even
.text
mov	$L140,(sp)
jsr	pc,*$_error
L139:.data
L141:.byte 0162,045,0144,0
.even
.text
mov	-10.(r5),(sp)
mov	$L141,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L81
L142:cmpb	*-16.(r5),$050
jne	L143
inc	_nstack
cmp	6.(r5),$_lsptab
jeq	L144
mov	$055,(sp)
jsr	pc,*$_putchar
L144:jbr	L81
L143:jbr	L82
L145:mov	$051,(sp)
jsr	pc,*$_putchar
cmpb	*-16.(r5),$053
jne	L146
dec	_nstack
L146:jbr	L81
L147:mov	6.(r3),r4
jbr	L148
L149:mov	-14.(r5),r0
mov	6.(r0),r4
jbr	L148
L150:mov	r3,r4
L148:mov	r4,(sp)
jsr	pc,*$_collcon
tst	r0
jeq	L151
mov	8.(r4),r0
mov	r0,r4
mov	(r0),r0
jbr	L153
L154:tst	6.(r4)
jeq	L155
.data
L156:.byte 045,0144,056,0
.even
.text
mov	6.(r4),(sp)
mov	$L156,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L155:jbr	L152
L157:clr	(sp)
mov	6.(r4),-(sp)
jsr	pc,*$_pname
tst	(sp)+
jbr	L152
L158:mov	6.(r4),r4
.data
L159:.byte 045,0144,056,0
.even
.text
mov	10.(r4),(sp)
add	8.(r4),(sp)
mov	$L159,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L152
jbr	L152
L153:mov	$L10020,r1
mov	r0,L10021
L10022:cmp	r0,(r1)+
jne	L10022
jmp	*L10021-L10020(r1)
.data
L10020:25
35
43
L10021:..
L154
L158
L157
L160
.text
/esw
L152:L160:L151:jbr	L81
L161:mov	(r2),r0
movb	-60.+_maprel(r0),r0
mov	r0,(r2)
jbr	L81
L162:.data
L163:.byte 044,060,045,0157,0
.even
.text
mov	8.(r3),(sp)
add	10.(r3),(sp)
mov	$L163,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L81
L164:.data
L165:.byte 060,045,0157,0
.even
.text
mov	8.(r2),(sp)
mov	$L165,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L81
L166:mov	$05,(sp)
mov	r3,-(sp)
jsr	pc,*$_dcalc
tst	(sp)+
cmp	$014,r0
jge	L167
mov	$012,(sp)
mov	$_cctab,-(sp)
mov	r3,-(sp)
jsr	pc,*$_match
cmp	(sp)+,(sp)+
tst	r0
jne	L167
.data
L168:.byte 0164,0163,0164,011,0162,045,0144,012,0
.even
.text
mov	-18.(r5),(sp)
mov	$L168,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L167:jbr	L81
L169:tst	-28.(r5)
jeq	L170
.data
L171:.byte 052,044,0
.even
.text
mov	$L171,(sp)
jsr	pc,*$_printf
L170:jbr	L81
jbr	L82
L83:jsr	r2,hsw; 10; L10024
jmp	*L10033-L10025(r1)
.data
L10024:L10025
L10026
L10027
L10028
L10029
L10030
L10031
L10032
L10033
L10025:..
0
11
12
14
L10026:..
5
10
11
L10027:..
4
10
11
13
L10028:..
4
10
11
L10029:..
10
11
12
L10030:..
5
10
11
L10031:..
10
12
13
17
L10032:..
10
11
L10033:L172
L84
L114
L100
L169
L172
L145
L88
L135
L172
L149
L90
L138
L162
L172
L147
L96
L113
L172
L102
L105
L166
L172
L142
L107
L93
L172
L109
L161
L164
L150
L172
L111
L91
.text
/esw
L82:L172:mov	-8.(r5),(sp)
jsr	pc,*$_putchar
jbr	L81
L65:jmp	rretrn
.globl	_chkleaf
.text
_chkleaf:
jsr	r5,rsave; 14
mov	4.(r5),r4
mov	_nreg,(sp)
sub	8.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_dcalc
tst	(sp)+
cmp	$014,r0
jge	L174
mov	$0177777,r0
jbr	L173
L174:mov	$0152,-16.(r5)
mov	2.(r4),2.+-16.(r5)
mov	4.(r4),4.+-16.(r5)
mov	r4,6.+-16.(r5)
mov	8.(r5),(sp)
mov	6.(r5),-(sp)
mov	r5,r0
add	$0177760,r0
mov	r0,-(sp)
jsr	pc,*$_cexpr
cmp	(sp)+,(sp)+
jbr	L173
L173:jmp	rretrn
.globl	_comarg
.text
_comarg:
jsr	r5,rsave; 2
mov	4.(r5),r4
cmp	2.(r4),$04
jne	L176
.data
L177:.byte 0111,0154,0154,0145,0147,0141,0154,040,0163,0164,0162,0165,0143,0164,0165,0162,0145,0
.even
.text
mov	$L177,(sp)
jsr	pc,*$_error
L176:tst	_nstack
jne	L10034
mov	r4,(sp)
jsr	pc,*$_isfloat
tst	r0
jeq	L178
L10034:clr	(sp)
mov	$_sptab,-(sp)
mov	r4,-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	2.(r4),(sp)
jsr	pc,*$_arlength
jbr	L175
L178:inc	*6.(r5)
clr	(sp)
mov	$_lsptab,-(sp)
mov	r4,-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
clr	r0
jbr	L175
L175:jmp	rretrn
.globl	_optim
.text
_optim:
jsr	r5,rsave; 10
mov	4.(r5),r2
jne	L180
clr	r0
jbr	L179
L180:mov	(r2),r4
tst	r4
jne	L181
mov	r2,r0
jbr	L179
L181:mov	r4,r0
asl	r0
mov	_opdope(r0),r3
bit	$0400,r3
jeq	L182
mov	r2,r0
jbr	L179
L182:bit	$01,r3
jne	L183
mov	r2,(sp)
jsr	pc,*$_unoptim
jbr	L179
L183:bit	$0100,r3
jeq	L184
L185:mov	2.(r2),-8.(r5)
mov	r2,(sp)
jsr	pc,*$_acommute
mov	r0,r2
mov	-8.(r5),2.(r2)
mov	r2,r0
jbr	L179
L184:mov	6.(r2),(sp)
jsr	pc,*$_optim
mov	r0,6.(r2)
mov	8.(r2),(sp)
jsr	pc,*$_optim
mov	r0,8.(r2)
bit	$04,r3
jeq	L186
mov	8.(r2),(sp)
jsr	pc,*$_degree
mov	r0,-(sp)
mov	6.(r2),-(sp)
jsr	pc,_degree
tst	(sp)+
cmp	(sp)+,r0
jle	L187
mov	6.(r2),-12.(r5)
mov	8.(r2),6.(r2)
mov	-12.(r5),8.(r2)
movb	-60.+_maprel(r4),r0
mov	r0,(r2)
L187:mov	6.(r2),r0
cmp	2.(r0),$01
jne	L188
cmp	*8.(r2),$025
jne	L188
mov	6.(r2),(sp)
jsr	pc,*$_dcalc
cmp	$014,r0
jlt	L188
mov	8.(r2),r0
cmp	6.(r0),$0177
jgt	L188
mov	8.(r2),r0
tst	6.(r0)
jlt	L188
mov	8.(r2),r0
mov	$01,2.(r0)
L188:L186:mov	$01,(sp)
mov	6.(r2),-(sp)
jsr	pc,_degree
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_max
tst	(sp)+
mov	r0,-8.(r5)
clr	(sp)
mov	8.(r2),-(sp)
jsr	pc,_degree
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_max
tst	(sp)+
mov	r0,-10.(r5)
mov	r4,r0
jbr	L190
L191:mov	$012,4.(r2)
jbr	L189
L192:L193:mov	-10.(r5),(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_max
tst	(sp)+
mov	r0,4.(r2)
jbr	L189
L194:cmp	*8.(r2),$025
jne	L195
mov	$050,(r2)
mov	8.(r2),r0
mov	8.(r2),r1
mov	6.(r1),r1
neg	r1
mov	r1,6.(r0)
jbr	L185
L195:jbr	L196
L197:L198:L199:mov	r2,(sp)
jsr	pc,*$_ispow2
tst	r0
jne	L200
L201:L202:add	$02,-8.(r5)
add	$02,-10.(r5)
L200:L203:L204:cmp	*6.(r2),$025
jne	L205
cmp	*8.(r2),$025
jne	L205
mov	8.(r2),r0
mov	6.(r0),(sp)
mov	6.(r2),-(sp)
add	$06,(sp)
mov	r4,-(sp)
jsr	pc,*$_const
cmp	(sp)+,(sp)+
mov	6.(r2),r0
jbr	L179
L205:L196:L206:cmp	-8.(r5),-10.(r5)
jne	L10035
inc	-8.(r5)
mov	-8.(r5),r0
jbr	L10036
L10035:mov	-10.(r5),(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_max
tst	(sp)+
L10036:mov	r0,4.(r2)
jbr	L189
jbr	L189
L190:jsr	r2,hsw; 3; L10038
jmp	*L10042-L10039(r1)
.data
L10038:L10039
L10040
L10041
L10042
L10039:..
17
30
36
L10040:..
16
17
30
41
L10041:..
2
15
16
30
L10042:L206
L204
L199
L192
L206
L197
L203
L198
L191
L206
L193
L194
L201
L202
.text
/esw
L189:mov	r2,r0
jbr	L179
L179:jmp	rretrn
.globl	_unoptim
.text
_unoptim:
.bss
L208:.=.+8.
.text
jsr	r5,rsave; 2
mov	4.(r5),r3
jne	L209
clr	r0
jbr	L207
L209:cmp	(r3),$0147
jne	L210
mov	2.(r3),(sp)
jsr	pc,*$_optim
mov	r0,2.(r3)
mov	r3,r0
jbr	L207
L210:mov	6.(r3),(sp)
jsr	pc,*$_optim
mov	r0,6.(r3)
mov	r0,r4
cmp	(r3),$044
jne	L211
cmp	(r4),$043
jeq	L10043
cmp	(r4),$035
jne	L211
L10043:mov	6.(r4),r0
jbr	L207
L211:cmp	(r3),$063
jne	L212
cmp	(r4),$025
jne	L212
movif	6.(r4),r0
movf	r0,L208
mov	$L208,r2
add	$02,r2
tst	(r2)+
jne	L213
tst	(r2)+
jne	L213
tst	(r2)+
jne	L213
mov	$03,2.(r4)
mov	L208,6.(r4)
mov	$030,(r4)
mov	r4,r0
jbr	L207
L213:L212:cmp	(r4),$025
jne	L214
mov	(r3),r0
jbr	L216
L217:mov	6.(r4),r0
neg	r0
mov	r0,6.(r4)
mov	r4,r0
jbr	L207
L218:mov	6.(r4),r0
com	r0
mov	r0,6.(r4)
mov	r4,r0
jbr	L207
jbr	L215
L216:sub	$045,r0
cmp	r0,$01
jhi	L219
asl	r0
jmp	*.+4(r0)
L217
L218
/esw
L215:L219:L214:mov	r4,(sp)
jsr	pc,*$_degree
mov	r0,(sp)
mov	$01,-(sp)
jsr	pc,*$_max
tst	(sp)+
mov	r0,4.(r3)
mov	r3,r0
jbr	L207
L207:jmp	rretrn
.globl	_acommute
.text
_acommute:
jsr	r5,rsave; 142
clr	-92.(r5)
clr	2.+-92.(r5)
mov	4.(r5),r2
mov	(r2),-98.(r5)
mov	r2,(sp)
jsr	pc,*$_isfloat
mov	r0,-100.(r5)
mov	r5,r0
add	$0177644,r0
mov	r0,(sp)
mov	r2,-(sp)
mov	-98.(r5),-(sp)
jsr	pc,*$_insert
cmp	(sp)+,(sp)+
dec	-92.(r5)
tst	-100.(r5)
jne	L221
mov	-92.(r5),r0
asl	r0
mov	r5,r1
add	$0177720,r1
add	r1,r0
mov	r0,r3
mov	-92.(r5),-96.(r5)
L222:tst	-96.(r5)
jle	L223
cmp	*(r3),$025
jne	L223
cmp	*-2.(r3),$025
jne	L223
dec	-92.(r5)
sub	$02,r3
mov	2.(r3),r0
mov	6.(r0),(sp)
mov	(r3),-(sp)
add	$06,(sp)
mov	-98.(r5),-(sp)
jsr	pc,*$_const
cmp	(sp)+,(sp)+
L224:dec	-96.(r5)
jbr	L222
L223:L221:cmp	-98.(r5),$050
jne	L225
tst	-100.(r5)
jne	L225
tst	-92.(r5)
jle	L226
cmp	*(r3),$025
jne	L226
mov	(r3),r0
tst	6.(r0)
jne	L226
dec	-92.(r5)
sub	$02,r3
L226:tst	-92.(r5)
jgt	L227
mov	(r3),r0
jbr	L220
L227:cmp	*(r3),$025
jne	L228
cmp	*-2.(r3),$043
jeq	L10045
cmp	*-2.(r3),$035
jne	L228
L10045:sub	$02,r3
mov	2.(r3),r0
mov	(r3),r1
mov	6.(r1),r1
add	6.(r0),8.(r1)
dec	-92.(r5)
L228:jbr	L229
L225:cmp	-98.(r5),$052
jne	L230
mov	-92.(r5),r0
asl	r0
add	r5,r0
mov	-48.(r0),r4
cmp	(r4),$025
jne	L231
tst	6.(r4)
jne	L231
mov	r4,r0
jbr	L220
L231:L230:L229:cmp	-98.(r5),$050
jne	L232
tst	-100.(r5)
jne	L232
mov	r5,r0
add	$0177644,r0
mov	r0,(sp)
jsr	pc,*$_distrib
L232:mov	r5,r0
add	$0177720,r0
mov	r0,r3
mov	(r0),r2
mov	$01,(sp)
mov	r2,-(sp)
jsr	pc,_degree
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_max
tst	(sp)+
mov	r0,-94.(r5)
cmp	-98.(r5),$052
jne	L233
tst	-100.(r5)
jne	L233
inc	-94.(r5)
L233:clr	-96.(r5)
L234:cmp	-96.(r5),-92.(r5)
jge	L235
mov	-96.(r5),r0
asl	r0
add	r5,r0
mov	-88.(r0),r4
add	$02,r3
mov	r3,r0
mov	(r0),r0
mov	r0,-102.(r5)
mov	r0,8.(r4)
mov	-102.(r5),(sp)
jsr	pc,*$_degree
cmp	-94.(r5),r0
jgt	L10046
mov	-94.(r5),r0
inc	r0
jbr	L10047
L10046:mov	-94.(r5),r0
L10047:mov	r0,-94.(r5)
mov	r0,4.(r4)
mov	r2,6.(r4)
mov	r4,r2
L236:inc	-96.(r5)
jbr	L234
L235:cmp	(r2),$052
jne	L237
mov	r2,(sp)
jsr	pc,*$_ispow2
tst	r0
jeq	L237
mov	$01,(sp)
mov	6.(r2),-(sp)
jsr	pc,_degree
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_max
tst	(sp)+
mov	r0,4.(r2)
L237:mov	r2,r0
jbr	L220
L220:jmp	rretrn
.globl	_distrib
.text
_distrib:
jsr	r5,rsave; 20
L239:mov	*4.(r5),r0
asl	r0
add	4.(r5),r0
add	$054,r0
mov	r0,-18.(r5)
mov	$01750,-10.(r5)
clr	-14.(r5)
mov	4.(r5),r0
add	$054,r0
mov	r0,r4
L240:cmp	r4,-18.(r5)
jhi	L241
cmp	*(r4),$052
jne	L242
mov	(r4),r0
mov	8.(r0),r0
cmp	(r0),$025
jne	L242
clr	-12.(r5)
mov	4.(r5),r0
add	$054,r0
mov	r0,r3
L243:cmp	r3,-18.(r5)
jhi	L244
cmp	r4,r3
jeq	L245
cmp	*(r3),$052
jne	L245
mov	(r3),r0
mov	8.(r0),r0
cmp	(r0),$025
jne	L245
mov	(r4),r0
mov	8.(r0),r0
mov	(r3),r1
mov	8.(r1),r1
cmp	6.(r0),6.(r1)
jne	L246
mov	(r3),r0
mov	(r4),r1
mov	6.(r1),8.(r0)
mov	$050,*(r3)
mov	(r4),r0
mov	(r3),6.(r0)
mov	(r4),(sp)
jsr	pc,*$_optim
mov	r0,(r4)
mov	-18.(r5),(sp)
mov	r3,-(sp)
jsr	pc,*$_squash
tst	(sp)+
dec	*4.(r5)
jbr	L239
L246:mov	(r3),r1
mov	8.(r1),r1
mov	6.(r1),r1
sxt	r0
mov	(r4),r2
mov	8.(r2),r2
div	6.(r2),r0
tst	r1
jeq	L247
mov	(r4),r1
mov	8.(r1),r1
mov	6.(r1),r1
sxt	r0
mov	(r3),r2
mov	8.(r2),r2
div	6.(r2),r0
tst	r1
jne	L248
inc	-12.(r5)
mov	r3,-20.(r5)
L248:L245:add	$02,r3
jbr	L243
L244:tst	-12.(r5)
jle	L249
cmp	-12.(r5),-10.(r5)
jge	L249
mov	-12.(r5),-10.(r5)
mov	r4,-14.(r5)
mov	-20.(r5),-16.(r5)
L249:L247:L242:add	$02,r4
jbr	L240
L241:tst	-14.(r5)
jeq	L238
mov	4.(r5),r0
dec	2.(r0)
mov	2.(r0),r0
asl	r0
add	4.(r5),r0
mov	4.(r0),-8.(r5)
mov	-14.(r5),r4
mov	-16.(r5),r3
mov	$050,*-8.(r5)
mov	-8.(r5),r0
mov	(r4),r1
mov	2.(r1),2.(r0)
mov	-8.(r5),r0
mov	(r4),6.(r0)
mov	-8.(r5),r0
mov	(r3),r1
mov	6.(r1),8.(r0)
mov	(r3),r1
mov	8.(r1),r1
mov	6.(r1),-(sp)
mov	(r4),r2
mov	8.(r2),r2
mov	6.(r2),r1
sxt	r0
div	(sp)+,r0
mov	r0,6.(r2)
mov	(r3),r0
mov	-8.(r5),6.(r0)
mov	(r3),(sp)
jsr	pc,*$_optim
mov	r0,-8.(r5)
cmp	r4,r3
jhis	L250
mov	-8.(r5),(r4)
mov	-18.(r5),(sp)
mov	r3,-(sp)
jsr	pc,*$_squash
tst	(sp)+
dec	*4.(r5)
jbr	L239
L250:mov	-8.(r5),(r3)
mov	-18.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_squash
tst	(sp)+
dec	*4.(r5)
jbr	L239
L238:jmp	rretrn
.globl	_squash
.text
_squash:
jsr	r5,rsave; 2
mov	4.(r5),r4
L252:cmp	r4,6.(r5)
jhis	L253
mov	2.(r4),(r4)
L254:add	$02,r4
jbr	L252
L253:L251:jmp	rretrn
.globl	_const
.text
_const:
jsr	r5,rsave; 2
mov	8.(r5),r4
mov	4.(r5),r0
jbr	L257
L258:add	r4,*6.(r5)
jbr	L255
L259:mov	*6.(r5),r1
mul	r4,r1
mov	r1,*6.(r5)
jbr	L255
L260:mov	r4,r0
com	r0
bic	r0,*6.(r5)
jbr	L255
L261:bis	r4,*6.(r5)
jbr	L255
L262:mov	r4,r0
xor	r0,*6.(r5)
mov	*6.(r5),r0
jbr	L255
L263:L264:tst	r4
jne	L265
.data
L266:.byte 0104,0151,0166,0151,0144,0145,040,0143,0150,0145,0143,0153,0
.even
.text
mov	$L266,(sp)
jsr	pc,*$_error
jbr	L267
L265:cmp	4.(r5),$053
jne	L268
mov	*6.(r5),r1
sxt	r0
div	r4,r0
mov	r0,*6.(r5)
jbr	L269
L268:mov	*6.(r5),r1
sxt	r0
div	r4,r0
mov	r1,*6.(r5)
L269:L267:jbr	L255
L270:mov	r4,-(sp)
mov	*6.(r5),r0
neg	(sp)
ash	(sp)+,r0
mov	r0,*6.(r5)
jbr	L255
L271:mov	*6.(r5),r0
ash	r4,r0
mov	r0,*6.(r5)
jbr	L255
jbr	L256
L257:sub	$050,r0
cmp	r0,$011
jhi	L272
asl	r0
jmp	*.+4(r0)
L258
L272
L259
L263
L264
L270
L271
L260
L261
L262
/esw
L256:L272:.data
L273:.byte 0103,040,0145,0162,0162,0157,0162,072,040,0143,0157,0156,0163,0164,0
.even
.text
mov	$L273,(sp)
jsr	pc,*$_error
L255:jmp	rretrn
.globl	_insert
.text
_insert:
jsr	r5,rsave; 10
mov	6.(r5),r2
mov	8.(r5),r3
cmp	(r2),4.(r5)
jne	L275
L276:mov	2.(r3),r0
inc	2.(r3)
asl	r0
add	r3,r0
mov	r2,4.(r0)
mov	r3,(sp)
mov	6.(r2),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_insert
cmp	(sp)+,(sp)+
mov	r3,(sp)
mov	8.(r2),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_insert
cmp	(sp)+,(sp)+
jbr	L274
L275:mov	r2,(sp)
jsr	pc,*$_optim
mov	r0,r2
cmp	(r2),4.(r5)
jeq	L276
mov	r2,(sp)
jsr	pc,*$_isfloat
tst	r0
jne	L277
cmp	(r2),$052
jeq	L10049
cmp	(r2),$056
jne	L278
L10049:cmp	*8.(r2),$025
jne	L278
cmp	*6.(r2),$050
jne	L278
mov	6.(r2),r0
mov	8.(r0),r0
cmp	(r0),$025
jne	L278
mov	8.(r2),r0
mov	6.(r0),r4
cmp	(r2),$052
jne	L279
mov	8.(r2),-(sp)
add	$06,(sp)
mov	6.(r2),r1
mov	8.(r1),r1
mov	6.(r1),r1
mul	*(sp),r1
mov	r1,*(sp)+
jbr	L280
L279:mov	8.(r2),r0
mov	6.(r2),r1
mov	8.(r1),r1
mov	6.(r1),r1
ash	r4,r1
mov	r1,6.(r0)
L280:mov	6.(r2),r0
mov	8.(r0),r0
mov	r4,6.(r0)
mov	(r2),*6.(r2)
mov	$050,(r2)
cmp	4.(r5),$050
jeq	L276
L278:L277:mov	r2,(sp)
jsr	pc,*$_degree
mov	r0,r4
clr	-10.(r5)
L281:cmp	(r3),-10.(r5)
jle	L282
mov	-10.(r5),r0
asl	r0
add	r3,r0
mov	44.(r0),(sp)
jsr	pc,*$_degree
mov	r0,-8.(r5)
cmp	r4,r0
jle	L284
mov	-10.(r5),r0
asl	r0
add	r3,r0
mov	44.(r0),-12.(r5)
mov	-10.(r5),r0
asl	r0
add	r3,r0
mov	r2,44.(r0)
mov	-12.(r5),r2
mov	-8.(r5),r4
L284:L283:inc	-10.(r5)
jbr	L281
L282:mov	(r3),r0
inc	(r3)
asl	r0
add	r3,r0
mov	r2,44.(r0)
L274:jmp	rretrn
.globl	_putchar
.text
_putchar:
jsr	r5,rsave; 2
mov	$_outbuf,(sp)
mov	4.(r5),-(sp)
bic	$!0177,(sp)
jsr	pc,*$_putc
tst	(sp)+
L285:jmp	rretrn
.globl	fltused
