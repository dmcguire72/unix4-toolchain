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
.globl	_build
.text
_build:
jsr	r5,rsave; 24
cmp	4.(r5),$04
jne	L2
mov	$050,(sp)
jsr	pc,*$_build
mov	$044,4.(r5)
L2:mov	4.(r5),r0
asl	r0
mov	_opdope(r0),-18.(r5)
bit	$01,-18.(r5)
jeq	L3
sub	$02,_cp
mov	_cp,r0
mov	(r0),(sp)
jsr	pc,*$_disarray
mov	r0,(sp)
jsr	pc,*$_chkfun
mov	r0,r2
mov	2.(r2),-8.(r5)
L3:sub	$02,_cp
mov	_cp,r0
mov	(r0),r3
cmp	4.(r5),$035
jne	L4
mov	r3,(sp)
jsr	pc,*$_length
mov	r0,r4
mov	$025,(r3)
clr	2.(r3)
clr	4.(r3)
mov	r4,6.(r3)
mov	_cp,r0
add	$02,_cp
mov	r3,(r0)
jbr	L1
L4:cmp	4.(r5),$043
jeq	L5
mov	r3,(sp)
jsr	pc,*$_disarray
mov	r0,r3
cmp	4.(r5),$0144
jeq	L6
mov	r3,(sp)
jsr	pc,*$_chkfun
mov	r0,r3
L6:L5:mov	2.(r3),r4
clr	-24.(r5)
mov	4.(r5),r0
jbr	L8
L9:mov	_cp,r0
add	$02,_cp
mov	r3,(r0)
jbr	L1
L10:L11:L12:mov	r2,(sp)
mov	r3,-(sp)
clr	-(sp)
clr	-(sp)
mov	4.(r5),-(sp)
mov	$02,-(sp)
jsr	pc,*$_block
add	$012,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
jbr	L1
L13:cmp	(r2),$010
jeq	L14
.data
L15:.byte 0111,0154,0154,0145,0147,0141,0154,040,0143,0157,0156,0144,0151,0164,0151,0157,0156,0141,0154,0
.even
.text
mov	$L15,(sp)
jsr	pc,*$_error
L14:mov	-8.(r5),-12.(r5)
jbr	L16
L17:mov	r4,r0
bic	$!030,r0
cmp	$020,r0
jeq	L18
.data
L19:.byte 0103,0141,0154,0154,040,0157,0146,040,0156,0157,0156,055,0146,0165,0156,0143,0164,0151,0157,0156,0
.even
.text
mov	$L19,(sp)
jsr	pc,*$_error
L18:mov	r2,(sp)
mov	r3,-(sp)
mov	4.(r3),-(sp)
mov	r4,-(sp)
jsr	pc,_decref
tst	(sp)+
mov	r0,-(sp)
mov	$0144,-(sp)
mov	$02,-(sp)
jsr	pc,*$_block
add	$012,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
jbr	L1
L20:cmp	(r3),$043
jeq	L10000
cmp	(r3),$035
jne	L21
L10000:mov	_cp,r0
add	$02,_cp
mov	6.(r3),(r0)
jbr	L1
L21:mov	r4,r0
bic	$!030,r0
cmp	$020,r0
jne	L22
.data
L23:.byte 0111,0154,0154,0145,0147,0141,0154,040,0151,0156,0144,0151,0162,0145,0143,0164,0151,0157,0156,0
.even
.text
mov	$L23,(sp)
jsr	pc,*$_error
L22:mov	r3,(sp)
mov	4.(r3),-(sp)
mov	r4,-(sp)
jsr	pc,_decref
tst	(sp)+
mov	r0,-(sp)
mov	$044,-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
jbr	L1
L24:cmp	(r3),$044
jne	L25
mov	6.(r3),r0
mov	4.(r3),4.(r0)
mov	r4,(sp)
jsr	pc,*$_incref
mov	6.(r3),r1
mov	r0,2.(r1)
mov	_cp,r0
add	$02,_cp
mov	6.(r3),(r0)
jbr	L1
L25:cmp	(r3),$024
jne	L26
cmp	6.(r3),$05
jne	L27
mov	$035,4.(r5)
L27:mov	r3,(sp)
mov	4.(r3),-(sp)
mov	r4,-(sp)
jsr	pc,_incref
tst	(sp)+
mov	r0,-(sp)
mov	4.(r5),-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
jbr	L1
L26:.data
L28:.byte 0111,0154,0154,0145,0147,0141,0154,040,0154,0166,0141,0154,0165,0145,0
.even
.text
mov	$L28,(sp)
jsr	pc,*$_error
jbr	L7
L29:L30:L31:L32:mov	r3,(sp)
jsr	pc,*$_chklval
mov	r3,(sp)
jsr	pc,*$_plength
mov	r0,(sp)
mov	r3,-(sp)
mov	4.(r3),-(sp)
mov	r4,-(sp)
mov	4.(r5),-(sp)
mov	$02,-(sp)
jsr	pc,*$_block
add	$012,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
jbr	L1
L33:mov	_cp,r0
add	$02,_cp
mov	r3,(r0)
mov	r3,(sp)
jsr	pc,*$_chkw
mov	$014,2.(r3)
mov	$044,(sp)
jsr	pc,*$_build
sub	$02,_cp
mov	_cp,r0
mov	(r0),r3
L34:cmp	(r2),$024
jne	L10001
cmp	6.(r2),$04
jeq	L35
L10001:.data
L36:.byte 0111,0154,0154,0145,0147,0141,0154,040,0163,0164,0162,0165,0143,0164,0165,0162,0145,040,0162,0145,0146,0
.even
.text
mov	$L36,(sp)
jsr	pc,*$_error
L35:mov	_cp,r0
add	$02,_cp
mov	r3,(r0)
mov	-8.(r5),-12.(r5)
mov	-12.(r5),r0
bic	$!030,r0
cmp	$030,r0
jne	L37
mov	-12.(r5),(sp)
jsr	pc,*$_decref
mov	r0,-12.(r5)
incb	4.(r2)
L37:mov	4.(r2),(sp)
mov	-12.(r5),-(sp)
mov	r3,-(sp)
jsr	pc,*$_setype
cmp	(sp)+,(sp)+
mov	$043,(sp)
jsr	pc,*$_build
mov	10.(r2),(sp)
clr	-(sp)
mov	$07,-(sp)
mov	$025,-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
mov	$050,(sp)
jsr	pc,*$_build
mov	-8.(r5),r0
bic	$!030,r0
cmp	$030,r0
jeq	L38
mov	$044,(sp)
jsr	pc,*$_build
L38:jbr	L1
jbr	L7
L8:jsr	r2,hsw; 4; L10003
jmp	*L10008-L10004(r1)
.data
L10003:L10004
L10005
L10006
L10007
L10008
L10004:..
0
10
11
31
L10005:..
2
10
15
L10006:..
7
14
15
26
L10007:..
7
10
11
L10008:L39
L9
L31
L20
L17
L39
L10
L32
L11
L39
L29
L33
L12
L13
L39
L30
L24
L34
.text
/esw
L7:L39:bit	$02,-18.(r5)
jeq	L40
mov	r3,(sp)
jsr	pc,*$_chklval
L40:bit	$020,-18.(r5)
jeq	L41
mov	r3,(sp)
jsr	pc,*$_chkw
L41:bit	$040,-18.(r5)
jeq	L42
mov	r2,(sp)
jsr	pc,*$_chkw
L42:bit	$01,-18.(r5)
jne	L43
clr	(sp)
mov	r3,-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_fold
cmp	(sp)+,(sp)+
tst	r0
jne	L44
mov	r3,(sp)
mov	4.(r3),-(sp)
mov	r4,-(sp)
mov	4.(r5),-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
L44:jbr	L1
L43:cmp	-8.(r5),$07
jne	L45
mov	r4,-12.(r5)
clr	2.(r2)
clr	-8.(r5)
jbr	L16
L45:mov	-8.(r5),(sp)
jsr	pc,*$_lintyp
mov	r0,-(sp)
mov	r4,-(sp)
jsr	pc,_lintyp
tst	(sp)+
mov	r0,r1
mul	$013,r1
add	(sp)+,r1
movb	_cvtab(r1),r1
mov	r1,-22.(r5)
mov	-22.(r5),r0
bic	$!0100,r0
mov	r0,-20.(r5)
tst	-20.(r5)
jeq	L10009
mov	-8.(r5),r0
jbr	L10010
L10009:mov	r4,r0
L10010:mov	r0,-12.(r5)
cmp	4.(r5),$0120
jne	L46
cmp	r4,$04
jeq	L46
cmp	-8.(r5),$04
jeq	L46
mov	r4,-12.(r5)
tst	-20.(r5)
jne	L16
cmp	-22.(r5),$03
jne	L16
L46:bic	$!077,-22.(r5)
jeq	L47
cmp	4.(r5),$010
jne	L10011
cmp	r4,$010
jlt	L10011
cmp	r4,-8.(r5)
jeq	L16
L10011:cmp	-22.(r5),$077
jne	L48
bit	$04,-18.(r5)
jeq	L10012
cmp	r4,$010
jlt	L10012
cmp	-8.(r5),$010
jge	L49
L10012:L50:.data
L51:.byte 0111,0154,0154,0145,0147,0141,0154,040,0143,0157,0156,0166,0145,0162,0163,0151,0157,0156,0
.even
.text
mov	$L51,(sp)
jsr	pc,*$_error
L49:jbr	L16
L48:cmp	-22.(r5),$02
jne	L52
clr	-12.(r5)
bit	$04,-18.(r5)
jeq	L53
clr	-22.(r5)
jbr	L16
L53:cmp	4.(r5),$051
jne	L50
mov	-22.(r5),-24.(r5)
clr	-22.(r5)
jbr	L16
L52:tst	-20.(r5)
jeq	L54
bit	$010,-18.(r5)
jeq	L55
cmp	-22.(r5),$03
jne	L56
clr	-20.(r5)
mov	$04,-22.(r5)
mov	r4,-12.(r5)
jbr	L57
jbr	L58
L56:jbr	L50
L58:L55:mov	r2,(sp)
jsr	pc,*$_plength
mov	r0,(sp)
mov	-22.(r5),-(sp)
mov	-12.(r5),-(sp)
mov	r3,-(sp)
jsr	pc,*$_convert
add	$06,sp
mov	r0,r3
jbr	L59
L54:L57:mov	r3,(sp)
jsr	pc,*$_plength
mov	r0,(sp)
mov	-22.(r5),-(sp)
mov	-12.(r5),-(sp)
mov	r2,-(sp)
jsr	pc,*$_convert
add	$06,sp
mov	r0,r2
L59:L16:L47:bit	$04,-18.(r5)
jeq	L60
cmp	4.(r5),$075
jle	L61
cmp	r4,$010
jge	L10013
cmp	-8.(r5),$010
jlt	L61
L10013:add	$04,4.(r5)
L61:clr	-12.(r5)
L60:mov	r2,(sp)
mov	r3,-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_fold
cmp	(sp)+,(sp)+
tst	r0
jne	L1
mov	r2,(sp)
mov	r3,-(sp)
tst	4.(r3)
jne	L10014
mov	r2,r0
jbr	L10015
L10014:mov	r3,r0
L10015:mov	4.(r0),-(sp)
mov	-12.(r5),-(sp)
mov	4.(r5),-(sp)
mov	$02,-(sp)
jsr	pc,*$_block
add	$012,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
tst	-24.(r5)
jeq	L62
sub	$02,_cp
mov	_cp,r0
mov	(r0),r3
mov	6.(r3),(sp)
jsr	pc,*$_plength
mov	r0,(sp)
mov	-24.(r5),-(sp)
clr	-(sp)
mov	r3,-(sp)
jsr	pc,*$_convert
add	$06,sp
mov	_cp,r1
add	$02,_cp
mov	r0,(r1)
L62:L1:jmp	rretrn
.globl	_convert
.text
_convert:
jsr	r5,rsave; 2
mov	8.(r5),r0
jbr	L65
L66:mov	$052,r4
jbr	L67
L68:mov	$053,r4
L67:mov	10.(r5),(sp)
clr	-(sp)
clr	-(sp)
mov	$025,-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
mov	r0,(sp)
mov	4.(r5),-(sp)
clr	-(sp)
mov	6.(r5),-(sp)
mov	r4,-(sp)
mov	$02,-(sp)
jsr	pc,*$_block
add	$012,sp
jbr	L63
L69:mov	$063,r4
jbr	L70
L71:mov	$064,r4
L70:mov	4.(r5),(sp)
clr	-(sp)
mov	6.(r5),-(sp)
mov	r4,-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
jbr	L63
jbr	L64
L65:sub	$01,r0
cmp	r0,$03
jhi	L72
asl	r0
jmp	*.+4(r0)
L66
L68
L69
L71
/esw
L64:L72:.data
L73:.byte 0103,040,0145,0162,0162,0157,0162,055,055,040,0143,0157,0156,0166,0145,0162,0164,0
.even
.text
mov	$L73,(sp)
jsr	pc,*$_error
mov	4.(r5),r0
jbr	L63
L63:jmp	rretrn
.globl	_setype
.text
_setype:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	6.(r5),r3
mov	8.(r5),r2
mov	r3,2.(r4)
cmp	r2,$0177777
jeq	L75
mov	r2,4.(r4)
L75:mov	(r4),r0
jbr	L77
L78:L79:mov	r2,(sp)
mov	r3,-(sp)
jsr	pc,_decref
tst	(sp)+
mov	r0,-(sp)
mov	6.(r4),-(sp)
jsr	pc,*$_setype
cmp	(sp)+,(sp)+
jbr	L74
L80:mov	r2,(sp)
mov	r3,-(sp)
jsr	pc,_incref
tst	(sp)+
mov	r0,-(sp)
mov	6.(r4),-(sp)
jsr	pc,*$_setype
cmp	(sp)+,(sp)+
jbr	L74
L81:L82:mov	r2,(sp)
mov	r3,-(sp)
mov	6.(r4),-(sp)
jsr	pc,*$_setype
cmp	(sp)+,(sp)+
jbr	L76
L77:sub	$035,r0
cmp	r0,$014
jhi	L83
asl	r0
jmp	*.+4(r0)
L78
L83
L83
L83
L83
L83
L79
L80
L83
L83
L83
L81
L82
/esw
L76:L83:L74:jmp	rretrn
.globl	_chkfun
.text
_chkfun:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	2.(r4),r0
mov	r0,r3
bic	$!030,r0
cmp	$020,r0
jne	L85
mov	r4,(sp)
mov	4.(r4),-(sp)
mov	r3,-(sp)
jsr	pc,_incref
tst	(sp)+
mov	r0,-(sp)
mov	$043,-(sp)
mov	$01,-(sp)
jsr	pc,*$_block
add	$010,sp
jbr	L84
L85:mov	r4,r0
jbr	L84
L84:jmp	rretrn
.globl	_disarray
.text
_disarray:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	2.(r3),r0
mov	r0,r4
bic	$!030,r0
cmp	$030,r0
jne	L10018
cmp	(r3),$024
jne	L87
cmp	6.(r3),$04
jne	L87
L10018:mov	r3,r0
jbr	L86
L87:incb	4.(r3)
mov	_cp,r0
add	$02,_cp
mov	r3,(r0)
mov	$0177777,(sp)
mov	r4,-(sp)
jsr	pc,_decref
tst	(sp)+
mov	r0,-(sp)
mov	r3,-(sp)
jsr	pc,*$_setype
cmp	(sp)+,(sp)+
mov	$043,(sp)
jsr	pc,*$_build
sub	$02,_cp
mov	_cp,r0
mov	(r0),r0
jbr	L86
L86:jmp	rretrn
.globl	_chkw
.text
_chkw:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	2.(r0),r0
mov	r0,r4
cmp	$01,r0
jge	L89
cmp	r4,$010
jge	L89
.data
L90:.byte 0111,0156,0164,0145,0147,0145,0162,040,0157,0160,0145,0162,0141,0156,0144,040,0162,0145,0161,0165,0151,0162,0145,0144,0
.even
.text
mov	$L90,(sp)
jsr	pc,*$_error
L89:jbr	L88
L88:jmp	rretrn
.globl	_lintyp
.text
_lintyp:
jsr	r5,rsave; 2
mov	4.(r5),r4
cmp	r4,$010
jge	L92
mov	r4,r0
jbr	L91
L92:mov	r4,r0
bic	$!0170,r0
cmp	$0150,r0
jne	L93
mov	$011,r0
jbr	L91
L93:mov	r4,r0
bic	$!037,r0
cmp	r4,r0
jne	L94
mov	r4,r0
bic	$!07,r0
add	$05,r0
jbr	L91
L94:mov	$012,r0
jbr	L91
L91:jmp	rretrn
.globl	_error
.text
_error:
jsr	r5,rsave; 2
inc	_nerror
mov	$_ascbuf,(sp)
jsr	pc,*$_fflush
movb	_ascbuf,r4
movb	$01,_ascbuf
.data
L96:.byte 045,0144,072,040,0
.even
.text
mov	_line,(sp)
mov	$L96,-(sp)
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
mov	$_ascbuf,(sp)
jsr	pc,*$_fflush
movb	r4,_ascbuf
L95:jmp	rretrn
.globl	_block
.text
_block:
jsr	r5,rsave; 2
mov	r5,r0
add	$06,r0
mov	r0,r4
add	$03,4.(r5)
mov	_space,r3
L98:mov	4.(r5),r0
dec	4.(r5)
tst	r0
jeq	L99
mov	(r4)+,(sp)
jsr	pc,*$_pblock
jbr	L98
L99:mov	r3,r0
jbr	L97
L97:jmp	rretrn
.globl	_pblock
.text
_pblock:
jsr	r5,rsave; 2
mov	_space,r0
add	$02,_space
mov	4.(r5),(r0)
dec	_osleft
jgt	L101
.data
L102:.byte 0105,0170,0160,0162,0145,0163,0163,0151,0157,0156,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L102,(sp)
jsr	pc,*$_error
mov	$01,(sp)
jsr	pc,*$_exit
L101:L100:jmp	rretrn
.globl	_chklval
.text
_chklval:
jsr	r5,rsave; 2
mov	4.(r5),r4
cmp	(r4),$024
jeq	L104
cmp	(r4),$044
jeq	L104
.data
L105:.byte 0114,0166,0141,0154,0165,0145,040,0162,0145,0161,0165,0151,0162,0145,0144,0
.even
.text
mov	$L105,(sp)
jsr	pc,*$_error
L104:L103:jmp	rretrn
.globl	_fold
.text
_fold:
jsr	r5,rsave; 2
mov	6.(r5),r4
cmp	(r4),$025
jne	L10019
tst	8.(r5)
jeq	L107
cmp	*8.(r5),$025
jeq	L107
L10019:clr	r0
jbr	L106
L107:mov	6.(r4),r3
mov	8.(r5),r0
mov	6.(r0),r2
mov	4.(r5),r0
jbr	L109
L110:add	r2,r3
jbr	L108
L111:sub	r2,r3
jbr	L108
L112:mov	r3,r1
mul	r2,r1
mov	r1,r3
jbr	L108
L113:mov	r3,r1
sxt	r0
div	r2,r0
mov	r0,r3
jbr	L108
L114:mov	r3,r1
sxt	r0
div	r2,r0
mov	r1,r3
jbr	L108
L115:mov	r2,r0
com	r0
bic	r0,r3
jbr	L108
L116:bis	r2,r3
jbr	L108
L117:mov	r2,r0
xor	r0,r3
mov	r3,r0
jbr	L108
L118:mov	r3,r0
neg	r0
mov	r0,r3
jbr	L108
L119:mov	r3,r0
com	r0
mov	r0,r3
jbr	L108
L120:mov	r3,r0
ash	r2,r0
mov	r0,r3
jbr	L108
L121:mov	r2,-(sp)
mov	r3,r0
neg	(sp)
ash	(sp)+,r0
mov	r0,r3
jbr	L108
L122:clr	r0
jbr	L106
jbr	L108
L109:sub	$045,r0
cmp	r0,$014
jhi	L122
asl	r0
jmp	*.+4(r0)
L118
L119
L122
L110
L111
L112
L113
L114
L121
L120
L115
L116
L117
/esw
L108:mov	r3,6.(r4)
mov	_cp,r0
add	$02,_cp
mov	r4,(r0)
mov	$01,r0
jbr	L106
L106:jmp	rretrn
.globl	_conexp
.text
_conexp:
jsr	r5,rsave; 2
inc	_initflg
jsr	pc,_tree
mov	r0,r4
jeq	L124
cmp	(r4),$025
jeq	L125
.data
L126:.byte 0103,0157,0156,0163,0164,0141,0156,0164,040,0162,0145,0161,0165,0151,0162,0145,0144,0
.even
.text
mov	$L126,(sp)
jsr	pc,*$_error
L125:L124:dec	_initflg
mov	6.(r4),r0
jbr	L123
L123:jmp	rretrn
