.globl	_dargv
.data
_dargv=.
0.
.text
L1:.byte 057,0144,0145,0166,057,0162,0153,062,0
.even
.data
L1
.text
L2:.byte 057,0144,0145,0166,057,0162,0160,060,0
.even
.data
L2
0.
.globl	_bufa
.comm	_bufa,02
.globl	_sflg
.comm	_sflg,02
.text
L3:.byte 057,0164,0155,0160,057,0153,0164,0155,0160,0
.even
.data
.globl	_lfile
.data
_lfile=.
.text
_lfile=L3
.globl	_lflg
.comm	_lflg,02
.globl	_uflg
.comm	_uflg,02
.globl	_lfdes
.comm	_lfdes,02
.globl	_lfptr
.comm	_lfptr,02
.globl	_sblock
.comm	_sblock,01004
.globl	_bitcnt
.data
_bitcnt=.
.byte 4.
.byte 3.
.byte 3.
.byte 2.
.byte 3.
.byte 2.
.byte 2.
.byte 1.
.byte 3.
.byte 2.
.byte 2.
.byte 1.
.byte 2.
.byte 1.
.byte 1.
.byte 0.
.globl	_ndlist
.comm	_ndlist,02
.globl	_dlist
.comm	_dlist,0310
.globl	_great
.comm	_great,02
.globl	_nblist
.comm	_nblist,02
.globl	_blist
.comm	_blist,0310
.globl	_bmap
.data
_bmap=.
-1.
.globl	_fi
.comm	_fi,02
.globl	_nfile
.comm	_nfile,02
.globl	_nspcl
.comm	_nspcl,02
.globl	_nlarg
.comm	_nlarg,02
.globl	_nindir
.comm	_nindir,02
.globl	_ndir
.comm	_ndir,02
.globl	_nused
.comm	_nused,02
.globl	_hiwat
.comm	_hiwat,02
.globl	_nfree
.comm	_nfree,02
.globl	_ino
.comm	_ino,02
.globl	_ndup
.comm	_ndup,02
.globl	_nnf
.data
_nnf=.
100.
.globl	_icnt
.comm	_icnt,02
.globl	_ucnt
.comm	_ucnt,02
.globl	_dn
.comm	_dn,02
.globl	_edn
.comm	_edn,02
.globl	_ldivr
.comm	_ldivr,02
.globl	_fout
.comm	_fout,02
.globl	_main
.text
_main:
jsr	r5,rsave; 4
mov	4.(r5),r0
asl	r0
add	6.(r5),r0
clr	(r0)
cmp	4.(r5),$01
jne	L5
mov	$_dargv,6.(r5)
mov	$01,4.(r5)
L6:mov	4.(r5),r0
asl	r0
tst	_dargv(r0)
jeq	L7
L8:inc	4.(r5)
jbr	L6
L7:L5:cmp	4.(r5),$01
jle	L9
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jne	L9
dec	4.(r5)
add	$02,6.(r5)
mov	*6.(r5),-8.(r5)
L10:tstb	*-8.(r5)
jeq	L11
mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
jbr	L13
L14:inc	_uflg
L15:inc	_lflg
jbr	L10
L16:inc	_sflg
jbr	L10
L17:mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
mov	_ndlist,r1
inc	_ndlist
asl	r1
mov	r0,_dlist(r1)
dec	4.(r5)
add	$02,6.(r5)
jbr	L10
L18:mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
mov	_nblist,r1
inc	_nblist
asl	r1
mov	r0,_blist(r1)
dec	4.(r5)
add	$02,6.(r5)
jbr	L10
L19:mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
mov	r0,_great
dec	4.(r5)
add	$02,6.(r5)
jbr	L10
jbr	L12
L13:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:142
147
151
154
163
165
L10002:..
L18
L19
L17
L15
L16
L14
L20
.text
/esw
L12:L20:jbr	L10
L11:L9:tst	_lflg
jeq	L21
clr	_sflg
L21:mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
dec	4.(r5)
add	$02,6.(r5)
.data
L22:.byte 045,0163,072,012,0
.even
.text
mov	*6.(r5),(sp)
mov	$L22,-(sp)
jsr	pc,*$_fprintf
tst	(sp)+
mov	*6.(r5),(sp)
jsr	pc,*$_check
jsr	pc,_flush
mov	_fout,(sp)
jsr	pc,*$_close
mov	$01,_fout
cmp	4.(r5),$01
jle	L23
.data
L24:.byte 057,0142,0151,0156,057,0143,0150,0145,0143,0153,0
.even
.text
mov	6.(r5),(sp)
mov	$L24,-(sp)
jsr	pc,*$_execv
tst	(sp)+
L23:L4:jmp	rretrn
.globl	_check
.text
_check:
jsr	r5,rsave; 1004
clr	(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
tst	_fi
jge	L26
.data
L27:.byte 0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L27,-(sp)
jsr	pc,*$_fprintf
tst	(sp)+
jbr	L25
L26:jsr	pc,_sync
mov	$_sblock,(sp)
mov	$01,-(sp)
jsr	pc,*$_bread
tst	(sp)+
tst	_lflg
jeq	L28
tst	_uflg
jeq	L29
mov	_sblock,r0
asl	r0
mov	r0,(sp)
jsr	pc,*$_sbrk
mov	r0,_ucnt
mov	_ucnt,(sp)
jsr	pc,*$_bad
tst	r0
jne	L25
jbr	L30
L29:mov	$0666,(sp)
mov	$_lfile,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,_lfdes
mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
bit	$01,r0
jne	L31
mov	$_cintr,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L31:L30:mov	$01136,(sp)
jsr	pc,*$_sbrk
mov	r0,_edn
mov	r0,_dn
mov	_dn,(sp)
jsr	pc,*$_bad
tst	r0
jne	L25
jbr	L32
L28:mov	$020000,(sp)
jsr	pc,*$_sbrk
mov	r0,_bmap
mov	_sblock,r0
ash	$04,r0
mov	r0,(sp)
jsr	pc,*$_sbrk
mov	r0,_icnt
mov	_bmap,(sp)
jsr	pc,*$_bad
tst	r0
jne	L25
mov	_icnt,(sp)
jsr	pc,*$_bad
tst	r0
jne	L25
L32:clr	r4
L33:cmp	r4,_sblock
jhis	L34
mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	r4,-(sp)
add	$02,(sp)
jsr	pc,*$_bread
tst	(sp)+
clr	r3
L36:cmp	r3,$020
jge	L37
inc	_ino
mov	r3,r0
ash	$05,r0
mov	r0,(sp)
mov	r5,r0
add	$0176772,r0
add	r0,(sp)
jsr	pc,*$_pass1
L38:inc	r3
jbr	L36
L37:L35:inc	r4
jbr	L33
L34:tst	_uflg
jeq	L39
mov	$02,_ino
L40:mov	_sblock,r0
ash	$04,r0
cmp	_ino,r0
jhis	L41
mov	$010,(sp)
mov	_ino,-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
mov	_ucnt,r0
add	r4,r0
movb	(r0),r0
mov	$01,r1
ash	_ldivr,r1
bit	r1,r0
jeq	L43
clr	(sp)
mov	_ino,-(sp)
jsr	pc,*$_trace
tst	(sp)+
tst	r0
jne	L44
.data
L45:.byte 045,0154,012,0
.even
.text
mov	_ino,(sp)
mov	$L45,-(sp)
jsr	pc,*$_fprintf
tst	(sp)+
L44:L43:L42:inc	_ino
jbr	L40
L41:jbr	L25
L39:clr	_ino
tst	_lflg
jeq	L46
mov	_lfdes,(sp)
jsr	pc,*$_close
clr	(sp)
mov	$_lfile,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_lfdes
mov	$_compar,(sp)
mov	$06,-(sp)
mov	_edn,r1
sub	_dn,r1
sxt	r0
div	$06,r0
mov	r0,-(sp)
mov	_dn,-(sp)
jsr	pc,*$_qsort
add	$06,sp
mov	$0177777,r4
clr	r3
mov	_dn,-520.(r5)
L47:cmp	-520.(r5),_edn
jhis	L48
mov	-520.(r5),r0
tst	4.(r0)
jne	L50
cmp	*-520.(r5),$01
jeq	L51
mov	$01,r3
L51:jbr	L49
L50:.data
L52:.byte 045,064,0154,0
.even
.text
mov	*-520.(r5),(sp)
mov	$L52,-(sp)
jsr	pc,*$_printf
tst	(sp)+
cmp	*-520.(r5),r4
jne	L53
mov	$0154,(sp)
jsr	pc,*$_putchar
jbr	L54
L53:mov	$040,(sp)
jsr	pc,*$_putchar
L54:mov	*-520.(r5),r4
mov	$040,(sp)
jsr	pc,*$_putchar
cmp	_lflg,$01
jle	L55
mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	r4,-(sp)
jsr	pc,*$_pstat
tst	(sp)+
L55:clr	(sp)
mov	-520.(r5),-(sp)
jsr	pc,*$_pfilenam
tst	(sp)+
tst	r3
jeq	L56
clr	r3
.data
L57:.byte 057,056,0
.even
.text
mov	$L57,(sp)
jsr	pc,*$_printf
L56:mov	$012,(sp)
jsr	pc,*$_putchar
L49:add	$06,-520.(r5)
jbr	L47
L48:mov	_lfdes,(sp)
jsr	pc,*$_close
mov	$_lfile,(sp)
jsr	pc,*$_unlink
jbr	L25
L46:jsr	pc,_sync
mov	$_sblock,(sp)
mov	$01,-(sp)
jsr	pc,*$_bread
tst	(sp)+
tst	_sflg
jeq	L58
mov	_fi,(sp)
jsr	pc,*$_close
mov	$01,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
tst	_fi
jge	L59
.data
L60:.byte 0143,0141,0156,0156,0157,0164,040,0167,0162,0151,0164,0145,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L60,-(sp)
jsr	pc,*$_fprintf
tst	(sp)+
jbr	L25
L59:clr	4.+_sblock
clr	206.+_sblock
clrb	408.+_sblock
clrb	409.+_sblock
clrb	410.+_sblock
clr	(sp)
jsr	pc,*$_free
mov	2.+_sblock,r0
add	$0177777,r0
mov	r0,r4
L61:mov	_sblock,r0
add	$02,r0
cmp	r4,r0
jlo	L62
clr	_ndup
.data
L64:.byte 0125,0122,0113,0
.even
.text
clr	(sp)
mov	$L64,-(sp)
mov	r4,-(sp)
jsr	pc,*$_chk
cmp	(sp)+,(sp)+
tst	_ndup
jne	L65
mov	r4,(sp)
jsr	pc,*$_free
L65:L63:dec	r4
jbr	L61
L62:mov	$_sblock,(sp)
mov	$01,-(sp)
jsr	pc,*$_bwrite
tst	(sp)+
mov	_fi,(sp)
jsr	pc,*$_close
jsr	pc,_sync
jbr	L25
L58:L66:jsr	pc,_alloc
mov	r0,r4
jeq	L67
.data
L68:.byte 0146,0162,0145,0145,0
.even
.text
clr	(sp)
mov	$L68,-(sp)
mov	r4,-(sp)
jsr	pc,*$_chk
cmp	(sp)+,(sp)+
tst	r0
jne	L67
inc	_nfree
jbr	L66
L67:tst	_ndup
jeq	L69
.data
L70:.byte 045,0154,040,0144,0165,0160,0163,040,0151,0156,040,0146,0162,0145,0145,012,0
.even
.text
mov	_ndup,(sp)
mov	$L70,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L69:mov	2.+_sblock,r0
sub	_sblock,r0
add	$0177776,r0
mov	r0,_ndup
clr	r4
L71:cmp	r4,$020000
jge	L72
mov	_bmap,r0
add	r4,r0
movb	(r0),r3
mov	r3,r0
bic	$!017,r0
movb	_bitcnt(r0),r0
add	r0,_ndup
mov	r3,r0
ash	$-04,r0
bic	$!017,r0
movb	_bitcnt(r0),r0
add	r0,_ndup
L73:inc	r4
jbr	L71
L72:tst	_ndup
jeq	L74
.data
L75:.byte 045,0154,040,0155,0151,0163,0163,0151,0156,0147,012,0
.even
.text
mov	_ndup,(sp)
mov	$L75,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L74:clr	r4
L76:mov	_sblock,r0
ash	$04,r0
cmp	r4,r0
jhis	L77
mov	_icnt,r0
add	r4,r0
movb	(r0),r0
bic	$!0377,r0
mov	r0,r3
tst	r3
jeq	L79
cmp	r3,$0200
jeq	L79
.data
L80:.byte 045,066,0154,040,045,063,0157,012,0
.even
.text
mov	r3,(sp)
mov	r4,-(sp)
inc	(sp)
mov	$L80,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L79:L78:inc	r4
jbr	L76
L77:.data
L81:.byte 0163,0160,0143,0154,040,040,045,066,0154,012,0
.even
.text
mov	_nspcl,(sp)
mov	$L81,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L82:.byte 0146,0151,0154,0145,0163,040,045,066,0154,012,0
.even
.text
mov	_nfile,(sp)
mov	$L82,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L83:.byte 0154,0141,0162,0147,0145,040,045,066,0154,012,0
.even
.text
mov	_nlarg,(sp)
mov	$L83,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L84:.byte 0144,0151,0162,0145,0143,040,045,066,0154,012,0
.even
.text
mov	_ndir,(sp)
mov	$L84,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L85:.byte 0151,0156,0144,0151,0162,040,045,066,0154,012,0
.even
.text
mov	_nindir,(sp)
mov	$L85,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L86:.byte 0165,0163,0145,0144,040,040,045,066,0154,012,0
.even
.text
mov	_nused,(sp)
mov	$L86,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L87:.byte 0154,0141,0163,0164,040,040,045,066,0154,012,0
.even
.text
mov	_hiwat,(sp)
mov	$L87,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L88:.byte 0146,0162,0145,0145,040,040,045,066,0154,012,0
.even
.text
mov	_nfree,(sp)
mov	$L88,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	_fi,(sp)
jsr	pc,*$_close
L25:jmp	rretrn
.globl	_cintr
.text
_cintr:
jsr	r5,rsave; 2
mov	$_lfile,(sp)
jsr	pc,*$_unlink
jsr	pc,_exit
L89:jmp	rretrn
.globl	_compar
.text
_compar:
jsr	r5,rsave; 4
mov	*4.(r5),r0
sub	*6.(r5),r0
mov	r0,-8.(r5)
tst	-8.(r5)
jeq	L91
mov	-8.(r5),r0
jbr	L90
L91:mov	4.(r5),r0
mov	4.(r0),r0
mov	6.(r5),r1
sub	4.(r1),r0
jbr	L90
L90:jmp	rretrn
.globl	_pass1
.text
_pass1:
jsr	r5,rsave; 1002
bit	$0100000,*4.(r5)
jeq	L92
tst	_uflg
jeq	L93
mov	$010,(sp)
mov	_ino,-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
mov	$01,r0
ash	_ldivr,r0
mov	_ucnt,r1
add	r4,r1
bisb	r0,(r1)
L93:tst	_lflg
jne	L94
mov	$0100,-(sp)
mov	_icnt,r0
add	_ino,r0
movb	-1.(r0),r1
add	(sp)+,r1
movb	r1,-1.(r0)
mov	4.(r5),r0
tstb	2.(r0)
jeq	L95
mov	4.(r5),r0
movb	2.(r0),r0
mov	r0,-(sp)
add	$0100,(sp)
mov	_icnt,r0
add	_ino,r0
movb	-1.(r0),r1
add	(sp)+,r1
movb	r1,-1.(r0)
L95:L94:bit	$020000,*4.(r5)
jeq	L96
inc	_nspcl
jbr	L92
L96:clr	r2
mov	*4.(r5),r0
bic	$!060000,r0
cmp	$040000,r0
jne	L97
mov	$020,(sp)
mov	4.(r5),r0
mov	6.(r0),-(sp)
mov	4.(r5),r0
movb	5.(r0),r0
mov	r0,-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r2
tst	_lflg
jeq	L98
tst	_uflg
jne	L98
dec	_nnf
jge	L99
mov	$0144,_nnf
mov	$01136,(sp)
jsr	pc,*$_sbrk
mov	r0,_edn
mov	_edn,(sp)
jsr	pc,*$_bad
tst	r0
jeq	L100
clr	_lflg
L100:L99:mov	_edn,r0
clr	4.(r0)
mov	_ino,*_edn
add	$06,_edn
L98:inc	_ndir
jbr	L101
L97:tst	_lflg
jne	L92
L101:inc	_nfile
bit	$010000,*4.(r5)
jeq	L102
inc	_nlarg
clr	r4
L103:cmp	r4,$010
jge	L104
mov	r4,r0
asl	r0
add	4.(r5),r0
tst	8.(r0)
jeq	L106
inc	_nindir
.data
L107:.byte 0151,0144,0151,0162,0
.even
.text
clr	(sp)
mov	$L107,-(sp)
mov	r4,r0
asl	r0
add	4.(r5),r0
mov	8.(r0),-(sp)
jsr	pc,*$_chk
cmp	(sp)+,(sp)+
tst	r0
jne	L105
mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	r4,r0
asl	r0
add	4.(r5),r0
mov	8.(r0),-(sp)
jsr	pc,*$_bread
tst	(sp)+
clr	r3
L108:cmp	r3,$0400
jge	L109
mov	r3,r0
asl	r0
add	r5,r0
tst	-518.(r0)
jeq	L111
.data
L112:.byte 0154,0144,0151,0162,0
.even
.text
mov	r2,(sp)
mov	$L112,-(sp)
mov	r3,r0
asl	r0
add	r5,r0
mov	-518.(r0),-(sp)
jsr	pc,*$_chk
cmp	(sp)+,(sp)+
L111:L110:inc	r3
jbr	L108
L109:sub	$040,r2
L106:L105:inc	r4
jbr	L103
L104:jbr	L92
L102:clr	r4
L113:cmp	r4,$010
jge	L114
mov	r4,r0
asl	r0
add	4.(r5),r0
tst	8.(r0)
jeq	L116
.data
L117:.byte 0163,0144,0151,0162,0
.even
.text
mov	r2,(sp)
mov	$L117,-(sp)
mov	r4,r0
asl	r0
add	4.(r5),r0
mov	8.(r0),-(sp)
jsr	pc,*$_chk
cmp	(sp)+,(sp)+
L116:sub	$040,r2
L115:inc	r4
jbr	L113
L114:L92:jmp	rretrn
.globl	_chk
.text
_chk:
jsr	r5,rsave; 1004
mov	4.(r5),r4
clr	r2
L119:cmp	r2,_nblist
jge	L120
mov	r2,r0
asl	r0
cmp	_blist(r0),r4
jne	L122
.data
L123:.byte 045,0154,040,0142,0154,0153,073,040,0151,075,045,0154,050,045,0163,051,012,0
.even
.text
mov	6.(r5),(sp)
mov	_ino,-(sp)
mov	r4,-(sp)
mov	$L123,-(sp)
jsr	pc,*$_fprintf
add	$06,sp
L122:L121:inc	r2
jbr	L119
L120:tst	_great
jeq	L124
cmp	r4,_great
jlo	L124
tst	r4
jeq	L124
.data
L125:.byte 045,0154,040,0147,0145,0161,073,040,0151,075,045,0154,050,045,0163,051,012,0
.even
.text
mov	6.(r5),(sp)
mov	_ino,-(sp)
mov	r4,-(sp)
mov	$L125,-(sp)
jsr	pc,*$_fprintf
add	$06,sp
L124:tst	_ino
jeq	L126
inc	_nused
cmp	r4,_hiwat
jlos	L127
mov	r4,_hiwat
L127:L126:mov	_sblock,r0
add	$02,r0
cmp	r4,r0
jlo	L10004
cmp	r4,2.+_sblock
jlo	L128
L10004:.data
L129:.byte 045,0154,040,0142,0141,0144,073,040,0151,075,045,0154,050,045,0163,051,012,0
.even
.text
mov	6.(r5),(sp)
mov	_ino,-(sp)
mov	r4,-(sp)
mov	$L129,-(sp)
jsr	pc,*$_fprintf
add	$06,sp
mov	$01,r0
jbr	L118
L128:mov	$010,(sp)
mov	r4,-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r3
tst	_lflg
jne	L130
mov	$01,r0
ash	_ldivr,r0
mov	r0,r2
mov	_bmap,r0
add	r3,r0
bitb	r2,(r0)
jeq	L131
tst	_ino
jne	L132
inc	_ndup
clr	r0
jbr	L118
L132:.data
L133:.byte 045,0154,040,0144,0165,0160,073,040,0151,075,045,0154,050,045,0163,051,012,0
.even
.text
mov	6.(r5),(sp)
mov	_ino,-(sp)
mov	r4,-(sp)
mov	$L133,-(sp)
jsr	pc,*$_fprintf
add	$06,sp
mov	$01,r0
jbr	L118
L131:mov	_bmap,r0
add	r3,r0
bisb	r2,(r0)
L130:tst	8.(r5)
jle	L134
mov	r5,r0
add	$0176770,r0
mov	r0,(sp)
mov	r4,-(sp)
jsr	pc,*$_bread
tst	(sp)+
clr	r3
L135:cmp	r3,$0400
jge	L136
tst	8.(r5)
jle	L136
dec	8.(r5)
mov	r3,r0
asl	r0
add	r5,r0
mov	-520.(r0),-8.(r5)
jeq	L137
tst	_lflg
jeq	L138
mov	r3,r0
asl	r0
add	r5,r0
movb	-518.(r0),r0
cmp	$056,r0
jne	L10005
mov	r3,r0
asl	r0
add	r5,r0
tstb	-517.(r0)
jeq	L139
mov	r3,r0
asl	r0
add	r5,r0
movb	-517.(r0),r0
cmp	$056,r0
jne	L10006
mov	r3,r0
asl	r0
add	r5,r0
tstb	-516.(r0)
jeq	L139
L10006:L10005:dec	_nnf
jge	L140
mov	$0144,_nnf
mov	$01136,(sp)
jsr	pc,*$_sbrk
mov	r0,_edn
mov	_edn,(sp)
jsr	pc,*$_bad
tst	r0
jeq	L141
clr	_lflg
L141:L140:mov	_edn,r0
mov	_ino,4.(r0)
mov	r3,r0
asl	r0
add	r5,r0
mov	-520.(r0),*_edn
mov	_edn,r0
mov	_lfptr,2.(r0)
clr	r2
L142:cmp	r2,$016
jge	L143
mov	r3,r0
asl	r0
add	r2,r0
add	r5,r0
tstb	-518.(r0)
jeq	L143
L144:inc	r2
jbr	L142
L143:inc	r2
tst	_uflg
jne	L145
mov	r2,(sp)
mov	r3,r0
asl	r0
mov	r0,-(sp)
mov	r5,r0
add	$0176772,r0
add	r0,(sp)
mov	_lfdes,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L145:add	r2,_lfptr
add	$06,_edn
L139:L138:clr	r2
L146:cmp	r2,_ndlist
jge	L147
mov	r2,r0
asl	r0
cmp	_dlist(r0),-8.(r5)
jne	L149
.data
L150:.byte 045,0154,040,0151,0156,0157,073,040,0151,075,045,0154,050,045,0163,051,040,042,045,056,061,066,0163,042,012,0
.even
.text
mov	r3,r0
asl	r0
mov	r0,(sp)
mov	r5,r0
add	$0176772,r0
add	r0,(sp)
mov	6.(r5),-(sp)
mov	_ino,-(sp)
mov	-8.(r5),-(sp)
mov	$L150,-(sp)
jsr	pc,*$_fprintf
add	$010,sp
L149:L148:inc	r2
jbr	L146
L147:cmp	-8.(r5),$01
jlt	L10007
mov	_sblock,r0
ash	$04,r0
cmp	-8.(r5),r0
jle	L151
L10007:.data
L152:.byte 045,0154,040,0144,0151,0156,073,040,0151,075,045,0154,050,045,0163,051,012,0
.even
.text
mov	6.(r5),(sp)
mov	_ino,-(sp)
mov	r4,-(sp)
mov	$L152,-(sp)
jsr	pc,*$_fprintf
add	$06,sp
jbr	L137
L151:tst	_lflg
jne	L153
mov	_icnt,r0
add	-8.(r5),r0
decb	-1.(r0)
L153:L137:add	$010,r3
jbr	L135
L136:L134:clr	r0
jbr	L118
L118:jmp	rretrn
.globl	_pfilenam
.text
_pfilenam:
jsr	r5,rsave; 20
inc	6.(r5)
mov	6.(r5),r0
cmp	$012,r0
jge	L155
mov	$056,(sp)
jsr	pc,*$_putchar
mov	$056,(sp)
jsr	pc,*$_putchar
mov	$056,(sp)
jsr	pc,*$_putchar
jbr	L156
L155:mov	4.(r5),r0
cmp	4.(r0),$01
jeq	L157
mov	_dn,r4
L158:cmp	r4,_edn
jhis	L159
mov	r4,r0
mov	4.(r5),r1
cmp	(r0),4.(r1)
jne	L161
tst	4.(r4)
jeq	L161
mov	6.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_pfilenam
tst	(sp)+
jbr	L162
L161:L160:add	$06,r4
jbr	L158
L159:mov	$077,(sp)
jsr	pc,*$_putchar
mov	$077,(sp)
jsr	pc,*$_putchar
mov	$077,(sp)
jsr	pc,*$_putchar
L162:mov	$057,(sp)
jsr	pc,*$_putchar
L157:L156:clr	(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	_lfdes,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$016,(sp)
mov	r5,r0
add	$0177754,r0
mov	r0,-(sp)
mov	_lfdes,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
.data
L163:.byte 045,056,061,064,0163,0
.even
.text
mov	r5,r0
add	$0177754,r0
mov	r0,(sp)
mov	$L163,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L154:jmp	rretrn
.globl	_pstat
.text
_pstat:
jsr	r5,rsave; 2
mov	$020,(sp)
mov	4.(r5),-(sp)
add	$037,(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
cmp	r4,_bufa
jeq	L165
mov	r4,_bufa
mov	6.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_bread
tst	(sp)+
L165:mov	_ldivr,r0
ash	$05,r0
add	6.(r5),r0
mov	r0,r3
.data
L166:.byte 045,064,0157,040,0
.even
.text
mov	(r3),(sp)
bic	$!07777,(sp)
mov	$L166,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	r3,(sp)
add	$030,(sp)
jsr	pc,*$_pdate
mov	r3,(sp)
add	$034,(sp)
jsr	pc,*$_pdate
L164:jmp	rretrn
.globl	_pdate
.text
_pdate:
jsr	r5,rsave; 2
mov	4.(r5),(sp)
jsr	pc,*$_localtim
mov	r0,r4
mov	$057,(sp)
mov	10.(r4),-(sp)
jsr	pc,*$_p2dig
tst	(sp)+
mov	$057,(sp)
mov	8.(r4),-(sp)
inc	(sp)
jsr	pc,*$_p2dig
tst	(sp)+
mov	$040,(sp)
mov	6.(r4),-(sp)
jsr	pc,*$_p2dig
tst	(sp)+
mov	$072,(sp)
mov	4.(r4),-(sp)
jsr	pc,*$_p2dig
tst	(sp)+
mov	$040,(sp)
mov	2.(r4),-(sp)
jsr	pc,*$_p2dig
tst	(sp)+
L167:jmp	rretrn
.globl	_p2dig
.text
_p2dig:
jsr	r5,rsave; 2
mov	4.(r5),r1
sxt	r0
div	$012,r0
mov	r0,(sp)
add	$060,(sp)
jsr	pc,*$_putchar
mov	4.(r5),r1
sxt	r0
div	$012,r0
mov	r1,(sp)
add	$060,(sp)
jsr	pc,*$_putchar
mov	6.(r5),(sp)
jsr	pc,*$_putchar
L168:jmp	rretrn
.globl	_alloc
.text
_alloc:
jsr	r5,rsave; 1002
dec	4.+_sblock
mov	4.+_sblock,r0
mov	r0,r3
tst	r3
jlt	L10008
cmp	r3,$0144
jlt	L170
L10008:.data
L171:.byte 0142,0141,0144,040,0146,0162,0145,0145,0142,0154,0157,0143,0153,012,0
.even
.text
mov	$L171,(sp)
jsr	pc,*$_fprintf
clr	r0
jbr	L169
L170:mov	r3,r0
asl	r0
mov	6.+_sblock(r0),r4
tst	r4
jne	L172
clr	r0
jbr	L169
L172:tst	4.+_sblock
jgt	L173
mov	r5,r0
add	$0176772,r0
mov	r0,(sp)
mov	r4,-(sp)
jsr	pc,*$_bread
tst	(sp)+
mov	-518.(r5),4.+_sblock
clr	r3
L174:cmp	r3,$0144
jge	L175
mov	r3,r0
asl	r0
mov	r3,r1
asl	r1
add	r5,r1
mov	-516.(r1),6.+_sblock(r0)
L176:inc	r3
jbr	L174
L175:L173:mov	r4,r0
jbr	L169
L169:jmp	rretrn
.globl	_bread
.text
_bread:
jsr	r5,rsave; 2
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L178
.data
L179:.byte 0162,0145,0141,0144,040,0145,0162,0162,0157,0162,040,045,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L179,-(sp)
jsr	pc,*$_fprintf
tst	(sp)+
jsr	pc,_exit
L178:L177:jmp	rretrn
.globl	_free
.text
_free:
jsr	r5,rsave; 1004
cmp	4.+_sblock,$0144
jlt	L181
mov	4.+_sblock,-520.(r5)
clr	-8.(r5)
L182:cmp	-8.(r5),$0144
jge	L183
mov	-8.(r5),r0
asl	r0
add	r5,r0
mov	-8.(r5),r1
asl	r1
mov	6.+_sblock(r1),-518.(r0)
L184:inc	-8.(r5)
jbr	L182
L183:clr	4.+_sblock
mov	r5,r0
add	$0176770,r0
mov	r0,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_bwrite
tst	(sp)+
L181:mov	4.+_sblock,r0
inc	4.+_sblock
asl	r0
mov	4.(r5),6.+_sblock(r0)
L180:jmp	rretrn
.globl	_bwrite
.text
_bwrite:
jsr	r5,rsave; 2
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fi,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
cmp	$01000,r0
jeq	L186
.data
L187:.byte 0167,0162,0151,0164,0145,040,0145,0162,0162,0157,0162,040,045,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L187,-(sp)
jsr	pc,*$_fprintf
tst	(sp)+
jsr	pc,_exit
L186:L185:jmp	rretrn
.globl	_number
.text
_number:
jsr	r5,rsave; 6
clr	-8.(r5)
L189:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
mov	r0,-10.(r5)
jeq	L190
cmp	-10.(r5),$060
jlt	L189
cmp	-10.(r5),$071
jgt	L189
mov	-8.(r5),r1
mul	$012,r1
add	-10.(r5),r1
add	$0177720,r1
mov	r1,-8.(r5)
jbr	L189
L190:mov	-8.(r5),r0
jbr	L188
L188:jmp	rretrn
.globl	_fprintf
.text
_fprintf:
jsr	r5,rsave; 2
mov	12.(r5),(sp)
mov	10.(r5),-(sp)
mov	8.(r5),-(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_printf
add	$010,sp
jsr	pc,_flush
L191:jmp	rretrn
.globl	_bad
.text
_bad:
jsr	r5,rsave; 2
cmp	4.(r5),$0177777
jne	L193
.data
L194:.byte 0157,0165,0164,040,0157,0146,040,0155,0145,0155,0157,0162,0171,012,0
.even
.text
mov	$L194,(sp)
jsr	pc,*$_fprintf
mov	$01,r0
jbr	L192
L193:clr	r0
jbr	L192
L192:jmp	rretrn
.globl	_trace
.text
_trace:
jsr	r5,rsave; 2
mov	4.(r5),r3
inc	6.(r5)
cmp	6.(r5),$012
jle	L196
clr	r0
jbr	L195
L196:mov	_dn,r4
L197:cmp	r4,_edn
jhis	L198
cmp	(r4),r3
jne	L200
mov	4.(r4),r2
tst	r2
jeq	L10009
cmp	r2,$01
jne	L201
L10009:mov	r2,r0
jbr	L195
L201:mov	6.(r5),(sp)
mov	r2,-(sp)
jsr	pc,*$_trace
tst	(sp)+
cmp	$01,r0
jne	L202
mov	$01,(sp)
mov	r2,-(sp)
jsr	pc,*$_setpn
tst	(sp)+
mov	$01,r0
jbr	L195
L202:L200:L199:add	$06,r4
jbr	L197
L198:clr	(sp)
mov	r3,-(sp)
jsr	pc,*$_setpn
tst	(sp)+
clr	r0
jbr	L195
L195:jmp	rretrn
.globl	_setpn
.text
_setpn:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	6.(r5),r2
mov	_dn,r4
L204:cmp	r4,_edn
jhis	L205
cmp	4.(r4),r3
jne	L207
mov	r2,4.(r4)
L207:L206:add	$06,r4
jbr	L204
L205:L203:jmp	rretrn
