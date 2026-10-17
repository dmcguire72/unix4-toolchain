.globl	_password
.comm	_password,0144
.globl	_pwbuf
.comm	_pwbuf,0144
.globl	_ttybuf
.comm	_ttybuf,06
.globl	_main
.text
_main:
jsr	r5,rsave; 2
mov	$_pwbuf,(sp)
clr	-(sp)
jsr	pc,*$_getpw
tst	(sp)+
tst	r0
jne	L2
clr	2.+_fin
mov	$_pwbuf,r4
L3:cmpb	(r4),$072
jeq	L4
tstb	(r4)+
jeq	L2
jbr	L3
L4:inc	r4
mov	r4,r0
movb	(r0),r0
cmp	$072,r0
jeq	L5
mov	$_ttybuf,(sp)
clr	-(sp)
jsr	pc,*$_gtty
tst	(sp)+
bic	$!0177767,4.+_ttybuf
mov	$_ttybuf,(sp)
clr	-(sp)
jsr	pc,*$_stty
tst	(sp)+
.data
L6:.byte 0160,0141,0163,0163,0167,0157,0162,0144,072,040,0
.even
.text
mov	$L6,(sp)
jsr	pc,*$_printf
mov	$_password,r3
L7:jsr	pc,_getchar
movb	r0,(r3)
cmp	$012,r0
jeq	L8
tstb	(r3)+
jeq	L1
jbr	L7
L8:clrb	(r3)
bis	$010,4.+_ttybuf
mov	$_ttybuf,(sp)
clr	-(sp)
jsr	pc,*$_stty
tst	(sp)+
.data
L9:.byte 012,0
.even
.text
mov	$L9,(sp)
jsr	pc,*$_printf
mov	$_password,(sp)
jsr	pc,*$_crypt
mov	r0,r3
L10:cmpb	(r3)+,(r4)+
jne	L11
jbr	L10
L11:tstb	-(r3)
jne	L10000
cmpb	-(r4),$072
jeq	L5
L10000:jbr	L12
L2:.data
L13:.byte 0142,0141,0144,040,0160,0141,0163,0163,0167,0157,0162,0144,040,0146,0151,0154,0145,012,0
.even
.text
mov	$L13,(sp)
jsr	pc,*$_printf
L5:clr	(sp)
jsr	pc,*$_setuid
.data
L14:.byte 057,0142,0151,0156,057,0163,0150,0
.even
.text
.data
L15:.byte 055,0
.even
.text
clr	(sp)
mov	$L15,-(sp)
mov	$L14,-(sp)
jsr	pc,*$_execl
cmp	(sp)+,(sp)+
.data
L16:.byte 0143,0141,0156,0156,0157,0164,040,0145,0170,0145,0143,0165,0164,0145,040,0163,0150,0145,0154,0154,012,0
.even
.text
mov	$L16,(sp)
jsr	pc,*$_printf
L12:.data
L17:.byte 0163,0157,0162,0162,0171,012,0
.even
.text
mov	$L17,(sp)
jsr	pc,*$_printf
L1:jmp	rretrn
