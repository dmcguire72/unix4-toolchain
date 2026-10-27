.globl	_dolp
.comm	_dolp,02
.globl	_dolv
.comm	_dolv,02
.globl	_dolc
.comm	_dolc,02
.globl	_promp
.comm	_promp,02
.globl	_linep
.comm	_linep,02
.globl	_elinep
.comm	_elinep,02
.globl	_argp
.comm	_argp,02
.globl	_eargp
.comm	_eargp,02
.globl	_treep
.comm	_treep,02
.globl	_peekc
.comm	_peekc,02
.globl	_gflg
.comm	_gflg,02
.globl	_error
.comm	_error,02
.globl	_acctf
.comm	_acctf,02
.globl	_uid
.comm	_uid,02
.globl	_setintr
.comm	_setintr,02
.globl	_mesg
.data
_mesg=.
0.
.text
L1:.byte 0110,0141,0156,0147,0165,0160,0
.even
.data
L1
0.
.text
L2:.byte 0121,0165,0151,0164,0
.even
.data
L2
.text
L3:.byte 0111,0154,0154,0145,0147,0141,0154,040,0151,0156,0163,0164,0162,0165,0143,0164,0151,0157,0156,0
.even
.data
L3
.text
L4:.byte 0124,0162,0141,0143,0145,057,0102,0120,0124,040,0164,0162,0141,0160,0
.even
.data
L4
.text
L5:.byte 0111,0117,0124,040,0164,0162,0141,0160,0
.even
.data
L5
.text
L6:.byte 0105,0115,0124,040,0164,0162,0141,0160,0
.even
.data
L6
.text
L7:.byte 0106,0154,0157,0141,0164,0151,0156,0147,040,0145,0170,0143,0145,0160,0164,0151,0157,0156,0
.even
.data
L7
.text
L8:.byte 0113,0151,0154,0154,0145,0144,0
.even
.data
L8
.text
L9:.byte 0102,0165,0163,040,0145,0162,0162,0157,0162,0
.even
.data
L9
.text
L10:.byte 0115,0145,0155,0157,0162,0171,040,0146,0141,0165,0154,0164,0
.even
.data
L10
.text
L11:.byte 0102,0141,0144,040,0163,0171,0163,0164,0145,0155,040,0143,0141,0154,0154,0
.even
.data
L11
.globl	_timeb
.comm	_timeb,014
.globl	_main
.text
_main:
jsr	r5,rsave; 2
mov	$02,(sp)
jsr	pc,*$_close
mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,r4
cmp	$02,r0
jeq	L13
mov	r4,(sp)
jsr	pc,*$_close
L13:mov	6.(r5),r2
.data
L14:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0163,0150,0137,0141,0143,0143,0164,0
.even
.text
mov	$L14,r3
.data
L15:.byte 045,040,0
.even
.text
mov	$L15,_promp
jsr	pc,_getuid
movb	r0,_uid
bit	$0377,r0
jne	L16
.data
L17:.byte 043,040,0
.even
.text
mov	$L17,_promp
.data
L18:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0163,0165,0137,0141,0143,0143,0164,0
.even
.text
mov	$L18,r3
L16:mov	$01,(sp)
mov	r3,-(sp)
jsr	pc,*$_open
tst	(sp)+
movb	r0,_acctf
cmp	4.(r5),$01
jle	L19
clr	_promp
clr	(sp)
jsr	pc,*$_close
clr	(sp)
mov	2.(r2),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,r4
tst	r4
jge	L20
mov	2.(r2),(sp)
jsr	pc,*$_prs
.data
L21:.byte 072,040,0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,0
.even
.text
mov	$L21,(sp)
jsr	pc,*$_err
L20:L19:cmpb	*(r2),$055
jne	L22
incb	_setintr
mov	$01,(sp)
mov	$03,-(sp)
jsr	pc,*$_signal
tst	(sp)+
mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L22:mov	r2,r0
add	$02,r0
mov	r0,_dolv
mov	4.(r5),r0
add	$0177777,r0
mov	r0,_dolc
L23:tst	_promp
jeq	L24
mov	_promp,(sp)
jsr	pc,*$_prs
L24:jsr	pc,_getc
movb	r0,_peekc
jsr	pc,_main1
jbr	L23
L12:jmp	rretrn
.globl	_main1
.text
_main1:
jsr	r5,rsave; 2262
mov	r5,r0
add	$0175656,r0
mov	r0,_argp
mov	r5,r0
add	$0176010,r0
mov	r0,_eargp
mov	r5,r0
add	$0176022,r0
mov	r0,_linep
mov	r5,r0
add	$0177765,r0
mov	r0,_elinep
clrb	_error
clrb	_gflg
L28:mov	_linep,r3
jsr	pc,_word
L26:cmpb	(r3),$012
jne	L28
L27:mov	r5,r0
add	$0175512,r0
mov	r0,_treep
tstb	_gflg
jne	L29
tstb	_error
jne	L30
mov	_argp,(sp)
mov	r5,r0
add	$0175656,r0
mov	r0,-(sp)
jsr	pc,*$_syntax
tst	(sp)+
mov	r0,r2
L30:tstb	_error
jeq	L31
.data
L32:.byte 0163,0171,0156,0164,0141,0170,040,0145,0162,0162,0157,0162,0
.even
.text
mov	$L32,(sp)
jsr	pc,*$_err
jbr	L33
L31:mov	r2,(sp)
jsr	pc,*$_execute
L33:L29:L25:jmp	rretrn
.globl	_word
.text
_word:
jsr	r5,rsave; 2
mov	_argp,r0
add	$02,_argp
mov	_linep,(r0)
L35:jsr	pc,_getc
movb	r0,r4
jbr	L37
L38:L39:jbr	L35
L40:L41:movb	r4,r3
L42:jsr	pc,_readc
movb	r0,r4
movb	r3,r1
cmp	r0,r1
jeq	L43
cmpb	r4,$012
jne	L44
incb	_error
movb	r4,_peekc
jbr	L34
L44:mov	_linep,r0
inc	_linep
movb	r4,r1
bis	$0200,r1
movb	r1,(r0)
jbr	L42
L43:jbr	L45
L46:L47:L48:L49:L50:L51:L52:L53:L54:mov	_linep,r0
inc	_linep
movb	r4,(r0)
mov	_linep,r0
inc	_linep
clrb	(r0)
jbr	L34
jbr	L36
L37:jsr	r2,hsw; 3; L10001
jmp	*L10005-L10002(r1)
.data
L10001:L10002
L10003
L10004
L10005
L10002:..
3
15
24
L10003:..
3
13
15
37
51
L10004:..
12
14
15
23
24
L10005:L55
L39
L40
L48
L55
L54
L41
L50
L53
L52
L55
L38
L46
L51
L47
L49
.text
/esw
L36:L55:movb	r4,_peekc
L45:L56:jsr	pc,_getc
movb	r0,r4
.data
L58:.byte 040,047,042,011,073,046,074,076,050,051,0174,0136,012,0
.even
.text
mov	$L58,(sp)
movb	r4,r0
mov	r0,-(sp)
jsr	pc,*$_any
tst	(sp)+
tst	r0
jeq	L59
movb	r4,_peekc
.data
L60:.byte 042,047,0
.even
.text
mov	$L60,(sp)
movb	r4,r0
mov	r0,-(sp)
jsr	pc,*$_any
tst	(sp)+
tst	r0
jne	L35
mov	_linep,r0
inc	_linep
clrb	(r0)
jbr	L34
L59:mov	_linep,r0
inc	_linep
movb	r4,(r0)
jbr	L56
L57:L34:jmp	rretrn
.globl	_tree
.text
_tree:
jsr	r5,rsave; 2
mov	_treep,r4
mov	4.(r5),r0
asl	r0
add	r0,_treep
mov	r4,r0
jbr	L61
L61:jmp	rretrn
.globl	_getc
.text
_getc:
jsr	r5,rsave; 2
tstb	_peekc
jeq	L63
movb	_peekc,r4
clrb	_peekc
movb	r4,r0
jbr	L62
L63:cmp	_argp,_eargp
jlos	L64
sub	$024,_argp
L65:jsr	pc,_getc
movb	r0,r4
cmp	$012,r0
jeq	L66
jbr	L65
L66:add	$024,_argp
.data
L67:.byte 0124,0157,0157,040,0155,0141,0156,0171,040,0141,0162,0147,0163,0
.even
.text
mov	$L67,(sp)
jsr	pc,*$_err
incb	_gflg
movb	r4,r0
jbr	L62
L64:cmp	_linep,_elinep
jlos	L68
sub	$012,_linep
L69:jsr	pc,_getc
movb	r0,r4
cmp	$012,r0
jeq	L70
jbr	L69
L70:add	$012,_linep
.data
L71:.byte 0124,0157,0157,040,0155,0141,0156,0171,040,0143,0150,0141,0162,0141,0143,0164,0145,0162,0163,0
.even
.text
mov	$L71,(sp)
jsr	pc,*$_err
incb	_gflg
movb	r4,r0
jbr	L62
L68:L72:tst	_dolp
jeq	L73
mov	_dolp,r0
inc	_dolp
movb	(r0),r4
tstb	r4
jeq	L74
movb	r4,r0
jbr	L62
L74:clr	_dolp
L73:jsr	pc,_readc
movb	r0,r4
cmpb	r4,$0134
jne	L75
jsr	pc,_readc
movb	r0,r4
cmpb	r4,$012
jne	L76
mov	$040,r0
jbr	L62
L76:movb	r4,r0
bis	$0200,r0
jbr	L62
L75:cmpb	r4,$044
jne	L77
jsr	pc,_getc
movb	r0,r4
cmpb	r4,$060
jlt	L78
cmpb	r4,$071
jgt	L78
movb	r4,r0
add	$0177720,r0
cmp	_dolc,r0
jle	L79
movb	r4,r0
asl	r0
add	_dolv,r0
mov	-96.(r0),_dolp
L79:jbr	L72
L78:L77:movb	r4,r0
bic	$!0177,r0
jbr	L62
L62:jmp	rretrn
.globl	_readc
.text
_readc:
jsr	r5,rsave; 4
mov	$01,(sp)
mov	r5,r0
add	$0177770,r0
mov	r0,-(sp)
clr	-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01,r0
jeq	L81
jsr	pc,_exit
L81:mov	-8.(r5),r0
bic	$!0377,r0
jbr	L80
L80:jmp	rretrn
.globl	_syntax
.text
_syntax:
jsr	r5,rsave; 2
L83:cmp	4.(r5),6.(r5)
jeq	L84
.data
L85:.byte 073,046,012,0
.even
.text
mov	$L85,(sp)
mov	4.(r5),r0
movb	*(r0),r0
mov	r0,-(sp)
jsr	pc,*$_any
tst	(sp)+
tst	r0
jeq	L86
add	$02,4.(r5)
jbr	L87
L86:mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_syn1
tst	(sp)+
jbr	L82
L87:jbr	L83
L84:clr	r0
jbr	L82
L82:jmp	rretrn
.globl	_syn1
.text
_syn1:
jsr	r5,rsave; 4
clr	-8.(r5)
mov	4.(r5),r4
L89:cmp	r4,6.(r5)
jeq	L90
movb	*(r4),r0
jbr	L93
L94:inc	-8.(r5)
jbr	L91
L95:dec	-8.(r5)
tst	-8.(r5)
jge	L96
incb	_error
L96:jbr	L91
L97:L98:L99:tst	-8.(r5)
jne	L100
mov	$04,(sp)
jsr	pc,*$_tree
mov	r0,r3
mov	$04,(r3)
mov	r4,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_syn2
tst	(sp)+
mov	r0,2.(r3)
clr	6.(r3)
cmpb	*(r4),$046
jne	L101
mov	2.(r3),r2
bis	$041,6.(r2)
L101:mov	6.(r5),(sp)
mov	r4,-(sp)
add	$02,(sp)
jsr	pc,*$_syntax
tst	(sp)+
mov	r0,4.(r3)
mov	r3,r0
jbr	L88
L100:jbr	L92
L93:mov	$L10007,r1
mov	r0,L10008
L10009:cmp	r0,(r1)+
jne	L10009
jmp	*L10008-L10007(r1)
.data
L10007:12
46
50
51
73
L10008:..
L99
L97
L94
L95
L98
L102
.text
/esw
L92:L102:L91:add	$02,r4
jbr	L89
L90:tst	-8.(r5)
jne	L103
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_syn2
tst	(sp)+
jbr	L88
L103:incb	_error
L88:jmp	rretrn
.globl	_syn2
.text
_syn2:
jsr	r5,rsave; 10
clr	-10.(r5)
mov	4.(r5),-8.(r5)
L105:cmp	-8.(r5),6.(r5)
jeq	L106
mov	-8.(r5),r0
movb	*(r0),r0
jbr	L109
L110:inc	-10.(r5)
jbr	L107
L111:dec	-10.(r5)
jbr	L107
L112:L113:tst	-10.(r5)
jne	L114
mov	$04,(sp)
jsr	pc,*$_tree
mov	r0,-12.(r5)
mov	$03,*-12.(r5)
mov	-8.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_syn3
tst	(sp)+
mov	-12.(r5),r1
mov	r0,2.(r1)
mov	6.(r5),(sp)
mov	-8.(r5),-(sp)
add	$02,(sp)
jsr	pc,*$_syn2
tst	(sp)+
mov	-12.(r5),r1
mov	r0,4.(r1)
mov	-12.(r5),r0
clr	6.(r0)
mov	-12.(r5),r0
jbr	L104
L114:jbr	L108
L109:mov	$L10011,r1
mov	r0,L10012
L10013:cmp	r0,(r1)+
jne	L10013
jmp	*L10012-L10011(r1)
.data
L10011:50
51
136
174
L10012:..
L110
L111
L113
L112
L115
.text
/esw
L108:L115:L107:add	$02,-8.(r5)
jbr	L105
L106:mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_syn3
tst	(sp)+
jbr	L104
L104:jmp	rretrn
.globl	_syn3
.text
_syn3:
jsr	r5,rsave; 332
clr	-222.(r5)
mov	6.(r5),r0
movb	*(r0),r0
cmp	$051,r0
jne	L117
bis	$020,-222.(r5)
L117:clr	-8.(r5)
clr	-10.(r5)
clr	-216.(r5)
clr	-218.(r5)
clr	-212.(r5)
clr	-214.(r5)
mov	4.(r5),r4
L118:cmp	r4,6.(r5)
jeq	L119
movb	*(r4),r0
mov	r0,-220.(r5)
jbr	L122
L123:tst	-214.(r5)
jne	L124
tst	-8.(r5)
jeq	L125
incb	_error
L125:mov	r4,r0
add	$02,r0
mov	r0,-8.(r5)
L124:inc	-214.(r5)
jbr	L120
L126:dec	-214.(r5)
tst	-214.(r5)
jne	L127
mov	r4,-10.(r5)
L127:jbr	L120
L128:add	$02,r4
cmp	r4,6.(r5)
jeq	L129
cmpb	*(r4),$076
jne	L129
bis	$02,-222.(r5)
jbr	L130
L129:sub	$02,r4
L130:L131:tst	-214.(r5)
jne	L132
add	$02,r4
cmp	r4,6.(r5)
jne	L133
incb	_error
sub	$02,r4
L133:.data
L134:.byte 074,076,050,0
.even
.text
mov	$L134,(sp)
mov	r4,r0
movb	*(r0),r0
mov	r0,-(sp)
jsr	pc,*$_any
tst	(sp)+
tst	r0
jeq	L135
incb	_error
L135:cmp	-220.(r5),$074
jne	L136
tst	-216.(r5)
jeq	L137
incb	_error
L137:mov	(r4),-216.(r5)
jbr	L120
L136:tst	-218.(r5)
jeq	L138
incb	_error
L138:mov	(r4),-218.(r5)
L132:jbr	L120
L139:tst	-214.(r5)
jne	L140
mov	-212.(r5),r0
inc	-212.(r5)
asl	r0
add	r5,r0
mov	(r4),-210.(r0)
L140:jbr	L121
L122:mov	$L10015,r1
mov	r0,L10016
L10017:cmp	r0,(r1)+
jne	L10017
jmp	*L10016-L10015(r1)
.data
L10015:50
51
74
76
L10016:..
L123
L126
L131
L128
L139
.text
/esw
L121:L120:add	$02,r4
jbr	L118
L119:tst	-8.(r5)
jeq	L141
tst	-212.(r5)
jeq	L142
incb	_error
L142:mov	$05,(sp)
jsr	pc,*$_tree
mov	r0,r3
mov	$02,(r3)
mov	-10.(r5),(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_syn1
tst	(sp)+
mov	r0,8.(r3)
jbr	L143
L141:tst	-212.(r5)
jne	L144
incb	_error
L144:mov	-212.(r5),r0
inc	-212.(r5)
asl	r0
add	r5,r0
clr	-210.(r0)
mov	-212.(r5),(sp)
add	$05,(sp)
jsr	pc,*$_tree
mov	r0,r3
mov	$01,(r3)
clr	-214.(r5)
L145:cmp	-214.(r5),-212.(r5)
jge	L146
mov	-214.(r5),r0
asl	r0
add	r3,r0
mov	-214.(r5),r1
asl	r1
add	r5,r1
mov	-210.(r1),10.(r0)
L147:inc	-214.(r5)
jbr	L145
L146:L143:mov	-222.(r5),6.(r3)
mov	-216.(r5),2.(r3)
mov	-218.(r5),4.(r3)
mov	r3,r0
jbr	L116
L116:jmp	rretrn
.globl	_scan
.text
_scan:
jsr	r5,rsave; 2
add	$012,4.(r5)
L149:mov	4.(r5),r0
add	$02,4.(r5)
mov	(r0),r4
jeq	L150
L151:movb	(r4),r3
jeq	L152
movb	r3,r0
mov	r0,-(sp)
jsr	pc,*6.(r5)
tst	(sp)+
movb	r0,(r4)+
jbr	L151
L152:jbr	L149
L150:L148:jmp	rretrn
.globl	_tglob
.text
_tglob:
jsr	r5,rsave; 2
.data
L154:.byte 0133,077,052,0
.even
.text
mov	$L154,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_any
tst	(sp)+
tst	r0
jeq	L155
movb	$01,_gflg
L155:mov	4.(r5),r0
jbr	L153
L153:jmp	rretrn
.globl	_trim
.text
_trim:
jsr	r5,rsave; 2
mov	4.(r5),r0
bic	$!0177,r0
jbr	L156
L156:jmp	rretrn
.globl	_execute
.text
_execute:
jsr	r5,rsave; 12
tst	4.(r5)
jeq	L158
mov	*4.(r5),r0
jbr	L160
L161:mov	4.(r5),r0
mov	10.(r0),r3
.data
L162:.byte 0143,0150,0144,0151,0162,0
.even
.text
mov	$L162,(sp)
mov	r3,-(sp)
jsr	pc,*$_equal
tst	(sp)+
tst	r0
jeq	L163
mov	4.(r5),r0
tst	12.(r0)
jeq	L164
mov	4.(r5),r0
mov	12.(r0),(sp)
jsr	pc,*$_chdir
tst	r0
jge	L165
.data
L166:.byte 0143,0150,0144,0151,0162,072,040,0142,0141,0144,040,0144,0151,0162,0145,0143,0164,0157,0162,0171,0
.even
.text
mov	$L166,(sp)
jsr	pc,*$_err
L165:jbr	L167
L164:.data
L168:.byte 0143,0150,0144,0151,0162,072,040,0141,0162,0147,040,0143,0157,0165,0156,0164,0
.even
.text
mov	$L168,(sp)
jsr	pc,*$_err
L167:jbr	L157
L163:.data
L169:.byte 0163,0150,0151,0146,0164,0
.even
.text
mov	$L169,(sp)
mov	r3,-(sp)
jsr	pc,*$_equal
tst	(sp)+
tst	r0
jeq	L170
cmp	_dolc,$01
jge	L171
.data
L172:.byte 0163,0150,0151,0146,0164,072,040,0156,0157,040,0141,0162,0147,0163,012,0
.even
.text
mov	$L172,(sp)
jsr	pc,*$_prs
jbr	L157
L171:mov	_dolv,r0
mov	*_dolv,2.(r0)
add	$02,_dolv
dec	_dolc
jbr	L157
L170:.data
L173:.byte 0154,0157,0147,0151,0156,0
.even
.text
mov	$L173,(sp)
mov	r3,-(sp)
jsr	pc,*$_equal
tst	(sp)+
tst	r0
jeq	L174
tst	_promp
jeq	L175
movb	_acctf,r0
mov	r0,(sp)
jsr	pc,*$_close
.data
L176:.byte 057,0142,0151,0156,057,0154,0157,0147,0151,0156,0
.even
.text
mov	4.(r5),(sp)
add	$012,(sp)
mov	$L176,-(sp)
jsr	pc,*$_execv
tst	(sp)+
L175:.data
L177:.byte 0154,0157,0147,0151,0156,072,040,0143,0141,0156,0156,0157,0164,040,0145,0170,0145,0143,0165,0164,0145,012,0
.even
.text
mov	$L177,(sp)
jsr	pc,*$_prs
jbr	L157
L174:.data
L178:.byte 0167,0141,0151,0164,0
.even
.text
mov	$L178,(sp)
mov	r3,-(sp)
jsr	pc,*$_equal
tst	(sp)+
tst	r0
jeq	L179
clr	(sp)
mov	$0177777,-(sp)
jsr	pc,*$_pwait
tst	(sp)+
jbr	L157
L179:.data
L180:.byte 072,0
.even
.text
mov	$L180,(sp)
mov	r3,-(sp)
jsr	pc,*$_equal
tst	(sp)+
tst	r0
jne	L157
L181:mov	4.(r5),r0
mov	6.(r0),-10.(r5)
clr	-8.(r5)
bit	$020,-10.(r5)
jne	L182
jsr	pc,_fork
mov	r0,-8.(r5)
L182:cmp	-8.(r5),$0177777
jne	L183
.data
L184:.byte 0164,0162,0171,040,0141,0147,0141,0151,0156,0
.even
.text
mov	$L184,(sp)
jsr	pc,*$_err
jbr	L157
L183:tst	-8.(r5)
jeq	L185
bit	$04,-10.(r5)
jeq	L186
mov	*6.(r5),(sp)
jsr	pc,*$_close
mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_close
L186:bit	$01,-10.(r5)
jeq	L187
mov	-8.(r5),(sp)
jsr	pc,*$_prn
.data
L188:.byte 012,0
.even
.text
mov	$L188,(sp)
jsr	pc,*$_prs
jbr	L157
L187:bit	$010,-10.(r5)
jne	L189
mov	4.(r5),(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_pwait
tst	(sp)+
L189:jbr	L157
L185:mov	4.(r5),r0
tst	2.(r0)
jeq	L190
clr	(sp)
jsr	pc,*$_close
clr	(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jge	L191
mov	4.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_prs
.data
L192:.byte 072,040,0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,0
.even
.text
mov	$L192,(sp)
jsr	pc,*$_err
jsr	pc,_exit
L191:L190:mov	4.(r5),r0
tst	4.(r0)
jeq	L193
bit	$02,-10.(r5)
jeq	L194
mov	$01,(sp)
mov	4.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jlt	L195
mov	$02,(sp)
clr	-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
jbr	L196
L195:L194:mov	$0666,(sp)
mov	4.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jge	L197
mov	4.(r5),r0
mov	4.(r0),(sp)
jsr	pc,*$_prs
.data
L198:.byte 072,040,0143,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,0145,0
.even
.text
mov	$L198,(sp)
jsr	pc,*$_err
jsr	pc,_exit
L197:L196:mov	$01,(sp)
jsr	pc,*$_close
mov	-8.(r5),(sp)
jsr	pc,*$_dup
mov	-8.(r5),(sp)
jsr	pc,*$_close
L193:bit	$04,-10.(r5)
jeq	L199
clr	(sp)
jsr	pc,*$_close
mov	*6.(r5),(sp)
jsr	pc,*$_dup
mov	*6.(r5),(sp)
jsr	pc,*$_close
mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_close
L199:bit	$010,-10.(r5)
jeq	L200
mov	$01,(sp)
jsr	pc,*$_close
mov	8.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_dup
mov	*8.(r5),(sp)
jsr	pc,*$_close
mov	8.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_close
L200:bit	$040,-10.(r5)
jeq	L201
mov	4.(r5),r0
tst	2.(r0)
jne	L201
bit	$04,-10.(r5)
jne	L201
clr	(sp)
jsr	pc,*$_close
.data
L202:.byte 057,0144,0145,0166,057,0156,0165,0154,0154,0
.even
.text
clr	(sp)
mov	$L202,-(sp)
jsr	pc,*$_open
tst	(sp)+
L201:bit	$040,-10.(r5)
jne	L203
tstb	_setintr
jeq	L203
clr	(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
clr	(sp)
mov	$03,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L203:cmp	*4.(r5),$02
jne	L204
mov	4.(r5),r0
mov	8.(r0),r4
jeq	L205
mov	-10.(r5),r0
bic	$!040,r0
bis	r0,6.(r4)
L205:mov	r4,(sp)
jsr	pc,*$_execute
jsr	pc,_exit
L204:movb	_acctf,r0
mov	r0,(sp)
jsr	pc,*$_close
clrb	_gflg
mov	$_tglob,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_scan
tst	(sp)+
tstb	_gflg
jeq	L206
.data
L207:.byte 057,0145,0164,0143,057,0147,0154,0157,0142,0
.even
.text
mov	4.(r5),r0
mov	$L207,8.(r0)
mov	4.(r5),(sp)
add	$010,(sp)
mov	4.(r5),r0
mov	8.(r0),-(sp)
jsr	pc,*$_execv
tst	(sp)+
.data
L208:.byte 0147,0154,0157,0142,072,040,0143,0141,0156,0156,0157,0164,040,0145,0170,0145,0143,0165,0164,0145,012,0
.even
.text
mov	$L208,(sp)
jsr	pc,*$_prs
jsr	pc,_exit
L206:mov	$_trim,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_scan
tst	(sp)+
mov	4.(r5),(sp)
add	$012,(sp)
mov	4.(r5),r0
mov	10.(r0),-(sp)
jsr	pc,*$_execv
tst	(sp)+
mov	_linep,r3
.data
L209:.byte 057,0165,0163,0162,057,0142,0151,0156,057,0
.even
.text
mov	$L209,r2
L210:movb	(r2)+,(r3)
jeq	L211
inc	r3
jbr	L210
L211:mov	4.(r5),r0
mov	10.(r0),r2
L212:movb	(r2)+,(r3)+
jeq	L213
jbr	L212
L213:mov	4.(r5),(sp)
add	$012,(sp)
mov	_linep,-(sp)
add	$04,(sp)
jsr	pc,*$_execv
tst	(sp)+
mov	4.(r5),(sp)
add	$012,(sp)
mov	_linep,-(sp)
jsr	pc,*$_execv
tst	(sp)+
jsr	pc,_runcom
tst	r0
jeq	L214
.data
L215:.byte 057,0142,0151,0156,057,0163,0150,0
.even
.text
mov	4.(r5),r0
mov	$L215,8.(r0)
mov	4.(r5),r0
mov	_linep,10.(r0)
mov	4.(r5),(sp)
add	$010,(sp)
mov	4.(r5),r0
mov	8.(r0),-(sp)
jsr	pc,*$_execv
tst	(sp)+
L214:mov	4.(r5),r0
mov	10.(r0),(sp)
jsr	pc,*$_prs
.data
L216:.byte 072,040,0156,0157,0164,040,0146,0157,0165,0156,0144,0
.even
.text
mov	$L216,(sp)
jsr	pc,*$_err
jsr	pc,_exit
L217:mov	4.(r5),r0
mov	6.(r0),-10.(r5)
mov	r5,r0
add	$0177762,r0
mov	r0,(sp)
jsr	pc,*$_pipe
mov	4.(r5),r0
mov	2.(r0),r4
mov	-10.(r5),r0
bic	$!044,r0
bis	$010,r0
bis	r0,6.(r4)
mov	r5,r0
add	$0177762,r0
mov	r0,(sp)
mov	6.(r5),-(sp)
mov	r4,-(sp)
jsr	pc,*$_execute
cmp	(sp)+,(sp)+
mov	4.(r5),r0
mov	4.(r0),r4
mov	-10.(r5),r0
bic	$!051,r0
bis	$04,r0
bis	r0,6.(r4)
mov	8.(r5),(sp)
mov	r5,r0
add	$0177762,r0
mov	r0,-(sp)
mov	r4,-(sp)
jsr	pc,*$_execute
cmp	(sp)+,(sp)+
jbr	L157
L218:mov	4.(r5),r0
mov	6.(r0),r0
bic	$!040,r0
mov	r0,-10.(r5)
mov	4.(r5),r0
mov	2.(r0),r4
jeq	L219
bis	-10.(r5),6.(r4)
L219:mov	r4,(sp)
jsr	pc,*$_execute
mov	4.(r5),r0
mov	4.(r0),r4
jeq	L220
bis	-10.(r5),6.(r4)
L220:mov	r4,(sp)
jsr	pc,*$_execute
jbr	L157
jbr	L159
L160:sub	$01,r0
cmp	r0,$03
jhi	L221
asl	r0
jmp	*.+4(r0)
L161
L181
L217
L218
/esw
L159:L221:L158:L157:jmp	rretrn
.globl	_err
.text
_err:
jsr	r5,rsave; 2
mov	4.(r5),(sp)
jsr	pc,*$_prs
.data
L223:.byte 012,0
.even
.text
mov	$L223,(sp)
jsr	pc,*$_prs
tst	_promp
jne	L224
mov	$02,(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
jsr	pc,_exit
L224:L222:jmp	rretrn
.globl	_prs
.text
_prs:
jsr	r5,rsave; 2
L226:tstb	*4.(r5)
jeq	L227
mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
mov	r0,(sp)
jsr	pc,*$_putc
jbr	L226
L227:L225:jmp	rretrn
.globl	_putc
.text
_putc:
jsr	r5,rsave; 2
mov	$01,(sp)
mov	r5,r0
add	$04,r0
mov	r0,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L228:jmp	rretrn
.globl	_prn
.text
_prn:
jsr	r5,rsave; 2
mov	$012,(sp)
mov	4.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
jeq	L230
mov	r4,(sp)
jsr	pc,*$_prn
L230:mov	$012,(sp)
mov	4.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_lrem
cmp	(sp)+,(sp)+
mov	r0,(sp)
add	$060,(sp)
jsr	pc,*$_putc
L229:jmp	rretrn
.globl	_any
.text
_any:
jsr	r5,rsave; 2
L232:tstb	*6.(r5)
jeq	L233
mov	6.(r5),r0
inc	6.(r5)
movb	(r0),r0
cmp	4.(r5),r0
jne	L234
mov	$01,r0
jbr	L231
L234:jbr	L232
L233:clr	r0
jbr	L231
L231:jmp	rretrn
.globl	_equal
.text
_equal:
jsr	r5,rsave; 2
L236:mov	4.(r5),r0
inc	4.(r5)
cmpb	(r0),*6.(r5)
jne	L237
mov	6.(r5),r0
inc	6.(r5)
tstb	(r0)
jne	L238
mov	$01,r0
jbr	L235
L238:jbr	L236
L237:clr	r0
jbr	L235
L235:jmp	rretrn
.globl	_pwait
.text
_pwait:
jsr	r5,rsave; 4
tst	4.(r5)
jeq	L240
L241:mov	$_timeb,(sp)
jsr	pc,*$_times
mov	$_timeb,(sp)
jsr	pc,*$_time
mov	r5,r0
add	$0177770,r0
mov	r0,(sp)
jsr	pc,*$_wait
mov	r0,r4
cmp	r4,$0177777
jeq	L242
mov	-8.(r5),r0
bic	$!0177,r0
mov	r0,r3
mov	r3,r0
asl	r0
tst	_mesg(r0)
jeq	L243
cmp	r4,4.(r5)
jeq	L244
mov	r4,(sp)
jsr	pc,*$_prn
.data
L245:.byte 072,040,0
.even
.text
mov	$L245,(sp)
jsr	pc,*$_prs
L244:mov	r3,r0
asl	r0
mov	_mesg(r0),(sp)
jsr	pc,*$_prs
bit	$0200,-8.(r5)
jeq	L246
.data
L247:.byte 040,055,055,040,0103,0157,0162,0145,040,0144,0165,0155,0160,0145,0144,0
.even
.text
mov	$L247,(sp)
jsr	pc,*$_prs
L246:L243:tst	r3
jeq	L248
.data
L249:.byte 0
.even
.text
mov	$L249,(sp)
jsr	pc,*$_err
L248:cmp	4.(r5),r4
jne	L250
mov	6.(r5),(sp)
jsr	pc,*$_acct
jbr	L242
jbr	L251
L250:clr	(sp)
jsr	pc,*$_acct
L251:jbr	L241
L242:L240:L239:jmp	rretrn
.globl	_acct
.text
_acct:
jsr	r5,rsave; 2
tst	4.(r5)
jne	L253
.data
L254:.byte 052,052,0147,0157,0153,0
.even
.text
mov	$L254,(sp)
jsr	pc,*$_enacct
jbr	L255
L253:cmp	*4.(r5),$02
jne	L256
.data
L257:.byte 050,051,0
.even
.text
mov	$L257,(sp)
jsr	pc,*$_enacct
jbr	L258
L256:mov	4.(r5),r0
mov	10.(r0),(sp)
jsr	pc,*$_enacct
L258:L255:L252:jmp	rretrn
.globl	_enacct
.text
_enacct:
jsr	r5,rsave; 42
mov	r5,r0
add	$0177756,r0
mov	r0,(sp)
jsr	pc,*$_times
mov	r5,r0
add	$0177756,r0
mov	r0,(sp)
jsr	pc,*$_time
mov	2.+-18.(r5),r0
sub	2.+_timeb,r0
mov	r0,14.+-38.(r5)
mov	6.+-18.(r5),r0
sub	6.+_timeb,r0
mov	r0,16.+-38.(r5)
mov	10.+-18.(r5),r0
sub	10.+_timeb,r0
mov	r0,18.+-38.(r5)
L262:mov	4.(r5),r3
L263:tstb	*4.(r5)
jeq	L264
cmpb	*4.(r5),$057
jeq	L264
inc	4.(r5)
jbr	L263
L264:L260:mov	4.(r5),r0
inc	4.(r5)
tstb	(r0)
jne	L262
L261:clr	r4
L265:cmp	r4,$015
jge	L266
mov	r4,r0
add	r5,r0
movb	(r3),-38.(r0)
tstb	(r3)
jeq	L268
inc	r3
L268:L267:inc	r4
jbr	L265
L266:movb	_uid,13.+-38.(r5)
mov	$02,(sp)
clr	-(sp)
movb	_acctf,r0
mov	r0,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$024,(sp)
mov	r5,r0
add	$0177732,r0
mov	r0,-(sp)
movb	_acctf,r0
mov	r0,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L259:jmp	rretrn
.globl	_runcom
.text
_runcom:
jsr	r5,rsave; 122
mov	r5,r0
add	$0177652,r0
mov	r0,(sp)
mov	_linep,-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jge	L270
clr	r0
jbr	L269
L270:bit	$060111,4.+-86.(r5)
jeq	L271
clr	r0
jbr	L269
L271:mov	$01,r0
jbr	L269
L269:jmp	rretrn
