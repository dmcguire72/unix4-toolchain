.globl	_lflg
.comm	_lflg,02
.globl	_cflg
.comm	_cflg,02
.globl	_jflg
.comm	_jflg,02
.globl	_nflg
.comm	_nflg,02
.globl	_aflg
.comm	_aflg,02
.globl	_rflg
.comm	_rflg,02
.globl	_tflg
.comm	_tflg,02
.globl	_vflg
.comm	_vflg,02
.globl	_uflg
.comm	_uflg,02
.globl	_thres
.data
_thres=.
1.
.globl	_sflg
.comm	_sflg,02
.globl	_bflg
.comm	_bflg,02
.globl	_fout
.comm	_fout,02
.globl	_tab
.comm	_tab,052760
.globl	_treal
.comm	_treal,04
.globl	_tcpu
.comm	_tcpu,04
.globl	_tsys
.comm	_tsys,04
.globl	_junkp
.data
_junkp=.
-1.
.globl	_sname
.comm	_sname,02
.globl	_ncom
.comm	_ncom,04
.globl	_main
.text
_main:
jsr	r5,rsave; 14
jsr	pc,_init
cmp	4.(r5),$01
jle	L2
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jne	L3
add	$02,6.(r5)
dec	4.(r5)
mov	$01,-8.(r5)
L4:mov	*6.(r5),r0
add	-8.(r5),r0
tstb	(r0)
jeq	L5
mov	*6.(r5),r0
add	-8.(r5),r0
movb	(r0),r0
jbr	L8
L9:inc	_bflg
jbr	L7
L10:inc	_lflg
jbr	L7
L11:inc	_cflg
jbr	L7
L12:inc	_jflg
jbr	L7
L13:inc	_nflg
jbr	L7
L14:inc	_aflg
jbr	L7
L15:inc	_rflg
jbr	L7
L16:inc	_tflg
jbr	L7
L17:inc	_sflg
inc	_aflg
jbr	L7
L18:L19:L20:L21:L22:L23:L24:L25:L26:L27:mov	*6.(r5),r0
add	-8.(r5),r0
movb	(r0),r0
add	$0177720,r0
mov	r0,_thres
jbr	L7
L28:inc	_vflg
jbr	L7
L29:inc	_uflg
jbr	L7
jbr	L7
L8:jsr	r2,hsw; 5; L10001
jmp	*L10007-L10002(r1)
.data
L10001:L10002
L10003
L10004
L10005
L10006
L10007
L10002:..
12
13
26
27
L10003:..
12
13
25
27
L10004:..
12
13
23
27
L10005:..
11
12
23
25
27
L10006:..
11
12
23
26
L10007:L30
L20
L25
L13
L17
L30
L21
L26
L12
L16
L30
L22
L27
L14
L29
L30
L18
L23
L9
L10
L28
L30
L19
L24
L11
L15
.text
/esw
L7:L30:L6:inc	-8.(r5)
jbr	L4
L5:L3:L2:mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
cmp	4.(r5),$02
jge	L31
.data
L32:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0163,0150,0137,0141,0143,0143,0164,0
.even
.text
mov	$L32,(sp)
jsr	pc,*$_acct
jbr	L33
L31:L34:dec	4.(r5)
jeq	L35
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),(sp)
jsr	pc,*$_acct
jbr	L34
L35:L33:tst	_uflg
jeq	L36
jsr	pc,_flush
jbr	L1
L36:tst	_vflg
jeq	L37
jsr	pc,_strip
L37:tst	_aflg
jne	L38
clr	-8.(r5)
L39:cmp	-8.(r5),$01750
jge	L40
mov	-8.(r5),r1
mul	$026,r1
tstb	_tab(r1)
jeq	L42
clr	-10.(r5)
L43:cmp	-10.(r5),$010
jge	L44
mov	-8.(r5),r1
mul	$026,r1
add	-10.(r5),r1
movb	_tab(r1),r1
cmp	$077,r1
jeq	L46
L45:inc	-10.(r5)
jbr	L43
L44:mov	-8.(r5),r1
mul	$026,r1
cmp	8.+_tab(r1),$01
jne	L41
L46:cmp	_junkp,$0177777
jne	L47
.data
L48:.byte 052,052,052,0157,0164,0150,0145,0162,0
.even
.text
mov	$L48,(sp)
jsr	pc,*$_enter
mov	r0,_junkp
L47:mov	-8.(r5),r1
mul	$026,r1
mov	_junkp,r3
mul	$026,r3
add	8.+_tab(r1),8.+_tab(r3)
mov	-8.(r5),r1
mul	$026,r1
movof	10.+_tab(r1),r1
movf	r1,-(sp)
mov	_junkp,r1
mul	$026,r1
movof	10.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,10.+_tab(r1)
movf	r2,r1
mov	-8.(r5),r1
mul	$026,r1
movof	14.+_tab(r1),r1
movf	r1,-(sp)
mov	_junkp,r1
mul	$026,r1
movof	14.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,14.+_tab(r1)
movf	r2,r1
mov	-8.(r5),r1
mul	$026,r1
movof	18.+_tab(r1),r1
movf	r1,-(sp)
mov	_junkp,r1
mul	$026,r1
movof	18.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,18.+_tab(r1)
movf	r2,r1
mov	-8.(r5),r1
mul	$026,r1
clrb	_tab(r1)
L42:L41:inc	-8.(r5)
jbr	L39
L40:L38:clr	r0
mov	r0,-12.(r5)
mov	r0,-8.(r5)
L49:cmp	-8.(r5),$01750
jge	L50
mov	-8.(r5),r1
mul	$026,r1
tstb	_tab(r1)
jeq	L52
clr	-10.(r5)
L53:cmp	-10.(r5),$010
jge	L54
mov	-12.(r5),r1
mul	$026,r1
add	-10.(r5),r1
mov	-8.(r5),r3
mul	$026,r3
add	-10.(r5),r3
movb	_tab(r3),_tab(r1)
L55:inc	-10.(r5)
jbr	L53
L54:mov	-12.(r5),r1
mul	$026,r1
mov	-8.(r5),r3
mul	$026,r3
mov	8.+_tab(r3),8.+_tab(r1)
mov	-8.(r5),r1
mul	$026,r1
movof	10.+_tab(r1),r1
mov	-12.(r5),r3
mul	$026,r3
movfo	r1,10.+_tab(r3)
mov	-8.(r5),r1
mul	$026,r1
movof	14.+_tab(r1),r1
mov	-12.(r5),r3
mul	$026,r3
movfo	r1,14.+_tab(r3)
mov	-8.(r5),r1
mul	$026,r1
movof	18.+_tab(r1),r1
mov	-12.(r5),r3
mul	$026,r3
movfo	r1,18.+_tab(r3)
inc	-12.(r5)
L52:L51:inc	-8.(r5)
jbr	L49
L50:tst	_sflg
jeq	L56
mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
.data
L57:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0163,0150,0164,0137,0141,0143,0143,0164,0
.even
.text
mov	$0666,(sp)
mov	$L57,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-8.(r5)
jge	L58
.data
L59:.byte 0103,0141,0156,047,0164,040,0163,0141,0166,0145,012,0
.even
.text
mov	$L59,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L58:mov	-12.(r5),r1
mul	$026,r1
mov	r1,(sp)
mov	$_tab,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	-8.(r5),(sp)
jsr	pc,*$_close
tst	_sname
jeq	L60
mov	$0666,(sp)
mov	_sname,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,-8.(r5)
jge	L61
.data
L62:.byte 0103,0141,0156,047,0164,040,0164,0162,0165,0156,0143,0141,0164,0145,012,0
.even
.text
mov	$L62,(sp)
jsr	pc,*$_printf
L61:mov	-8.(r5),(sp)
jsr	pc,*$_close
L60:clr	(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L56:tst	_nflg
jeq	L10008
mov	$_ncmp,(sp)
jbr	L10009
L10008:tst	_bflg
jeq	L10010
mov	$_bcmp,(sp)
jbr	L10011
L10010:mov	$_tcmp,(sp)
L10011:L10009:mov	$026,-(sp)
mov	-12.(r5),-(sp)
mov	$_tab,-(sp)
jsr	pc,*$_qsort
add	$06,sp
.data
L63:.byte 045,070,0163,0
.even
.text
.data
L64:.byte 0
.even
.text
mov	$L64,(sp)
mov	$L63,-(sp)
jsr	pc,*$_printf
tst	(sp)+
movof	_tsys,r0
movf	r0,-(sp)
movof	_tcpu,r0
movf	r0,-(sp)
movof	_treal,r0
movf	r0,-(sp)
movof	_ncom,r0
movf	r0,-(sp)
jsr	pc,_column
add	$040,sp
clr	-8.(r5)
L65:cmp	-8.(r5),-12.(r5)
jge	L66
mov	-8.(r5),r1
mul	$026,r1
tstb	_tab(r1)
jeq	L68
mov	-8.(r5),r1
mul	$026,r1
movif	8.+_tab(r1),r1
movfo	r1,-16.(r5)
.data
L69:.byte 045,055,070,056,070,0163,0
.even
.text
mov	-8.(r5),r1
mul	$026,r1
mov	r1,(sp)
add	$_tab,(sp)
mov	$L69,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	-8.(r5),r1
mul	$026,r1
movof	18.+_tab(r1),r1
movf	r1,-(sp)
mov	-8.(r5),r1
mul	$026,r1
movof	14.+_tab(r1),r1
movf	r1,-(sp)
mov	-8.(r5),r1
mul	$026,r1
movof	10.+_tab(r1),r1
movf	r1,-(sp)
movof	-16.(r5),r0
movf	r0,-(sp)
jsr	pc,_column
add	$040,sp
L68:L67:inc	-8.(r5)
jbr	L65
L66:jsr	pc,_flush
L1:jmp	rretrn
.globl	_column
.text
_column:
jsr	r5,rsave; 2
.data
L71:.byte 045,066,056,060,0146,0
.even
.text
movf	4.(r5),r0
movf	r0,-(sp)
mov	$L71,-(sp)
jsr	pc,_printf
add	$012,sp
tst	_cflg
jeq	L72
movf	4.(r5),r0
movof	_ncom,r1
cmpf	r0,r1
cfcc
jne	L73
.data
L74:.byte 045,067,0163,0
.even
.text
.data
L75:.byte 0
.even
.text
mov	$L75,(sp)
mov	$L74,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L76
L73:.data
L77:.byte 045,066,056,062,0146,045,045,0
.even
.text
movf	$041710,r0
mulf	4.(r5),r0
movof	_ncom,r1
divf	r1,r0
movf	r0,-(sp)
mov	$L77,-(sp)
jsr	pc,_printf
add	$012,sp
L76:L72:movof	_treal,r0
movf	r0,-(sp)
movf	12.(r5),r0
movf	r0,-(sp)
movf	4.(r5),r0
movf	r0,-(sp)
jsr	pc,_col
add	$030,sp
tst	_lflg
jeq	L78
movof	_tcpu,r0
movf	r0,-(sp)
movf	20.(r5),r0
movf	r0,-(sp)
movf	4.(r5),r0
movf	r0,-(sp)
jsr	pc,_col
add	$030,sp
movof	_tsys,r0
movf	r0,-(sp)
movf	28.(r5),r0
movf	r0,-(sp)
movf	4.(r5),r0
movf	r0,-(sp)
jsr	pc,_col
add	$030,sp
jbr	L79
L78:movof	_tcpu,r0
movof	_tsys,r1
addf	r1,r0
movf	r0,-(sp)
movf	20.(r5),r0
addf	28.(r5),r0
movf	r0,-(sp)
movf	4.(r5),r0
movf	r0,-(sp)
jsr	pc,_col
add	$030,sp
L79:tst	_tflg
jeq	L80
.data
L81:.byte 045,066,056,061,0146,0
.even
.text
movf	12.(r5),r0
movf	20.(r5),r1
addf	28.(r5),r1
divf	r1,r0
movf	r0,-(sp)
mov	$L81,-(sp)
jsr	pc,_printf
add	$012,sp
L80:mov	$012,(sp)
jsr	pc,*$_putchar
L70:jmp	rretrn
.globl	_col
.text
_col:
jsr	r5,rsave; 2
tst	_jflg
jeq	L83
.data
L84:.byte 045,071,056,062,0146,0
.even
.text
movf	12.(r5),r0
movf	4.(r5),r1
mulf	$041560,r1
divf	r1,r0
movf	r0,-(sp)
mov	$L84,-(sp)
jsr	pc,_printf
add	$012,sp
jbr	L85
L83:.data
L86:.byte 045,071,056,062,0146,0
.even
.text
movf	12.(r5),r0
divf	$043141,r0
movf	r0,-(sp)
mov	$L86,-(sp)
jsr	pc,_printf
add	$012,sp
L85:tst	_cflg
jeq	L87
movf	12.(r5),r0
cmpf	20.(r5),r0
cfcc
jne	L88
.data
L89:.byte 045,067,0163,0
.even
.text
.data
L90:.byte 0
.even
.text
mov	$L90,(sp)
mov	$L89,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L91
L88:.data
L92:.byte 045,066,056,062,0146,045,045,0
.even
.text
movf	$041710,r0
mulf	12.(r5),r0
divf	20.(r5),r0
movf	r0,-(sp)
mov	$L92,-(sp)
jsr	pc,_printf
add	$012,sp
L91:L87:L82:jmp	rretrn
.globl	_acct
.text
_acct:
jsr	r5,rsave; 40
tst	_sflg
jeq	L94
tst	_sname
jeq	L94
.data
L95:.byte 0117,0156,0154,0171,040,061,040,0146,0151,0154,0145,040,0167,0151,0164,0150,040,055,0163,012,0
.even
.text
mov	$L95,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L94:tst	_sflg
jeq	L96
mov	4.(r5),_sname
L96:clr	(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-8.(r5)
jge	L97
.data
L98:.byte 0103,0141,0156,047,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L98,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L93
L97:L99:mov	$024,(sp)
mov	r5,r0
add	$0177734,r0
mov	r0,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$024,r0
jne	L100
mov	r5,r0
add	$0177734,r0
mov	r0,r4
L101:mov	r4,r0
mov	r5,r1
add	$0177744,r1
cmp	r0,r1
jhis	L102
movb	(r4),r0
bic	$!0377,r0
mov	r0,r3
tst	r3
jeq	L104
cmp	r3,$040
jlt	L10012
cmp	r3,$0200
jlt	L104
L10012:movb	$077,(r4)
L104:L103:inc	r4
jbr	L101
L102:tst	_uflg
jeq	L105
.data
L106:.byte 045,063,0144,040,045,056,070,0163,012,0
.even
.text
mov	r5,r0
add	$0177734,r0
mov	r0,(sp)
movb	13.+-36.(r5),r0
mov	r0,-(sp)
mov	$L106,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L99
L105:movf	$040200,r0
movf	r0,-(sp)
movof	_ncom,r0
addf	(sp)+,r0
movfo	r0,_ncom
mov	r5,r0
add	$0177734,r0
mov	r0,(sp)
jsr	pc,*$_enter
mov	r0,-10.(r5)
mov	-10.(r5),r1
mul	$026,r1
inc	8.+_tab(r1)
movif	14.+-36.(r5),r0
movfo	r0,-16.(r5)
movof	-16.(r5),r0
cfcc
jge	L107
movf	$044200,r0
movf	r0,-(sp)
movof	-16.(r5),r0
addf	(sp)+,r0
movfo	r0,-16.(r5)
L107:movf	$041560,r0
movf	r0,-(sp)
movof	-16.(r5),r0
mulf	(sp)+,r0
movfo	r0,-16.(r5)
movof	-16.(r5),r0
movf	r0,-(sp)
mov	-10.(r5),r1
mul	$026,r1
movof	10.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,10.+_tab(r1)
movf	r2,r1
movof	-16.(r5),r0
movf	r0,-(sp)
movof	_treal,r0
addf	(sp)+,r0
movfo	r0,_treal
movif	16.+-36.(r5),r0
movfo	r0,-16.(r5)
movof	-16.(r5),r0
cfcc
jge	L108
movf	$044200,r0
movf	r0,-(sp)
movof	-16.(r5),r0
addf	(sp)+,r0
movfo	r0,-16.(r5)
L108:movof	-16.(r5),r0
movf	r0,-(sp)
mov	-10.(r5),r1
mul	$026,r1
movof	14.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,14.+_tab(r1)
movf	r2,r1
movof	-16.(r5),r0
movf	r0,-(sp)
movof	_tcpu,r0
addf	(sp)+,r0
movfo	r0,_tcpu
movif	18.+-36.(r5),r0
movfo	r0,-16.(r5)
movof	-16.(r5),r0
cfcc
jge	L109
movf	$044200,r0
movf	r0,-(sp)
movof	-16.(r5),r0
addf	(sp)+,r0
movfo	r0,-16.(r5)
L109:movof	-16.(r5),r0
movf	r0,-(sp)
mov	-10.(r5),r1
mul	$026,r1
movof	18.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,18.+_tab(r1)
movf	r2,r1
movof	-16.(r5),r0
movf	r0,-(sp)
movof	_tsys,r0
addf	(sp)+,r0
movfo	r0,_tsys
jbr	L99
L100:mov	-8.(r5),(sp)
jsr	pc,*$_close
L93:jmp	rretrn
.globl	_ncmp
.text
_ncmp:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	6.(r5),r1
cmp	8.(r0),8.(r1)
jne	L111
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_tcmp
tst	(sp)+
jbr	L110
L111:tst	_rflg
jeq	L112
mov	4.(r5),r0
mov	8.(r0),r0
mov	6.(r5),r1
sub	8.(r1),r0
jbr	L110
L112:mov	6.(r5),r0
mov	8.(r0),r0
mov	4.(r5),r1
sub	8.(r1),r0
jbr	L110
L110:jmp	rretrn
.globl	_bcmp
.text
_bcmp:
jsr	r5,rsave; 12
mov	4.(r5),(sp)
jsr	pc,*$_sum
mov	4.(r5),r1
movif	8.(r1),r1
divf	r1,r0
movfo	r0,-10.(r5)
mov	6.(r5),(sp)
jsr	pc,*$_sum
mov	6.(r5),r1
movif	8.(r1),r1
divf	r1,r0
movfo	r0,-14.(r5)
movof	-10.(r5),r0
movof	-14.(r5),r1
cmpf	r0,r1
cfcc
jge	L114
tst	_rflg
jeq	L115
mov	$0177777,r0
jbr	L113
L115:mov	$01,r0
jbr	L113
L114:movof	-10.(r5),r0
movof	-14.(r5),r1
cmpf	r0,r1
cfcc
jle	L116
tst	_rflg
jeq	L117
mov	$01,r0
jbr	L113
L117:mov	$0177777,r0
jbr	L113
L116:clr	r0
jbr	L113
L113:jmp	rretrn
.globl	_tcmp
.text
_tcmp:
jsr	r5,rsave; 12
mov	4.(r5),(sp)
jsr	pc,*$_sum
movfo	r0,-10.(r5)
mov	6.(r5),(sp)
jsr	pc,*$_sum
movfo	r0,-14.(r5)
movof	-10.(r5),r0
movof	-14.(r5),r1
cmpf	r0,r1
cfcc
jge	L119
tst	_rflg
jeq	L120
mov	$0177777,r0
jbr	L118
L120:mov	$01,r0
jbr	L118
L119:movof	-10.(r5),r0
movof	-14.(r5),r1
cmpf	r0,r1
cfcc
jle	L121
tst	_rflg
jeq	L122
mov	$01,r0
jbr	L118
L122:mov	$0177777,r0
jbr	L118
L121:clr	r0
jbr	L118
L118:jmp	rretrn
.globl	_sum
.text
_sum:
jsr	r5,rsave; 2
tstb	*4.(r5)
jne	L124
clrf	r0
jbr	L123
L124:mov	4.(r5),r0
movof	14.(r0),r0
mov	4.(r5),r1
movof	18.(r1),r1
addf	r1,r0
jbr	L123
L123:jmp	rretrn
.globl	_init
.text
_init:
jsr	r5,rsave; 36
.data
L126:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0163,0150,0164,0137,0141,0143,0143,0164,0
.even
.text
clr	(sp)
mov	$L126,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-34.(r5)
jlt	L125
L127:mov	$026,(sp)
mov	r5,r0
add	$0177744,r0
mov	r0,-(sp)
mov	-34.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$026,r0
jne	L128
mov	r5,r0
add	$0177744,r0
mov	r0,(sp)
jsr	pc,*$_enter
mov	r0,-30.(r5)
movif	8.+-28.(r5),r0
movf	r0,-(sp)
movof	_ncom,r0
addf	(sp)+,r0
movfo	r0,_ncom
mov	-30.(r5),r1
mul	$026,r1
mov	8.+-28.(r5),8.+_tab(r1)
movof	10.+-28.(r5),r0
movf	r0,-(sp)
movof	_treal,r0
addf	(sp)+,r0
movfo	r0,_treal
mov	-30.(r5),r1
mul	$026,r1
movof	10.+-28.(r5),r1
movfo	r1,10.+_tab(r1)
movof	14.+-28.(r5),r0
movf	r0,-(sp)
movof	_tcpu,r0
addf	(sp)+,r0
movfo	r0,_tcpu
mov	-30.(r5),r1
mul	$026,r1
movof	14.+-28.(r5),r1
movfo	r1,14.+_tab(r1)
movof	18.+-28.(r5),r0
movf	r0,-(sp)
movof	_tsys,r0
addf	(sp)+,r0
movfo	r0,_tsys
mov	-30.(r5),r1
mul	$026,r1
movof	18.+-28.(r5),r1
movfo	r1,18.+_tab(r1)
jbr	L127
L128:mov	-34.(r5),(sp)
jsr	pc,*$_close
L125:jmp	rretrn
.globl	_enter
.text
_enter:
jsr	r5,rsave; 6
clr	-8.(r5)
clr	-10.(r5)
L130:cmp	-10.(r5),$010
jge	L131
mov	-8.(r5),r1
mul	$07,r1
mov	4.(r5),r2
add	-10.(r5),r2
movb	(r2),r2
add	r2,r1
mov	r1,-8.(r5)
L132:inc	-10.(r5)
jbr	L130
L131:tst	-8.(r5)
jge	L133
mov	-8.(r5),r0
neg	r0
mov	r0,-8.(r5)
L133:mov	-8.(r5),r1
sxt	r0
div	$01750,r0
mov	r1,-8.(r5)
L134:mov	-8.(r5),r1
mul	$026,r1
tstb	_tab(r1)
jeq	L135
clr	-10.(r5)
L137:cmp	-10.(r5),$010
jge	L138
mov	-8.(r5),r1
mul	$026,r1
add	-10.(r5),r1
mov	4.(r5),r2
add	-10.(r5),r2
cmpb	_tab(r1),(r2)
jne	L140
L139:inc	-10.(r5)
jbr	L137
L138:jbr	L141
L140:L136:mov	-8.(r5),r1
inc	r1
sxt	r0
div	$01750,r0
mov	r1,-8.(r5)
jbr	L134
L135:clr	-10.(r5)
L142:cmp	-10.(r5),$010
jge	L143
mov	-8.(r5),r1
mul	$026,r1
add	-10.(r5),r1
mov	4.(r5),r2
add	-10.(r5),r2
movb	(r2),_tab(r1)
L144:inc	-10.(r5)
jbr	L142
L143:L141:mov	-8.(r5),r0
jbr	L129
L129:jmp	rretrn
.globl	_strip
.text
_strip:
jsr	r5,rsave; 12
.data
L146:.byte 052,052,0152,0165,0156,0153,052,052,0
.even
.text
mov	$L146,(sp)
jsr	pc,*$_enter
mov	r0,-10.(r5)
clr	-8.(r5)
L147:cmp	-8.(r5),$01750
jge	L148
mov	-8.(r5),r1
mul	$026,r1
tstb	_tab(r1)
jeq	L150
mov	-8.(r5),r1
mul	$026,r1
cmp	8.+_tab(r1),_thres
jgt	L150
.data
L151:.byte 045,056,070,0163,055,055,0
.even
.text
mov	-8.(r5),r1
mul	$026,r1
mov	r1,(sp)
add	$_tab,(sp)
mov	$L151,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_flush
jsr	pc,_getchar
mov	r0,-14.(r5)
cmp	$0171,r0
jne	L152
mov	-8.(r5),r1
mul	$026,r1
clrb	_tab(r1)
mov	-8.(r5),r1
mul	$026,r1
mov	-10.(r5),r3
mul	$026,r3
add	8.+_tab(r1),8.+_tab(r3)
mov	-8.(r5),r1
mul	$026,r1
movof	10.+_tab(r1),r1
movf	r1,-(sp)
mov	-10.(r5),r1
mul	$026,r1
movof	10.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,10.+_tab(r1)
movf	r2,r1
mov	-8.(r5),r1
mul	$026,r1
movof	14.+_tab(r1),r1
movf	r1,-(sp)
mov	-10.(r5),r1
mul	$026,r1
movof	14.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,14.+_tab(r1)
movf	r2,r1
mov	-8.(r5),r1
mul	$026,r1
movof	18.+_tab(r1),r1
movf	r1,-(sp)
mov	-10.(r5),r1
mul	$026,r1
movof	18.+_tab(r1),r2
addf	(sp)+,r2
movfo	r2,18.+_tab(r1)
movf	r2,r1
L152:L153:tst	-14.(r5)
jeq	L154
cmp	-14.(r5),$012
jeq	L154
jsr	pc,_getchar
mov	r0,-14.(r5)
jbr	L153
L154:L150:L149:inc	-8.(r5)
jbr	L147
L148:L145:jmp	rretrn
.globl	fltused
