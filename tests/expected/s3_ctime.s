.globl	_cbuf
.comm	_cbuf,032
.globl	_dmsize
.data
_dmsize=.
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
.globl	_timezone
.data
_timezone=.
18000.
.globl	_tzname
.data
_tzname=.
.text
L1:.byte 0105,0123,0124,0
.even
.data
L1
.text
L2:.byte 0105,0104,0124,0
.even
.data
L2
.globl	_daylight
.data
_daylight=.
1.
.globl	_nixonflg
.data
_nixonflg=.
1.
.globl	_ctime
.text
_ctime:
jsr	r5,rsave; 2
mov	4.(r5),(sp)
jsr	pc,*$_localtim
mov	r0,(sp)
jsr	pc,*$_asctime
jbr	L3
L3:jmp	rretrn
.globl	_localtim
.text
_localtim:
jsr	r5,rsave; 12
mov	r5,r0
add	$0177762,r0
mov	r0,r4
mov	*4.(r5),(r4)
mov	4.(r5),r0
mov	2.(r0),2.(r4)
mov	_timezone,r0
neg	r0
mov	r0,(sp)
mov	r4,-(sp)
jsr	pc,*$_dpadd
tst	(sp)+
mov	r4,(sp)
jsr	pc,*$_gmtime
mov	r0,r3
mov	14.(r3),r2
tst	_nixonflg
jeq	L5
cmp	10.(r3),$0112
jgt	L10000
cmp	10.(r3),$0112
jne	L5
cmp	r2,$05
jgt	L10001
cmp	r2,$05
jne	L5
cmp	4.(r3),$02
jlt	L5
L10001:L10000:bis	$01,_daylight
mov	$0177777,-8.(r5)
mov	$0557,-10.(r5)
jbr	L6
L5:mov	$0167,(sp)
mov	r3,-(sp)
jsr	pc,*$_sunday
tst	(sp)+
mov	r0,-8.(r5)
mov	$0457,(sp)
mov	r3,-(sp)
jsr	pc,*$_sunday
tst	(sp)+
mov	r0,-10.(r5)
L6:tst	_daylight
jeq	L7
cmp	r2,-8.(r5)
jgt	L10002
cmp	r2,-8.(r5)
jne	L7
cmp	4.(r3),$02
jlt	L7
L10002:cmp	r2,-10.(r5)
jlt	L10003
cmp	r2,-10.(r5)
jne	L7
cmp	4.(r3),$01
jge	L7
L10003:mov	$07020,(sp)
mov	r4,-(sp)
jsr	pc,*$_dpadd
tst	(sp)+
mov	r4,(sp)
jsr	pc,*$_gmtime
mov	r0,r3
inc	16.(r3)
L7:mov	r3,r0
jbr	L4
L4:jmp	rretrn
.globl	_sunday
.text
_sunday:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	6.(r5),r3
mov	10.(r4),(sp)
jsr	pc,*$_dysize
add	6.(r5),r0
add	$0177223,r0
mov	r0,r3
mov	r3,r0
sub	14.(r4),r0
add	12.(r4),r0
add	$01274,r0
mov	r0,r1
sxt	r0
div	$07,r0
mov	r1,-(sp)
mov	r3,r0
sub	(sp)+,r0
jbr	L8
L8:jmp	rretrn
.globl	_gmtime
.text
_gmtime:
.bss
L10:.=.+18.
.text
jsr	r5,rsave; 2
mov	$070200,(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	*4.(r5),-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,r4
mov	_ldivr,r3
mov	$L10,r2
mov	r3,r1
sxt	r0
div	$074,r0
mov	r1,(r2)+
mov	r3,r1
sxt	r0
div	$074,r0
mov	r0,r3
mov	r3,r1
sxt	r0
div	$074,r0
mov	r1,(r2)+
mov	r3,r1
sxt	r0
div	$074,r0
mov	r0,r3
mov	r4,r1
sxt	r0
div	$03,r0
ash	$03,r1
add	r1,r3
mov	r4,r1
sxt	r0
div	$03,r0
mov	r0,r4
mov	r3,(r2)+
mov	r4,r1
add	$04,r1
sxt	r0
div	$07,r0
mov	r1,12.+L10
mov	$0106,r3
L11:mov	r3,(sp)
jsr	pc,*$_dysize
cmp	r4,r0
jlt	L12
mov	r3,(sp)
jsr	pc,*$_dysize
sub	r0,r4
L13:inc	r3
jbr	L11
L12:mov	r3,10.+L10
mov	r4,14.+L10
mov	r3,(sp)
jsr	pc,*$_dysize
cmp	$0556,r0
jne	L14
mov	$035,2.+_dmsize
L14:clr	r3
L15:mov	r3,r0
asl	r0
cmp	_dmsize(r0),r4
jgt	L16
mov	r3,r0
asl	r0
sub	_dmsize(r0),r4
L17:inc	r3
jbr	L15
L16:mov	$034,2.+_dmsize
mov	r4,r0
inc	r0
mov	r0,(r2)+
mov	r3,(r2)+
clr	16.+L10
mov	$L10,r0
jbr	L9
L9:jmp	rretrn
.globl	_asctime
.text
_asctime:
jsr	r5,rsave; 2
mov	$_cbuf,r4
.data
L21:.byte 0104,0141,0171,040,0115,0157,0156,040,060,060,040,060,060,072,060,060,072,060,060,040,061,071,060,060,012,0
.even
.text
mov	$L21,r3
L19:movb	(r3)+,(r4)+
jeq	L20
jbr	L19
L20:.data
L22:.byte 0123,0165,0156,0115,0157,0156,0124,0165,0145,0127,0145,0144,0124,0150,0165,0106,0162,0151,0123,0141,0164,0
.even
.text
mov	4.(r5),r1
mov	12.(r1),r1
mul	$03,r1
add	$L22,r1
mov	r1,r3
mov	$_cbuf,r4
movb	(r3)+,(r4)+
movb	(r3)+,(r4)+
movb	(r3)+,(r4)+
inc	r4
mov	4.(r5),r0
add	$010,r0
mov	r0,r2
.data
L23:.byte 0112,0141,0156,0106,0145,0142,0115,0141,0162,0101,0160,0162,0115,0141,0171,0112,0165,0156,0112,0165,0154,0101,0165,0147,0123,0145,0160,0117,0143,0164,0116,0157,0166,0104,0145,0143,0
.even
.text
mov	(r2),r1
mul	$03,r1
add	$L23,r1
mov	r1,r3
movb	(r3)+,(r4)+
movb	(r3)+,(r4)+
movb	(r3)+,(r4)+
mov	-(r2),(sp)
mov	r4,-(sp)
jsr	pc,*$_numb
tst	(sp)+
mov	r0,r4
mov	-(r2),(sp)
add	$0144,(sp)
mov	r4,-(sp)
jsr	pc,*$_numb
tst	(sp)+
mov	r0,r4
mov	-(r2),(sp)
add	$0144,(sp)
mov	r4,-(sp)
jsr	pc,*$_numb
tst	(sp)+
mov	r0,r4
mov	-(r2),(sp)
add	$0144,(sp)
mov	r4,-(sp)
jsr	pc,*$_numb
tst	(sp)+
mov	r0,r4
add	$02,r4
mov	4.(r5),r0
mov	10.(r0),(sp)
mov	r4,-(sp)
jsr	pc,*$_numb
tst	(sp)+
mov	r0,r4
mov	$_cbuf,r0
jbr	L18
L18:jmp	rretrn
.globl	_dysize
.text
_dysize:
jsr	r5,rsave; 2
mov	4.(r5),r1
sxt	r0
div	$04,r0
tst	r1
jne	L25
mov	$0556,r0
jbr	L24
L25:mov	$0555,r0
jbr	L24
L24:jmp	rretrn
.globl	_numb
.text
_numb:
jsr	r5,rsave; 2
mov	4.(r5),r4
inc	r4
cmp	6.(r5),$012
jlt	L27
mov	6.(r5),r1
sxt	r0
div	$012,r0
mov	r0,r1
sxt	r0
div	$012,r0
add	$060,r1
mov	r1,r0
movb	r0,(r4)+
jbr	L28
L27:movb	$040,(r4)+
L28:mov	6.(r5),r1
sxt	r0
div	$012,r0
add	$060,r1
mov	r1,r0
movb	r0,(r4)+
mov	r4,r0
jbr	L26
L26:jmp	rretrn
