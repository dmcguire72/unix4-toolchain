.globl	_dayw
.data
_dayw=.
.text
L1:.byte 040,0123,040,040,0115,040,0124,0165,040,040,0127,040,0124,0150,040,040,0106,040,040,0123,0
.even
.data
.text
_dayw=L1
.globl	_smon
.data
_smon=.
.text
L2:.byte 0112,0141,0156,0
.even
.data
L2
.text
L3:.byte 0106,0145,0142,0
.even
.data
L3
.text
L4:.byte 0115,0141,0162,0
.even
.data
L4
.text
L5:.byte 0101,0160,0162,0
.even
.data
L5
.text
L6:.byte 0115,0141,0171,0
.even
.data
L6
.text
L7:.byte 0112,0165,0156,0
.even
.data
L7
.text
L8:.byte 0112,0165,0154,0
.even
.data
L8
.text
L9:.byte 0101,0165,0147,0
.even
.data
L9
.text
L10:.byte 0123,0145,0160,0
.even
.data
L10
.text
L11:.byte 0117,0143,0164,0
.even
.data
L11
.text
L12:.byte 0116,0157,0166,0
.even
.data
L12
.text
L13:.byte 0104,0145,0143,0
.even
.data
L13
.globl	_string
.comm	_string,0660
.globl	_main
.text
_main:
jsr	r5,rsave; 4
cmp	4.(r5),$02
jge	L15
.data
L16:.byte 0165,0163,0141,0147,0145,072,040,0143,0141,0154,040,0133,0155,0157,0156,0164,0150,0135,040,0171,0145,0141,0162,012,0
.even
.text
mov	$L16,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L15:cmp	4.(r5),$02
jeq	L17
mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
mov	r0,-8.(r5)
cmp	-8.(r5),$01
jlt	L18
cmp	-8.(r5),$014
jgt	L18
mov	6.(r5),r0
mov	4.(r0),(sp)
jsr	pc,*$_number
mov	r0,r4
cmp	r4,$01
jlt	L18
cmp	r4,$023417
jgt	L18
.data
L19:.byte 040,040,040,040,040,040,045,0163,040,045,0154,012,0
.even
.text
mov	r4,(sp)
mov	-8.(r5),r0
asl	r0
mov	-2.+_smon(r0),-(sp)
mov	$L19,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
.data
L20:.byte 045,0163,012,0
.even
.text
mov	$_dayw,(sp)
mov	$L20,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$030,(sp)
mov	$_string,-(sp)
mov	r4,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_cal
add	$06,sp
clr	r3
L21:cmp	r3,$0220
jge	L22
mov	$030,(sp)
mov	r3,-(sp)
add	$_string,(sp)
jsr	pc,*$_pstr
tst	(sp)+
L23:add	$030,r3
jbr	L21
L22:jsr	pc,_exit
L17:mov	6.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_number
mov	r0,r4
cmp	r4,$01
jlt	L18
cmp	r4,$023417
jgt	L18
.data
L24:.byte 012,012,012,0
.even
.text
mov	$L24,(sp)
jsr	pc,*$_printf
.data
L25:.byte 011,011,011,011,045,0154,012,0
.even
.text
mov	r4,(sp)
mov	$L25,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L26:.byte 012,0
.even
.text
mov	$L26,(sp)
jsr	pc,*$_printf
clr	r3
L27:cmp	r3,$014
jge	L28
clr	r2
L30:cmp	r2,$0660
jge	L31
clrb	_string(r2)
L32:inc	r2
jbr	L30
L31:.data
L33:.byte 011,040,045,0163,0
.even
.text
mov	r3,r0
asl	r0
mov	_smon(r0),(sp)
mov	$L33,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L34:.byte 011,011,011,045,0163,0
.even
.text
mov	r3,r0
asl	r0
mov	2.+_smon(r0),(sp)
mov	$L34,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L35:.byte 011,011,040,040,040,040,040,040,040,045,0163,012,0
.even
.text
mov	r3,r0
asl	r0
mov	4.+_smon(r0),(sp)
mov	$L35,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L36:.byte 045,0163,040,040,040,045,0163,040,040,040,045,0163,012,0
.even
.text
mov	$_dayw,(sp)
mov	$_dayw,-(sp)
mov	$_dayw,-(sp)
mov	$L36,-(sp)
jsr	pc,*$_printf
add	$06,sp
mov	$0110,(sp)
mov	$_string,-(sp)
mov	r4,-(sp)
mov	r3,-(sp)
inc	(sp)
jsr	pc,*$_cal
add	$06,sp
mov	$0110,(sp)
mov	$23.+_string,-(sp)
mov	r4,-(sp)
mov	r3,-(sp)
add	$02,(sp)
jsr	pc,*$_cal
add	$06,sp
mov	$0110,(sp)
mov	$46.+_string,-(sp)
mov	r4,-(sp)
mov	r3,-(sp)
add	$03,(sp)
jsr	pc,*$_cal
add	$06,sp
clr	r2
L37:cmp	r2,$0660
jge	L38
mov	$0110,(sp)
mov	r2,-(sp)
add	$_string,(sp)
jsr	pc,*$_pstr
tst	(sp)+
L39:add	$0110,r2
jbr	L37
L38:L29:add	$03,r3
jbr	L27
L28:.data
L40:.byte 012,012,012,0
.even
.text
mov	$L40,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L18:.data
L41:.byte 0102,0141,0144,040,0141,0162,0147,0165,0155,0145,0156,0164,012,0
.even
.text
mov	$L41,(sp)
jsr	pc,*$_printf
L14:jmp	rretrn
.globl	_number
.text
_number:
jsr	r5,rsave; 2
clr	r4
mov	4.(r5),r2
L43:movb	(r2)+,r3
jeq	L44
cmp	r3,$060
jlt	L10000
cmp	r3,$071
jle	L45
L10000:clr	r0
jbr	L42
L45:mov	r4,r1
mul	$012,r1
add	r3,r1
add	$0177720,r1
mov	r1,r4
jbr	L43
L44:mov	r4,r0
jbr	L42
L42:jmp	rretrn
.globl	_pstr
.text
_pstr:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	6.(r5),r4
L47:mov	r4,r0
dec	r4
tst	r0
jeq	L48
tstb	(r3)+
jne	L49
movb	$040,-1.(r3)
L49:jbr	L47
L48:mov	6.(r5),r0
inc	r0
mov	r0,r4
L50:mov	r4,r0
dec	r4
tst	r0
jeq	L51
cmpb	-(r3),$040
jne	L51
jbr	L50
L51:clrb	1.(r3)
.data
L52:.byte 045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L52,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L46:jmp	rretrn
.globl	_mon
.data
_mon=.
.byte 0.
.byte 31.
.byte 29.
.byte 31.
.byte 30.
.byte 31.
.byte 30.
.byte 31.
.byte 31.
.byte 30.
.byte 31.
.byte 30.
.byte 31.
.even
.globl	_cal
.text
_cal:
jsr	r5,rsave; 2
mov	8.(r5),r2
mov	6.(r5),(sp)
jsr	pc,*$_jan1
mov	r0,r4
movb	$035,2.+_mon
movb	$036,9.+_mon
mov	6.(r5),(sp)
inc	(sp)
jsr	pc,*$_jan1
add	$07,r0
sub	r4,r0
mov	r0,r1
sxt	r0
div	$07,r0
mov	r1,r0
jbr	L55
L56:movb	$034,2.+_mon
jbr	L54
L57:movb	$023,9.+_mon
jbr	L54
L58:jbr	L54
L55:sub	$01,r0
cmp	r0,$01
jhi	L57
asl	r0
jmp	*.+4(r0)
L56
L58
/esw
L54:mov	$01,r3
L59:cmp	r3,4.(r5)
jge	L60
movb	_mon(r3),r0
add	r0,r4
L61:inc	r3
jbr	L59
L60:mov	r4,r1
sxt	r0
div	$07,r0
mov	r1,r4
mov	r4,r1
mul	$03,r1
add	r1,r2
mov	$01,r3
L62:mov	4.(r5),r0
cmpb	_mon(r0),r3
jlt	L63
cmp	r3,$03
jeq	L10002
clr	r0
jbr	L10003
L10002:mov	$01,r0
L10003:mov	4.(r5),r1
movb	_mon(r1),r1
cmp	$023,r1
jeq	L10004
clr	r1
jbr	L10005
L10004:mov	$01,r1
L10005:bit	r1,r0
jeq	L65
add	$013,r3
mov	$013,-(sp)
mov	4.(r5),r0
movb	_mon(r0),r1
add	(sp)+,r1
movb	r1,_mon(r0)
L65:cmp	r3,$011
jle	L66
mov	r3,r1
sxt	r0
div	$012,r0
add	$060,r0
movb	r0,(r2)
L66:inc	r2
mov	r3,r1
sxt	r0
div	$012,r0
add	$060,r1
mov	r1,r0
movb	r0,(r2)+
inc	r2
inc	r4
mov	r4,r0
cmp	$07,r0
jne	L67
clr	r4
mov	8.(r5),r0
add	10.(r5),r0
mov	r0,r2
mov	r2,8.(r5)
L67:L64:inc	r3
jbr	L62
L63:L53:jmp	rretrn
.globl	_jan1
.text
_jan1:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	r4,r0
add	$03,r0
ash	$-02,r0
add	r4,r0
add	$04,r0
mov	r0,r3
cmp	r4,$03410
jle	L69
mov	r4,r1
add	$0174533,r1
sxt	r0
div	$0144,r0
sub	r0,r3
mov	r4,r1
add	$0174677,r1
sxt	r0
div	$0620,r0
add	r0,r3
L69:cmp	r4,$03330
jle	L70
add	$03,r3
L70:mov	r3,r1
sxt	r0
div	$07,r0
mov	r1,r0
jbr	L68
L68:jmp	rretrn
