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
.globl	_extdef
.text
_extdef:
jsr	r5,rsave; 12
jsr	pc,_symbol
mov	r0,r4
jeq	L1
cmp	r4,$01
jeq	L1
clr	-8.(r5)
inc	_xdflg
L2:cmp	r4,$023
jne	L3
cmp	_cval,$06
jne	L4
jsr	pc,_symbol
mov	r0,r4
jbr	L2
L4:mov	_cval,r0
mov	r0,-8.(r5)
cmp	$04,r0
jlt	L5
clr	-10.(r5)
cmp	-8.(r5),$04
jne	L6
clr	(sp)
mov	r5,r0
add	$0177770,r0
mov	r0,-(sp)
jsr	pc,*$_strdec
tst	(sp)+
mov	r0,-10.(r5)
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$023,r0
jeq	L7
jsr	pc,_blkhed
L7:L6:jbr	L8
L3:cmp	r4,$024
jne	L5
mov	r4,_peeksym
L8:L11:clr	_defsym
mov	$01,_strflg
mov	-10.(r5),(sp)
clr	-(sp)
mov	-8.(r5),-(sp)
bic	$!07,(sp)
mov	$06,-(sp)
jsr	pc,*$_decl1
add	$06,sp
mov	_defsym,r2
jeq	L1
mov	r2,r0
add	$010,r0
mov	r0,-14.(r5)
mov	r2,_funcsym
bisb	$0200,*-14.(r5)
.data
L12:.byte 056,0147,0154,0157,0142,0154,011,0137,045,056,070,0163,012,0
.even
.text
mov	-14.(r5),(sp)
mov	$L12,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	_xdflg
mov	2.(r2),-8.(r5)
mov	-8.(r5),r0
bic	$!030,r0
cmp	$020,r0
jne	L13
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$02,r0
jeq	L10000
cmp	_peeksym,$023
jne	L13
L10000:mov	-14.(r5),(sp)
jsr	pc,*$_cfunc
jbr	L1
L13:mov	$01,-12.(r5)
L14:mov	2.(r2),r0
bic	$!030,r0
cmp	$030,r0
jne	L15
movb	4.(r2),r0
bic	$!0377,r0
asl	r0
mov	_dimtab(r0),-12.(r5)
mov	2.(r2),(sp)
jsr	pc,*$_decref
mov	r0,2.(r2)
jbr	L14
L15:mov	r2,(sp)
jsr	pc,*$_length
mov	r0,r3
cmp	2.(r2),$04
jne	L16
mov	r3,r1
asr	r1
mul	-12.(r5),r1
mov	r1,-12.(r5)
mov	$02,r3
L16:mov	-8.(r5),2.(r2)
mov	r3,(sp)
mov	-12.(r5),-(sp)
mov	-8.(r5),-(sp)
mov	-14.(r5),-(sp)
jsr	pc,*$_cinit
add	$06,sp
L9:jsr	pc,_symbol
mov	r0,r4
cmp	$011,r0
jeq	L11
L10:cmp	r4,$01
jeq	L1
L5:.data
L17:.byte 0105,0170,0164,0145,0162,0156,0141,0154,040,0144,0145,0146,0151,0156,0151,0164,0151,0157,0156,040,0163,0171,0156,0164,0141,0170,0
.even
.text
mov	$L17,(sp)
jsr	pc,*$_error
mov	r4,(sp)
jsr	pc,*$_errflush
clr	(sp)
jsr	pc,*$_statemen
L1:jmp	rretrn
.globl	_cfunc
.text
_cfunc:
jsr	r5,rsave; 2
clr	_strflg
mov	_dimp,r4
.data
L19:.byte 056,0164,0145,0170,0164,012,0137,045,056,070,0163,072,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L19,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$012,(sp)
jsr	pc,*$_declist
mov	$05,_regvar
mov	_isn,r0
inc	_isn
mov	r0,_retlab
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$02,r0
jeq	L20
.data
L21:.byte 0103,0157,0155,0160,0157,0165,0156,0144,040,0163,0164,0141,0164,0145,0155,0145,0156,0164,040,0162,0145,0161,0165,0151,0162,0145,0144,0
.even
.text
mov	$L21,(sp)
jsr	pc,*$_error
L20:mov	$01,(sp)
jsr	pc,*$_statemen
.data
L22:.byte 0114,045,0144,072,0152,0155,0160,011,0162,0162,0145,0164,0162,0156,012,0
.even
.text
mov	_retlab,(sp)
mov	$L22,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	r4,_dimp
L18:jmp	rretrn
.globl	_cinit
.text
_cinit:
jsr	r5,rsave; 2
mov	10.(r5),r2
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$011,r0
jeq	L10001
cmp	_peeksym,$01
jne	L24
L10001:.data
L25:.byte 056,0143,0157,0155,0155,011,0137,045,056,070,0163,054,060,045,0157,012,0
.even
.text
mov	8.(r5),r1
mul	r2,r1
mov	r1,(sp)
inc	(sp)
bic	$!0177776,(sp)
mov	4.(r5),-(sp)
mov	$L25,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L23
L24:clr	r3
.data
L26:.byte 056,0144,0141,0164,0141,012,0137,045,056,070,0163,075,056,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L26,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_symbol
mov	r0,r4
cmp	$02,r0
jne	L27
L30:mov	r3,(sp)
mov	r2,-(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_cinit1
add	$06,sp
mov	r0,r3
L28:jsr	pc,_symbol
mov	r0,r4
cmp	$011,r0
jeq	L30
L29:cmp	r4,$03
jeq	L31
mov	r4,_peeksym
L31:jbr	L32
L27:mov	r4,_peeksym
clr	(sp)
mov	r2,-(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_cinit1
add	$06,sp
mov	r0,r3
L32:cmp	r3,8.(r5)
jge	L33
.data
L34:.byte 056,075,056,053,045,0144,056,012,0
.even
.text
mov	8.(r5),r1
sub	r3,r1
mul	r2,r1
mov	r1,(sp)
mov	$L34,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L35
L33:mov	r3,8.(r5)
L35:cmp	8.(r5),$01
jle	L36
mov	6.(r5),r0
bic	$!030,r0
cmp	$030,r0
jeq	L36
mov	6.(r5),r0
bic	$!07,r0
cmp	$04,r0
jeq	L36
.data
L37:.byte 0124,0157,0157,040,0155,0141,0156,0171,040,0151,0156,0151,0164,0151,0141,0154,0151,0172,0145,0162,0163,0
.even
.text
mov	$L37,(sp)
jsr	pc,*$_error
L36:mov	8.(r5),r0
mov	r2,r1
com	r1
bic	r1,r0
bit	$01,r0
jeq	L38
.data
L39:.byte 056,0145,0166,0145,0156,012,0
.even
.text
mov	$L39,(sp)
jsr	pc,*$_printf
L38:L23:jmp	rretrn
.globl	_cinit1
.text
_cinit1:
jsr	r5,rsave; 6
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$026,r0
jne	L41
cmp	6.(r5),$031
jne	L41
mov	$0177777,_peeksym
tst	10.(r5)
jeq	L42
jsr	pc,_bxdec
L42:.data
L43:.byte 056,0164,0145,0170,0164,012,0137,045,056,070,0163,075,0114,045,0144,012,0
.even
.text
mov	_cval,(sp)
mov	4.(r5),-(sp)
mov	$L43,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
mov	_nchstr,r0
inc	r0
bic	$!0177776,r0
jbr	L40
L41:cmp	_peeksym,$03
jne	L44
mov	10.(r5),r0
jbr	L40
L44:inc	_initflg
jsr	pc,_tree
mov	r0,r4
clr	_initflg
mov	8.(r5),r0
jbr	L46
L47:.data
L48:.byte 056,0142,0171,0164,0145,040,0
.even
.text
mov	$L48,(sp)
jsr	pc,*$_printf
cmp	(r4),$025
jeq	L49
jsr	pc,_bxdec
L49:L50:cmp	(r4),$025
jne	L51
.data
L52:.byte 045,0144,056,012,0
.even
.text
mov	6.(r4),(sp)
mov	$L52,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L45
L51:clr	(sp)
mov	r4,-(sp)
clr	-(sp)
clr	-(sp)
mov	$0150,-(sp)
mov	$01,-(sp)
jsr	pc,_block
add	$012,sp
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
jbr	L45
L53:movf	_fcval,r0
movfo	r0,-10.(r5)
.data
L54:.byte 060,045,0157,073,060,045,0157,012,0
.even
.text
movof	-10.(r5),r0
movf	r0,-(sp)
mov	$L54,-(sp)
jsr	pc,_printf
add	$012,sp
jbr	L55
L56:.data
L57:.byte 060,045,0157,073,060,045,0157,073,060,045,0157,073,060,045,0157,012,0
.even
.text
movf	_fcval,r0
movf	r0,-(sp)
mov	$L57,-(sp)
jsr	pc,_printf
add	$012,sp
L55:cmp	(r4),$027
jeq	L45
cmp	(r4),$030
jeq	L45
L58:jsr	pc,_bxdec
jbr	L45
L46:sub	$01,r0
cmp	r0,$07
jhi	L58
asl	r0
jmp	*.+4(r0)
L47
L50
L58
L53
L58
L58
L58
L56
/esw
L45:inc	10.(r5)
mov	10.(r5),r0
jbr	L40
L40:jmp	rretrn
.globl	_bxdec
.text
_bxdec:
jsr	r5,rsave; 2
.data
L60:.byte 0111,0156,0143,0157,0156,0163,0151,0163,0164,0145,0156,0164,040,0145,0170,0164,0145,0162,0156,0141,0154,040,0151,0156,0151,0164,0151,0141,0154,0151,0172,0141,0164,0151,0157,0156,0
.even
.text
mov	$L60,(sp)
jsr	pc,*$_error
L59:jmp	rretrn
.globl	_statemen
.text
_statemen:
jsr	r5,rsave; 10
L62:jsr	pc,_symbol
mov	r0,r4
jbr	L64
L65:.data
L66:.byte 0125,0156,0145,0170,0160,0145,0143,0164,0145,0144,040,0105,0117,0106,0
.even
.text
mov	$L66,(sp)
jsr	pc,*$_error
L67:L68:jbr	L61
L69:tst	4.(r5)
jeq	L70
tst	_proflg
jeq	L71
.data
L72:.byte 0152,0163,0162,011,0162,065,054,0155,0162,0163,0141,0166,0145,073,060,0146,073,045,0157,012,056,0142,0163,0163,073,060,072,056,075,056,053,062,012,056,0164,0145,0170,0164,012,0
.even
.text
mov	$L72,r4
jbr	L73
L71:.data
L74:.byte 0152,0163,0162,011,0162,065,054,0162,0163,0141,0166,0145,073,040,045,0157,012,0
.even
.text
mov	$L74,r4
L73:jsr	pc,_blkhed
mov	r0,(sp)
add	$0177774,(sp)
mov	r4,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L70:L75:tst	_eof
jne	L76
jsr	pc,_symbol
mov	r0,r4
cmp	$03,r0
jeq	L61
mov	r4,_peeksym
clr	(sp)
jsr	pc,*$_statemen
jbr	L75
L76:.data
L77:.byte 0115,0151,0163,0163,0151,0156,0147,040,047,0175,047,0
.even
.text
mov	$L77,(sp)
jsr	pc,*$_error
jbr	L61
L78:mov	_cval,r0
jbr	L80
L81:jsr	pc,_simplego
mov	r0,r3
jeq	L82
mov	r3,(sp)
jsr	pc,*$_branch
jbr	L83
L82:jsr	pc,_dogoto
L83:jbr	L84
L85:jsr	pc,_doret
jbr	L84
L86:jsr	pc,_pexpr
mov	r0,-12.(r5)
clr	r2
jsr	pc,_symbol
mov	r0,r3
cmp	$023,r0
jne	L87
mov	_cval,r0
jbr	L89
L90:jsr	pc,_simplego
mov	r0,r2
jne	L91
clr	(sp)
mov	_isn,r0
inc	_isn
mov	r0,r2
mov	r0,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_cbranch
cmp	(sp)+,(sp)+
jsr	pc,_dogoto
mov	r2,(sp)
jsr	pc,*$_label
jbr	L92
L93:jsr	pc,_nextchar
cmp	$073,r0
jne	L94
mov	_retlab,r2
jbr	L91
L94:clr	(sp)
mov	_isn,r0
inc	_isn
mov	r0,r3
mov	r0,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_cbranch
cmp	(sp)+,(sp)+
jsr	pc,_doret
mov	r3,(sp)
jsr	pc,*$_label
inc	r2
jbr	L92
L95:mov	_brklab,r2
jbr	L91
L96:mov	_contlab,r2
L91:mov	r2,(sp)
jsr	pc,*$_chconbrk
mov	$01,(sp)
mov	r2,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_cbranch
cmp	(sp)+,(sp)+
L92:jsr	pc,_symbol
mov	r0,r4
cmp	$01,r0
jne	L97
jsr	pc,_symbol
mov	r0,r3
cmp	$023,r0
jne	L10003
cmp	_cval,$016
jeq	L62
L10003:mov	r3,_peeksym
jbr	L61
jbr	L88
L89:sub	$012,r0
cmp	r0,$010
jhi	L98
asl	r0
jmp	*.+4(r0)
L90
L93
L98
L98
L98
L98
L98
L95
L96
/esw
L88:L98:L87:mov	r3,_peeksym
clr	(sp)
mov	_isn,r0
inc	_isn
mov	r0,r3
mov	r0,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_cbranch
cmp	(sp)+,(sp)+
clr	(sp)
jsr	pc,*$_statemen
jsr	pc,_symbol
mov	r0,r4
cmp	$023,r0
jne	L99
cmp	_cval,$016
jne	L99
mov	_isn,r0
inc	_isn
mov	r0,r2
mov	r2,(sp)
jsr	pc,*$_branch
mov	r3,(sp)
jsr	pc,*$_label
clr	(sp)
jsr	pc,*$_statemen
mov	r2,(sp)
jsr	pc,*$_label
jbr	L61
L99:mov	r4,_peeksym
mov	r3,(sp)
jsr	pc,*$_label
jbr	L61
L100:mov	_contlab,r3
mov	_brklab,r2
mov	_isn,r0
inc	_isn
mov	r0,_contlab
mov	r0,(sp)
jsr	pc,*$_label
clr	(sp)
mov	_isn,r0
inc	_isn
mov	r0,_brklab
mov	r0,-(sp)
jsr	pc,_pexpr
mov	r0,-(sp)
jsr	pc,*$_cbranch
cmp	(sp)+,(sp)+
clr	(sp)
jsr	pc,*$_statemen
mov	_contlab,(sp)
jsr	pc,*$_branch
mov	_brklab,(sp)
jsr	pc,*$_label
mov	r3,_contlab
mov	r2,_brklab
jbr	L61
L101:mov	_brklab,(sp)
jsr	pc,*$_chconbrk
mov	_brklab,(sp)
jsr	pc,*$_branch
jbr	L84
L102:mov	_contlab,(sp)
jsr	pc,*$_chconbrk
mov	_contlab,(sp)
jsr	pc,*$_branch
jbr	L84
L103:mov	_contlab,r3
mov	_brklab,r2
mov	_isn,r0
inc	_isn
mov	r0,_contlab
mov	_isn,r0
inc	_isn
mov	r0,_brklab
mov	_isn,r0
inc	_isn
mov	r0,-8.(r5)
mov	r0,(sp)
jsr	pc,*$_label
clr	(sp)
jsr	pc,*$_statemen
mov	_contlab,(sp)
jsr	pc,*$_label
mov	r3,_contlab
jsr	pc,_symbol
mov	r0,r4
cmp	$023,r0
jne	L104
cmp	_cval,$015
jne	L104
mov	$01,(sp)
mov	-8.(r5),-(sp)
jsr	pc,_tree
mov	r0,-(sp)
jsr	pc,*$_cbranch
cmp	(sp)+,(sp)+
mov	_brklab,(sp)
jsr	pc,*$_label
mov	r2,_brklab
jbr	L84
L104:jbr	L97
L105:jsr	pc,_conexp
mov	r0,r3
jsr	pc,_symbol
mov	r0,r4
cmp	$010,r0
jne	L97
tst	_swp
jne	L106
.data
L107:.byte 0103,0141,0163,0145,040,0156,0157,0164,040,0151,0156,040,0163,0167,0151,0164,0143,0150,0
.even
.text
mov	$L107,(sp)
jsr	pc,*$_error
jbr	L62
L106:cmp	_swp,$800.+_swtab
jlo	L108
.data
L109:.byte 0123,0167,0151,0164,0143,0150,040,0164,0141,0142,0154,0145,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L109,(sp)
jsr	pc,*$_error
jbr	L110
L108:mov	_isn,*_swp
mov	_swp,r0
add	$04,_swp
mov	r3,2.(r0)
mov	_isn,(sp)
inc	_isn
jsr	pc,*$_label
L110:jbr	L62
L111:mov	_brklab,r3
mov	_isn,r0
inc	_isn
mov	r0,_brklab
jsr	pc,_pexpr
mov	r0,-12.(r5)
mov	-12.(r5),(sp)
jsr	pc,*$_chkw
clr	(sp)
mov	-12.(r5),-(sp)
clr	-(sp)
clr	-(sp)
mov	$0156,-(sp)
mov	$01,-(sp)
jsr	pc,_block
add	$012,sp
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
jsr	pc,_pswitch
mov	r3,_brklab
jbr	L61
L112:tst	_swp
jne	L113
.data
L114:.byte 0104,0145,0146,0141,0165,0154,0164,040,0156,0157,0164,040,0151,0156,040,0163,0167,0151,0164,0143,0150,0
.even
.text
mov	$L114,(sp)
jsr	pc,*$_error
L113:jsr	pc,_symbol
mov	r0,r4
cmp	$010,r0
jne	L97
mov	_isn,r0
inc	_isn
mov	r0,_deflab
mov	r0,(sp)
jsr	pc,*$_label
jbr	L62
L115:mov	_contlab,r3
mov	_brklab,r2
mov	_isn,r0
inc	_isn
mov	r0,_contlab
mov	_isn,r0
inc	_isn
mov	r0,_brklab
jsr	pc,_forstmt
mov	r0,r4
jne	L97
mov	_brklab,(sp)
jsr	pc,*$_label
mov	r3,_contlab
mov	r2,_brklab
jbr	L61
jbr	L79
L80:sub	$012,r0
cmp	r0,$013
jhi	L116
asl	r0
jmp	*.+4(r0)
L81
L85
L86
L100
L116
L111
L105
L101
L102
L103
L112
L115
/esw
L79:L116:.data
L117:.byte 0125,0156,0153,0156,0157,0167,0156,040,0153,0145,0171,0167,0157,0162,0144,0
.even
.text
mov	$L117,(sp)
jsr	pc,*$_error
jbr	L97
L118:jsr	pc,_nextchar
cmp	$072,r0
jne	L119
clr	_peekc
mov	_csym,r3
tst	(r3)
jle	L120
.data
L121:.byte 0122,0145,0144,0145,0146,0151,0156,0151,0164,0151,0157,0156,0
.even
.text
mov	$L121,(sp)
jsr	pc,*$_error
jbr	L62
L120:mov	$07,(r3)
mov	$030,2.(r3)
tst	6.(r3)
jne	L122
mov	_isn,r0
inc	_isn
mov	r0,6.(r3)
L122:mov	6.(r3),(sp)
jsr	pc,*$_label
jsr	pc,_symbol
mov	r0,_peeksym
cmp	$03,r0
jeq	L61
jbr	L62
L119:jbr	L63
L64:mov	$L10007,r1
mov	r0,L10008
L10009:cmp	r0,(r1)+
jne	L10009
jmp	*L10008-L10007(r1)
.data
L10007:0
1
2
3
23
24
L10008:..
L65
L67
L69
L68
L78
L118
L123
.text
/esw
L63:L123:mov	r4,_peeksym
mov	$01,(sp)
jsr	pc,_tree
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
L84:jsr	pc,_symbol
mov	r0,r4
cmp	$01,r0
jeq	L61
L97:.data
L124:.byte 0123,0164,0141,0164,0145,0155,0145,0156,0164,040,0163,0171,0156,0164,0141,0170,0
.even
.text
mov	$L124,(sp)
jsr	pc,*$_error
mov	r4,(sp)
jsr	pc,*$_errflush
jbr	L62
L61:jmp	rretrn
.globl	_forstmt
.text
_forstmt:
jsr	r5,rsave; 464
jsr	pc,_symbol
mov	r0,r2
cmp	$06,r0
jeq	L126
mov	r2,r0
jbr	L125
L126:jsr	pc,_symbol
mov	r0,r2
cmp	$01,r0
jeq	L127
mov	r2,_peeksym
mov	$01,(sp)
jsr	pc,_tree
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
jsr	pc,_symbol
mov	r0,r2
cmp	$01,r0
jeq	L128
mov	r2,r0
jbr	L125
L128:L127:mov	_contlab,(sp)
jsr	pc,*$_label
jsr	pc,_symbol
mov	r0,r2
cmp	$01,r0
jeq	L129
mov	r2,_peeksym
mov	$02,(sp)
clr	-(sp)
mov	_brklab,-(sp)
jsr	pc,_tree
mov	r0,-(sp)
mov	$0147,-(sp)
mov	$01,-(sp)
jsr	pc,_block
add	$012,sp
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
jsr	pc,_symbol
mov	r0,r2
cmp	$01,r0
jeq	L130
mov	r2,r0
jbr	L125
L130:L129:jsr	pc,_symbol
mov	r0,_peeksym
cmp	$07,r0
jne	L131
mov	$0177777,_peeksym
clr	(sp)
jsr	pc,*$_statemen
mov	_contlab,(sp)
jsr	pc,*$_branch
clr	r0
jbr	L125
L131:mov	_contlab,-8.(r5)
mov	_isn,r0
inc	_isn
mov	r0,_contlab
jsr	pc,_tree
mov	r0,-310.(r5)
jsr	pc,_symbol
mov	r0,r2
cmp	$07,r0
jeq	L132
mov	r2,r0
jbr	L125
L132:mov	_space,-312.(r5)
mov	_space,r0
sub	_treebase,r0
asr	r0
cmp	$0226,r0
jge	L133
.data
L134:.byte 0105,0170,0160,0162,0145,0163,0163,0151,0157,0156,040,0164,0157,0157,040,0154,0141,0162,0147,0145,0
.even
.text
mov	$L134,(sp)
jsr	pc,*$_error
mov	_treebase,r0
add	$0454,r0
mov	r0,_space
L133:mov	r5,r0
add	$0177314,r0
mov	r0,r3
mov	_treebase,r4
L135:cmp	r4,_space
jhis	L136
mov	(r4)+,(r3)+
jbr	L135
L136:clr	(sp)
jsr	pc,*$_statemen
mov	-312.(r5),_space
mov	r5,r0
add	$0177314,r0
mov	r0,r3
mov	_treebase,r4
L137:cmp	r4,_space
jhis	L138
mov	(r3)+,(r4)+
jbr	L137
L138:mov	_contlab,(sp)
jsr	pc,*$_label
mov	$01,(sp)
mov	-310.(r5),-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
mov	-8.(r5),(sp)
jsr	pc,*$_branch
clr	r0
jbr	L125
L125:jmp	rretrn
.globl	_pexpr
.text
_pexpr:
jsr	r5,rsave; 2
jsr	pc,_symbol
mov	r0,r4
cmp	$06,r0
jne	L140
jsr	pc,_tree
mov	r0,r3
jsr	pc,_symbol
mov	r0,r4
cmp	$07,r0
jne	L140
mov	r3,r0
jbr	L139
L140:.data
L141:.byte 0123,0164,0141,0164,0145,0155,0145,0156,0164,040,0163,0171,0156,0164,0141,0170,0
.even
.text
mov	$L141,(sp)
jsr	pc,*$_error
mov	r4,(sp)
jsr	pc,*$_errflush
clr	r0
jbr	L139
L139:jmp	rretrn
.globl	_pswitch
.text
_pswitch:
jsr	r5,rsave; 6
mov	_swp,r0
mov	r0,-8.(r5)
mov	r0,r4
tst	_swp
jne	L143
mov	$_swtab,r0
mov	r0,_swp
mov	r0,r4
L143:mov	_isn,r0
inc	_isn
mov	r0,-10.(r5)
mov	r0,(sp)
jsr	pc,*$_branch
mov	_deflab,r2
clr	_deflab
clr	(sp)
jsr	pc,*$_statemen
mov	_brklab,(sp)
jsr	pc,*$_branch
mov	-10.(r5),(sp)
jsr	pc,*$_label
mov	$043,(sp)
jsr	pc,*$_putchar
mov	_brklab,(sp)
jsr	pc,*$_label
tst	_deflab
jne	L144
mov	_isn,r0
inc	_isn
mov	r0,_deflab
mov	_deflab,(sp)
jsr	pc,*$_label
L144:mov	_swp,r3
mov	$_binbuf,(sp)
mov	r3,r0
sub	r4,r0
asr	r0
mov	r0,-(sp)
jsr	pc,*$_putw
tst	(sp)+
mov	$_binbuf,(sp)
mov	_deflab,-(sp)
jsr	pc,*$_putw
tst	(sp)+
mov	$_binbuf,(sp)
mov	$04,-(sp)
jsr	pc,*$_putw
tst	(sp)+
mov	$_binbuf,(sp)
mov	_line,-(sp)
jsr	pc,*$_putw
tst	(sp)+
L145:cmp	r4,r3
jhis	L146
mov	$_binbuf,(sp)
mov	(r4)+,-(sp)
jsr	pc,*$_putw
tst	(sp)+
jbr	L145
L146:mov	r2,_deflab
mov	-8.(r5),_swp
L142:jmp	rretrn
.globl	_blkhed
.text
_blkhed:
jsr	r5,rsave; 2
mov	$06,_autolen
clr	(sp)
jsr	pc,*$_declist
mov	$04,r4
L148:tst	_paraml
jeq	L149
mov	_parame,r0
clr	6.(r0)
mov	_paraml,r3
mov	_paraml,r0
mov	6.(r0),_paraml
cmp	2.(r3),$02
jne	L150
mov	$03,2.(r3)
L150:mov	r4,6.(r3)
mov	$05,(r3)
mov	2.(r3),r0
bic	$!030,r0
cmp	$030,r0
jne	L151
sub	$020,2.(r3)
incb	4.(r3)
L151:mov	r3,(sp)
jsr	pc,*$_rlength
add	r0,r4
jbr	L148
L149:mov	$_hshtab,r3
L152:cmp	r3,$3200.+_hshtab
jhis	L153
tstb	8.(r3)
jeq	L154
cmp	(r3),$01
jle	L155
mov	2.(r3),r0
bic	$!07,r0
cmp	$05,r0
jne	L155
movb	5.(r3),r0
bic	$!0377,r0
asl	r0
mov	_dimtab(r0),r0
movb	5.(r0),5.(r3)
mov	2.(r3),r0
bic	$!0177770,r0
bis	$04,r0
mov	r0,2.(r3)
L155:cmp	(r3),$011
jne	L156
movb	5.(r3),r0
bic	$!0377,r0
asl	r0
tst	_dimtab(r0)
jne	L156
.data
L157:.byte 0125,0156,0144,0145,0146,0151,0156,0145,0144,040,0163,0164,0162,0165,0143,0164,0165,0162,0145,072,040,045,056,070,0163,0
.even
.text
mov	r3,(sp)
add	$010,(sp)
mov	$L157,-(sp)
jsr	pc,*$_error
tst	(sp)+
L156:cmp	(r3),$012
jne	L158
.data
L159:.byte 0116,0157,0164,040,0141,0156,040,0141,0162,0147,0165,0155,0145,0156,0164,072,040,045,056,070,0163,0
.even
.text
mov	r3,(sp)
add	$010,(sp)
mov	$L159,-(sp)
jsr	pc,*$_error
tst	(sp)+
L158:L154:add	$020,r3
jbr	L152
L153:mov	$0764,_osleft
mov	_treebase,_space
clr	(sp)
mov	_regvar,-(sp)
mov	$0151,-(sp)
mov	$01,-(sp)
jsr	pc,_block
add	$06,sp
mov	r0,-(sp)
jsr	pc,*$_rcexpr
tst	(sp)+
mov	_autolen,r0
jbr	L147
L147:jmp	rretrn
.globl	_blkend
.text
_blkend:
jsr	r5,rsave; 2
mov	$_hshtab,r4
L161:cmp	r4,$3200.+_hshtab
jhis	L162
tstb	8.(r4)
jeq	L164
tst	(r4)
jne	L165
.data
L166:.byte 045,056,070,0163,040,0165,0156,0144,0145,0146,0151,0156,0145,0144,0
.even
.text
mov	r4,(sp)
add	$010,(sp)
mov	$L166,-(sp)
jsr	pc,*$_error
tst	(sp)+
L165:bitb	$0200,8.(r4)
jne	L167
clrb	8.(r4)
dec	_hshused
L167:L164:L163:add	$020,r4
jbr	L161
L162:L160:jmp	rretrn
.globl	_errflush
.text
_errflush:
jsr	r5,rsave; 2
mov	4.(r5),r4
L169:cmp	r4,$03
jle	L170
jsr	pc,_symbol
mov	r0,r4
jbr	L169
L170:mov	r4,_peeksym
L168:jmp	rretrn
.globl	_declist
.text
_declist:
jsr	r5,rsave; 10
clr	r4
L172:clr	-12.(r5)
mov	$0177777,r3
mov	4.(r5),r2
clr	-10.(r5)
L173:jsr	pc,_symbol
mov	r0,-8.(r5)
cmp	$023,r0
jne	L174
mov	_cval,r0
jbr	L176
L177:L178:L179:L180:tst	r2
jeq	L181
.data
L182:.byte 0103,0157,0156,0146,0154,0151,0143,0164,040,0151,0156,040,0163,0164,0157,0162,0141,0147,0145,040,0143,0154,0141,0163,0163,0
.even
.text
mov	$L182,(sp)
jsr	pc,*$_error
L181:mov	_cval,r2
inc	-12.(r5)
tst	r3
jlt	L173
jbr	L183
L184:mov	_cval,-8.(r5)
cmp	r2,$04
jeq	L10010
clr	(sp)
jbr	L10011
L10010:mov	$01,(sp)
L10011:mov	r5,r0
add	$0177770,r0
mov	r0,-(sp)
jsr	pc,*$_strdec
tst	(sp)+
mov	r0,-10.(r5)
mov	-8.(r5),_cval
L185:L186:L187:L188:inc	-12.(r5)
tst	r3
jlt	L189
.data
L190:.byte 0124,0171,0160,0145,040,0143,0154,0141,0163,0150,0
.even
.text
mov	$L190,(sp)
jsr	pc,*$_error
L189:mov	_cval,r3
tst	r2
jeq	L173
jbr	L183
L191:jbr	L192
jbr	L175
L176:cmp	r0,$010
jhi	L191
asl	r0
jmp	*.+4(r0)
L185
L186
L187
L188
L184
L177
L179
L178
L180
/esw
L175:jbr	L173
L174:L192:mov	-8.(r5),_peeksym
tst	-12.(r5)
jne	L193
mov	r4,r0
jbr	L171
L193:L183:tst	r3
jge	L194
clr	r3
L194:tst	r2
jne	L195
mov	$05,r2
L195:mov	-10.(r5),(sp)
mov	r4,-(sp)
mov	r3,-(sp)
mov	r2,-(sp)
jsr	pc,*$_declare
add	$06,sp
mov	r0,r4
jbr	L172
L171:jmp	rretrn
.globl	_strdec
.text
_strdec:
jsr	r5,rsave; 4
mov	$01,_mosflg
clr	r2
jsr	pc,_symbol
mov	r0,r3
cmp	$024,r0
jne	L197
mov	_csym,r2
tst	(r2)
jne	L198
mov	$011,(r2)
movb	_dimp,5.(r2)
jsr	pc,_chkdim
mov	_dimp,r0
inc	_dimp
asl	r0
clr	_dimtab(r0)
L198:cmp	(r2),$011
jeq	L199
jsr	pc,_redec
L199:mov	6.(r5),_mosflg
jsr	pc,_symbol
mov	r0,r3
L197:clr	_mosflg
cmp	r3,$02
jeq	L200
tst	r2
jne	L201
L202:mov	r3,(sp)
jsr	pc,*$_decsyn
clr	r0
jbr	L196
L201:cmp	(r2),$011
jeq	L203
.data
L204:.byte 0102,0141,0144,040,0163,0164,0162,0165,0143,0164,0165,0162,0145,040,0156,0141,0155,0145,0
.even
.text
mov	$L204,(sp)
jsr	pc,*$_error
L203:movb	5.(r2),r0
bic	$!0377,r0
asl	r0
mov	_dimtab(r0),r4
jne	L205
mov	$05,*4.(r5)
mov	r2,r4
L205:mov	r3,_peeksym
jbr	L206
L200:mov	_defsym,-8.(r5)
clr	_mosflg
mov	$04,(sp)
jsr	pc,*$_declist
mov	r0,r4
bit	$01,r4
jeq	L207
inc	r4
L207:mov	-8.(r5),_defsym
jsr	pc,_symbol
mov	r0,r3
cmp	$03,r0
jne	L202
tst	r2
jeq	L208
movb	5.(r2),r0
bic	$!0377,r0
asl	r0
tst	_dimtab(r0)
jeq	L209
.data
L210:.byte 045,056,070,0163,040,0162,0145,0144,0145,0143,0154,0141,0162,0145,0144,0
.even
.text
mov	r2,(sp)
add	$010,(sp)
mov	$L210,-(sp)
jsr	pc,*$_error
tst	(sp)+
L209:movb	5.(r2),r0
bic	$!0377,r0
asl	r0
mov	r4,_dimtab(r0)
L208:L206:mov	r4,r0
jbr	L196
L196:jmp	rretrn
.globl	_chkdim
.text
_chkdim:
jsr	r5,rsave; 2
cmp	_dimp,$0144
jlt	L212
.data
L213:.byte 0104,0151,0155,0145,0156,0163,0151,0157,0156,057,0163,0164,0162,0165,0143,0164,040,0164,0141,0142,0154,0145,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L213,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L212:L211:jmp	rretrn
.globl	fltused
