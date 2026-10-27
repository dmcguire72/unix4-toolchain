.text
L1:.byte 057,0142,0151,0156,057,0163,0150,0
.even
.data
.globl	_shell
.data
_shell=.
.text
_shell=L1
.text
L2:.byte 055,0
.even
.data
.globl	_minus
.data
_minus=.
.text
_minus=L2
.text
L3:.byte 057,0145,0164,0143,057,0162,0143,0
.even
.data
.globl	_runc
.data
_runc=.
.text
_runc=L3
.text
L4:.byte 057,0145,0164,0143,057,0151,0156,0151,0164,0
.even
.data
.globl	_init
.data
_init=.
.text
_init=L4
.text
L5:.byte 057,0145,0164,0143,057,0164,0164,0171,0163,0
.even
.data
.globl	_ifile
.data
_ifile=.
.text
_ifile=L5
.text
L6:.byte 057,0164,0155,0160,057,0165,0164,0155,0160,0
.even
.data
.globl	_utmp
.data
_utmp=.
.text
_utmp=L6
.text
L7:.byte 057,0165,0163,0162,057,0141,0144,0155,057,0167,0164,0155,0160,0
.even
.data
.globl	_wtmpf
.data
_wtmpf=.
.text
_wtmpf=L7
.text
L8:.byte 057,0144,0145,0166,057,0164,0164,0171,070,0
.even
.data
.globl	_ctty
.data
_ctty=.
.text
_ctty=L8
.globl	_com
.data
_com=.
.text
L9:.byte 057,0145,0164,0143,057,0147,0145,0164,0164,0171,0
.even
.data
L9
.globl	_fi
.comm	_fi,02
.globl	_line
.comm	_line,06
.globl	_itab
.comm	_itab,0170
.globl	_wtmp
.comm	_wtmp,020
.globl	_main
.text
_main:
jsr	r5,rsave; 2
jsr	pc,_getcsw
cmp	$0173030,r0
jeq	L11
jsr	pc,_fork
mov	r0,r4
tst	r4
jne	L12
clr	(sp)
mov	$_runc,-(sp)
mov	$_shell,-(sp)
mov	$_shell,-(sp)
jsr	pc,*$_execl
add	$06,sp
jsr	pc,_exit
L12:L13:jsr	pc,_wait
cmp	r4,r0
jeq	L14
jbr	L13
L14:mov	$0644,(sp)
mov	$_utmp,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,(sp)
jsr	pc,*$_close
mov	$01,(sp)
mov	$_wtmpf,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,r4
jlt	L15
mov	$02,(sp)
clr	-(sp)
mov	r4,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
movb	$0176,8.+_wtmp
mov	$10.+_wtmp,(sp)
jsr	pc,*$_time
mov	$020,(sp)
mov	$_wtmp,-(sp)
mov	r4,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r4,(sp)
jsr	pc,*$_close
L15:L11:jsr	pc,_setexit
mov	$_reset,(sp)
mov	$01,-(sp)
jsr	pc,*$_signal
tst	(sp)+
clr	r4
L16:cmp	r4,$012
jge	L17
mov	r4,(sp)
jsr	pc,*$_close
L18:inc	r4
jbr	L16
L17:jsr	pc,_getcsw
jbr	L20
L21:L22:jsr	pc,_termall
jsr	pc,_fork
mov	r0,r4
tst	r4
jne	L23
mov	$02,(sp)
mov	$_ctty,-(sp)
jsr	pc,*$_open
tst	(sp)+
clr	(sp)
jsr	pc,*$_dup
clr	(sp)
mov	$_minus,-(sp)
mov	$_shell,-(sp)
jsr	pc,*$_execl
cmp	(sp)+,(sp)+
jsr	pc,_exit
L23:L24:jsr	pc,_wait
cmp	r4,r0
jeq	L25
jbr	L24
L25:L26:jsr	pc,_termall
clr	(sp)
mov	$_minus,-(sp)
mov	$_init,-(sp)
jsr	pc,*$_execl
cmp	(sp)+,(sp)+
jsr	pc,_reset
jbr	L19
L20:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:173030
173040
L10002:..
L21
L26
L27
.text
/esw
L19:L27:clr	(sp)
mov	$_ifile,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
mov	$_itab,r2
L28:jsr	pc,_rline
tst	r0
jeq	L29
cmp	_line,$060
jeq	L28
mov	$_itab,r3
L30:cmp	r3,$120.+_itab
jhis	L31
cmp	2.(r3),2.+_line
jeq	L10004
tst	2.(r3)
jne	L33
L10004:cmp	r3,r2
jlo	L34
mov	(r3),r4
mov	(r2),(r3)
mov	r4,(r2)
mov	2.(r2),2.(r3)
mov	4.(r2),4.(r3)
mov	2.+_line,2.(r2)
mov	4.+_line,4.(r2)
add	$06,r2
L34:jbr	L31
L33:L32:add	$06,r3
jbr	L30
L31:jbr	L28
L29:mov	_fi,(sp)
jsr	pc,*$_close
cmp	r2,$_itab
jeq	L22
L35:cmp	r2,$120.+_itab
jhis	L36
mov	r2,(sp)
jsr	pc,*$_term
L37:add	$06,r2
jbr	L35
L36:mov	$_itab,r3
L38:cmp	r3,$120.+_itab
jhis	L39
tst	2.(r3)
jeq	L41
tst	(r3)
jne	L41
mov	r3,(sp)
jsr	pc,*$_dfork
L41:L40:add	$06,r3
jbr	L38
L39:L42:jsr	pc,_wait
mov	r0,r4
mov	$_itab,r3
L44:cmp	r3,$120.+_itab
jhis	L45
cmp	(r3),r4
jne	L47
mov	r3,(sp)
jsr	pc,*$_rmut
mov	r3,(sp)
jsr	pc,*$_dfork
L47:L46:add	$06,r3
jbr	L44
L45:jbr	L42
L43:L10:jmp	rretrn
.globl	_termall
.text
_termall:
jsr	r5,rsave; 2
mov	$_itab,r4
L49:cmp	r4,$120.+_itab
jhis	L50
mov	r4,(sp)
jsr	pc,*$_term
L51:add	$06,r4
jbr	L49
L50:L48:jmp	rretrn
.globl	_term
.text
_term:
jsr	r5,rsave; 2
mov	4.(r5),r4
tst	(r4)
jeq	L53
mov	r4,(sp)
jsr	pc,*$_rmut
mov	$011,(sp)
mov	(r4),-(sp)
jsr	pc,*$_kill
tst	(sp)+
L53:clr	(r4)
clr	2.(r4)
L52:jmp	rretrn
.globl	_rline
.text
_rline:
.bss
L55:.=.+4.
.text
jsr	r5,rsave; 2
mov	$04,(sp)
mov	$L55,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$04,r0
jne	L10005
cmpb	3.+L55,$012
jeq	L56
L10005:clr	r0
jbr	L54
L56:movb	L55,r0
mov	r0,_line
movb	1.+L55,r0
mov	r0,2.+_line
movb	2.+L55,r0
add	$0177720,r0
mov	r0,4.+_line
tst	4.+_line
jlt	L10006
cmp	4.+_line,$01
jlt	L57
L10006:clr	4.+_line
L57:mov	$01,r0
jbr	L54
L54:jmp	rretrn
.globl	_dfork
.text
_dfork:
jsr	r5,rsave; 2
mov	4.(r5),r2
jsr	pc,_fork
mov	r0,r4
tst	r4
jne	L59
clr	(sp)
mov	$01,-(sp)
jsr	pc,*$_signal
tst	(sp)+
.data
L60:.byte 057,0144,0145,0166,057,0164,0164,0171,0170,0
.even
.text
mov	$L60,r3
movb	2.(r2),8.(r3)
clr	(sp)
mov	r3,-(sp)
jsr	pc,*$_chown
tst	(sp)+
mov	$0622,(sp)
mov	r3,-(sp)
jsr	pc,*$_chmod
tst	(sp)+
mov	$02,(sp)
mov	r3,-(sp)
jsr	pc,*$_open
tst	(sp)+
clr	(sp)
jsr	pc,*$_dup
clr	(sp)
mov	$_minus,-(sp)
mov	4.(r2),r0
asl	r0
mov	_com(r0),-(sp)
jsr	pc,*$_execl
cmp	(sp)+,(sp)+
jsr	pc,_exit
L59:mov	r4,(r2)
L58:jmp	rretrn
.globl	_rmut
.text
_rmut:
.bss
L62:.=.+16.
.text
jsr	r5,rsave; 2
mov	$01,(sp)
mov	$_utmp,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,r3
tst	r3
jlt	L63
mov	4.(r5),r0
mov	2.(r0),r4
cmp	r4,$0141
jlt	L64
add	$0177731,r4
L64:clr	(sp)
mov	r4,r0
add	$0177720,r0
ash	$04,r0
mov	r0,-(sp)
mov	r3,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$020,(sp)
mov	$L62,-(sp)
mov	r3,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r3,(sp)
jsr	pc,*$_close
L63:mov	$01,(sp)
mov	$_wtmpf,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,r3
tst	r3
jlt	L65
mov	4.(r5),r0
movb	2.(r0),8.+_wtmp
mov	$10.+_wtmp,(sp)
jsr	pc,*$_time
mov	$02,(sp)
clr	-(sp)
mov	r3,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$020,(sp)
mov	$_wtmp,-(sp)
mov	r3,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r3,(sp)
jsr	pc,*$_close
L65:L61:jmp	rretrn
