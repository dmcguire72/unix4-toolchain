.globl	_inf
.comm	_inf,01006
.globl	_aflg
.comm	_aflg,02
.globl	_dflg
.comm	_dflg,02
.globl	_lflg
.comm	_lflg,02
.globl	_sflg
.comm	_sflg,02
.globl	_tflg
.comm	_tflg,02
.globl	_uflg
.comm	_uflg,02
.globl	_iflg
.comm	_iflg,02
.globl	_fflg
.comm	_fflg,02
.globl	_fout
.comm	_fout,02
.globl	_rflg
.data
_rflg=.
1.
.globl	_flags
.comm	_flags,02
.globl	_uidfil
.data
_uidfil=.
-1.
.globl	_tblocks
.comm	_tblocks,02
.globl	_statreq
.comm	_statreq,02
.globl	_lastp
.data
_lastp=.
_end
.globl	_rlastp
.data
_rlastp=.
_end
.text
L1:.byte 056,0
.even
.data
.globl	_dotp
.data
_dotp=.
L1
.globl	_main
.text
_main:
jsr	r5,rsave; 42
mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
dec	4.(r5)
jle	L3
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jne	L3
add	$02,6.(r5)
L4:inc	*6.(r5)
mov	*6.(r5),r0
tstb	(r0)
jeq	L5
mov	6.(r5),r0
movb	*(r0),r0
jbr	L7
L8:inc	_aflg
jbr	L4
L9:inc	_sflg
inc	_statreq
jbr	L4
L10:inc	_dflg
jbr	L4
L11:inc	_lflg
inc	_statreq
.data
L12:.byte 057,0145,0164,0143,057,0160,0141,0163,0163,0167,0144,0
.even
.text
clr	(sp)
mov	$L12,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_uidfil
jbr	L4
L13:mov	$0177777,_rflg
jbr	L4
L14:inc	_tflg
inc	_statreq
jbr	L4
L15:inc	_uflg
jbr	L4
L16:inc	_iflg
jbr	L4
L17:inc	_fflg
jbr	L4
L18:jbr	L4
jbr	L6
L7:sub	$0141,r0
cmp	r0,$024
jhi	L18
asl	r0
jmp	*.+4(r0)
L8
L18
L18
L10
L18
L17
L18
L18
L16
L18
L18
L11
L18
L18
L18
L18
L18
L13
L9
L14
L15
/esw
L6:jbr	L4
L5:dec	4.(r5)
L3:tst	_fflg
jeq	L19
inc	_aflg
clr	_lflg
clr	_sflg
clr	_tflg
clr	_statreq
L19:tst	4.(r5)
jne	L20
inc	4.(r5)
mov	$-2.+_dotp,6.(r5)
L20:clr	-8.(r5)
L21:cmp	-8.(r5),4.(r5)
jge	L22
mov	$01,(sp)
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_gstat
tst	(sp)+
mov	r0,r4
jeq	L23
mov	*6.(r5),(r4)
bis	$01000,16.(r4)
L23:inc	-8.(r5)
jbr	L21
L22:mov	$_compar,(sp)
mov	$034,-(sp)
mov	_lastp,r1
sub	$_end,r1
sxt	r0
div	$034,r0
mov	r0,-(sp)
mov	$_end,-(sp)
jsr	pc,*$_qsort
add	$06,sp
mov	_lastp,r2
mov	$_end,r4
L24:cmp	r4,r2
jhis	L25
bit	$0100000,16.(r4)
jeq	L10002
tst	_dflg
jeq	L10001
L10002:tst	_fflg
jeq	L27
L10001:cmp	4.(r5),$01
jle	L28
.data
L29:.byte 012,045,0163,072,012,0
.even
.text
mov	(r4),(sp)
mov	$L29,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L28:mov	r2,_lastp
mov	(r4),(sp)
jsr	pc,*$_readdir
tst	_fflg
jne	L30
mov	$_compar,(sp)
mov	$034,-(sp)
mov	_lastp,r1
sub	r2,r1
sxt	r0
div	$034,r0
mov	r0,-(sp)
mov	r2,-(sp)
jsr	pc,*$_qsort
add	$06,sp
L30:tst	_statreq
jeq	L31
.data
L32:.byte 0164,0157,0164,0141,0154,040,045,0144,012,0
.even
.text
mov	_tblocks,(sp)
mov	$L32,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L31:mov	r2,r3
L33:cmp	r3,_lastp
jhis	L34
mov	r3,(sp)
jsr	pc,*$_pentry
L35:add	$034,r3
jbr	L33
L34:jbr	L36
L27:mov	r4,(sp)
jsr	pc,*$_pentry
L36:L26:add	$034,r4
jbr	L24
L25:jsr	pc,_flush
L2:jmp	rretrn
.globl	_pentry
.text
_pentry:
jsr	r5,rsave; 22
mov	4.(r5),r4
cmp	14.(r4),$0177777
jeq	L37
tst	_iflg
jeq	L38
.data
L39:.byte 045,065,0144,040,0
.even
.text
mov	14.(r4),(sp)
mov	$L39,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L38:tst	_lflg
jeq	L40
mov	16.(r4),(sp)
jsr	pc,*$_pmode
.data
L41:.byte 045,062,0144,040,0
.even
.text
mov	r4,r0
movb	18.(r0),r0
mov	r0,(sp)
mov	$L41,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	r5,r0
add	$0177752,r0
mov	r0,(sp)
mov	r4,r0
movb	19.(r0),r0
mov	r0,-(sp)
bic	$!0377,(sp)
jsr	pc,*$_getname
tst	(sp)+
tst	r0
jne	L42
.data
L43:.byte 045,055,066,056,066,0163,0
.even
.text
mov	r5,r0
add	$0177752,r0
mov	r0,(sp)
mov	$L43,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L44
L42:.data
L45:.byte 045,055,066,0144,0
.even
.text
mov	r4,r0
movb	19.(r0),r0
mov	r0,(sp)
bic	$!0377,(sp)
mov	$L45,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L44:bit	$060000,16.(r4)
jeq	L46
.data
L47:.byte 045,063,0144,054,045,063,0144,0
.even
.text
mov	r4,r0
movb	22.(r0),r0
mov	r0,(sp)
bic	$!0377,(sp)
mov	r4,r0
movb	23.(r0),r0
mov	r0,-(sp)
bic	$!0377,(sp)
mov	$L47,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L48
L46:.data
L49:.byte 045,067,0163,0
.even
.text
mov	22.(r4),(sp)
mov	r4,r0
movb	21.(r0),r0
mov	r0,-(sp)
jsr	pc,*$_locv
tst	(sp)+
mov	r0,(sp)
mov	$L49,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L48:.data
L50:.byte 040,045,055,061,062,056,061,062,0163,040,0
.even
.text
mov	r4,(sp)
add	$030,(sp)
jsr	pc,*$_ctime
mov	r0,(sp)
add	$04,(sp)
mov	$L50,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L51
L40:tst	_sflg
jeq	L52
.data
L53:.byte 045,064,0144,040,0
.even
.text
mov	22.(r4),(sp)
mov	r4,r0
movb	21.(r0),r0
mov	r0,-(sp)
jsr	pc,*$_nblock
tst	(sp)+
mov	r0,(sp)
mov	$L53,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L52:L51:bit	$01000,16.(r4)
jeq	L54
.data
L55:.byte 045,0163,012,0
.even
.text
mov	(r4),(sp)
mov	$L55,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L56
L54:.data
L57:.byte 045,056,061,064,0163,012,0
.even
.text
mov	r4,(sp)
mov	$L57,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L56:L37:jmp	rretrn
.globl	_getname
.text
_getname:
jsr	r5,rsave; 12
mov	_uidfil,_inf
clr	(sp)
clr	-(sp)
mov	_inf,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
clr	2.+_inf
L61:clr	-14.(r5)
clr	-8.(r5)
clr	-12.(r5)
L62:mov	$_inf,(sp)
jsr	pc,*$_getc
mov	r0,-10.(r5)
cmp	$012,r0
jeq	L63
tst	-10.(r5)
jge	L64
mov	$0177777,r0
jbr	L58
L64:cmp	-10.(r5),$072
jne	L65
inc	-8.(r5)
mov	$060,-10.(r5)
L65:tst	-8.(r5)
jne	L66
mov	-14.(r5),r0
inc	-14.(r5)
add	6.(r5),r0
movb	-10.(r5),(r0)
L66:cmp	-8.(r5),$02
jne	L67
mov	-12.(r5),r1
mul	$012,r1
add	-10.(r5),r1
add	$0177720,r1
mov	r1,-12.(r5)
L67:jbr	L62
L63:L59:cmp	-12.(r5),4.(r5)
jne	L61
L60:mov	-14.(r5),r0
inc	-14.(r5)
add	6.(r5),r0
clrb	(r0)
clr	r0
jbr	L58
L58:jmp	rretrn
.globl	_nblock
.text
_nblock:
jsr	r5,rsave; 2
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
bit	$0777,6.(r5)
jeq	L69
inc	r4
L69:cmp	r4,$010
jle	L70
mov	r4,r0
add	$0377,r0
ash	$-010,r0
add	r0,r4
L70:mov	r4,r0
jbr	L68
L68:jmp	rretrn
.globl	_m0
.data
_m0=.
3.
-32768.
100.
16384.
98.
8192.
99.
45.
.globl	_m1
.data
_m1=.
1.
256.
114.
45.
.globl	_m2
.data
_m2=.
1.
128.
119.
45.
.globl	_m3
.data
_m3=.
2.
2048.
115.
64.
120.
45.
.globl	_m4
.data
_m4=.
1.
32.
114.
45.
.globl	_m5
.data
_m5=.
1.
16.
119.
45.
.globl	_m6
.data
_m6=.
2.
1024.
115.
8.
120.
45.
.globl	_m7
.data
_m7=.
1.
4.
114.
45.
.globl	_m8
.data
_m8=.
1.
2.
119.
45.
.globl	_m9
.data
_m9=.
1.
1.
120.
45.
.globl	_m
.data
_m=.
_m0
_m1
_m2
_m3
_m4
_m5
_m6
_m7
_m8
_m9
.globl	_pmode
.text
_pmode:
jsr	r5,rsave; 2
mov	4.(r5),_flags
mov	$_m,r4
L72:cmp	r4,$20.+_m
jhis	L73
mov	(r4)+,(sp)
jsr	pc,*$_select
jbr	L72
L73:L71:jmp	rretrn
.globl	_select
.text
_select:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	(r3)+,r4
L75:dec	r4
jlt	L76
bit	_flags,(r3)+
jne	L76
mov	(r3)+,r0
jbr	L75
L76:mov	(r3),(sp)
jsr	pc,*$_putchar
L74:jmp	rretrn
.globl	_makename
.text
_makename:
.bss
L78:.=.+100.
.text
jsr	r5,rsave; 2
mov	$L78,r4
mov	4.(r5),r3
L79:tstb	(r3)
jeq	L80
movb	(r3)+,(r4)+
jbr	L79
L80:movb	$057,(r4)+
mov	6.(r5),r3
clr	r2
L81:cmp	r2,$016
jge	L82
movb	(r3)+,(r4)+
L83:inc	r2
jbr	L81
L82:clrb	(r4)
mov	$L78,r0
jbr	L77
L77:jmp	rretrn
.globl	_readdir
.text
_readdir:
.bss
L85:.=.+16.
.text
jsr	r5,rsave; 2
mov	$_inf,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L86
.data
L87:.byte 045,0163,040,0165,0156,0162,0145,0141,0144,0141,0142,0154,0145,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L87,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L84
L86:clr	_tblocks
L88:mov	$L85,r4
clr	r3
L90:cmp	r3,$020
jge	L91
mov	$_inf,(sp)
jsr	pc,*$_getc
movb	r0,(r4)+
L92:inc	r3
jbr	L90
L91:tst	L85
jeq	L88
tst	_aflg
jne	L10003
cmpb	2.+L85,$056
jeq	L88
L10003:cmp	L85,$0177777
jeq	L89
clr	(sp)
mov	$2.+L85,-(sp)
mov	4.(r5),-(sp)
jsr	pc,_makename
cmp	(sp)+,(sp)+
mov	r0,-(sp)
jsr	pc,*$_gstat
tst	(sp)+
mov	r0,r2
cmp	14.(r2),$0177777
jeq	L93
mov	L85,14.(r2)
L93:clr	r3
L94:cmp	r3,$016
jge	L95
mov	r2,r0
add	r3,r0
movb	2.+L85(r3),(r0)
L96:inc	r3
jbr	L94
L95:jbr	L88
L89:mov	_inf,(sp)
jsr	pc,*$_close
L84:jmp	rretrn
.globl	_gstat
.text
_gstat:
jsr	r5,rsave; 46
mov	_lastp,r0
add	$034,r0
cmp	_rlastp,r0
jhi	L98
mov	$01000,(sp)
jsr	pc,*$_sbrk
add	$01000,_rlastp
L98:mov	_lastp,r4
add	$034,_lastp
clr	16.(r4)
clr	14.(r4)
tst	6.(r5)
jne	L10004
tst	_statreq
jeq	L99
L10004:mov	r5,r0
add	$0177726,r0
mov	r0,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jge	L100
.data
L101:.byte 045,0163,040,0156,0157,0164,040,0146,0157,0165,0156,0144,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L101,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$0177777,2.+-42.(r5)
clrb	9.+-42.(r5)
clr	10.+-42.(r5)
clr	4.+-42.(r5)
tst	6.(r5)
jeq	L102
sub	$034,_lastp
clr	r0
jbr	L97
L102:L100:mov	2.+-42.(r5),14.(r4)
bic	$!076777,4.+-42.(r5)
mov	4.+-42.(r5),r0
bic	$!060000,r0
cmp	$060000,r0
jne	L103
bic	$!0157777,4.+-42.(r5)
jbr	L104
L103:mov	4.+-42.(r5),r0
bic	$!060000,r0
cmp	$040000,r0
jne	L105
bic	$!0117777,4.+-42.(r5)
bis	$0100000,4.+-42.(r5)
L105:L104:mov	4.+-42.(r5),16.(r4)
movb	7.+-42.(r5),19.(r4)
movb	8.+-42.(r5),20.(r4)
movb	6.+-42.(r5),18.(r4)
movb	9.+-42.(r5),21.(r4)
mov	10.+-42.(r5),22.(r4)
bit	$060000,16.(r4)
jeq	L106
tst	_lflg
jeq	L106
mov	12.+-42.(r5),22.(r4)
L106:mov	32.+-42.(r5),24.(r4)
mov	34.+-42.(r5),26.(r4)
tst	_uflg
jeq	L107
mov	28.+-42.(r5),24.(r4)
mov	30.+-42.(r5),26.(r4)
L107:mov	10.+-42.(r5),(sp)
movb	9.+-42.(r5),r0
mov	r0,-(sp)
jsr	pc,*$_nblock
tst	(sp)+
add	r0,_tblocks
L99:mov	r4,r0
jbr	L97
L97:jmp	rretrn
.globl	_compar
.text
_compar:
jsr	r5,rsave; 4
mov	4.(r5),r4
mov	6.(r5),r3
tst	_dflg
jne	L109
mov	16.(r4),r0
bic	$!0101000,r0
cmp	$0101000,r0
jne	L110
mov	16.(r3),r0
bic	$!0101000,r0
cmp	$0101000,r0
jeq	L111
mov	$01,r0
jbr	L108
L111:jbr	L112
L110:mov	16.(r3),r0
bic	$!0101000,r0
cmp	$0101000,r0
jne	L113
mov	$0177777,r0
jbr	L108
L113:L112:L109:tst	_tflg
jeq	L114
clr	r2
cmp	24.(r3),24.(r4)
jlos	L115
inc	r2
jbr	L116
L115:cmp	24.(r3),24.(r4)
jhis	L117
dec	r2
jbr	L118
L117:cmp	26.(r3),26.(r4)
jlos	L119
inc	r2
jbr	L120
L119:cmp	26.(r3),26.(r4)
jhis	L121
dec	r2
L121:L120:L118:L116:mov	r2,r1
mul	_rflg,r1
mov	r1,r0
jbr	L108
L114:bit	$01000,16.(r4)
jeq	L122
mov	(r4),r4
jbr	L123
L122:mov	r4,r4
L123:bit	$01000,16.(r3)
jeq	L124
mov	(r3),r3
jbr	L125
L124:mov	r3,r3
L125:clr	r2
L126:cmp	r2,$016
jge	L127
movb	(r4)+,r0
movb	(r3)+,r1
sub	r1,r0
mov	r0,-8.(r5)
jne	L10005
tstb	-1.(r4)
jne	L129
L10005:mov	_rflg,r1
mul	-8.(r5),r1
mov	r1,r0
jbr	L108
L129:L128:inc	r2
jbr	L126
L127:clr	r0
jbr	L108
L108:jmp	rretrn
