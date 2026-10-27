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
.globl	_max
.text
_max:
jsr	r5,rsave; 2
cmp	4.(r5),6.(r5)
jle	L2
mov	4.(r5),r0
jbr	L1
L2:mov	6.(r5),r0
jbr	L1
L1:jmp	rretrn
.globl	_degree
.text
_degree:
jsr	r5,rsave; 2
mov	4.(r5),r4
jeq	L10000
tst	(r4)
jne	L4
L10000:clr	r0
jbr	L3
L4:cmp	(r4),$025
jne	L5
mov	$0177775,r0
jbr	L3
L5:cmp	(r4),$043
jne	L6
mov	$0177776,r0
jbr	L3
L6:cmp	(r4),$035
jne	L7
mov	$0177777,r0
jbr	L3
L7:mov	(r4),r0
asl	r0
bit	$0400,_opdope(r0)
jeq	L8
cmp	2.(r4),$01
jne	L9
mov	$01,r0
jbr	L3
L9:clr	r0
jbr	L3
L8:mov	4.(r4),r0
jbr	L3
L3:jmp	rretrn
.globl	_pname
.text
_pname:
jsr	r5,rsave; 2
mov	4.(r5),r3
L11:mov	(r3),r0
jbr	L13
L14:L15:.data
L16:.byte 044,060,045,0157,0
.even
.text
mov	6.(r3),(sp)
mov	$L16,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L17:.data
L18:.byte 0114,045,0144,0
.even
.text
mov	6.(r3),(sp)
mov	$L18,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L19:L20:mov	8.(r3),r4
jeq	L21
.data
L22:.byte 045,0144,056,053,0
.even
.text
mov	r4,(sp)
mov	$L22,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L21:mov	6.(r3),r0
jbr	L24
L25:.data
L26:.byte 045,0144,056,050,0162,065,051,0
.even
.text
mov	10.(r3),(sp)
mov	$L26,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L27:.data
L28:.byte 0137,045,056,070,0163,0
.even
.text
mov	r3,(sp)
add	$012,(sp)
mov	$L28,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L29:.data
L30:.byte 0111,0154,0154,0145,0147,0141,0154,040,0163,0164,0162,0165,0143,0164,0165,0162,0145,040,0162,0145,0146,0145,0162,0145,0156,0143,0145,0
.even
.text
mov	$L30,(sp)
jsr	pc,*$_error
.data
L31:.byte 044,060,0
.even
.text
mov	$L31,(sp)
jsr	pc,*$_printf
jbr	L10
L32:tst	r4
jeq	L33
.data
L34:.byte 0102,0141,0144,040,0162,0145,0147,056,040,0162,0145,0146,0145,0162,0145,0156,0143,0145,0
.even
.text
mov	$L34,(sp)
jsr	pc,*$_error
L33:.data
L35:.byte 0162,045,0144,0
.even
.text
mov	10.(r3),(sp)
mov	$L35,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
jbr	L23
L24:sub	$04,r0
cmp	r0,$04
jhi	L36
asl	r0
jmp	*.+4(r0)
L29
L25
L27
L36
L32
/esw
L23:L36:.data
L37:.byte 0114,045,0144,0
.even
.text
mov	10.(r3),(sp)
mov	$L37,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L38:mov	$044,(sp)
jsr	pc,*$_putchar
mov	6.(r3),r3
jbr	L11
L39:mov	6.(r3),r3
cmp	(r3),$050
jne	L40
cmp	*8.(r3),$043
jne	L41
clr	(sp)
mov	8.(r3),r0
mov	6.(r0),-(sp)
jsr	pc,*$_pname
tst	(sp)+
.data
L42:.byte 050,0162,045,0144,051,0
.even
.text
mov	6.(r3),r0
mov	10.(r0),(sp)
mov	$L42,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L41:.data
L43:.byte 045,0144,056,050,0162,045,0144,051,0
.even
.text
mov	6.(r3),r0
mov	10.(r0),(sp)
mov	8.(r3),r0
mov	6.(r0),-(sp)
mov	$L43,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L10
jbr	L44
L40:cmp	(r3),$040
jne	L45
.data
L46:.byte 050,0162,045,0144,051,045,0143,0
.even
.text
tst	6.(r5)
jeq	L10002
clr	(sp)
jbr	L10003
L10002:mov	$053,(sp)
L10003:mov	6.(r3),r0
mov	10.(r0),-(sp)
mov	$L46,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L10
jbr	L47
L45:cmp	(r3),$037
jne	L48
.data
L49:.byte 045,0143,050,0162,045,0144,051,0
.even
.text
mov	6.(r3),r0
mov	10.(r0),(sp)
tst	6.(r5)
jeq	L10004
clr	-(sp)
jbr	L10005
L10004:mov	$055,-(sp)
L10005:mov	$L49,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L10
jbr	L50
L48:cmp	(r3),$024
jne	L51
cmp	6.(r3),$010
jne	L51
.data
L52:.byte 050,0162,045,0144,051,0
.even
.text
mov	10.(r3),(sp)
mov	$L52,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L10
L51:L50:L47:L44:mov	$052,(sp)
jsr	pc,*$_putchar
jbr	L11
jbr	L12
L13:sub	$024,r0
cmp	r0,$020
jhi	L53
asl	r0
jmp	*.+4(r0)
L20
L15
L53
L17
L14
L53
L53
L53
L53
L53
L53
L53
L53
L53
L53
L38
L39
/esw
L12:L53:.data
L54:.byte 0160,0156,0141,0155,0145,040,0143,0141,0154,0154,0145,0144,040,0151,0154,0154,0145,0147,0141,0154,0154,0171,0
.even
.text
mov	$L54,(sp)
jsr	pc,*$_error
L10:jmp	rretrn
.globl	_xdcalc
.text
_xdcalc:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	6.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_dcalc
tst	(sp)+
mov	r0,r3
cmp	r3,$024
jge	L56
cmp	2.(r4),$01
jne	L56
cmp	6.(r5),$01
jlt	L57
mov	$024,r3
jbr	L58
L57:mov	$030,r3
L58:L56:mov	r3,r0
jbr	L55
L55:jmp	rretrn
.globl	_dcalc
.text
_dcalc:
jsr	r5,rsave; 2
mov	4.(r5),r4
jne	L60
clr	r0
jbr	L59
L60:mov	(r4),r0
jbr	L62
L63:L64:mov	$014,r0
jbr	L59
L65:L66:tst	6.(r4)
jne	L10007
mov	$04,r0
jbr	L10008
L10007:cmp	6.(r4),$01
jne	L10009
mov	$05,r0
jbr	L10010
L10009:mov	$010,r0
L10010:L10008:jbr	L59
L67:mov	$014,r0
jbr	L59
L68:mov	6.(r4),r3
cmp	(r3),$024
jeq	L10011
cmp	(r3),$025
jne	L69
L10011:mov	$014,r0
jbr	L59
L69:cmp	(r3),$044
jne	L70
mov	6.(r3),r3
L70:cmp	(r3),$050
jne	L71
cmp	*6.(r3),$024
jne	L71
mov	6.(r3),r0
cmp	6.(r0),$010
jne	L71
cmp	*8.(r3),$025
jne	L71
mov	$014,r0
jbr	L59
L71:cmp	(r3),$050
jne	L72
cmp	*8.(r3),$043
jne	L72
cmp	*6.(r3),$024
jne	L72
mov	8.(r3),r0
cmp	2.(r0),$011
jne	L72
mov	6.(r3),r0
cmp	6.(r0),$010
jne	L72
mov	$014,r0
jbr	L59
L72:cmp	(r3),$040
jeq	L10012
cmp	(r3),$037
jne	L73
L10012:mov	6.(r3),r0
cmp	2.(r0),$014
jeq	L73
mov	6.(r3),r3
L73:cmp	(r3),$024
jne	L74
cmp	6.(r3),$010
jne	L74
mov	$014,r0
jbr	L59
L74:jbr	L61
L62:sub	$024,r0
cmp	r0,$020
jhi	L75
asl	r0
jmp	*.+4(r0)
L63
L65
L75
L67
L66
L75
L75
L75
L75
L75
L75
L75
L75
L75
L75
L64
L68
/esw
L61:L75:cmp	4.(r4),6.(r5)
jgt	L10014
mov	$024,r0
jbr	L10015
L10014:mov	$030,r0
L10015:jbr	L59
L59:jmp	rretrn
.globl	_notcompa
.text
_notcompa:
jsr	r5,rsave; 2
mov	4.(r5),r2
mov	2.(r2),r4
mov	6.(r5),r3
mov	r4,r0
bic	$!07,r0
cmp	$04,r0
jne	L77
bic	$!077770,r4
L77:tst	r3
jne	L78
cmp	r4,$01
jgt	L10016
clr	r0
jbr	L10017
L10016:mov	$01,r0
L10017:cmp	r4,$07
jle	L10018
clr	r1
jbr	L10019
L10018:mov	$01,r1
L10019:com	r1
bic	r1,r0
jbr	L76
L78:cmp	r3,$01
jne	L79
tst	r4
jgt	L10020
clr	r0
jbr	L10021
L10020:mov	$01,r0
L10021:cmp	r4,$07
jle	L10022
clr	r1
jbr	L10023
L10022:mov	$01,r1
L10023:com	r1
bic	r1,r0
jbr	L76
L79:sub	$02,r3
bit	$077740,r4
jeq	L80
mov	$020,r4
L80:bit	$077770,r4
jeq	L81
mov	r4,r0
bic	$!07,r0
bis	$020,r0
mov	r0,r4
L81:cmp	r3,$02
jne	L82
cmp	r4,$03
jne	L82
mov	$02,r4
L82:cmp	(r2),$024
jne	L83
cmp	6.(r2),$010
jne	L83
tst	r4
jne	L83
cmp	r3,$01
jne	L83
clr	r0
jbr	L76
L83:cmp	r3,r4
jne	L10024
clr	r0
jbr	L10025
L10024:mov	$01,r0
L10025:jbr	L76
L76:jmp	rretrn
.globl	_prins
.text
_prins:
jsr	r5,rsave; 2
mov	$_instab,r4
L85:tst	(r4)
jeq	L86
cmp	(r4),4.(r5)
jne	L88
tst	6.(r5)
jeq	L10026
mov	4.(r4),r0
jbr	L10027
L10026:mov	2.(r4),r0
L10027:mov	r0,r3
tst	r3
jeq	L86
.data
L89:.byte 045,0163,0
.even
.text
mov	r3,(sp)
mov	$L89,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L84
L88:L87:add	$06,r4
jbr	L85
L86:.data
L90:.byte 0116,0157,040,0155,0141,0164,0143,0150,047,040,0146,0157,0162,040,0157,0160,040,045,0144,0
.even
.text
mov	4.(r5),(sp)
mov	$L90,-(sp)
jsr	pc,*$_error
tst	(sp)+
L84:jmp	rretrn
.globl	_collcon
.text
_collcon:
jsr	r5,rsave; 2
mov	4.(r5),r3
cmp	(r3),$050
jne	L92
mov	*8.(r3),r4
cmp	r4,$025
jeq	L10028
cmp	r4,$043
jeq	L10028
cmp	r4,$035
jne	L93
L10028:mov	$01,r0
jbr	L91
L93:L92:clr	r0
jbr	L91
L91:jmp	rretrn
.globl	_isfloat
.text
_isfloat:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	(r4),r0
asl	r0
bit	$04,_opdope(r0)
jeq	L95
mov	6.(r4),r4
L95:cmp	2.(r4),$02
jeq	L10029
cmp	2.(r4),$03
jne	L96
L10029:mov	$01,_nfloat
mov	$0146,r0
jbr	L94
L96:clr	r0
jbr	L94
L94:jmp	rretrn
.globl	_oddreg
.text
_oddreg:
jsr	r5,rsave; 2
mov	6.(r5),r4
mov	4.(r5),(sp)
jsr	pc,*$_isfloat
tst	r0
jne	L98
mov	*4.(r5),r0
jbr	L100
L101:L102:L103:L104:inc	r4
L105:L106:mov	r4,r0
bis	$01,r0
jbr	L97
jbr	L99
L100:mov	$L10031,r1
mov	r0,L10032
L10033:cmp	r0,(r1)+
jne	L10033
jmp	*L10032-L10031(r1)
.data
L10031:52
53
54
110
111
112
L10032:..
L105
L101
L102
L106
L103
L104
L107
.text
/esw
L99:L107:L98:mov	r4,r0
jbr	L97
L97:jmp	rretrn
.globl	_arlength
.text
_arlength:
jsr	r5,rsave; 2
cmp	4.(r5),$010
jlt	L109
mov	$02,r0
jbr	L108
L109:mov	4.(r5),r0
jbr	L111
L112:L113:mov	$02,r0
jbr	L108
L114:L115:mov	$010,r0
jbr	L108
jbr	L110
L111:cmp	r0,$03
jhi	L116
asl	r0
jmp	*.+4(r0)
L112
L113
L114
L115
/esw
L110:L116:mov	$02000,r0
jbr	L108
L108:jmp	rretrn
.globl	_pswitch
.text
_pswitch:
jsr	r5,rsave; 642
mov	4.(r5),r3
mov	6.(r5),r2
cmp	r3,r2
jne	L118
.data
L119:.byte 0152,0142,0162,011,0114,045,0144,012,0
.even
.text
mov	8.(r5),(sp)
mov	$L119,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L117
L118:mov	_isn,r0
inc	_isn
mov	r0,-8.(r5)
mov	r2,(sp)
mov	r3,-(sp)
jsr	pc,*$_sort
tst	(sp)+
tst	r0
jne	L117
mov	r2,r0
sub	r3,r0
ash	$-02,r0
mov	r0,-10.(r5)
sub	$04,r2
mov	2.(r2),r0
sub	2.(r3),r0
mov	r0,-22.(r5)
tst	-22.(r5)
jle	L120
mov	-10.(r5),r1
mul	$03,r1
cmp	-22.(r5),r1
jgt	L120
tst	2.(r3)
jeq	L121
.data
L122:.byte 0163,0165,0142,011,044,060,045,0157,054,0162,060,012,0
.even
.text
mov	2.(r3),(sp)
mov	$L122,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L121:.data
L123:.byte 0143,0155,0160,011,0162,060,054,044,060,045,0157,012,0
.even
.text
mov	-22.(r5),(sp)
mov	$L123,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L124:.byte 0152,0150,0151,011,0114,045,0144,012,0
.even
.text
mov	8.(r5),(sp)
mov	$L124,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L125:.byte 0141,0163,0154,011,0162,060,012,0
.even
.text
mov	$L125,(sp)
jsr	pc,*$_printf
.data
L126:.byte 0152,0155,0160,011,052,056,053,064,050,0162,060,051,012,0
.even
.text
mov	$L126,(sp)
jsr	pc,*$_printf
mov	2.(r3),-12.(r5)
L127:cmp	2.(r2),-12.(r5)
jlt	L128
cmp	2.(r3),-12.(r5)
jne	L130
.data
L131:.byte 0114,045,0144,012,0
.even
.text
mov	(r3),(sp)
mov	$L131,-(sp)
jsr	pc,*$_printf
tst	(sp)+
add	$04,r3
jbr	L132
L130:.data
L133:.byte 0114,045,0144,012,0
.even
.text
mov	8.(r5),(sp)
mov	$L133,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L132:L129:inc	-12.(r5)
jbr	L127
L128:jbr	L134
L120:cmp	-10.(r5),$010
jge	L135
mov	_isn,r0
inc	_isn
mov	r0,-12.(r5)
mov	_isn,r0
inc	_isn
mov	r0,-14.(r5)
.data
L136:.byte 0155,0157,0166,011,044,0114,045,0144,054,0162,061,012,0
.even
.text
mov	-12.(r5),(sp)
mov	$L136,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L137:.byte 0155,0157,0166,011,0162,060,054,0114,045,0144,012,0
.even
.text
mov	-14.(r5),(sp)
mov	$L137,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L138:.byte 0114,045,0144,072,0143,0155,0160,011,0162,060,054,050,0162,061,051,053,012,0
.even
.text
mov	_isn,(sp)
mov	$L138,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L139:.byte 0152,0156,0145,011,0114,045,0144,012,0
.even
.text
mov	_isn,(sp)
inc	_isn
mov	$L139,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L140:.byte 0152,0155,0160,011,052,0114,045,0144,055,0114,045,0144,050,0162,061,051,012,0
.even
.text
mov	-12.(r5),(sp)
mov	-14.(r5),-(sp)
mov	$L140,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
.data
L141:.byte 056,0144,0141,0164,0141,012,0114,045,0144,072,0
.even
.text
mov	-12.(r5),(sp)
mov	$L141,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L142:cmp	r3,r2
jhi	L143
.data
L145:.byte 045,0157,012,0
.even
.text
mov	2.(r3),(sp)
mov	$L145,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L144:add	$04,r3
jbr	L142
L143:.data
L146:.byte 0114,045,0144,072,056,056,012,0
.even
.text
mov	-14.(r5),(sp)
mov	$L146,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	4.(r5),r3
L147:cmp	r3,r2
jhi	L148
.data
L150:.byte 0114,045,0144,012,0
.even
.text
mov	(r3),(sp)
mov	$L150,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L149:add	$04,r3
jbr	L147
L148:.data
L151:.byte 0114,045,0144,012,056,0164,0145,0170,0164,012,0
.even
.text
mov	8.(r5),(sp)
mov	$L151,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L134
L135:mov	$077777,-20.(r5)
mov	-10.(r5),r0
ash	$-02,r0
mov	r0,-12.(r5)
L152:mov	-10.(r5),r0
asr	r0
cmp	-12.(r5),r0
jgt	L153
clr	-14.(r5)
L155:cmp	-14.(r5),-12.(r5)
jge	L156
mov	-14.(r5),r0
asl	r0
add	r5,r0
clr	-422.(r0)
L157:inc	-14.(r5)
jbr	L155
L156:mov	r3,r4
L158:cmp	r4,r2
jhi	L159
mov	-12.(r5),(sp)
mov	2.(r4),-(sp)
clr	-(sp)
jsr	pc,*$_lrem
cmp	(sp)+,(sp)+
asl	r0
add	r5,r0
inc	-422.(r0)
L160:add	$04,r4
jbr	L158
L159:clr	-18.(r5)
clr	-14.(r5)
L161:cmp	-14.(r5),-12.(r5)
jge	L162
mov	-14.(r5),r0
asl	r0
add	r5,r0
cmp	-422.(r0),-18.(r5)
jle	L164
mov	-14.(r5),r0
asl	r0
add	r5,r0
mov	-422.(r0),-18.(r5)
L164:L163:inc	-14.(r5)
jbr	L161
L162:mov	-12.(r5),r1
mul	-18.(r5),r1
cmp	-20.(r5),r1
jle	L165
mov	-12.(r5),-16.(r5)
mov	-12.(r5),r1
mul	-18.(r5),r1
mov	r1,-20.(r5)
L165:L154:inc	-12.(r5)
jbr	L152
L153:.data
L166:.byte 0152,0163,0162,011,0162,062,054,0150,0163,0167,073,040,045,0157,073,040,0114,045,0144,012,0
.even
.text
mov	_isn,(sp)
mov	-16.(r5),-(sp)
mov	$L166,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
.data
L167:.byte 0152,0155,0160,011,052,0114,045,0144,055,0114,045,0144,050,0162,061,051,012,0
.even
.text
mov	_isn,(sp)
inc	(sp)
mov	_isn,-(sp)
add	-16.(r5),(sp)
inc	(sp)
mov	$L167,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
.data
L168:.byte 056,0144,0141,0164,0141,012,0114,045,0144,072,0
.even
.text
mov	_isn,(sp)
inc	_isn
mov	$L168,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	-12.(r5)
L169:cmp	-12.(r5),-16.(r5)
jgt	L170
.data
L172:.byte 0114,045,0144,012,0
.even
.text
mov	_isn,(sp)
add	-12.(r5),(sp)
mov	$L172,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L171:inc	-12.(r5)
jbr	L169
L170:clr	-12.(r5)
L173:cmp	-12.(r5),-16.(r5)
jge	L174
.data
L176:.byte 0114,045,0144,072,056,056,012,0
.even
.text
mov	_isn,(sp)
inc	_isn
mov	$L176,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	r3,r4
L177:cmp	r4,r2
jhi	L178
mov	-16.(r5),(sp)
mov	2.(r4),-(sp)
clr	-(sp)
jsr	pc,*$_lrem
cmp	(sp)+,(sp)+
cmp	-12.(r5),r0
jne	L180
.data
L181:.byte 045,0157,012,0
.even
.text
mov	-16.(r5),(sp)
mov	2.(r4),-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,(sp)
mov	$L181,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L180:L179:add	$04,r4
jbr	L177
L178:L175:inc	-12.(r5)
jbr	L173
L174:.data
L182:.byte 0114,045,0144,072,0
.even
.text
mov	_isn,(sp)
inc	_isn
mov	$L182,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	-12.(r5)
L183:cmp	-12.(r5),-16.(r5)
jge	L184
.data
L186:.byte 0114,045,0144,012,0
.even
.text
mov	8.(r5),(sp)
mov	$L186,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	r3,r4
L187:cmp	r4,r2
jhi	L188
mov	-16.(r5),(sp)
mov	2.(r4),-(sp)
clr	-(sp)
jsr	pc,*$_lrem
cmp	(sp)+,(sp)+
cmp	-12.(r5),r0
jne	L190
.data
L191:.byte 0114,045,0144,012,0
.even
.text
mov	(r4),(sp)
mov	$L191,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L190:L189:add	$04,r4
jbr	L187
L188:L185:inc	-12.(r5)
jbr	L183
L184:.data
L192:.byte 056,0164,0145,0170,0164,012,0
.even
.text
mov	$L192,(sp)
jsr	pc,*$_printf
L134:.data
L193:.byte 057,0145,0163,0167,012,0
.even
.text
mov	$L193,(sp)
jsr	pc,*$_printf
L117:jmp	rretrn
.globl	_sort
.text
_sort:
jsr	r5,rsave; 6
mov	4.(r5),r3
mov	6.(r5),r2
L195:sub	$04,r2
mov	r2,r0
cmp	r3,r0
jhis	L196
clr	-8.(r5)
mov	r3,r4
L197:cmp	r4,r2
jhis	L198
cmp	2.(r4),6.(r4)
jne	L200
.data
L201:.byte 0104,0165,0160,0154,0151,0143,0141,0164,0145,040,0143,0141,0163,0145,040,050,045,0144,051,0
.even
.text
mov	2.(r4),(sp)
mov	$L201,-(sp)
jsr	pc,*$_error
tst	(sp)+
mov	$01,r0
jbr	L194
L200:cmp	2.(r4),6.(r4)
jle	L202
inc	-8.(r5)
mov	2.(r4),-10.(r5)
mov	6.(r4),2.(r4)
mov	-10.(r5),6.(r4)
mov	(r4),-10.(r5)
mov	4.(r4),(r4)
mov	-10.(r5),4.(r4)
L202:L199:add	$04,r4
jbr	L197
L198:tst	-8.(r5)
jeq	L196
jbr	L195
L196:clr	r0
jbr	L194
L194:jmp	rretrn
.globl	_ispow2
.text
_ispow2:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	r3,(sp)
jsr	pc,*$_isfloat
tst	r0
jne	L204
cmp	*8.(r3),$025
jne	L204
mov	8.(r3),r0
mov	6.(r0),r4
tst	r4
jle	L205
mov	r4,r0
add	$0177777,r0
bit	r4,r0
jne	L205
mov	r4,r0
jbr	L203
L205:L204:clr	r0
jbr	L203
L203:jmp	rretrn
.globl	_pow2
.text
_pow2:
jsr	r5,rsave; 2
mov	4.(r5),r2
mov	r2,(sp)
jsr	pc,*$_ispow2
mov	r0,r4
jeq	L207
clr	r3
L208:asr	r4
jeq	L209
L210:inc	r3
jbr	L208
L209:mov	8.(r2),r0
mov	r3,6.(r0)
mov	(r2),r4
cmp	r4,$052
jne	L10035
mov	$056,r0
jbr	L10036
L10035:cmp	r4,$053
jne	L10037
mov	$055,r0
jbr	L10038
L10037:cmp	r4,$0110
jne	L10039
mov	$0114,r0
jbr	L10040
L10039:mov	$0113,r0
L10040:L10038:L10036:mov	r0,(r2)
L207:L206:jmp	rretrn
.globl	_cbranch
.text
_cbranch:
jsr	r5,rsave; 20
mov	6.(r5),r4
mov	10.(r5),r3
mov	4.(r5),r2
jeq	L211
mov	(r2),r0
jbr	L213
L214:tst	8.(r5)
jeq	L215
mov	r3,(sp)
clr	-(sp)
mov	_isn,r0
inc	_isn
mov	r0,-8.(r5)
mov	r0,-(sp)
mov	6.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	r3,(sp)
mov	$01,-(sp)
mov	r4,-(sp)
mov	8.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	-8.(r5),(sp)
jsr	pc,*$_label
jbr	L216
L215:mov	r3,(sp)
clr	-(sp)
mov	r4,-(sp)
mov	6.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	r3,(sp)
clr	-(sp)
mov	r4,-(sp)
mov	8.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
L216:jbr	L211
L217:tst	8.(r5)
jeq	L218
mov	r3,(sp)
mov	$01,-(sp)
mov	r4,-(sp)
mov	6.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	r3,(sp)
mov	$01,-(sp)
mov	r4,-(sp)
mov	8.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
jbr	L219
L218:mov	r3,(sp)
mov	$01,-(sp)
mov	_isn,r0
inc	_isn
mov	r0,-8.(r5)
mov	r0,-(sp)
mov	6.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	r3,(sp)
clr	-(sp)
mov	r4,-(sp)
mov	8.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
mov	-8.(r5),(sp)
jsr	pc,*$_label
L219:jbr	L211
L220:mov	r3,(sp)
mov	8.(r5),r0
beq	1f
clr	r0
br	2f
1:	mov	$1,r0
2:
mov	r0,-(sp)
mov	r4,-(sp)
mov	6.(r2),-(sp)
jsr	pc,*$_cbranch
add	$06,sp
jbr	L211
L221:mov	r3,(sp)
mov	$_efftab,-(sp)
mov	6.(r2),-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	8.(r2),r2
jbr	L212
jbr	L212
L213:mov	$L10042,r1
mov	r0,L10043
L10044:cmp	r0,(r1)+
jne	L10044
jmp	*L10043-L10042(r1)
.data
L10042:11
42
65
66
L10043:..
L221
L220
L214
L217
L222
.text
/esw
L212:L222:clr	-10.(r5)
mov	(r2),r0
asl	r0
bit	$04,_opdope(r0)
jne	L223
mov	$075,-20.(r5)
mov	2.(r2),2.+-20.(r5)
mov	4.(r2),4.+-20.(r5)
mov	r2,6.+-20.(r5)
mov	$_czero,8.+-20.(r5)
mov	r2,(sp)
jsr	pc,*$_isfloat
tst	r0
jeq	L224
mov	$_fczero,8.+-20.(r5)
L224:mov	r5,r0
add	$0177754,r0
mov	r0,r2
L223:cmp	*8.(r2),$025
jne	L225
mov	8.(r2),r0
tst	6.(r0)
jne	L225
mov	$0310,-10.(r5)
L225:mov	r3,(sp)
mov	$_cctab,-(sp)
mov	r2,-(sp)
jsr	pc,*$_rcexpr
cmp	(sp)+,(sp)+
mov	r2,(sp)
jsr	pc,*$_isfloat
tst	r0
jeq	L226
.data
L227:.byte 0143,0146,0143,0143,012,0
.even
.text
mov	$L227,(sp)
jsr	pc,*$_printf
L226:mov	8.(r5),r0
beq	1f
clr	r0
br	2f
1:	mov	$1,r0
2:
mov	r0,(sp)
mov	(r2),-(sp)
add	-10.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_branch
cmp	(sp)+,(sp)+
L211:jmp	rretrn
.globl	_branch
.text
_branch:
jsr	r5,rsave; 2
mov	6.(r5),r4
jeq	L229
mov	8.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_prins
tst	(sp)+
jbr	L230
L229:.data
L231:.byte 0152,0142,0162,0
.even
.text
mov	$L231,(sp)
jsr	pc,*$_printf
L230:.data
L232:.byte 011,0114,045,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L232,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L228:jmp	rretrn
.globl	_label
.text
_label:
jsr	r5,rsave; 2
.data
L234:.byte 0114,045,0144,072,0
.even
.text
mov	4.(r5),(sp)
mov	$L234,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L233:jmp	rretrn
.globl	_popstk
.text
_popstk:
jsr	r5,rsave; 2
mov	4.(r5),r0
jbr	L237
L238:jbr	L235
L239:.data
L240:.byte 0164,0163,0164,011,050,0163,0160,051,053,012,0
.even
.text
mov	$L240,(sp)
jsr	pc,*$_printf
jbr	L235
L241:.data
L242:.byte 0143,0155,0160,011,050,0163,0160,051,053,054,050,0163,0160,051,053,012,0
.even
.text
mov	$L242,(sp)
jsr	pc,*$_printf
jbr	L235
jbr	L236
L237:cmp	r0,$04
jhi	L243
asl	r0
jmp	*.+4(r0)
L238
L243
L239
L243
L241
/esw
L236:L243:.data
L244:.byte 0141,0144,0144,011,044,060,045,0157,054,0163,0160,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L244,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L235:jmp	rretrn
.globl	_error
.text
_error:
jsr	r5,rsave; 2
inc	_nerror
mov	$_outbuf,(sp)
jsr	pc,*$_fflush
movb	_outbuf,r4
movb	$01,_outbuf
.data
L246:.byte 045,0144,072,040,0
.even
.text
mov	_line,(sp)
mov	$L246,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	16.(r5),(sp)
mov	14.(r5),-(sp)
mov	12.(r5),-(sp)
mov	10.(r5),-(sp)
mov	8.(r5),-(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_printf
add	$014,sp
mov	$012,(sp)
jsr	pc,*$_putchar
mov	$_outbuf,(sp)
jsr	pc,*$_fflush
movb	r4,_outbuf
L245:jmp	rretrn
