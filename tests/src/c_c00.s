.globl	_cvtab
.comm	_cvtab,02
.globl	_opdope
.comm	_opdope,02
.globl	_ctab
.comm	_ctab,02
.globl	_symbuf
.comm	_symbuf,010
.globl	_hshused
.comm	_hshused,02
.globl	_hshtab
.comm	_hshtab,06200
.globl	_space
.comm	_space,02
.globl	_cp
.comm	_cp,02
.globl	_cmst
.comm	_cmst,0120
.globl	_isn
.comm	_isn,02
.globl	_swtab
.comm	_swtab,01440
.globl	_swp
.comm	_swp,02
.globl	_contlab
.comm	_contlab,02
.globl	_brklab
.comm	_brklab,02
.globl	_retlab
.comm	_retlab,02
.globl	_deflab
.comm	_deflab,02
.globl	_nauto
.comm	_nauto,02
.globl	_autolen
.comm	_autolen,02
.globl	_peeksym
.comm	_peeksym,02
.globl	_peekc
.comm	_peekc,02
.globl	_eof
.comm	_eof,02
.globl	_line
.comm	_line,02
.globl	_treebase
.comm	_treebase,02
.globl	_debug
.comm	_debug,02
.globl	_defsym
.comm	_defsym,02
.globl	_funcsym
.comm	_funcsym,02
.globl	_xdflg
.comm	_xdflg,02
.globl	_proflg
.comm	_proflg,02
.globl	_csym
.comm	_csym,02
.globl	_cval
.comm	_cval,02
.globl	_fcval
.comm	_fcval,010
.globl	_nchstr
.comm	_nchstr,02
.globl	_nerror
.comm	_nerror,02
.globl	_paraml
.comm	_paraml,02
.globl	_parame
.comm	_parame,02
.globl	_strflg
.comm	_strflg,02
.globl	_osleft
.comm	_osleft,02
.globl	_mosflg
.comm	_mosflg,02
.globl	_initflg
.comm	_initflg,02
.globl	_inhdr
.comm	_inhdr,02
.globl	_dimtab
.comm	_dimtab,0310
.globl	_binbuf
.comm	_binbuf,01006
.globl	_ascbuf
.comm	_ascbuf,01006
.globl	_dimp
.comm	_dimp,02
.globl	_regvar
.comm	_regvar,02
.globl	_isn
.data
_isn=.
1.
.globl	_peeksym
.data
_peeksym=.
-1.
.globl	_line
.data
_line=.
1.
.globl	_debug
.data
_debug=.
0.
.globl	_dimp
.data
_dimp=.
0.
.globl	_kwtab
.data
_kwtab=.
.text
L1:.byte 0151,0156,0164,0
.even
.data
L1
0.
.text
L2:.byte 0143,0150,0141,0162,0
.even
.data
L2
1.
.text
L3:.byte 0146,0154,0157,0141,0164,0
.even
.data
L3
2.
.text
L4:.byte 0144,0157,0165,0142,0154,0145,0
.even
.data
L4
3.
.text
L5:.byte 0163,0164,0162,0165,0143,0164,0
.even
.data
L5
4.
.text
L6:.byte 0141,0165,0164,0157,0
.even
.data
L6
5.
.text
L7:.byte 0145,0170,0164,0145,0162,0156,0
.even
.data
L7
6.
.text
L8:.byte 0163,0164,0141,0164,0151,0143,0
.even
.data
L8
7.
.text
L9:.byte 0162,0145,0147,0151,0163,0164,0145,0162,0
.even
.data
L9
8.
.text
L10:.byte 0147,0157,0164,0157,0
.even
.data
L10
10.
.text
L11:.byte 0162,0145,0164,0165,0162,0156,0
.even
.data
L11
11.
.text
L12:.byte 0151,0146,0
.even
.data
L12
12.
.text
L13:.byte 0167,0150,0151,0154,0145,0
.even
.data
L13
13.
.text
L14:.byte 0145,0154,0163,0145,0
.even
.data
L14
14.
.text
L15:.byte 0163,0167,0151,0164,0143,0150,0
.even
.data
L15
15.
.text
L16:.byte 0143,0141,0163,0145,0
.even
.data
L16
16.
.text
L17:.byte 0142,0162,0145,0141,0153,0
.even
.data
L17
17.
.text
L18:.byte 0143,0157,0156,0164,0151,0156,0165,0145,0
.even
.data
L18
18.
.text
L19:.byte 0144,0157,0
.even
.data
L19
19.
.text
L20:.byte 0144,0145,0146,0141,0165,0154,0164,0
.even
.data
L20
20.
.text
L21:.byte 0146,0157,0162,0
.even
.data
L21
21.
.text
L22:.byte 0163,0151,0172,0145,0157,0146,0
.even
.data
L22
29.
0.
0.
.globl	_main
.text
_main:
jsr	r5,rsave; 1752
cmp	4.(r5),$04
jge	L24
.data
L25:.byte 0101,0162,0147,040,0143,0157,0165,0156,0164,0
.even
.text
mov	$L25,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L24:clr	(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fin
jge	L26
.data
L27:.byte 0103,0141,0156,047,0164,040,0146,0151,0156,0144,040,045,0163,0
.even
.text
mov	6.(r5),r0
mov	2.(r0),(sp)
mov	$L27,-(sp)
jsr	pc,*$_error
tst	(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L26:mov	$_ascbuf,(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jlt	L10000
mov	$_binbuf,(sp)
mov	6.(r5),r0
mov	6.(r0),-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jge	L28
L10000:.data
L29:.byte 0103,0141,0156,047,0164,040,0143,0162,0145,0141,0164,0145,040,0164,0145,0155,0160,0
.even
.text
mov	$L29,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L28:cmp	4.(r5),$04
jle	L30
inc	_proflg
L30:inc	_xdflg
mov	$_kwtab,r2
L31:mov	(r2),r3
jeq	L32
mov	$_symbuf,r4
L34:cmp	r4,$8.+_symbuf
jhis	L35
movb	(r3)+,(r4)+
jne	L36
dec	r3
L36:jbr	L34
L35:jsr	pc,_lookup
mov	r0,r3
mov	$01,(r3)
mov	2.(r2),2.(r3)
L33:add	$04,r2
jbr	L31
L32:clr	_xdflg
mov	r5,r0
add	$0176046,r0
mov	r0,_treebase
mov	$_binbuf,(sp)
mov	_treebase,-(sp)
jsr	pc,*$_putw
tst	(sp)+
L37:tst	_eof
jne	L38
jsr	pc,_extdef
jsr	pc,_blkend
jbr	L37
L38:mov	$_ascbuf,(sp)
jsr	pc,*$_fflush
mov	$_binbuf,(sp)
jsr	pc,*$_fflush
tst	_nerror
jne	L10001
clr	(sp)
jbr	L10002
L10001:mov	$01,(sp)
L10002:jsr	pc,*$_exit
L23:jmp	rretrn
.globl	_lookup
.text
_lookup:
jsr	r5,rsave; 4
clr	-8.(r5)
mov	$_symbuf,r3
L40:cmp	r3,$8.+_symbuf
jhis	L41
movb	(r3)+,r0
bic	$!0177,r0
add	r0,-8.(r5)
jbr	L40
L41:mov	-8.(r5),r1
sxt	r0
div	$0310,r0
ash	$04,r1
add	$_hshtab,r1
mov	r1,r4
L42:mov	r4,r0
add	$010,r0
mov	r0,r2
tstb	(r0)
jeq	L43
mov	$_symbuf,r3
L44:cmp	r3,$8.+_symbuf
jhis	L45
movb	(r2)+,r0
bic	$!0177,r0
movb	(r3)+,r1
cmp	r0,r1
jne	L46
jbr	L44
L45:mov	r4,r0
jbr	L39
L46:add	$020,r4
mov	r4,r0
cmp	$3200.+_hshtab,r0
jhi	L47
mov	$_hshtab,r4
L47:jbr	L42
L43:inc	_hshused
mov	_hshused,r0
cmp	$0310,r0
jgt	L48
.data
L49:.byte 0123,0171,0155,0142,0157,0154,040,0164,0141,0142,0154,0145,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L49,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L48:clr	(r4)
clr	2.(r4)
clr	6.(r4)
clr	4.(r4)
mov	$_symbuf,r3
tst	_xdflg
jeq	L50
bisb	$0200,(r3)
L50:mov	r4,r0
add	$010,r0
mov	r0,r2
L51:cmp	r3,$8.+_symbuf
jhis	L52
movb	(r3)+,(r2)+
jbr	L51
L52:mov	r4,r0
jbr	L39
L39:jmp	rretrn
.globl	_symbol
.text
_symbol:
jsr	r5,rsave; 2
tst	_peeksym
jlt	L54
mov	_peeksym,r4
mov	$0177777,_peeksym
cmp	r4,$024
jne	L55
clr	_mosflg
L55:mov	r4,r0
jbr	L53
L54:tst	_peekc
jeq	L56
mov	_peekc,r4
clr	_peekc
jbr	L57
L56:tst	_eof
jeq	L58
clr	r0
jbr	L53
L58:jsr	pc,_getchar
mov	r0,r4
L57:L59:movb	_ctab(r4),r0
jbr	L61
L62:mov	$01,_inhdr
jsr	pc,_getchar
mov	r0,r4
jbr	L59
L63:tst	_inhdr
jne	L64
inc	_line
L64:clr	_inhdr
L65:jsr	pc,_getchar
mov	r0,r4
jbr	L59
L66:inc	_eof
clr	r0
jbr	L53
L67:mov	$036,(sp)
mov	$050,-(sp)
mov	r4,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
jbr	L53
L68:mov	$037,(sp)
mov	$062,-(sp)
mov	$051,-(sp)
mov	$076,-(sp)
jsr	pc,_subseq
add	$06,sp
mov	r0,-(sp)
mov	r4,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
jbr	L53
L69:mov	$01,(sp)
clr	-(sp)
mov	$040,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
tst	r0
jeq	L70
mov	$0120,r0
jbr	L53
L70:jsr	pc,_symbol
mov	r0,r4
cmp	r4,$050
jlt	L71
cmp	r4,$061
jgt	L71
tst	_peekc
jne	L72
jsr	pc,_getchar
mov	r0,_peekc
L72:mov	_peekc,r0
movb	_ctab(r0),r0
cmp	$0176,r0
jeq	L73
cmp	r4,$051
jeq	L10003
cmp	r4,$057
jeq	L10003
cmp	r4,$052
jne	L73
L10003:.data
L74:.byte 0127,0141,0162,0156,0151,0156,0147,072,040,0141,0163,0163,0151,0147,0156,0155,0145,0156,0164,040,0165,0156,0144,0145,0162,0163,0164,0157,0157,0144,0
.even
.text
mov	$L74,(sp)
jsr	pc,*$_error
dec	_nerror
L73:mov	r4,r0
add	$036,r0
jbr	L53
L71:cmp	r4,$0120
jne	L75
mov	$074,r0
jbr	L53
L75:mov	r4,_peeksym
mov	$0120,r0
jbr	L53
L76:mov	$01,(sp)
clr	-(sp)
mov	r4,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
tst	r0
jeq	L77
mov	$056,r0
jbr	L53
L77:mov	$076,(sp)
mov	$077,-(sp)
mov	$075,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
jbr	L53
L78:mov	$01,(sp)
clr	-(sp)
mov	r4,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
tst	r0
jeq	L79
mov	$055,r0
jbr	L53
L79:mov	$0100,(sp)
mov	$0101,-(sp)
mov	$075,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
jbr	L53
L80:mov	$075,(sp)
mov	$042,-(sp)
mov	$075,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
jbr	L53
L81:clr	(sp)
mov	$01,-(sp)
mov	$052,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
tst	r0
jeq	L82
mov	$053,r0
jbr	L53
L82:L83:jsr	pc,_getchar
mov	r0,r4
L84:mov	r4,r0
jbr	L86
L87:inc	_eof
.data
L88:.byte 0116,0157,0156,0164,0145,0162,0155,0151,0156,0141,0164,0145,0144,040,0143,0157,0155,0155,0145,0156,0164,0
.even
.text
mov	$L88,(sp)
jsr	pc,*$_error
clr	r0
jbr	L53
L89:tst	_inhdr
jne	L90
inc	_line
L90:clr	_inhdr
jbr	L83
L91:inc	_inhdr
L92:jbr	L83
L93:jsr	pc,_getchar
mov	r0,r4
cmp	r4,$057
jne	L84
jbr	L85
L86:mov	$L10005,r1
mov	r0,L10006
L10007:cmp	r0,(r1)+
jne	L10007
jmp	*L10006-L10005(r1)
.data
L10005:0
1
12
52
L10006:..
L87
L91
L89
L93
L92
.text
/esw
L85:jsr	pc,_getchar
mov	r0,r4
jbr	L59
L94:L95:mov	r4,_peekc
cmp	r4,$060
jne	L10008
mov	$010,(sp)
jbr	L10009
L10008:mov	$012,(sp)
L10009:jsr	pc,*$_getnum
mov	r0,r4
cmp	$027,r0
jne	L96
mov	_isn,r0
inc	_isn
mov	r0,_cval
L96:mov	r4,r0
jbr	L53
L97:jsr	pc,_getstr
jbr	L53
L98:jsr	pc,_getcc
jbr	L53
L99:mov	$_symbuf,r3
tst	_mosflg
jeq	L100
movb	$056,(r3)+
clr	_mosflg
L100:L101:cmpb	_ctab(r4),$0173
jeq	L10010
cmpb	_ctab(r4),$0174
jne	L102
L10010:cmp	r3,$8.+_symbuf
jhis	L103
movb	r4,(r3)+
L103:jsr	pc,_getchar
mov	r0,r4
jbr	L101
L102:L104:cmp	r3,$8.+_symbuf
jhis	L105
clrb	(r3)+
jbr	L104
L105:mov	r4,_peekc
jsr	pc,_lookup
mov	r0,_csym
cmp	*_csym,$01
jne	L106
mov	_csym,r0
cmp	2.(r0),$035
jne	L107
mov	$035,r0
jbr	L53
L107:mov	_csym,r0
mov	2.(r0),_cval
mov	$023,r0
jbr	L53
L106:mov	$024,r0
jbr	L53
L108:mov	$065,(sp)
mov	$057,-(sp)
mov	$046,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
jbr	L53
L109:mov	$066,(sp)
mov	$060,-(sp)
mov	$0174,-(sp)
jsr	pc,*$_subseq
cmp	(sp)+,(sp)+
jbr	L53
L110:.data
L111:.byte 0125,0156,0153,0156,0157,0167,0156,040,0143,0150,0141,0162,0141,0143,0164,0145,0162,0
.even
.text
mov	$L111,(sp)
jsr	pc,*$_error
jsr	pc,_getchar
mov	r0,r4
jbr	L59
jbr	L60
L61:jsr	r2,hsw; 4; L10012
jmp	*L10017-L10013(r1)
.data
L10012:L10013
L10014
L10015
L10016
L10017
L10013:..
0
12
14
24
36
37
L10014:..
12
20
36
37
L10015:..
10
36
37
L10016:..
12
13
17
35
36
37
L10017:L112
L66
L67
L109
L69
L94
L95
L112
L68
L78
L98
L63
L112
L80
L97
L65
L112
L81
L108
L76
L62
L99
L110
.text
/esw
L60:L112:movb	_ctab(r4),r0
jbr	L53
L53:jmp	rretrn
.globl	_subseq
.text
_subseq:
jsr	r5,rsave; 2
tst	_peekc
jne	L114
jsr	pc,_getchar
mov	r0,_peekc
L114:cmp	_peekc,4.(r5)
jeq	L115
mov	6.(r5),r0
jbr	L113
L115:clr	_peekc
mov	8.(r5),r0
jbr	L113
L113:jmp	rretrn
.globl	_getstr
.text
_getstr:
jsr	r5,rsave; 2
mov	$01,_nchstr
.data
L117:.byte 056,0164,0145,0170,0164,0
.even
.text
mov	$L117,r3
.data
L118:.byte 056,0144,0141,0164,0141,0
.even
.text
mov	$L118,r2
.data
L119:.byte 045,0163,012,0114,045,0144,072,056,0142,0171,0164,0145,040,0
.even
.text
mov	_isn,r0
inc	_isn
mov	r0,_cval
mov	r0,(sp)
tst	_strflg
jeq	L10018
mov	r3,-(sp)
jbr	L10019
L10018:mov	r2,-(sp)
L10019:mov	$L119,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L120:mov	$042,(sp)
jsr	pc,*$_mapch
mov	r0,r4
jlt	L121
.data
L122:.byte 060,045,0157,054,0
.even
.text
mov	r4,(sp)
mov	$L122,-(sp)
jsr	pc,*$_printf
tst	(sp)+
inc	_nchstr
jbr	L120
L121:.data
L123:.byte 060,012,056,0145,0166,0145,0156,012,045,0163,012,0
.even
.text
tst	_strflg
jeq	L10020
mov	r2,(sp)
jbr	L10021
L10020:mov	r3,(sp)
L10021:mov	$L123,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$026,r0
jbr	L116
L116:jmp	rretrn
.globl	_getcc
.text
_getcc:
jsr	r5,rsave; 2
clr	_cval
mov	$_cval,r2
clr	r3
L125:mov	$047,(sp)
jsr	pc,*$_mapch
mov	r0,r4
jlt	L126
mov	r3,r0
inc	r3
cmp	$02,r0
jle	L127
movb	r4,(r2)+
L127:jbr	L125
L126:cmp	r3,$02
jle	L128
.data
L129:.byte 0114,0157,0156,0147,040,0143,0150,0141,0162,0141,0143,0164,0145,0162,040,0143,0157,0156,0163,0164,0141,0156,0164,0
.even
.text
mov	$L129,(sp)
jsr	pc,*$_error
L128:mov	$025,r0
jbr	L124
L124:jmp	rretrn
.globl	_mapch
.text
_mapch:
.bss
L131:.=.+2.
.text
jsr	r5,rsave; 2
mov	4.(r5),r3
tst	L131
jeq	L132
mov	L131,r4
clr	L131
jbr	L133
L132:jsr	pc,_getchar
mov	r0,r4
L133:L134:cmp	r4,r3
jne	L135
mov	$0177777,r0
jbr	L130
L135:mov	r4,r0
jbr	L137
L138:L139:.data
L140:.byte 0116,0157,0156,0164,0145,0162,0155,0151,0156,0141,0164,0145,0144,040,0163,0164,0162,0151,0156,0147,0
.even
.text
mov	$L140,(sp)
jsr	pc,*$_error
mov	r4,_peekc
mov	$0177777,r0
jbr	L130
L141:jsr	pc,_getchar
mov	r0,r4
jbr	L143
L144:mov	$011,r0
jbr	L130
L145:mov	$012,r0
jbr	L130
L146:mov	$010,r0
jbr	L130
L147:L148:L149:L150:L151:L152:L153:L154:clr	r2
L155:cmp	r2,$0100
jge	L156
cmp	r4,$060
jlt	L156
cmp	r4,$067
jgt	L156
mov	r2,r0
ash	$03,r0
mov	r0,r2
mov	r4,r0
add	$0177720,r0
add	r0,r2
jsr	pc,_getchar
mov	r0,r4
jbr	L155
L156:mov	r4,L131
mov	r2,r0
jbr	L130
L157:mov	$015,r0
jbr	L130
L158:tst	_inhdr
jne	L159
inc	_line
L159:clr	_inhdr
jsr	pc,_getchar
mov	r0,r4
jbr	L134
jbr	L142
L143:jsr	r2,hsw; 3; L10023
jmp	*L10027-L10024(r1)
.data
L10023:L10024
L10025
L10026
L10027
L10024:..
20
21
22
46
L10025:..
3
20
21
22
L10026:..
20
21
40
44
46
L10027:L160
L147
L150
L153
L157
L160
L158
L148
L151
L154
L160
L149
L152
L146
L145
L144
.text
/esw
L142:L160:jbr	L136
L137:mov	$L10029,r1
mov	r0,L10030
L10031:cmp	r0,(r1)+
jne	L10031
jmp	*L10030-L10029(r1)
.data
L10029:0
12
134
L10030:..
L139
L138
L141
L161
.text
/esw
L136:L161:mov	r4,r0
jbr	L130
L130:jmp	rretrn
.globl	_tree
.text
_tree:
jsr	r5,rsave; 136
mov	$0764,_osleft
mov	_treebase,_space
mov	r5,r0
add	$0177720,r0
mov	r0,-8.(r5)
mov	r5,r0
add	$0177646,r0
mov	r0,-50.(r5)
mov	$_cmst,_cp
mov	$0310,*-8.(r5)
mov	$06,*-50.(r5)
clr	r4
L163:jsr	pc,_symbol
mov	r0,r3
jbr	L165
L166:mov	_csym,r2
tst	(r2)
jne	L167
tst	2.(r2)
jne	L167
jsr	pc,_nextchar
cmp	$050,r0
jne	L168
mov	$06,(r2)
mov	$020,2.(r2)
jbr	L169
L168:tst	_initflg
jeq	L170
mov	$06,(r2)
jbr	L171
L170:mov	$030,2.(r2)
tst	6.(r2)
jne	L172
mov	_isn,r0
inc	_isn
mov	r0,6.(r2)
L172:L171:L169:L167:clr	(sp)
mov	(r2),-(sp)
mov	4.(r2),-(sp)
mov	2.(r2),-(sp)
mov	$024,-(sp)
mov	$02,-(sp)
jsr	pc,*$_block
add	$012,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
cmp	(r2),$06
jne	L173
mov	r2,r0
add	$010,r0
mov	r0,-98.(r5)
clr	r3
L174:cmp	r3,$04
jge	L175
mov	*-98.(r5),(sp)
jsr	pc,*$_pblock
mov	-98.(r5),r0
add	$02,-98.(r5)
bit	$0177600,(r0)
jeq	L175
L176:inc	r3
jbr	L174
L175:jbr	L177
L173:mov	6.(r2),(sp)
jsr	pc,*$_pblock
L177:jbr	L178
L179:tst	_initflg
jne	L180
.data
L181:.byte 056,0144,0141,0164,0141,012,0114,045,0144,072,060,045,0157,073,060,045,0157,073,060,045,0157,073,060,045,0157,012,056,0164,0145,0170,0164,012,0
.even
.text
movf	_fcval,r0
movf	r0,-(sp)
mov	_cval,-(sp)
mov	$L181,-(sp)
jsr	pc,_printf
add	$014,sp
L180:L182:L183:mov	_cval,(sp)
clr	-(sp)
cmp	r3,$025
jne	L10032
clr	-(sp)
jbr	L10033
L10032:mov	$03,-(sp)
L10033:mov	r3,-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
jbr	L178
L184:mov	_cval,(sp)
clr	-(sp)
mov	$07,-(sp)
clr	-(sp)
mov	$031,-(sp)
mov	$024,-(sp)
mov	$03,-(sp)
jsr	pc,*$_block
add	$014,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
L178:cmp	_cp,$80.+_cmst
jlo	L185
.data
L186:.byte 0105,0170,0160,0162,0145,0163,0163,0151,0157,0156,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L186,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L185:tst	r4
jne	L187
mov	$01,r4
jbr	L163
L188:L189:tst	r4
jeq	L190
add	$02,r3
L190:jbr	L191
L192:L193:L194:tst	r4
jne	L187
jbr	L191
L195:tst	r4
jne	L196
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$027,r0
jne	L197
movf	_fcval,r0
negf	r0
movf	r0,_fcval
jbr	L163
L197:cmp	_peeksym,$030
jne	L198
movf	_fcval,r0
negf	r0
movf	r0,_fcval
mov	$0100000,r0
xor	r0,_cval
mov	_cval,r0
jbr	L163
L198:mov	$045,r3
L196:clr	r4
jbr	L191
L199:L200:tst	r4
jeq	L201
clr	r4
jbr	L202
L201:cmp	r3,$057
jne	L203
mov	$043,r3
jbr	L204
L203:mov	$044,r3
L204:L202:jbr	L191
L205:tst	r4
jeq	L206
jsr	pc,_symbol
mov	r0,r3
cmp	r3,$07
jne	L207
mov	$0145,r3
jbr	L208
L207:mov	r3,_peeksym
mov	$0144,r3
clr	r4
L208:L206:jbr	L191
L209:L210:tst	r4
jeq	L187
jbr	L191
L211:L212:inc	_mosflg
jbr	L164
jbr	L164
L165:sub	$05,r0
cmp	r0,$055
jhi	L213
asl	r0
jmp	*.+4(r0)
L209
L205
L210
L213
L213
L213
L213
L213
L213
L213
L213
L213
L213
L213
L213
L166
L182
L184
L179
L183
L213
L213
L213
L213
L194
L188
L189
L213
L213
L193
L213
L213
L213
L192
L211
L213
L195
L200
L213
L213
L213
L213
L199
L213
L213
L212
/esw
L164:L213:tst	r4
jeq	L187
clr	r4
L191:mov	r3,r0
asl	r0
mov	_opdope(r0),r0
ash	$-011,r0
bic	$!077,r0
mov	r0,-92.(r5)
cmp	r3,$011
jeq	L10035
cmp	r3,$010
jne	L214
L10035:tst	_initflg
jeq	L214
mov	$05,-92.(r5)
L214:L215:mov	*-50.(r5),-94.(r5)
cmp	-92.(r5),-94.(r5)
jgt	L10036
cmp	-92.(r5),-94.(r5)
jne	L216
mov	r3,r0
asl	r0
bit	$0200,_opdope(r0)
jeq	L216
L10036:mov	r3,r0
jbr	L218
L219:L220:mov	$045,-92.(r5)
jbr	L217
L221:L222:L223:mov	$04,-92.(r5)
jbr	L217
L218:mov	$L10038,r1
mov	r0,L10039
L10040:cmp	r0,(r1)+
jne	L10040
jmp	*L10039-L10038(r1)
.data
L10038:4
6
40
41
144
L10039:..
L222
L221
L219
L220
L223
L224
.text
/esw
L217:L224:mov	-8.(r5),r0
mov	r5,r1
add	$0177770,r1
cmp	r0,r1
jlo	L225
.data
L226:.byte 0145,0170,0160,0162,0145,0163,0163,0151,0157,0156,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L226,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L225:add	$02,-8.(r5)
mov	-8.(r5),r0
mov	r3,(r0)
add	$02,-50.(r5)
mov	-50.(r5),r0
mov	-92.(r5),(r0)
jbr	L163
L216:sub	$02,-50.(r5)
mov	-8.(r5),r0
sub	$02,-8.(r5)
mov	(r0),r0
mov	r0,-96.(r5)
jbr	L228
L229:mov	r3,_peeksym
clr	(sp)
jsr	pc,*$_build
sub	$02,_cp
mov	_cp,r0
mov	(r0),r0
jbr	L162
L230:cmp	r3,$07
jne	L187
mov	-96.(r5),(sp)
jsr	pc,*$_build
jbr	L163
L231:clr	(sp)
clr	-(sp)
clr	-(sp)
clr	-(sp)
jsr	pc,*$_block
add	$06,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
mov	$0144,-96.(r5)
jbr	L232
L233:cmp	r3,$07
jne	L187
jbr	L163
L234:cmp	r3,$05
jne	L187
mov	$04,(sp)
jsr	pc,*$_build
jbr	L163
jbr	L227
L228:mov	$L10042,r1
mov	r0,L10043
L10044:cmp	r0,(r1)+
jne	L10044
jmp	*L10043-L10042(r1)
.data
L10042:4
6
144
145
310
L10043:..
L234
L233
L230
L231
L229
L235
.text
/esw
L227:L235:L232:mov	-96.(r5),(sp)
jsr	pc,*$_build
jbr	L215
L187:.data
L236:.byte 0105,0170,0160,0162,0145,0163,0163,0151,0157,0156,040,0163,0171,0156,0164,0141,0170,0
.even
.text
mov	$L236,(sp)
jsr	pc,*$_error
mov	r3,(sp)
jsr	pc,*$_errflush
clr	r0
jbr	L162
L162:jmp	rretrn
.globl	_declare
.text
_declare:
jsr	r5,rsave; 2
mov	4.(r5),r3
L240:mov	10.(r5),(sp)
mov	8.(r5),-(sp)
mov	6.(r5),-(sp)
mov	r3,-(sp)
jsr	pc,*$_decl1
add	$06,sp
add	r0,8.(r5)
tst	_xdflg
jeq	L10045
cmp	r3,$04
jne	L237
L10045:L238:jsr	pc,_symbol
mov	r0,r4
cmp	$011,r0
jeq	L240
L239:cmp	r4,$01
jeq	L10046
cmp	r4,$07
jne	L241
cmp	r3,$013
jne	L241
L10046:mov	8.(r5),r0
jbr	L237
L241:mov	r4,(sp)
jsr	pc,*$_decsyn
L237:jmp	rretrn
.globl	_decl1
.text
_decl1:
jsr	r5,rsave; 6
mov	4.(r5),r3
clr	-10.(r5)
cmp	r3,$04
jeq	L10047
clr	r0
jbr	L10048
L10047:mov	$01,r0
L10048:mov	r0,_mosflg
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$01,r0
jeq	L10049
cmp	_peeksym,$07
jne	L243
L10049:clr	r0
jbr	L242
L243:jsr	pc,_getype
mov	r0,-8.(r5)
jlt	L244
clr	r4
L247:mov	r4,r0
ash	$02,r0
mov	-8.(r5),r1
bic	$!030,r1
bis	r1,r0
mov	r0,r4
L245:mov	-8.(r5),r0
ash	$-02,r0
mov	r0,-8.(r5)
bit	$030,r0
jne	L247
L246:bis	6.(r5),r4
mov	_defsym,r2
tst	(r2)
jeq	L248
cmp	r3,$012
jne	L10050
cmp	(r2),$013
jeq	L248
L10050:cmp	r3,$06
jne	L10051
cmp	(r2),$06
jne	L10051
cmp	2.(r2),r4
jeq	L248
L10051:cmp	r3,$04
jne	L249
cmp	(r2),$04
jne	L249
cmp	2.(r2),r4
jne	L249
mov	$01,-10.(r5)
jbr	L250
L249:jsr	pc,_redec
jbr	L244
L250:L248:mov	r4,2.(r2)
tst	r3
jeq	L251
mov	r3,(r2)
L251:cmp	r3,$013
jne	L252
tst	_paraml
jne	L253
mov	r2,_paraml
jbr	L254
L253:mov	_parame,r0
mov	r2,6.(r0)
L254:mov	r2,_parame
L252:tst	10.(r5)
jeq	L255
mov	r4,r0
bic	$!07,r0
cmp	$05,r0
jeq	L10052
mov	r4,r0
bic	$!07,r0
cmp	$04,r0
jne	L255
L10052:movb	_dimp,5.(r2)
jsr	pc,_chkdim
mov	_dimp,r0
inc	_dimp
asl	r0
mov	10.(r5),_dimtab(r0)
L255:clr	10.(r5)
cmp	r3,$04
jne	L256
mov	r2,(sp)
jsr	pc,*$_length
mov	r0,10.(r5)
bit	$01,8.(r5)
jeq	L257
cmp	10.(r5),$01
jeq	L257
inc	8.(r5)
inc	10.(r5)
L257:tst	-10.(r5)
jeq	L258
cmp	6.(r2),8.(r5)
jeq	L258
jsr	pc,_redec
L258:mov	8.(r5),6.(r2)
L256:mov	2.(r2),r0
bic	$!030,r0
cmp	$020,r0
jne	L259
cmp	(r2),$06
jeq	L260
cmp	(r2),$05
jeq	L260
.data
L261:.byte 0102,0141,0144,040,0146,0165,0156,0143,0164,0151,0157,0156,0
.even
.text
mov	$L261,(sp)
jsr	pc,*$_error
L260:mov	$06,(r2)
L259:cmp	(r2),$05
jne	L262
mov	r2,(sp)
jsr	pc,*$_rlength
add	r0,_autolen
mov	_autolen,r0
neg	r0
mov	r0,6.(r2)
jbr	L263
L262:cmp	(r2),$07
jne	L264
mov	_isn,6.(r2)
.data
L265:.byte 056,0142,0163,0163,012,0114,045,0144,072,056,075,056,053,045,0144,056,012,056,0164,0145,0170,0164,012,0
.even
.text
mov	r2,(sp)
jsr	pc,*$_rlength
mov	r0,(sp)
mov	_isn,-(sp)
inc	_isn
mov	$L265,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L266
L264:cmp	(r2),$010
jne	L267
mov	r4,r0
bic	$!07,r0
cmp	$01,r0
jge	L10054
bit	$030,r4
jeq	L10053
L10054:mov	r4,r0
bic	$!030,r0
cmp	$010,r0
jlt	L10053
cmp	_regvar,$03
jge	L268
L10053:.data
L269:.byte 0102,0141,0144,040,0162,0145,0147,0151,0163,0164,0145,0162,040,045,0157,0
.even
.text
mov	r4,(sp)
mov	$L269,-(sp)
jsr	pc,*$_error
tst	(sp)+
L268:dec	_regvar
mov	_regvar,r0
mov	r0,6.(r2)
L267:L266:L263:L244:mov	10.(r5),r0
jbr	L242
L242:jmp	rretrn
.globl	_getype
.text
_getype:
jsr	r5,rsave; 2
jsr	pc,_symbol
mov	r0,r4
jbr	L272
L273:jsr	pc,_getype
ash	$02,r0
bis	$010,r0
jbr	L270
L274:jsr	pc,_getype
mov	r0,r3
jsr	pc,_symbol
mov	r0,r4
cmp	$07,r0
jne	L275
jbr	L276
L277:mov	_csym,r0
mov	r0,r2
mov	r0,_defsym
clr	r3
movb	_dimp,4.(r2)
L276:jsr	pc,_symbol
mov	r0,r4
jbr	L279
L280:tst	_xdflg
jeq	L281
clr	_xdflg
mov	_defsym,r2
clr	(sp)
clr	-(sp)
clr	-(sp)
mov	$013,-(sp)
jsr	pc,*$_declare
add	$06,sp
mov	r2,_defsym
inc	_xdflg
jbr	L282
L281:jsr	pc,_symbol
mov	r0,r4
cmp	$07,r0
jne	L275
L282:mov	r3,r0
ash	$02,r0
bis	$020,r0
mov	r0,r3
jbr	L276
L283:jsr	pc,_symbol
mov	r0,r4
cmp	$05,r0
jeq	L284
mov	r4,_peeksym
jsr	pc,_conexp
mov	r0,_cval
movb	4.(r2),r0
bic	$!0377,r0
mov	r0,r4
L285:cmp	r4,_dimp
jge	L286
mov	r4,r1
asl	r1
mov	r1,-(sp)
add	$_dimtab,(sp)
mov	_cval,r1
mul	*(sp),r1
mov	r1,*(sp)+
L287:inc	r4
jbr	L285
L286:mov	_dimp,r0
inc	_dimp
asl	r0
mov	_cval,_dimtab(r0)
jsr	pc,_symbol
mov	r0,r4
cmp	$05,r0
jne	L275
jbr	L288
L284:mov	_dimp,r0
inc	_dimp
asl	r0
mov	$01,_dimtab(r0)
L288:mov	r3,r0
ash	$02,r0
bis	$030,r0
mov	r0,r3
jbr	L276
jbr	L278
L279:sub	$04,r0
cmp	r0,$02
jhi	L289
asl	r0
jmp	*.+4(r0)
L283
L289
L280
/esw
L278:L289:mov	r4,_peeksym
mov	r3,r0
jbr	L270
jbr	L271
L272:mov	$L10057,r1
mov	r0,L10058
L10059:cmp	r0,(r1)+
jne	L10059
jmp	*L10058-L10057(r1)
.data
L10057:6
24
52
L10058:..
L274
L277
L273
L290
.text
/esw
L271:L290:L275:mov	r4,(sp)
jsr	pc,*$_decsyn
mov	$0177777,r0
jbr	L270
L270:jmp	rretrn
.globl	_decsyn
.text
_decsyn:
jsr	r5,rsave; 2
.data
L292:.byte 0104,0145,0143,0154,0141,0162,0141,0164,0151,0157,0156,040,0163,0171,0156,0164,0141,0170,0
.even
.text
mov	$L292,(sp)
jsr	pc,*$_error
mov	4.(r5),(sp)
jsr	pc,*$_errflush
L291:jmp	rretrn
.globl	_redec
.text
_redec:
jsr	r5,rsave; 2
.data
L294:.byte 045,056,070,0163,040,0162,0145,0144,0145,0143,0154,0141,0162,0145,0144,0
.even
.text
mov	_defsym,(sp)
add	$010,(sp)
mov	$L294,-(sp)
jsr	pc,*$_error
tst	(sp)+
L293:jmp	rretrn
.globl	fltused
