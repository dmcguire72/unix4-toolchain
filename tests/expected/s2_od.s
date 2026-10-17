.globl	_word
.comm	_word,040
.globl	_gbuf
.comm	_gbuf,01000
.globl	_fi
.comm	_fi,02
.globl	_conv
.comm	_conv,02
.globl	_base
.data
_base=.
8.
.globl	_basem
.data
_basem=.
512.
.globl	_max
.comm	_max,02
.globl	_gidx
.comm	_gidx,02
.globl	_gcnt
.comm	_gcnt,02
.globl	_eof
.comm	_eof,02
.globl	_addr
.comm	_addr,04
.globl	_from
.comm	_from,04
.globl	_key
.comm	_key,02
.globl	_flag
.comm	_flag,02
.globl	_nword
.data
_nword=.
8.
.globl	_main
.text
_main:
jsr	r5,rsave; 20
mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
add	$02,6.(r5)
cmp	4.(r5),$01
jle	L2
mov	*6.(r5),-20.(r5)
cmpb	*-20.(r5),$055
jne	L3
L4:mov	-20.(r5),r0
inc	-20.(r5)
movb	(r0),r0
mov	r0,-14.(r5)
jeq	L5
mov	-14.(r5),r0
jbr	L7
L8:bis	$01,_conv
mov	$06,-8.(r5)
jbr	L6
L9:bis	$02,_conv
mov	$05,-8.(r5)
jbr	L6
L10:bis	$04,_conv
mov	$04,-8.(r5)
jbr	L6
L11:bis	$010,_conv
mov	$04,-8.(r5)
jbr	L6
L12:bis	$020,_conv
mov	$05,-8.(r5)
jbr	L6
L13:bis	$040,_conv
mov	$07,-8.(r5)
jbr	L6
jbr	L6
L7:sub	$0141,r0
cmp	r0,$016
jhi	L14
asl	r0
jmp	*.+4(r0)
L10
L13
L12
L9
L14
L14
L14
L11
L14
L14
L14
L14
L14
L14
L8
/esw
L6:L14:cmp	-8.(r5),_max
jle	L15
mov	-8.(r5),_max
L15:jbr	L4
L5:dec	4.(r5)
add	$02,6.(r5)
L3:L2:tst	_conv
jne	L16
mov	$06,_max
mov	$01,_conv
L16:cmp	4.(r5),$01
jle	L17
mov	6.(r5),r0
movb	*(r0),r0
cmp	$053,r0
jeq	L18
clr	(sp)
mov	*6.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
tst	_fi
jge	L19
.data
L20:.byte 0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	*6.(r5),(sp)
mov	$L20,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L21
L19:add	$02,6.(r5)
dec	4.(r5)
L18:L17:cmp	4.(r5),$01
jle	L22
mov	*6.(r5),(sp)
jsr	pc,*$_offset
L22:L23:mov	$01,-8.(r5)
mov	_addr,-18.(r5)
mov	2.+_addr,2.+-18.(r5)
clr	-14.(r5)
L24:cmp	-14.(r5),_nword
jge	L25
jsr	pc,_getw
mov	r0,-12.(r5)
tst	_eof
jne	L25
mov	-14.(r5),r0
asl	r0
mov	-12.(r5),_word(r0)
tst	-14.(r5)
jeq	L27
cmp	-12.(r5),-10.(r5)
jeq	L10001
clr	r0
jbr	L10002
L10001:mov	$01,r0
L10002:com	r0
bic	r0,-8.(r5)
jbr	L28
L27:mov	-12.(r5),-10.(r5)
L28:L26:inc	-14.(r5)
jbr	L24
L25:tst	-14.(r5)
jeq	L29
tst	-8.(r5)
jeq	L30
tst	_eof
jne	L30
tst	_flag
jeq	L10003
cmp	-10.(r5),_key
jeq	L31
L10003:jsr	pc,_dupl
mov	-10.(r5),_key
mov	-18.(r5),_from
mov	2.+-18.(r5),2.+_from
L31:add	-14.(r5),_flag
jbr	L32
L30:jsr	pc,_dupl
mov	-14.(r5),(sp)
mov	$_word,-(sp)
mov	r5,r0
add	$0177756,r0
mov	r0,-(sp)
jsr	pc,*$_line
cmp	(sp)+,(sp)+
L32:L29:tst	_eof
jeq	L23
mov	$_addr,(sp)
jsr	pc,*$_puta
mov	$012,(sp)
jsr	pc,*$_putchar
L21:jsr	pc,_flush
L1:jmp	rretrn
.globl	_dupl
.text
_dupl:
jsr	r5,rsave; 2
tst	_flag
jeq	L34
clr	_flag
mov	$01,(sp)
mov	$_key,-(sp)
mov	$_from,-(sp)
jsr	pc,*$_line
cmp	(sp)+,(sp)+
L34:L33:jmp	rretrn
.globl	_puta
.text
_puta:
jsr	r5,rsave; 2
mov	$04,(sp)
mov	_base,-(sp)
mov	*4.(r5),-(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
mov	$03,(sp)
mov	_base,-(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
mov	$040,(sp)
jsr	pc,*$_putchar
L35:jmp	rretrn
.globl	_line
.text
_line:
jsr	r5,rsave; 10
mov	$01,-10.(r5)
mov	$01,-12.(r5)
L37:tst	-12.(r5)
jeq	L38
bit	_conv,-12.(r5)
jeq	L39
tst	-10.(r5)
jeq	L40
mov	4.(r5),(sp)
jsr	pc,*$_puta
clr	-10.(r5)
jbr	L41
L40:clr	-8.(r5)
L42:cmp	-8.(r5),$010
jge	L43
mov	$040,(sp)
jsr	pc,*$_putchar
L44:inc	-8.(r5)
jbr	L42
L43:L41:clr	-8.(r5)
L45:cmp	-8.(r5),8.(r5)
jge	L46
mov	-12.(r5),(sp)
mov	-8.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_putx
tst	(sp)+
mov	8.(r5),r0
add	$0177777,r0
cmp	-8.(r5),r0
jne	L10004
mov	$012,(sp)
jbr	L10005
L10004:mov	$040,(sp)
L10005:jsr	pc,*$_putchar
L47:inc	-8.(r5)
jbr	L45
L46:L39:add	-12.(r5),-12.(r5)
jbr	L37
L38:L36:jmp	rretrn
.globl	_putx
.text
_putx:
jsr	r5,rsave; 2
mov	6.(r5),r0
jbr	L50
L51:mov	$06,(sp)
jsr	pc,*$_pre
mov	$06,(sp)
mov	$010,-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
jbr	L49
L52:mov	$05,(sp)
jsr	pc,*$_pre
mov	$05,(sp)
mov	$012,-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
jbr	L49
L53:mov	$04,(sp)
jsr	pc,*$_pre
mov	4.(r5),(sp)
jsr	pc,*$_putop
jbr	L49
L54:mov	$04,(sp)
jsr	pc,*$_pre
mov	$04,(sp)
mov	$020,-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
jbr	L49
L55:mov	$05,(sp)
jsr	pc,*$_pre
mov	4.(r5),(sp)
jsr	pc,*$_putc
mov	$040,(sp)
jsr	pc,*$_putchar
mov	4.(r5),r0
ash	$-010,r0
mov	r0,(sp)
jsr	pc,*$_putc
jbr	L49
L56:mov	$07,(sp)
jsr	pc,*$_pre
mov	$03,(sp)
mov	$010,-(sp)
mov	4.(r5),-(sp)
bic	$!0377,(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
mov	$040,(sp)
jsr	pc,*$_putchar
mov	$03,(sp)
mov	$010,-(sp)
mov	4.(r5),r0
ash	$-010,r0
mov	r0,-(sp)
bic	$!0377,(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
jbr	L49
jbr	L49
L50:mov	$L10007,r1
mov	r0,L10008
L10009:cmp	r0,(r1)+
jne	L10009
jmp	*L10008-L10007(r1)
.data
L10007:1
2
4
10
20
40
L10008:..
L51
L52
L53
L54
L55
L56
L57
.text
/esw
L49:L57:L48:jmp	rretrn
.globl	_getw
.text
_getw:
jsr	r5,rsave; 6
jsr	pc,_getc
mov	r0,-8.(r5)
cmp	-8.(r5),$0177777
jne	L59
mov	$01,_eof
clr	r0
jbr	L58
L59:jsr	pc,_getc
mov	r0,-10.(r5)
cmp	-10.(r5),$0177777
jne	L60
clr	-10.(r5)
L60:mov	-10.(r5),r0
ash	$010,r0
bis	-8.(r5),r0
jbr	L58
L58:jmp	rretrn
.globl	_getc
.text
_getc:
jsr	r5,rsave; 2
cmp	_gidx,_gcnt
jlt	L62
mov	$01000,(sp)
mov	$_gbuf,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,_gcnt
tst	_gcnt
jgt	L63
mov	$0177777,r0
jbr	L61
L63:clr	_gidx
L62:inc	2.+_addr
mov	2.+_addr,r0
cmp	_basem,r0
jgt	L64
inc	_addr
clr	2.+_addr
L64:mov	_gidx,r0
inc	_gidx
movb	_gbuf(r0),r0
bic	$!0377,r0
jbr	L61
L61:jmp	rretrn
.globl	_putc
.text
_putc:
jsr	r5,rsave; 2
bic	$!0377,4.(r5)
cmp	4.(r5),$037
jle	L66
cmp	4.(r5),$0177
jge	L66
cmp	4.(r5),$0134
jeq	L66
mov	$040,(sp)
jsr	pc,*$_putchar
mov	4.(r5),(sp)
jsr	pc,*$_putchar
jbr	L65
L66:mov	$0134,(sp)
jsr	pc,*$_putchar
mov	4.(r5),r0
jbr	L68
L69:mov	$060,(sp)
jsr	pc,*$_putchar
jbr	L67
L70:mov	$0156,(sp)
jsr	pc,*$_putchar
jbr	L67
L71:mov	$0134,(sp)
jsr	pc,*$_putchar
jbr	L67
L72:mov	$0164,(sp)
jsr	pc,*$_putchar
jbr	L67
L73:mov	$077,(sp)
jsr	pc,*$_putchar
jbr	L67
L68:mov	$L10011,r1
mov	r0,L10012
L10013:cmp	r0,(r1)+
jne	L10013
jmp	*L10012-L10011(r1)
.data
L10011:0
11
12
134
L10012:..
L69
L72
L70
L71
L73
.text
/esw
L67:L65:jmp	rretrn
.globl	_putn
.text
_putn:
jsr	r5,rsave; 2
tst	8.(r5)
jeq	L74
mov	8.(r5),(sp)
add	$0177777,(sp)
mov	6.(r5),-(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
clr	-(sp)
jsr	pc,_ldiv
add	$06,sp
mov	r0,-(sp)
jsr	pc,*$_putn
cmp	(sp)+,(sp)+
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_lrem
cmp	(sp)+,(sp)+
mov	r0,4.(r5)
cmp	$011,r0
jge	L75
mov	4.(r5),(sp)
add	$0127,(sp)
jsr	pc,*$_putchar
jbr	L76
L75:mov	4.(r5),(sp)
add	$060,(sp)
jsr	pc,*$_putchar
L76:L74:jmp	rretrn
.globl	_pre
.text
_pre:
jsr	r5,rsave; 4
mov	4.(r5),-8.(r5)
L78:cmp	-8.(r5),_max
jge	L79
mov	$040,(sp)
jsr	pc,*$_putchar
L80:inc	-8.(r5)
jbr	L78
L79:L77:jmp	rretrn
.globl	_offset
.text
_offset:
jsr	r5,rsave; 20
mov	4.(r5),-8.(r5)
L82:mov	-8.(r5),r0
inc	-8.(r5)
tstb	(r0)
jeq	L83
jbr	L82
L83:mov	-8.(r5),r0
sub	4.(r5),r0
add	$0177777,r0
mov	r0,-16.(r5)
clr	-20.(r5)
tst	-16.(r5)
jle	L84
mov	-8.(r5),r0
movb	-2.(r0),r0
cmp	$0142,r0
jne	L85
dec	-16.(r5)
inc	-20.(r5)
dec	-8.(r5)
L85:L84:tst	-16.(r5)
jle	L86
mov	-8.(r5),r0
movb	-2.(r0),r0
cmp	$056,r0
jne	L87
dec	-16.(r5)
mov	$012,_base
mov	$01750,_basem
L87:L86:clr	-12.(r5)
clr	-18.(r5)
L88:mov	-16.(r5),r0
sub	-18.(r5),r0
cmp	$03,r0
jge	L89
mov	4.(r5),r0
add	-18.(r5),r0
movb	(r0),r0
mov	r0,-14.(r5)
cmp	-14.(r5),$060
jlt	L91
cmp	-14.(r5),$071
jgt	L91
mov	-12.(r5),r1
mul	_base,r1
add	-14.(r5),r1
add	$0177720,r1
mov	r1,-12.(r5)
L91:L90:inc	-18.(r5)
jbr	L88
L89:clr	2.+-12.(r5)
L92:mov	-16.(r5),r0
sub	-18.(r5),r0
jle	L93
mov	4.(r5),r0
add	-18.(r5),r0
movb	(r0),r0
mov	r0,-14.(r5)
cmp	-14.(r5),$060
jlt	L95
cmp	-14.(r5),$071
jgt	L95
mov	2.+-12.(r5),r1
mul	_base,r1
add	-14.(r5),r1
add	$0177720,r1
mov	r1,2.+-12.(r5)
L95:L94:inc	-18.(r5)
jbr	L92
L93:tst	-20.(r5)
jeq	L96
mov	-12.(r5),r1
mul	_basem,r1
add	2.+-12.(r5),r1
mov	r1,-16.(r5)
clr	-12.(r5)
clr	2.+-12.(r5)
L97:mov	-16.(r5),r0
dec	-16.(r5)
tst	r0
jeq	L98
add	$01000,2.+-12.(r5)
L99:cmp	2.+-12.(r5),_basem
jlt	L100
sub	_basem,2.+-12.(r5)
inc	-12.(r5)
jbr	L99
L100:jbr	L97
L98:L96:clr	-16.(r5)
L101:mov	_addr,r0
inc	r0
cmp	-12.(r5),r0
jle	L102
add	$01000,2.+_addr
L103:cmp	2.+_addr,_basem
jlt	L104
sub	_basem,2.+_addr
inc	_addr
jbr	L103
L104:inc	-16.(r5)
jbr	L101
L102:mov	$03,(sp)
mov	-16.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L105:cmp	-12.(r5),_addr
jne	L10014
cmp	2.+-12.(r5),2.+_addr
jeq	L106
L10014:jsr	pc,_getc
cmp	$0177777,r0
jeq	L106
jbr	L105
L106:L81:jmp	rretrn
.globl	_putop
.text
_putop:
jsr	r5,rsave; 10
mov	4.(r5),(sp)
jsr	pc,*$_getop
mov	r0,-8.(r5)
clr	-10.(r5)
L108:mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
mov	r0,-12.(r5)
jeq	L109
mov	-12.(r5),(sp)
jsr	pc,*$_putchar
L110:inc	-10.(r5)
jbr	L108
L109:L111:cmp	-10.(r5),$04
jge	L112
mov	$040,(sp)
jsr	pc,*$_putchar
L113:inc	-10.(r5)
jbr	L111
L112:L107:jmp	rretrn
.globl	_getop
.text
_getop:
jsr	r5,rsave; 2
mov	4.(r5),r0
bic	$!0170000,r0
jbr	L116
L117:mov	4.(r5),r0
bic	$!0177000,r0
jbr	L119
L120:.data
L121:.byte 0152,0163,0162,0
.even
.text
mov	$L121,r0
jbr	L114
L122:.data
L123:.byte 0163,0157,0142,0
.even
.text
mov	$L123,r0
jbr	L114
jbr	L118
L119:mov	$L10016,r1
mov	r0,L10017
L10018:cmp	r0,(r1)+
jne	L10018
jmp	*L10017-L10016(r1)
.data
L10016:4000
77000
L10017:..
L120
L122
L124
.text
/esw
L118:L124:mov	4.(r5),r0
bic	$!0177400,r0
jbr	L126
L127:.data
L128:.byte 0142,0162,0
.even
.text
mov	$L128,r0
jbr	L114
L129:.data
L130:.byte 0142,0156,0145,0
.even
.text
mov	$L130,r0
jbr	L114
L131:.data
L132:.byte 0142,0145,0161,0
.even
.text
mov	$L132,r0
jbr	L114
L133:.data
L134:.byte 0142,0147,0145,0
.even
.text
mov	$L134,r0
jbr	L114
L135:.data
L136:.byte 0142,0154,0164,0
.even
.text
mov	$L136,r0
jbr	L114
L137:.data
L138:.byte 0142,0147,0164,0
.even
.text
mov	$L138,r0
jbr	L114
L139:.data
L140:.byte 0142,0154,0145,0
.even
.text
mov	$L140,r0
jbr	L114
jbr	L125
L126:mov	$L10020,r1
mov	r0,L10021
L10022:cmp	r0,(r1)+
jne	L10022
jmp	*L10021-L10020(r1)
.data
L10020:400
1000
1400
2000
2400
3000
3400
L10021:..
L127
L129
L131
L133
L135
L137
L139
L141
.text
/esw
L125:L141:mov	4.(r5),r0
bic	$!0177700,r0
jbr	L143
L144:.data
L145:.byte 0152,0155,0160,0
.even
.text
mov	$L145,r0
jbr	L114
L146:.data
L147:.byte 0163,0167,0141,0142,0
.even
.text
mov	$L147,r0
jbr	L114
L148:.data
L149:.byte 0143,0154,0162,0
.even
.text
mov	$L149,r0
jbr	L114
L150:.data
L151:.byte 0143,0157,0155,0
.even
.text
mov	$L151,r0
jbr	L114
L152:.data
L153:.byte 0151,0156,0143,0
.even
.text
mov	$L153,r0
jbr	L114
L154:.data
L155:.byte 0144,0145,0143,0
.even
.text
mov	$L155,r0
jbr	L114
L156:.data
L157:.byte 0156,0145,0147,0
.even
.text
mov	$L157,r0
jbr	L114
L158:.data
L159:.byte 0141,0144,0143,0
.even
.text
mov	$L159,r0
jbr	L114
L160:.data
L161:.byte 0163,0142,0143,0
.even
.text
mov	$L161,r0
jbr	L114
L162:.data
L163:.byte 0164,0163,0164,0
.even
.text
mov	$L163,r0
jbr	L114
L164:.data
L165:.byte 0162,0157,0162,0
.even
.text
mov	$L165,r0
jbr	L114
L166:.data
L167:.byte 0162,0157,0154,0
.even
.text
mov	$L167,r0
jbr	L114
L168:.data
L169:.byte 0141,0163,0162,0
.even
.text
mov	$L169,r0
jbr	L114
L170:.data
L171:.byte 0141,0163,0154,0
.even
.text
mov	$L171,r0
jbr	L114
L172:.data
L173:.byte 0155,0141,0162,0153,0
.even
.text
mov	$L173,r0
jbr	L114
L174:.data
L175:.byte 0155,0146,0160,0151,0
.even
.text
mov	$L175,r0
jbr	L114
L176:.data
L177:.byte 0155,0164,0160,0151,0
.even
.text
mov	$L177,r0
jbr	L114
L178:.data
L179:.byte 0163,0170,0164,0
.even
.text
mov	$L179,r0
jbr	L114
jbr	L142
L143:jsr	r2,hsw; 5; L10024
jmp	*L10030-L10025(r1)
.data
L10024:L10025
L10026
L10027
L10028
L10029
L10030
L10025:..
1000
1100
1200
1300
L10026:..
1063
1163
1263
L10027:..
46
1046
1146
1246
L10028:..
1031
1131
1231
L10029:..
14
1014
1114
1214
L10030:L180
L148
L158
L168
L178
L180
L156
L166
L176
L180
L146
L154
L164
L174
L180
L152
L162
L172
L180
L144
L150
L160
L170
.text
/esw
L142:L180:mov	4.(r5),r0
bic	$!0177740,r0
jbr	L182
L183:.data
L184:.byte 0146,0154,0141,0147,0
.even
.text
mov	$L184,r0
jbr	L114
jbr	L181
L182:mov	$L10032,r1
mov	r0,L10033
L10034:cmp	r0,(r1)+
jne	L10034
jmp	*L10033-L10032(r1)
.data
L10032:240
L10033:..
L183
L185
.text
/esw
L181:L185:mov	4.(r5),r0
bic	$!0177770,r0
jbr	L187
L188:.data
L189:.byte 0162,0164,0163,0
.even
.text
mov	$L189,r0
jbr	L114
L190:.data
L191:.byte 0163,0160,0154,0
.even
.text
mov	$L191,r0
jbr	L114
jbr	L186
L187:mov	$L10036,r1
mov	r0,L10037
L10038:cmp	r0,(r1)+
jne	L10038
jmp	*L10037-L10036(r1)
.data
L10036:200
230
L10037:..
L188
L190
L192
.text
/esw
L186:L192:mov	4.(r5),r0
bic	$!0177777,r0
jbr	L194
L195:.data
L196:.byte 0150,0141,0154,0164,0
.even
.text
mov	$L196,r0
jbr	L114
L197:.data
L198:.byte 0167,0141,0151,0164,0
.even
.text
mov	$L198,r0
jbr	L114
L199:.data
L200:.byte 0162,0164,0151,0
.even
.text
mov	$L200,r0
jbr	L114
L201:.data
L202:.byte 0142,0160,0164,0
.even
.text
mov	$L202,r0
jbr	L114
L203:.data
L204:.byte 0151,0157,0164,0
.even
.text
mov	$L204,r0
jbr	L114
L205:.data
L206:.byte 0162,0163,0145,0164,0
.even
.text
mov	$L206,r0
jbr	L114
L207:.data
L208:.byte 0162,0164,0164,0
.even
.text
mov	$L208,r0
jbr	L114
jbr	L193
L194:cmp	r0,$06
jhi	L209
asl	r0
jmp	*.+4(r0)
L195
L197
L199
L201
L203
L205
L207
/esw
L193:L209:jbr	L115
L210:.data
L211:.byte 0155,0157,0166,040,0
.even
.text
mov	$L211,r0
jbr	L114
L212:.data
L213:.byte 0143,0155,0160,0
.even
.text
mov	$L213,r0
jbr	L114
L214:.data
L215:.byte 0142,0151,0164,0
.even
.text
mov	$L215,r0
jbr	L114
L216:.data
L217:.byte 0142,0151,0143,0
.even
.text
mov	$L217,r0
jbr	L114
L218:.data
L219:.byte 0142,0151,0163,0
.even
.text
mov	$L219,r0
jbr	L114
L220:.data
L221:.byte 0141,0144,0144,0
.even
.text
mov	$L221,r0
jbr	L114
L222:mov	4.(r5),r0
bic	$!0177000,r0
jbr	L224
L225:.data
L226:.byte 0155,0165,0154,0
.even
.text
mov	$L226,r0
jbr	L114
L227:.data
L228:.byte 0144,0151,0166,0
.even
.text
mov	$L228,r0
jbr	L114
L229:.data
L230:.byte 0141,0163,0150,0
.even
.text
mov	$L230,r0
jbr	L114
L231:.data
L232:.byte 0141,0163,0150,0143,0
.even
.text
mov	$L232,r0
jbr	L114
L233:.data
L234:.byte 0170,0157,0162,0
.even
.text
mov	$L234,r0
jbr	L114
jbr	L223
L224:mov	$L10041,r1
mov	r0,L10042
L10043:cmp	r0,(r1)+
jne	L10043
jmp	*L10042-L10041(r1)
.data
L10041:70000
71000
72000
73000
74000
L10042:..
L225
L227
L229
L231
L233
L235
.text
/esw
L223:L235:jbr	L115
L236:mov	4.(r5),r0
bic	$!0177400,r0
jbr	L238
L239:.data
L240:.byte 0142,0160,0154,0
.even
.text
mov	$L240,r0
jbr	L114
L241:.data
L242:.byte 0142,0155,0151,0
.even
.text
mov	$L242,r0
jbr	L114
L243:.data
L244:.byte 0142,0150,0151,0
.even
.text
mov	$L244,r0
jbr	L114
L245:.data
L246:.byte 0142,0154,0157,0163,0
.even
.text
mov	$L246,r0
jbr	L114
L247:.data
L248:.byte 0142,0166,0143,0
.even
.text
mov	$L248,r0
jbr	L114
L249:.data
L250:.byte 0142,0166,0163,0
.even
.text
mov	$L250,r0
jbr	L114
L251:.data
L252:.byte 0142,0150,0151,0163,0
.even
.text
mov	$L252,r0
jbr	L114
L253:.data
L254:.byte 0142,0154,0157,0
.even
.text
mov	$L254,r0
jbr	L114
L255:.data
L256:.byte 0145,0155,0164,0
.even
.text
mov	$L256,r0
jbr	L114
L257:.data
L258:.byte 0163,0171,0163,0
.even
.text
mov	$L258,r0
jbr	L114
jbr	L237
L238:jsr	r2,hsw; 5; L10045
jmp	*L10051-L10046(r1)
.data
L10045:L10046
L10047
L10048
L10049
L10050
L10051
L10046:..
15000
15400
L10047:..
15063
15463
L10048:..
15146
15546
L10049:..
14631
15231
L10050:..
14714
15314
L10051:L259
L243
L253
L259
L245
L255
L259
L247
L257
L259
L239
L249
L259
L241
L251
.text
/esw
L237:L259:mov	4.(r5),r0
bic	$!0177700,r0
jbr	L261
L262:.data
L263:.byte 0143,0154,0162,0142,0
.even
.text
mov	$L263,r0
jbr	L114
L264:.data
L265:.byte 0143,0157,0155,0142,0
.even
.text
mov	$L265,r0
jbr	L114
L266:.data
L267:.byte 0151,0156,0143,0142,0
.even
.text
mov	$L267,r0
jbr	L114
L268:.data
L269:.byte 0144,0145,0143,0142,0
.even
.text
mov	$L269,r0
jbr	L114
L270:.data
L271:.byte 0156,0145,0147,0142,0
.even
.text
mov	$L271,r0
jbr	L114
L272:.data
L273:.byte 0141,0144,0143,0142,0
.even
.text
mov	$L273,r0
jbr	L114
L274:.data
L275:.byte 0163,0142,0143,0142,0
.even
.text
mov	$L275,r0
jbr	L114
L276:.data
L277:.byte 0164,0163,0164,0142,0
.even
.text
mov	$L277,r0
jbr	L114
L278:.data
L279:.byte 0162,0157,0162,0142,0
.even
.text
mov	$L279,r0
jbr	L114
L280:.data
L281:.byte 0162,0157,0154,0142,0
.even
.text
mov	$L281,r0
jbr	L114
L282:.data
L283:.byte 0141,0163,0162,0142,0
.even
.text
mov	$L283,r0
jbr	L114
L284:.data
L285:.byte 0141,0163,0154,0142,0
.even
.text
mov	$L285,r0
jbr	L114
L286:.data
L287:.byte 0155,0146,0160,0144,0
.even
.text
mov	$L287,r0
jbr	L114
L288:.data
L289:.byte 0155,0146,0160,0144,0
.even
.text
mov	$L289,r0
jbr	L114
jbr	L260
L261:jsr	r2,hsw; 3; L10053
jmp	*L10057-L10054(r1)
.data
L10053:L10054
L10055
L10056
L10057
L10054:..
27000
27100
27200
27300
L10055:..
27025
27125
27225
27325
27425
L10056:..
27052
27152
27252
27352
27452
L10057:L290
L262
L268
L274
L280
L290
L264
L270
L276
L282
L286
L290
L266
L272
L278
L284
L288
.text
/esw
L260:L290:jbr	L115
L291:.data
L292:.byte 0155,0157,0166,0142,0
.even
.text
mov	$L292,r0
jbr	L114
L293:.data
L294:.byte 0143,0155,0160,0142,0
.even
.text
mov	$L294,r0
jbr	L114
L295:.data
L296:.byte 0142,0151,0164,0142,0
.even
.text
mov	$L296,r0
jbr	L114
L297:.data
L298:.byte 0142,0151,0143,0142,0
.even
.text
mov	$L298,r0
jbr	L114
L299:.data
L300:.byte 0142,0151,0163,0142,0
.even
.text
mov	$L300,r0
jbr	L114
L301:.data
L302:.byte 0163,0165,0142,0
.even
.text
mov	$L302,r0
jbr	L114
L303:mov	4.(r5),r0
bic	$!0177000,r0
jbr	L305
L306:clr	r0
jbr	L304
L305:mov	$L10059,r1
mov	r0,L10060
L10061:cmp	r0,(r1)+
jne	L10061
jmp	*L10060-L10059(r1)
.data
L10059:0
L10060:..
L306
L307
.text
/esw
L304:L307:jbr	L115
jbr	L115
L116:jsr	r2,hsw; 5; L10063
jmp	*L10069-L10064(r1)
.data
L10063:L10064
L10065
L10066
L10067
L10068
L10069
L10064:..
20000
30000
0
10000
L10065:..
21463
1463
11463
L10066:..
23146
3146
13146
L10067:..
14631
24631
4631
L10068:..
16314
26314
6314
L10069:L308
L293
L303
L117
L218
L308
L295
L210
L220
L308
L297
L212
L222
L308
L236
L299
L214
L308
L291
L301
L216
.text
/esw
L115:L308:.data
L309:.byte 077,077,077,0
.even
.text
mov	$L309,r0
jbr	L114
L114:jmp	rretrn
