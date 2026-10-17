.globl	_fout
.comm	_fout,02
.globl	_cflg
.comm	_cflg,02
.globl	_nflg
.comm	_nflg,02
.globl	_uflg
.comm	_uflg,02
.globl	_rflg
.data
_rflg=.
1.
.globl	_gflg
.comm	_gflg,02
.globl	_pflg
.comm	_pflg,02
.globl	_nlp
.comm	_nlp,02
.globl	_fi
.comm	_fi,02
.globl	_buf
.comm	_buf,020
.globl	_main
.text
_main:
jsr	r5,rsave; 10
dec	4.(r5)
jle	L2
mov	6.(r5),r0
movb	*2.(r0),r0
cmp	$055,r0
jne	L2
add	$02,6.(r5)
L3:inc	*6.(r5)
mov	*6.(r5),r0
tstb	(r0)
jeq	L4
mov	6.(r5),r0
movb	*(r0),r0
jbr	L6
L7:inc	_nflg
jbr	L3
L8:inc	_cflg
jbr	L3
L9:inc	_gflg
jbr	L3
L10:inc	_uflg
jbr	L3
L11:mov	$0177777,_rflg
jbr	L3
L12:inc	_pflg
jbr	L3
L13:jbr	L3
jbr	L5
L6:sub	$0143,r0
cmp	r0,$022
jhi	L13
asl	r0
jmp	*.+4(r0)
L8
L13
L13
L13
L9
L13
L13
L13
L13
L13
L13
L7
L13
L12
L13
L11
L13
L13
L10
/esw
L5:jbr	L3
L4:dec	4.(r5)
L2:tst	4.(r5)
jne	L14
.data
L15:.byte 0141,056,0157,0165,0164,0
.even
.text
clr	(sp)
mov	$L15,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
jbr	L16
L14:clr	(sp)
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fi
L16:tst	_fi
jge	L17
.data
L18:.byte 0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,0151,0156,0160,0165,0164,012,0
.even
.text
mov	$L18,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L17:mov	$020,(sp)
mov	$_buf,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	_buf,$0407
jeq	L19
cmp	_buf,$0410
jeq	L19
.data
L20:.byte 0142,0141,0144,040,0146,0157,0162,0155,0141,0164,012,0
.even
.text
mov	$L20,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L19:mov	$01,(sp)
mov	2.+_buf,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01,(sp)
mov	4.+_buf,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
cmp	14.+_buf,$01
jeq	L21
mov	$01,(sp)
mov	2.+_buf,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01,(sp)
mov	4.+_buf,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L21:mov	$014,(sp)
mov	8.+_buf,-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jne	L22
.data
L23:.byte 0156,0157,040,0156,0141,0155,0145,040,0154,0151,0163,0164,012,0
.even
.text
mov	$L23,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L22:mov	-8.(r5),r1
mul	$014,r1
mov	r1,(sp)
jsr	pc,*$_sbrk
mov	r0,_nlp
mov	-8.(r5),r1
mul	$014,r1
mov	r1,(sp)
mov	_nlp,-(sp)
mov	_fi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
tst	_pflg
jne	L24
mov	$_compare,(sp)
mov	$014,-(sp)
mov	-8.(r5),-(sp)
mov	_nlp,-(sp)
jsr	pc,*$_qsort
add	$06,sp
L24:mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
mov	$01,(sp)
jsr	pc,*$_close
clr	-10.(r5)
L25:cmp	-10.(r5),-8.(r5)
jge	L26
tst	_gflg
jeq	L10001
mov	_nlp,r0
bit	$040,8.(r0)
jeq	L28
L10001:tst	_cflg
jeq	L29
cmpb	*_nlp,$0137
jne	L28
clr	-12.(r5)
L30:cmp	-12.(r5),$07
jge	L31
mov	_nlp,r0
add	-12.(r5),r0
mov	_nlp,r1
add	-12.(r5),r1
movb	1.(r1),(r0)
L32:inc	-12.(r5)
jbr	L30
L31:mov	_nlp,r0
clrb	7.(r0)
L29:mov	_nlp,r0
mov	8.(r0),r0
bic	$!037,r0
mov	r0,-12.(r5)
cmp	-12.(r5),$04
jle	L33
mov	$01,-12.(r5)
L33:tst	-12.(r5)
jne	L34
mov	_nlp,r0
tst	10.(r0)
jeq	L34
mov	$05,-12.(r5)
L34:tst	_uflg
jeq	L10002
tst	-12.(r5)
jne	L28
L10002:tst	_uflg
jne	L35
tst	-12.(r5)
jne	L36
.data
L37:.byte 040,040,040,040,040,040,0
.even
.text
mov	$L37,(sp)
jsr	pc,*$_printf
jbr	L38
L36:mov	_nlp,r0
mov	10.(r0),(sp)
jsr	pc,*$_printo
L38:.data
L39:.byte 045,0143,040,0
.even
.text
.data
L40:.byte 0125,0101,0124,0104,0102,0103,0
.even
.text
.data
L41:.byte 0165,0141,0164,0144,0142,0143,0
.even
.text
mov	_nlp,r0
bit	$040,8.(r0)
jeq	L10003
mov	$L40,r0
jbr	L10004
L10003:mov	$L41,r0
L10004:add	-12.(r5),r0
movb	(r0),r0
mov	r0,(sp)
mov	$L39,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L35:.data
L42:.byte 045,056,070,0163,012,0
.even
.text
mov	_nlp,(sp)
mov	$L42,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L28:add	$014,_nlp
L27:inc	-10.(r5)
jbr	L25
L26:jsr	pc,_flush
L1:jmp	rretrn
.globl	_compare
.text
_compare:
jsr	r5,rsave; 6
clr	-8.(r5)
tst	_nflg
jeq	L44
mov	4.(r5),r0
mov	6.(r5),r1
cmp	10.(r0),10.(r1)
jlos	L45
mov	$01,-8.(r5)
jbr	L46
L45:mov	4.(r5),r0
mov	6.(r5),r1
cmp	10.(r0),10.(r1)
jhis	L47
mov	$0177777,-8.(r5)
jbr	L46
L47:L44:clr	-10.(r5)
L48:cmp	-10.(r5),$010
jge	L49
mov	4.(r5),r0
add	-10.(r5),r0
mov	6.(r5),r1
add	-10.(r5),r1
cmpb	(r0),(r1)
jeq	L51
mov	4.(r5),r0
add	-10.(r5),r0
mov	6.(r5),r1
add	-10.(r5),r1
cmpb	(r0),(r1)
jle	L52
mov	$01,-8.(r5)
jbr	L53
L52:mov	$0177777,-8.(r5)
L53:jbr	L46
L51:L50:inc	-10.(r5)
jbr	L48
L49:L46:mov	-8.(r5),r1
mul	_rflg,r1
mov	r1,r0
jbr	L43
L43:jmp	rretrn
.globl	_printo
.text
_printo:
jsr	r5,rsave; 4
.data
L55:.byte 045,0143,0
.even
.text
tst	4.(r5)
jge	L10005
mov	$061,(sp)
jbr	L10006
L10005:mov	$060,(sp)
L10006:mov	$L55,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	-8.(r5)
L56:cmp	-8.(r5),$05
jge	L57
.data
L59:.byte 045,0143,0
.even
.text
mov	4.(r5),r0
ash	$-014,r0
mov	r0,(sp)
bic	$!07,(sp)
add	$060,(sp)
mov	$L59,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	4.(r5),r0
ash	$03,r0
mov	r0,4.(r5)
L58:inc	-8.(r5)
jbr	L56
L57:L54:jmp	rretrn
