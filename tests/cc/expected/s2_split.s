.globl	_count
.data
_count=.
1000.
.globl	_fnumber
.comm	_fnumber,02
.globl	_ibuf
.comm	_ibuf,01006
.globl	_obuf
.comm	_obuf,01006
.globl	_fname
.comm	_fname,0144
.globl	_ifil
.comm	_ifil,02
.globl	_ofil
.comm	_ofil,02
.globl	_main
.text
_main:
jsr	r5,rsave; 2
mov	$01,r4
L2:cmp	r4,4.(r5)
jge	L3
mov	r4,r0
asl	r0
add	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L5
mov	r4,r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
movb	1.(r0),r0
jbr	L7
L8:mov	$01,_ifil
jbr	L4
L9:L10:L11:L12:L13:L14:L15:L16:L17:L18:mov	r4,r0
asl	r0
add	6.(r5),r0
mov	(r0),(sp)
inc	(sp)
jsr	pc,*$_number
mov	r0,_count
jbr	L4
jbr	L6
L7:jsr	r2,hsw; 2; L10001
jmp	*L10004-L10002(r1)
.data
L10001:L10002
L10003
L10004
L10002:..
0
30
31
32
33
34
L10003:..
30
31
32
33
34
L10004:L19
L8
L9
L11
L13
L15
L17
L19
L10
L12
L14
L16
L18
.text
/esw
L6:L19:jbr	L20
L5:tst	_ifil
jeq	L21
mov	r4,r0
asl	r0
add	6.(r5),r0
mov	(r0),_ofil
jbr	L22
L21:mov	r4,r0
asl	r0
add	6.(r5),r0
mov	(r0),_ifil
L22:L20:L4:inc	r4
jbr	L2
L3:tst	_ifil
jeq	L23
cmp	_ifil,$01
jeq	L23
mov	$_ibuf,(sp)
mov	_ifil,-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L24
.data
L25:.byte 0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,0151,0156,0160,0165,0164,012,0
.even
.text
mov	$022,(sp)
mov	$L25,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L24:L23:tst	_ofil
jne	L26
.data
L27:.byte 0170,0
.even
.text
mov	$L27,_ofil
L26:L28:mov	$01,r2
clr	r4
L29:cmp	r4,_count
jge	L30
L34:mov	$_ibuf,(sp)
jsr	pc,*$_getc
mov	r0,r3
tst	r3
jge	L35
tst	r2
jne	L36
mov	$_obuf,(sp)
jsr	pc,*$_fflush
L36:jsr	pc,_exit
L35:tst	r2
jeq	L37
clr	r2
L38:mov	_ofil,r0
add	r2,r0
tstb	(r0)
jeq	L39
mov	_ofil,r0
add	r2,r0
movb	(r0),_fname(r2)
L40:inc	r2
jbr	L38
L39:mov	_fnumber,r1
sxt	r0
div	$032,r0
add	$0141,r0
mov	r2,r1
inc	r2
movb	r0,_fname(r1)
mov	_fnumber,r1
sxt	r0
div	$032,r0
add	$0141,r1
mov	r1,r0
mov	r2,r1
inc	r2
movb	r0,_fname(r1)
clrb	_fname(r2)
inc	_fnumber
mov	$_obuf,(sp)
mov	$_fname,-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jge	L41
.data
L42:.byte 0103,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,0145,040,0157,0165,0164,0160,0165,0164,012,0
.even
.text
mov	$024,(sp)
mov	$L42,-(sp)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L41:clr	r2
L37:mov	$_obuf,(sp)
mov	r3,-(sp)
jsr	pc,*$_putc
tst	(sp)+
L32:cmp	r3,$012
jne	L34
L33:L31:inc	r4
jbr	L29
L30:mov	$_obuf,(sp)
jsr	pc,*$_fflush
mov	_obuf,(sp)
jsr	pc,*$_close
jbr	L28
L1:jmp	rretrn
.globl	_number
.text
_number:
jsr	r5,rsave; 2
clr	r4
mov	4.(r5),r3
L44:tstb	(r3)
jeq	L45
cmpb	(r3),$060
jlt	L47
cmpb	(r3),$071
jgt	L47
mov	r4,r1
mul	$012,r1
mov	r1,r0
movb	(r3),r1
add	r1,r0
add	$0177720,r0
mov	r0,r4
L47:L46:inc	r3
jbr	L44
L45:mov	r4,r0
jbr	L43
L43:jmp	rretrn
