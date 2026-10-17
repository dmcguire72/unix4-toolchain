.globl	_utmp
.comm	_utmp,020
.globl	_ttyb
.comm	_ttyb,06
.globl	_ttyx
.comm	_ttyx,02
.globl	_main
.text
_main:
jsr	r5,rsave; 230
mov	$01,(sp)
mov	$03,-(sp)
jsr	pc,*$_signal
tst	(sp)+
mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
.data
L2:.byte 057,0144,0145,0166,057,0164,0164,0171,0170,0
.even
.text
mov	$L2,_ttyx
clr	(sp)
jsr	pc,*$_ttyn
movb	r0,8.+_utmp
cmp	$0170,r0
jne	L3
.data
L4:.byte 0123,0157,0162,0162,0171,056,012,0
.even
.text
mov	$07,(sp)
mov	$L4,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L3:mov	_ttyx,r0
movb	8.+_utmp,8.(r0)
L5:mov	$_utmp,r4
cmp	4.(r5),$01
jle	L6
mov	6.(r5),r0
mov	2.(r0),r3
L7:cmp	r4,$8.+_utmp
jhis	L8
tstb	(r3)
jeq	L8
movb	(r3)+,(r4)+
jbr	L7
L8:clr	4.(r5)
jbr	L9
L6:.data
L10:.byte 0116,0141,0155,0145,072,040,0
.even
.text
mov	$07,(sp)
mov	$L10,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L11:jsr	pc,_getchar
mov	r0,-152.(r5)
cmp	$012,r0
jeq	L12
tst	-152.(r5)
jgt	L13
jsr	pc,_exit
L13:cmp	r4,$8.+_utmp
jhis	L14
movb	-152.(r5),(r4)+
L14:jbr	L11
L12:L9:L15:cmp	r4,$8.+_utmp
jhis	L16
movb	$040,(r4)+
jbr	L15
L16:mov	r5,r0
add	$0177572,r0
mov	r0,(sp)
mov	$_utmp,-(sp)
jsr	pc,*$_getpwent
tst	(sp)+
tst	r0
jne	L17
mov	r5,r0
add	$0177572,r0
mov	r0,(sp)
jsr	pc,*$_colon
mov	r0,r3
cmpb	(r3),$072
jeq	L18
mov	$_ttyb,(sp)
clr	-(sp)
jsr	pc,*$_gtty
tst	(sp)+
mov	4.+_ttyb,-148.(r5)
bic	$!0177767,4.+_ttyb
mov	$_ttyb,(sp)
clr	-(sp)
jsr	pc,*$_stty
tst	(sp)+
.data
L19:.byte 0120,0141,0163,0163,0167,0157,0162,0144,072,040,0
.even
.text
mov	$012,(sp)
mov	$L19,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r5,r0
add	$0177560,r0
mov	r0,r4
L20:jsr	pc,_getchar
mov	r0,-152.(r5)
cmp	$012,r0
jeq	L21
tst	-152.(r5)
jgt	L22
jsr	pc,_exit
L22:mov	r4,r0
mov	r5,r1
add	$0177570,r1
cmp	r0,r1
jhis	L23
movb	-152.(r5),(r4)+
L23:jbr	L20
L21:clrb	(r4)+
mov	-148.(r5),4.+_ttyb
mov	$_ttyb,(sp)
clr	-(sp)
jsr	pc,*$_stty
tst	(sp)+
.data
L24:.byte 012,0
.even
.text
mov	$01,(sp)
mov	$L24,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r5,r0
add	$0177560,r0
mov	r0,(sp)
jsr	pc,*$_crypt
mov	r0,r4
L25:cmpb	(r4)+,(r3)+
jne	L26
jbr	L25
L26:tstb	-(r4)
jne	L17
cmpb	-(r3),$072
jne	L17
L18:mov	r3,(sp)
jsr	pc,*$_colon
mov	r0,r3
clr	-154.(r5)
L27:cmpb	(r3),$072
jeq	L28
mov	-154.(r5),r1
mul	$012,r1
mov	r1,r0
movb	(r3)+,r1
add	r1,r0
add	$0177720,r0
mov	r0,-154.(r5)
jbr	L27
L28:inc	r3
clr	-156.(r5)
L29:cmpb	(r3),$072
jeq	L30
mov	-156.(r5),r1
mul	$012,r1
mov	r1,r0
movb	(r3)+,r1
add	r1,r0
add	$0177720,r0
mov	r0,-156.(r5)
jbr	L29
L30:inc	r3
mov	r3,(sp)
jsr	pc,*$_colon
mov	r0,r3
mov	r3,r4
mov	r3,(sp)
jsr	pc,*$_colon
mov	r0,r3
mov	r4,(sp)
jsr	pc,*$_chdir
tst	r0
jge	L31
.data
L32:.byte 0116,0157,040,0144,0151,0162,0145,0143,0164,0157,0162,0171,012,0
.even
.text
mov	$015,(sp)
mov	$L32,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L5
L31:mov	$10.+_utmp,(sp)
jsr	pc,*$_time
.data
L33:.byte 057,0164,0155,0160,057,0165,0164,0155,0160,0
.even
.text
mov	$01,(sp)
mov	$L33,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-150.(r5)
jlt	L34
movb	8.+_utmp,r0
mov	r0,-146.(r5)
cmp	-146.(r5),$0141
jlt	L35
sub	$047,-146.(r5)
L35:clr	(sp)
mov	-146.(r5),r0
add	$0177720,r0
ash	$04,r0
mov	r0,-(sp)
mov	-150.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$020,(sp)
mov	$_utmp,-(sp)
mov	-150.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	-150.(r5),(sp)
jsr	pc,*$_close
L34:.data
L36:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0167,0164,0155,0160,0
.even
.text
mov	$01,(sp)
mov	$L36,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-150.(r5)
jlt	L37
mov	$02,(sp)
clr	-(sp)
mov	-150.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$020,(sp)
mov	$_utmp,-(sp)
mov	-150.(r5),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	-150.(r5),(sp)
jsr	pc,*$_close
L37:.data
L38:.byte 057,0145,0164,0143,057,0155,0157,0164,0144,0
.even
.text
clr	(sp)
mov	$L38,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-150.(r5)
jlt	L39
L40:mov	$01,(sp)
mov	r5,r0
add	$0177556,r0
mov	r0,-(sp)
mov	-150.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
tst	r0
jle	L41
mov	$01,(sp)
mov	r5,r0
add	$0177556,r0
mov	r0,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L40
L41:mov	-150.(r5),(sp)
jsr	pc,*$_close
L39:.data
L42:.byte 0155,0141,0151,0154,0142,0157,0170,0
.even
.text
clr	(sp)
mov	$L42,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-150.(r5)
jlt	L43
.data
L44:.byte 0131,0157,0165,040,0150,0141,0166,0145,040,0155,0141,0151,0154,056,012,0
.even
.text
mov	$017,(sp)
mov	$L44,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	-150.(r5),(sp)
jsr	pc,*$_close
L43:mov	-154.(r5),(sp)
mov	_ttyx,-(sp)
jsr	pc,*$_chown
tst	(sp)+
mov	-156.(r5),(sp)
jsr	pc,*$_setgid
mov	-154.(r5),(sp)
jsr	pc,*$_setuid
tstb	(r3)
jne	L45
.data
L46:.byte 057,0142,0151,0156,057,0163,0150,0
.even
.text
mov	$L46,r3
L45:.data
L47:.byte 055,0
.even
.text
clr	(sp)
mov	$L47,-(sp)
mov	r3,-(sp)
jsr	pc,*$_execl
cmp	(sp)+,(sp)+
.data
L48:.byte 0116,0157,040,0163,0150,0145,0154,0154,056,012,0
.even
.text
mov	$011,(sp)
mov	$L48,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L17:.data
L49:.byte 0114,0157,0147,0151,0156,040,0151,0156,0143,0157,0162,0162,0145,0143,0164,056,012,0
.even
.text
mov	$021,(sp)
mov	$L49,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L5
L1:jmp	rretrn
.globl	_getpwent
.text
_getpwent:
jsr	r5,rsave; 10
mov	_fin,-8.(r5)
mov	$01,-10.(r5)
.data
L51:.byte 057,0145,0164,0143,057,0160,0141,0163,0163,0167,0144,0
.even
.text
clr	(sp)
mov	$L51,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fin
jlt	L52
L53:mov	4.(r5),r4
mov	6.(r5),r3
L54:jsr	pc,_getchar
mov	r0,-12.(r5)
cmp	$012,r0
jeq	L55
tst	-12.(r5)
jle	L52
movb	-12.(r5),(r3)+
jbr	L54
L55:clrb	(r3)+
mov	6.(r5),r3
L56:cmpb	(r4)+,(r3)+
jne	L57
jbr	L56
L57:cmpb	-(r4),$040
jeq	L10000
mov	4.(r5),r0
add	$010,r0
cmp	r4,r0
jlo	L53
L10000:cmpb	-(r3),$072
jne	L53
clr	-10.(r5)
L52:mov	_fin,(sp)
jsr	pc,*$_close
clr	_fin
clr	2.+_fin
clr	4.+_fin
mov	-10.(r5),r0
jbr	L50
L50:jmp	rretrn
.globl	_colon
.text
_colon:
jsr	r5,rsave; 2
mov	4.(r5),r4
L59:cmpb	(r4),$072
jeq	L60
tstb	(r4)+
jne	L61
.data
L62:.byte 0102,0141,0144,040,057,0145,0164,0143,057,0160,0141,0163,0163,0167,0144,012,0
.even
.text
mov	$020,(sp)
mov	$L62,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L61:jbr	L59
L60:clrb	(r4)+
mov	r4,r0
jbr	L58
L58:jmp	rretrn
