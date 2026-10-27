.globl	_ibuf
.comm	_ibuf,020
.globl	_ubuf
.comm	_ubuf,04540
.globl	_tbuf
.comm	_tbuf,0264
.globl	_wtmp
.comm	_wtmp,02
.globl	_pflag
.comm	_pflag,02
.globl	_byday
.comm	_byday,02
.globl	_dtime
.comm	_dtime,010
.globl	_midnight
.comm	_midnight,010
.globl	_lastime
.comm	_lastime,010
.globl	_day
.data
_day=.
042664;00;00;00
.globl	_pcount
.comm	_pcount,02
.globl	_pptr
.comm	_pptr,02
.globl	_montab
.data
_montab=.
31.
28.
31.
30.
31.
30.
31.
31.
30.
31.
30.
31.
31.
28.
31.
30.
31.
30.
31.
31.
30.
31.
30.
31.
.globl	_monasc
.data
_monasc=.
.text
L1:.byte 0112,0141,0156,0
.even
.data
L1
.text
L2:.byte 0106,0145,0142,0
.even
.data
L2
.text
L3:.byte 0115,0141,0162,0
.even
.data
L3
.text
L4:.byte 0101,0160,0162,0
.even
.data
L4
.text
L5:.byte 0115,0141,0171,0
.even
.data
L5
.text
L6:.byte 0112,0165,0156,0
.even
.data
L6
.text
L7:.byte 0112,0165,0154,0
.even
.data
L7
.text
L8:.byte 0101,0165,0147,0
.even
.data
L8
.text
L9:.byte 0123,0145,0160,0
.even
.data
L9
.text
L10:.byte 0117,0143,0164,0
.even
.data
L10
.text
L11:.byte 0116,0157,0166,0
.even
.data
L11
.text
L12:.byte 0104,0145,0143,0
.even
.data
L12
.globl	_main
.text
_main:
jsr	r5,rsave; 10
.data
L14:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0167,0164,0155,0160,0
.even
.text
mov	$L14,_wtmp
L15:dec	4.(r5)
jle	L16
add	$02,6.(r5)
mov	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L16
inc	*6.(r5)
mov	*6.(r5),r0
movb	(r0),r0
jbr	L18
L19:inc	_byday
jbr	L15
L20:dec	4.(r5)
jle	L21
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),_wtmp
L21:jbr	L15
L22:inc	_pflag
jbr	L15
jbr	L17
L18:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:144
160
167
L10002:..
L19
L22
L20
L23
.text
/esw
L17:L23:jbr	L15
L16:mov	4.(r5),_pcount
mov	6.(r5),_pptr
mov	$_fin,(sp)
mov	_wtmp,-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L24
.data
L25:.byte 0116,0157,040,045,0163,012,0
.even
.text
mov	_wtmp,(sp)
mov	$L25,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L13
L24:L26:mov	$_ibuf,r3
clr	r4
L28:cmp	r4,$020
jge	L29
mov	$_fin,(sp)
jsr	pc,*$_getc
mov	r0,-8.(r5)
jlt	L31
movb	-8.(r5),(r3)+
L30:inc	r4
jbr	L28
L29:clr	-10.(r5)
clr	r4
L32:cmp	r4,$010
jge	L33
movb	_ibuf(r4),r0
mov	r0,-8.(r5)
cmp	-8.(r5),$060
jlt	L10005
cmp	-8.(r5),$071
jle	L10004
L10005:cmp	-8.(r5),$0141
jlt	L10006
cmp	-8.(r5),$0172
jle	L10004
L10006:cmp	-8.(r5),$0101
jlt	L35
cmp	-8.(r5),$0132
jgt	L35
L10004:tst	-10.(r5)
jne	L36
jbr	L34
L35:cmp	-8.(r5),$040
jeq	L10007
tst	-8.(r5)
jne	L37
L10007:inc	-10.(r5)
clrb	_ibuf(r4)
jbr	L38
L37:jbr	L36
L38:L34:inc	r4
jbr	L32
L33:jsr	pc,_loop
L36:jbr	L26
L27:L31:clrb	_ibuf
movb	$0176,8.+_ibuf
mov	$10.+_ibuf,(sp)
jsr	pc,*$_time
jsr	pc,_loop
jsr	pc,_print
L13:jmp	rretrn
.globl	_loop
.text
_loop:
jsr	r5,rsave; 2
tstb	9.+_ibuf
jne	L39
tst	14.+_ibuf
jne	L39
mov	$10.+_ibuf,(sp)
jsr	pc,*$_ltod
divf	$041560,r0
movfo	r0,10.+_ibuf
cmpb	8.+_ibuf,$0174
jne	L40
movof	10.+_ibuf,r0
movf	r0,_dtime
jbr	L39
L40:cmpb	8.+_ibuf,$0175
jne	L41
tstf	_dtime
cfcc
jeq	L39
mov	$_tbuf,r3
L42:cmp	r3,$180.+_tbuf
jhis	L43
movof	10.+_ibuf,r0
subf	_dtime,r0
movf	r0,-(sp)
movof	2.(r3),r0
addf	(sp)+,r0
movfo	r0,2.(r3)
L44:add	$06,r3
jbr	L42
L43:clrf	_dtime
jbr	L39
L41:movf	_lastime,r0
movof	10.+_ibuf,r1
cmpf	r0,r1
cfcc
jgt	L10008
movf	$040300,r0
mulf	_day,r0
addf	_lastime,r0
movof	10.+_ibuf,r1
cmpf	r0,r1
cfcc
jge	L45
L10008:clrf	_midnight
L45:tstf	_midnight
cfcc
jne	L46
jsr	pc,_newday
L46:movof	10.+_ibuf,r0
movf	r0,_lastime
tst	_byday
jeq	L47
movof	10.+_ibuf,r0
cmpf	_midnight,r0
cfcc
jge	L47
mov	$01,(sp)
jsr	pc,*$_upall
jsr	pc,_print
jsr	pc,_newday
mov	$_ubuf,r2
L48:cmp	r2,$2400.+_ubuf
jhis	L49
clrf	r0
movfo	r0,8.(r2)
L50:add	$014,r2
jbr	L48
L49:L47:cmpb	8.+_ibuf,$0176
jne	L51
clrb	_ibuf
clr	(sp)
jsr	pc,*$_upall
jbr	L39
L51:movb	8.+_ibuf,r0
mov	r0,r4
cmp	$0141,r0
jgt	L52
sub	$050,r4
L52:sub	$060,r4
tst	r4
jlt	L10009
cmp	r4,$036
jlt	L53
L10009:mov	$035,r4
L53:mov	r4,r1
mul	$06,r1
add	$_tbuf,r1
mov	r1,r3
clr	(sp)
mov	r3,-(sp)
jsr	pc,*$_update
tst	(sp)+
L39:jmp	rretrn
.globl	_print
.text
_print:
jsr	r5,rsave; 14
clrf	r0
movfo	r0,-12.(r5)
clr	-8.(r5)
L55:cmp	-8.(r5),$0310
jge	L56
mov	-8.(r5),(sp)
jsr	pc,*$_among
tst	r0
jeq	L57
mov	-8.(r5),r1
mul	$014,r1
movof	8.+_ubuf(r1),r1
movfo	r1,-16.(r5)
movof	-16.(r5),r0
cfcc
jle	L58
movof	-16.(r5),r0
movf	r0,-(sp)
movof	-12.(r5),r0
addf	(sp)+,r0
movfo	r0,-12.(r5)
L58:tst	_pflag
jeq	L59
mov	-8.(r5),r1
mul	$014,r1
movof	8.+_ubuf(r1),r1
cfcc
jle	L59
.data
L60:.byte 011,045,055,070,056,070,0163,045,066,056,062,0146,012,0
.even
.text
mov	-8.(r5),r1
mul	$014,r1
movof	8.+_ubuf(r1),r1
divf	$041560,r1
movf	r1,-(sp)
mov	-8.(r5),r1
mul	$014,r1
mov	r1,-(sp)
add	$_ubuf,(sp)
mov	$L60,-(sp)
jsr	pc,_printf
add	$014,sp
L59:L57:inc	-8.(r5)
jbr	L55
L56:movof	-12.(r5),r0
cfcc
jle	L61
jsr	pc,_pdate
.data
L62:.byte 011,0164,0157,0164,0141,0154,045,071,056,062,0146,012,0
.even
.text
movof	-12.(r5),r0
divf	$041560,r0
movf	r0,-(sp)
mov	$L62,-(sp)
jsr	pc,_printf
add	$012,sp
L61:L54:jmp	rretrn
.globl	_upall
.text
_upall:
jsr	r5,rsave; 2
mov	$_tbuf,r4
L64:cmp	r4,$180.+_tbuf
jhis	L65
mov	4.(r5),(sp)
mov	r4,-(sp)
jsr	pc,*$_update
tst	(sp)+
L66:add	$06,r4
jbr	L64
L65:L63:jmp	rretrn
.globl	_update
.text
_update:
jsr	r5,rsave; 30
tst	6.(r5)
jeq	L68
movf	_midnight,r0
movf	r0,-20.(r5)
jbr	L69
L68:movof	10.+_ibuf,r0
movf	r0,-20.(r5)
L69:tst	*4.(r5)
jeq	L70
movf	-20.(r5),r0
mov	4.(r5),r1
movof	2.(r1),r1
subf	r1,r0
movf	r0,-28.(r5)
tstf	-28.(r5)
cfcc
jle	L71
movf	$040300,r0
mulf	_day,r0
cmpf	-28.(r5),r0
cfcc
jge	L71
movf	-28.(r5),r0
movf	r0,-(sp)
mov	*4.(r5),r0
movof	8.(r0),r1
addf	(sp)+,r1
movfo	r1,8.(r0)
movf	r1,r0
L71:L70:mov	4.(r5),r0
movf	-20.(r5),r0
movfo	r0,2.(r0)
tst	6.(r5)
jne	L67
tstb	_ibuf
jne	L72
clr	*4.(r5)
jbr	L67
L72:mov	$_ubuf,-12.(r5)
L73:cmp	-12.(r5),$2400.+_ubuf
jhis	L74
tstb	*-12.(r5)
jeq	L74
clr	-10.(r5)
L76:cmp	-10.(r5),$010
jge	L77
mov	-12.(r5),r0
add	-10.(r5),r0
mov	-10.(r5),r1
cmpb	(r0),_ibuf(r1)
jne	L77
L78:inc	-10.(r5)
jbr	L76
L77:cmp	-10.(r5),$010
jge	L74
L75:add	$014,-12.(r5)
jbr	L73
L74:clr	-10.(r5)
L79:cmp	-10.(r5),$010
jge	L80
mov	-12.(r5),r0
add	-10.(r5),r0
mov	-10.(r5),r1
movb	_ibuf(r1),(r0)
L81:inc	-10.(r5)
jbr	L79
L80:mov	-12.(r5),*4.(r5)
L67:jmp	rretrn
.globl	_among
.text
_among:
jsr	r5,rsave; 2
tst	_pcount
jne	L83
mov	$01,r0
jbr	L82
L83:clr	r4
L84:cmp	r4,_pcount
jge	L85
mov	r4,r0
asl	r0
add	_pptr,r0
mov	(r0),r2
clr	r3
L87:cmp	r3,$010
jge	L88
mov	4.(r5),r1
mul	$014,r1
add	r3,r1
mov	r1,r0
cmpb	_ubuf(r0),(r2)
jne	L90
tstb	(r2)+
jne	L91
mov	$01,r0
jbr	L82
L91:jbr	L92
L90:jbr	L88
L92:L89:inc	r3
jbr	L87
L88:L86:inc	r4
jbr	L84
L85:clr	r0
jbr	L82
L82:jmp	rretrn
.globl	_newday
.text
_newday:
jsr	r5,rsave; 2
tstf	_midnight
cfcc
jne	L94
movf	$042160,r0
movf	r0,_midnight
L94:L95:movf	_midnight,r0
movof	10.+_ibuf,r1
cmpf	r0,r1
cfcc
jgt	L96
movf	_day,r0
movf	r0,-(sp)
movf	_midnight,r0
addf	(sp)+,r0
movf	r0,_midnight
jbr	L95
L96:L93:jmp	rretrn
.globl	_pdate
.text
_pdate:
jsr	r5,rsave; 22
tst	_byday
jeq	L97
mov	$02,r2
clrf	-22.(r5)
L98:.data
L100:042266;0100000;00;00
.text
movf	L100,r0
mulf	_day,r0
movf	r0,-14.(r5)
mov	r2,r1
sxt	r0
div	$04,r0
tst	r1
jne	L101
movf	_day,r0
movf	r0,-(sp)
movf	-14.(r5),r0
addf	(sp)+,r0
movf	r0,-14.(r5)
L101:movf	-22.(r5),r0
addf	-14.(r5),r0
cmpf	_midnight,r0
cfcc
jlt	L99
inc	r2
movf	-14.(r5),r0
movf	r0,-(sp)
movf	-22.(r5),r0
addf	(sp)+,r0
movf	r0,-22.(r5)
jbr	L98
L99:movf	_midnight,r0
subf	-22.(r5),r0
subf	$042464,r0
divf	_day,r0
movfi	r0,r0
mov	r0,r4
mov	$034,2.+_montab
mov	r2,r1
sxt	r0
div	$04,r0
tst	r1
jne	L102
inc	2.+_montab
L102:clr	r3
L103:mov	r3,r0
asl	r0
cmp	_montab(r0),r4
jgt	L104
mov	r3,r0
asl	r0
sub	_montab(r0),r4
L105:inc	r3
jbr	L103
L104:mov	r3,r1
sxt	r0
div	$014,r0
mov	r1,r3
.data
L106:.byte 045,0163,040,045,062,0144,0
.even
.text
mov	r4,(sp)
inc	(sp)
mov	r3,r0
asl	r0
mov	_monasc(r0),-(sp)
mov	$L106,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
L97:jmp	rretrn
.globl	fltused
