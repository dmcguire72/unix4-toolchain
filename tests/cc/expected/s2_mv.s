.globl	_stbuf
.comm	_stbuf,0124
.globl	_strbuf
.comm	_strbuf,0106
.globl	_main
.text
_main:
jsr	r5,rsave; 174
mov	6.(r5),-8.(r5)
cmp	4.(r5),$03
jeq	L2
.data
L3:.byte 0125,0163,0141,0147,0145,072,040,0155,0166,040,0156,0141,0155,0145,061,040,0156,0141,0155,0145,062,012,0
.even
.text
mov	$026,(sp)
mov	$L3,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L2:mov	-8.(r5),r0
mov	2.(r0),-14.(r5)
mov	-8.(r5),r0
mov	4.(r0),-16.(r5)
mov	$_stbuf,(sp)
mov	-8.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jge	L4
.data
L5:.byte 0123,0157,0165,0162,0143,0145,040,0146,0151,0154,0145,040,0156,0157,0156,055,0145,0170,0151,0163,0164,0145,0156,0164,012,0
.even
.text
mov	$031,(sp)
mov	$L5,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L4:mov	4.+_stbuf,r0
bic	$!060000,r0
cmp	$040000,r0
jne	L6
mov	$_stbuf,(sp)
mov	-8.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jlt	L7
.data
L8:.byte 0104,0151,0162,0145,0143,0164,0157,0162,0171,040,0164,0141,0162,0147,0145,0164,040,0145,0170,0151,0163,0164,0163,056,012,0
.even
.text
mov	$031,(sp)
mov	$L8,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L7:mov	-8.(r5),r0
mov	2.(r0),-10.(r5)
mov	-8.(r5),r0
mov	4.(r0),-12.(r5)
L9:cmpb	*-10.(r5),*-12.(r5)
jne	L10
inc	-10.(r5)
mov	-12.(r5),r0
inc	-12.(r5)
tstb	(r0)
jne	L11
.data
L12:.byte 077,077,077,012,0
.even
.text
mov	$04,(sp)
mov	$L12,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L11:jbr	L9
L10:L13:tstb	*-10.(r5)
jeq	L14
mov	-10.(r5),r0
inc	-10.(r5)
movb	(r0),r0
cmp	$057,r0
jne	L15
.data
L16:.byte 0104,0151,0162,0145,0143,0164,0157,0162,0171,040,0162,0145,0156,0141,0155,0145,040,0157,0156,0154,0171,012,0
.even
.text
mov	$026,(sp)
mov	$L16,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L15:jbr	L13
L14:L17:tstb	*-12.(r5)
jeq	L18
mov	-12.(r5),r0
inc	-12.(r5)
movb	(r0),r0
cmp	$057,r0
jne	L19
.data
L20:.byte 0104,0151,0162,0145,0143,0164,0157,0162,0171,040,0162,0145,0156,0141,0155,0145,040,0157,0156,0154,0171,012,0
.even
.text
mov	$026,(sp)
mov	$L20,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L19:jbr	L17
L18:dec	-10.(r5)
mov	-10.(r5),r0
movb	(r0),r0
cmp	$056,r0
jne	L21
.data
L22:.byte 0166,0141,0154,0165,0145,0163,040,0157,0146,040,016,0102,017,040,0167,0151,0154,0154,040,0147,0151,0166,0145,040,0162,0151,0163,0145,040,0164,0157,040,0144,0157,0155,041,012,0
.even
.text
mov	$045,(sp)
mov	$L22,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L21:jbr	L23
L6:jsr	pc,_getuid
mov	r0,(sp)
jsr	pc,*$_setuid
mov	$4.+_stbuf,(sp)
mov	-16.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jlt	L24
mov	8.+_stbuf,r0
bic	$!060000,r0
cmp	$040000,r0
jne	L25
mov	$_strbuf,-12.(r5)
L26:mov	-12.(r5),r0
inc	-12.(r5)
mov	-16.(r5),r1
inc	-16.(r5)
movb	(r1),(r0)
jeq	L27
jbr	L26
L27:mov	-12.(r5),r0
movb	$057,-1.(r0)
mov	-8.(r5),r0
mov	2.(r0),-16.(r5)
mov	-8.(r5),r0
mov	2.(r0),-10.(r5)
L28:tstb	*-16.(r5)
jeq	L29
mov	-16.(r5),r0
inc	-16.(r5)
movb	(r0),r0
cmp	$057,r0
jne	L30
mov	-16.(r5),-10.(r5)
L30:jbr	L28
L29:L31:mov	-12.(r5),r0
inc	-12.(r5)
mov	-10.(r5),r1
inc	-10.(r5)
movb	(r1),(r0)
jeq	L32
jbr	L31
L32:mov	$_strbuf,-16.(r5)
L25:mov	$4.+_stbuf,(sp)
mov	-16.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jlt	L33
cmp	_stbuf,4.+_stbuf
jne	L34
cmp	2.+_stbuf,6.+_stbuf
jne	L34
.data
L35:.byte 0106,0151,0154,0145,0163,040,0141,0162,0145,040,0151,0144,0145,0156,0164,0151,0143,0141,0154,056,012,0
.even
.text
mov	$025,(sp)
mov	$L35,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L34:jsr	pc,_getuid
movb	11.+_stbuf,r1
cmp	r0,r1
jne	L36
mov	$0200,-128.(r5)
jbr	L37
L36:jsr	pc,_getgid
movb	12.+_stbuf,r1
cmp	r0,r1
jne	L38
mov	$020,-128.(r5)
jbr	L39
L38:mov	$02,-128.(r5)
L39:L37:bit	-128.(r5),8.+_stbuf
jne	L40
.data
L41:.byte 045,0163,072,040,045,0157,040,0155,0157,0144,0145,040,0
.even
.text
mov	8.+_stbuf,(sp)
bic	$!07777,(sp)
mov	-16.(r5),-(sp)
mov	$L41,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_getchar
mov	r0,-128.(r5)
mov	r0,-124.(r5)
L42:cmp	-128.(r5),$012
jeq	L43
tst	-128.(r5)
jeq	L43
jsr	pc,_getchar
mov	r0,-128.(r5)
jbr	L42
L43:cmp	-124.(r5),$0171
jeq	L44
jsr	pc,_exit
L44:L40:mov	-16.(r5),(sp)
jsr	pc,*$_unlink
tst	r0
jge	L45
.data
L46:.byte 0103,0141,0156,0156,0157,0164,040,0162,0145,0155,0157,0166,0145,040,0164,0141,0162,0147,0145,0164,040,0146,0151,0154,0145,056,012,0
.even
.text
mov	$033,(sp)
mov	$L46,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L45:L33:L24:L23:mov	-16.(r5),(sp)
mov	-14.(r5),-(sp)
jsr	pc,*$_link
tst	(sp)+
tst	r0
jge	L47
jsr	pc,_fork
mov	r0,-124.(r5)
cmp	-124.(r5),$0177777
jne	L48
.data
L49:.byte 0124,0162,0171,040,0141,0147,0141,0151,0156,056,012,0
.even
.text
mov	$013,(sp)
mov	$L49,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L48:tst	-124.(r5)
jeq	L50
L51:mov	r5,r0
add	$0177602,r0
mov	r0,(sp)
jsr	pc,*$_wait
cmp	-124.(r5),r0
jeq	L52
jbr	L51
L52:jbr	L53
L50:mov	r5,r0
add	$0177606,r0
mov	r0,-18.(r5)
mov	-18.(r5),-20.(r5)
L54:mov	-18.(r5),r0
inc	-18.(r5)
mov	-14.(r5),r1
inc	-14.(r5)
movb	(r1),(r0)
jeq	L55
jbr	L54
L55:mov	-18.(r5),-22.(r5)
L56:mov	-18.(r5),r0
inc	-18.(r5)
mov	-16.(r5),r1
inc	-16.(r5)
movb	(r1),(r0)
jeq	L57
jbr	L56
L57:.data
L58:.byte 057,0142,0151,0156,057,0143,0160,0
.even
.text
.data
L59:.byte 0143,0160,0
.even
.text
clr	(sp)
mov	-22.(r5),-(sp)
mov	-20.(r5),-(sp)
mov	$L59,-(sp)
mov	$L58,-(sp)
jsr	pc,*$_execl
add	$010,sp
.data
L60:.byte 0156,0157,040,0143,0160,012,0
.even
.text
mov	$06,(sp)
mov	$L60,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L53:bit	$0377,-126.(r5)
jeq	L61
.data
L62:.byte 077,012,0
.even
.text
mov	$02,(sp)
mov	$L62,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L61:tst	-126.(r5)
jeq	L63
jsr	pc,_exit
L63:L47:mov	-14.(r5),(sp)
jsr	pc,*$_unlink
tst	r0
jge	L64
.data
L65:.byte 0103,0141,0156,0156,0157,0164,040,0165,0156,0154,0151,0156,0153,040,0163,0157,0165,0162,0143,0145,040,0146,0151,0154,0145,056,012,0
.even
.text
mov	$032,(sp)
mov	$L65,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L64:L1:jmp	rretrn
.globl	_putchar
.text
_putchar:
jsr	r5,rsave; 2
mov	$01,(sp)
mov	r5,r0
add	$04,r0
mov	r0,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
L66:jmp	rretrn
.globl	_getchar
.text
_getchar:
jsr	r5,rsave; 4
mov	$01,(sp)
mov	r5,r0
add	$0177770,r0
mov	r0,-(sp)
clr	-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$01,r0
jeq	L68
clr	r0
jbr	L67
L68:movb	-8.(r5),r0
jbr	L67
L67:jmp	rretrn
