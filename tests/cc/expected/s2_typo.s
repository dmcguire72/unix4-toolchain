.globl	_w2006
.comm	_w2006,030
.globl	_flg
.data
_flg=.
0.
.globl	_realwd
.comm	_realwd,032
.globl	_wd
.data
_wd=.
1.+_realwd
.globl	_neng
.comm	_neng,02
.globl	_npr
.comm	_npr,02
.globl	_table
.comm	_table,04
.globl	_tab1
.comm	_tab1,064
.globl	_tab2
.comm	_tab2,02664
.globl	_tab3
.comm	_tab3,046344
.globl	_logtab
.comm	_logtab,01000
.globl	_inctab
.comm	_inctab,02000
.globl	_nwd
.comm	_nwd,032
.globl	_mask
.data
_mask=.
255.
.globl	_tot
.comm	_tot,02
.globl	_wtot
.comm	_wtot,02
.globl	_buf
.comm	_buf,06
.globl	_file
.comm	_file,06
.globl	_ptr
.comm	_ptr,06
.globl	_name
.comm	_name,010
.globl	_bsp
.comm	_bsp,03000
.globl	_main
.text
_main:
jsr	r5,rsave; 40
dec	4.(r5)
mov	4.(r5),r0
cmp	$01,r0
jle	L2
.data
L3:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L2:mov	$_bsp,_buf
mov	$512.+_bsp,2.+_buf
mov	$1024.+_bsp,4.+_buf
clr	_ptr
clr	2.+_ptr
mov	$01,4.+_ptr
mov	$01,-12.(r5)
L4:mov	-12.(r5),r0
asl	r0
add	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L5
mov	-12.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
movb	1.(r0),r0
jbr	L7
L8:.data
L9:.byte 0125,0156,0162,0145,0143,0157,0147,0156,0151,0172,0141,0142,0154,0145,040,0141,0162,0147,0165,0155,0145,0156,0164,072,040,045,0143,012,0
.even
.text
mov	-12.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
movb	1.(r0),r0
mov	r0,(sp)
mov	$L9,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L10:L11:inc	_neng
jbr	L6
L12:inc	_npr
jbr	L6
L7:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:0
61
156
L10002:..
L10
L12
L11
L8
.text
/esw
L6:inc	-12.(r5)
dec	4.(r5)
mov	4.(r5),r0
cmp	$01,r0
jle	L13
.data
L14:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L14,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L13:jbr	L4
L5:tst	_neng
jne	L15
.data
L16:.byte 057,0165,0163,0162,057,0154,0151,0142,057,0163,0141,0154,0164,0
.even
.text
clr	(sp)
mov	$L16,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-30.(r5)
mov	$04,(sp)
mov	$_table,-(sp)
mov	-30.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-32.(r5)
cmp	-32.(r5),$04
jeq	L17
.data
L18:.byte 0162,0145,0141,0144,040,0163,0141,0154,0164,0
.even
.text
mov	$L18,(sp)
jsr	pc,*$_err
L17:mov	$064,(sp)
mov	$_tab1,-(sp)
mov	-30.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-32.(r5)
cmp	-32.(r5),$064
jeq	L19
.data
L20:.byte 0162,0145,0141,0144,040,0163,0141,0154,0164,0
.even
.text
mov	$L20,(sp)
jsr	pc,*$_err
L19:mov	$02664,(sp)
mov	$_tab2,-(sp)
mov	-30.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-32.(r5)
cmp	-32.(r5),$02664
jeq	L21
.data
L22:.byte 0162,0145,0141,0144,040,0163,0141,0154,0164,0
.even
.text
mov	$L22,(sp)
jsr	pc,*$_err
L21:mov	$046344,(sp)
mov	$_tab3,-(sp)
mov	-30.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-32.(r5)
cmp	-32.(r5),$046344
jeq	L23
.data
L24:.byte 0162,0145,0141,0144,040,0163,0141,0154,0164,0
.even
.text
mov	$L24,(sp)
jsr	pc,*$_err
L23:mov	-30.(r5),(sp)
jsr	pc,*$_close
L15:mov	$_unl,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
.data
L25:.byte 057,0164,0155,0160,057,0164,0164,0155,0160,0141,061,0
.even
.text
mov	$L25,_name
.data
L26:.byte 057,0164,0155,0160,057,0164,0164,0155,0160,0141,062,0
.even
.text
mov	$L26,2.+_name
.data
L27:.byte 057,0164,0155,0160,057,0164,0164,0155,0160,0141,063,0
.even
.text
mov	$L27,4.+_name
.data
L28:.byte 057,0164,0155,0160,057,0164,0164,0155,0160,0141,064,0
.even
.text
mov	$L28,6.+_name
L29:mov	$01,(sp)
mov	_name,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_file
tst	_file
jle	L30
mov	_file,(sp)
jsr	pc,*$_close
mov	$0177777,-22.(r5)
L31:inc	-22.(r5)
mov	-22.(r5),r0
cmp	$04,r0
jle	L32
mov	-22.(r5),r0
asl	r0
mov	_name(r0),r0
incb	9.(r0)
jbr	L31
L32:mov	_name,r0
movb	9.(r0),r0
cmp	$0172,r0
jne	L33
.data
L34:.byte 0143,0162,0145,0141,0164,040,0164,0155,0160,040,0146,0151,0154,0145,0
.even
.text
mov	$L34,(sp)
jsr	pc,*$_err
L33:jbr	L29
L30:mov	$02,-22.(r5)
L35:mov	-22.(r5),r0
dec	-22.(r5)
tst	r0
jeq	L36
mov	$0666,(sp)
mov	-22.(r5),r0
asl	r0
mov	_name(r0),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	-22.(r5),r1
asl	r1
mov	r0,_file(r1)
jbr	L35
L36:L37:mov	4.(r5),r0
dec	4.(r5)
tst	r0
jeq	L38
clr	(sp)
mov	-12.(r5),r0
inc	-12.(r5)
asl	r0
add	6.(r5),r0
mov	(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,4.+_file
tst	4.+_file
jge	L39
.data
L40:.byte 0157,0160,0145,0156,040,0151,0156,0160,0165,0164,040,0146,0151,0154,0145,0
.even
.text
mov	$L40,(sp)
jsr	pc,*$_err
L39:L41:mov	$02,(sp)
jsr	pc,*$_wdval
mov	r0,-22.(r5)
jeq	L42
mov	-22.(r5),(sp)
inc	(sp)
mov	$_nwd,-(sp)
movb	_nwd,r0
mov	r0,-(sp)
jsr	pc,_dfile
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_put
cmp	(sp)+,(sp)+
mov	$0177777,-24.(r5)
clr	-26.(r5)
mov	$01,-28.(r5)
mov	_table,(sp)
jsr	pc,*$_incr
mov	r0,_table
L43:mov	-22.(r5),r0
inc	r0
cmp	-28.(r5),r0
jgt	L44
mov	-24.(r5),r1
inc	-24.(r5)
add	_wd,r1
movb	(r1),r1
mul	$033,r1
mov	-26.(r5),r2
inc	-26.(r5)
add	_wd,r2
movb	(r2),r2
add	r2,r1
mov	r1,-34.(r5)
mov	-34.(r5),r0
asl	r0
mov	_tab2(r0),(sp)
jsr	pc,*$_incr
mov	-34.(r5),r1
asl	r1
mov	r0,_tab2(r1)
mov	-34.(r5),r1
mul	$033,r1
mov	-28.(r5),r2
inc	-28.(r5)
add	_wd,r2
movb	(r2),r2
add	r2,r1
mov	r1,-34.(r5)
mov	-34.(r5),r0
movb	_tab3(r0),r0
mov	r0,(sp)
bic	$!0377,(sp)
jsr	pc,*$_incr
mov	-34.(r5),r1
movb	r0,_tab3(r1)
jbr	L43
L44:mov	_wd,r1
add	-24.(r5),r1
movb	(r1),r1
mul	$033,r1
mov	_wd,r2
add	-26.(r5),r2
movb	(r2),r2
add	r2,r1
mov	r1,-34.(r5)
mov	-34.(r5),r0
asl	r0
mov	_tab2(r0),(sp)
jsr	pc,*$_incr
mov	-34.(r5),r1
asl	r1
mov	r0,_tab2(r1)
jbr	L41
L42:L45:mov	4.+_file,(sp)
jsr	pc,*$_close
jbr	L37
L38:mov	$02,-22.(r5)
L46:mov	-22.(r5),r0
dec	-22.(r5)
tst	r0
jeq	L47
clr	(sp)
mov	-22.(r5),-(sp)
jsr	pc,*$_flsh
tst	(sp)+
mov	-22.(r5),r0
asl	r0
mov	_file(r0),(sp)
jsr	pc,*$_close
jbr	L46
L47:mov	$02,-22.(r5)
L48:mov	-22.(r5),r0
dec	-22.(r5)
tst	r0
jeq	L49
jsr	pc,_fork
mov	r0,-18.(r5)
tst	-18.(r5)
jne	L50
.data
L51:.byte 057,0142,0151,0156,057,0163,0157,0162,0164,0
.even
.text
.data
L52:.byte 0163,0157,0162,0164,0
.even
.text
.data
L53:.byte 055,0157,0
.even
.text
clr	(sp)
mov	-22.(r5),r0
asl	r0
mov	_name(r0),-(sp)
mov	-22.(r5),r0
asl	r0
mov	_name(r0),-(sp)
mov	$L53,-(sp)
mov	$L52,-(sp)
mov	$L51,-(sp)
jsr	pc,*$_execl
add	$012,sp
.data
L54:.byte 0163,0157,0162,0164,0
.even
.text
mov	$L54,(sp)
jsr	pc,*$_err
L50:tst	-18.(r5)
jge	L55
.data
L56:.byte 0146,0157,0162,0153,0
.even
.text
mov	$L56,(sp)
jsr	pc,*$_err
L55:jsr	pc,_wait
mov	r0,-32.(r5)
cmp	-32.(r5),-18.(r5)
jeq	L57
.data
L58:.byte 0160,0162,0157,0142,0163,0
.even
.text
mov	$L58,(sp)
jsr	pc,*$_err
L57:jbr	L48
L49:mov	$0177777,-22.(r5)
L59:inc	-22.(r5)
mov	-22.(r5),r0
cmp	$02,r0
jle	L60
jsr	pc,_fork
mov	r0,-18.(r5)
tst	-18.(r5)
jne	L61
.data
L62:.byte 057,0142,0151,0156,057,0165,0156,0151,0161,0
.even
.text
.data
L63:.byte 0165,0156,0151,0161,0
.even
.text
mov	-22.(r5),r0
asl	r0
mov	4.+_name(r0),(sp)
mov	-22.(r5),r0
asl	r0
mov	_name(r0),-(sp)
mov	$L63,-(sp)
mov	$L62,-(sp)
jsr	pc,*$_execl
add	$06,sp
.data
L64:.byte 0165,0156,0151,0161,0
.even
.text
mov	$L64,(sp)
jsr	pc,*$_err
L61:tst	-18.(r5)
jge	L65
.data
L66:.byte 0146,0157,0162,0153,0
.even
.text
mov	$L66,(sp)
jsr	pc,*$_err
L65:jsr	pc,_wait
mov	r0,-32.(r5)
cmp	-32.(r5),-18.(r5)
jeq	L67
.data
L68:.byte 0160,0162,0157,0142,0
.even
.text
mov	$L68,(sp)
jsr	pc,*$_err
L67:jbr	L59
L60:mov	$0666,(sp)
mov	_name,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,_file
tst	_file
jge	L69
.data
L70:.byte 0143,0162,0145,0141,0164,040,0164,0155,0160,0
.even
.text
mov	$L70,(sp)
jsr	pc,*$_err
L69:.data
L71:.byte 057,0165,0163,0162,057,0154,0151,0142,057,0167,062,060,060,066,0
.even
.text
clr	(sp)
mov	$L71,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,2.+_file
tst	2.+_file
jge	L72
.data
L73:.byte 0157,0160,0145,0156,040,0167,062,060,060,066,0
.even
.text
mov	$L73,(sp)
jsr	pc,*$_err
L72:mov	$01,2.+_ptr
mov	$0177777,-22.(r5)
L74:inc	-22.(r5)
mov	-22.(r5),-(sp)
add	$_w2006,(sp)
mov	$01,-(sp)
jsr	pc,_get
tst	(sp)+
movb	r0,*(sp)+
cmp	$012,r0
jeq	L75
jbr	L74
L75:movb	$01,-10.(r5)
L76:incb	-10.(r5)
movb	-10.(r5),r0
cmp	$04,r0
jle	L77
clr	(sp)
movb	-10.(r5),r0
asl	r0
mov	_name(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,4.+_file
tst	4.+_file
jge	L78
.data
L79:.byte 0157,0160,0145,0156,040,0164,0155,0160,0
.even
.text
mov	$L79,(sp)
jsr	pc,*$_err
L78:mov	$01,4.+_ptr
L80:mov	$0177777,-24.(r5)
L81:inc	-24.(r5)
mov	-24.(r5),-(sp)
add	_wd,(sp)
mov	$02,-(sp)
jsr	pc,_get
tst	(sp)+
movb	r0,*(sp)+
cmp	$012,r0
jeq	L82
mov	_wd,r0
add	-24.(r5),r0
tstb	(r0)
jeq	L83
jbr	L81
L82:L84:mov	$0177777,-20.(r5)
clr	-26.(r5)
L85:inc	-20.(r5)
mov	-20.(r5),r0
cmp	-24.(r5),r0
jlt	L86
mov	_wd,r0
add	-20.(r5),r0
mov	-26.(r5),r1
cmpb	(r0),_w2006(r1)
jge	L87
mov	-24.(r5),(sp)
mov	_wd,-(sp)
clr	-(sp)
jsr	pc,*$_put
cmp	(sp)+,(sp)+
jbr	L80
L87:mov	_wd,r0
add	-20.(r5),r0
mov	-26.(r5),r1
cmpb	(r0),_w2006(r1)
jle	L88
mov	$0177777,-26.(r5)
L89:inc	-26.(r5)
mov	-26.(r5),-(sp)
add	$_w2006,(sp)
mov	$01,-(sp)
jsr	pc,_get
tst	(sp)+
movb	r0,*(sp)+
cmp	$012,r0
jeq	L90
mov	-26.(r5),r0
tstb	_w2006(r0)
jeq	L91
jbr	L89
L90:jbr	L84
L88:inc	-26.(r5)
jbr	L85
L86:jbr	L80
L91:mov	-24.(r5),(sp)
mov	_wd,-(sp)
clr	-(sp)
jsr	pc,*$_put
cmp	(sp)+,(sp)+
mov	$0177777,-24.(r5)
L92:mov	$02,(sp)
jsr	pc,*$_get
inc	-24.(r5)
mov	-24.(r5),r1
add	_wd,r1
movb	r0,(r1)
jeq	L93
mov	_wd,r0
add	-24.(r5),r0
movb	(r0),r0
cmp	$012,r0
jne	L94
mov	-24.(r5),(sp)
mov	_wd,-(sp)
clr	-(sp)
jsr	pc,*$_put
cmp	(sp)+,(sp)+
mov	$0177777,-24.(r5)
L94:jbr	L92
L93:L83:mov	4.+_file,(sp)
jsr	pc,*$_close
movb	-10.(r5),r0
asl	r0
mov	_name(r0),(sp)
jsr	pc,*$_unlink
jbr	L76
L77:clr	(sp)
clr	-(sp)
jsr	pc,*$_flsh
tst	(sp)+
mov	2.+_file,(sp)
jsr	pc,*$_close
mov	_file,(sp)
jsr	pc,*$_close
mov	$01,2.+_ptr
clr	(sp)
mov	_name,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,2.+_file
tst	2.+_file
jge	L95
.data
L96:.byte 0157,0160,0145,0156,040,0164,0155,0160,040,0
.even
.text
mov	$L96,(sp)
jsr	pc,*$_err
L95:mov	$0666,(sp)
mov	2.+_name,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,_file
tst	_file
jge	L97
.data
L98:.byte 0143,0162,0145,0141,0164,0145,040,0164,0155,0160,0
.even
.text
mov	$L98,(sp)
jsr	pc,*$_err
L97:L99:mov	$01,(sp)
jsr	pc,*$_wdval
mov	r0,-22.(r5)
jeq	L100
clr	_wtot
clr	_flg
mov	$0177777,-24.(r5)
clr	-26.(r5)
mov	$01,-28.(r5)
L101:mov	-22.(r5),r0
inc	r0
cmp	-28.(r5),r0
jgt	L102
clr	_tot
mov	-24.(r5),r1
inc	-24.(r5)
add	_wd,r1
movb	(r1),r1
mul	$033,r1
mov	-26.(r5),r2
inc	-26.(r5)
add	_wd,r2
movb	(r2),r2
add	r2,r1
mov	r1,-34.(r5)
mov	-34.(r5),(sp)
jsr	pc,*$_digram
mov	_wd,r1
add	-24.(r5),r1
movb	(r1),r1
mul	$033,r1
mov	r1,(sp)
mov	_wd,r0
add	-26.(r5),r0
movb	(r0),r0
add	r0,(sp)
jsr	pc,*$_digram
asr	_tot
mov	-34.(r5),r1
mul	$033,r1
mov	-28.(r5),r2
inc	-28.(r5)
add	_wd,r2
movb	(r2),r2
add	r2,r1
mov	r1,-34.(r5)
mov	-34.(r5),(sp)
jsr	pc,*$_trigram
cmp	_tot,_wtot
jle	L103
mov	_tot,_wtot
L103:jbr	L101
L102:tst	_wtot
jge	L104
clr	_wtot
L104:mov	_wd,(sp)
mov	$03,-(sp)
mov	_wtot,-(sp)
jsr	pc,*$_conf
cmp	(sp)+,(sp)+
mov	r0,-16.(r5)
mov	_wd,r0
movb	$040,3.(r0)
mov	$03,(sp)
mov	_wd,-(sp)
clr	-(sp)
jsr	pc,*$_put
cmp	(sp)+,(sp)+
mov	-22.(r5),(sp)
inc	(sp)
mov	$_nwd,-(sp)
clr	-(sp)
jsr	pc,*$_put
cmp	(sp)+,(sp)+
jbr	L99
L100:clr	(sp)
clr	-(sp)
jsr	pc,*$_flsh
tst	(sp)+
mov	2.+_file,(sp)
jsr	pc,*$_close
mov	_file,(sp)
jsr	pc,*$_close
jsr	pc,_fork
mov	r0,-18.(r5)
tst	-18.(r5)
jne	L105
.data
L106:.byte 057,0142,0151,0156,057,0163,0157,0162,0164,0
.even
.text
.data
L107:.byte 0163,0157,0162,0164,0
.even
.text
.data
L108:.byte 055,0162,0
.even
.text
.data
L109:.byte 055,0157,0
.even
.text
clr	(sp)
mov	2.+_name,-(sp)
mov	2.+_name,-(sp)
mov	$L109,-(sp)
mov	$L108,-(sp)
mov	$L107,-(sp)
mov	$L106,-(sp)
jsr	pc,*$_execl
add	$014,sp
.data
L110:.byte 0163,0157,0162,0164,0
.even
.text
mov	$L110,(sp)
jsr	pc,*$_err
L105:tst	-18.(r5)
jge	L111
.data
L112:.byte 0146,0157,0162,0153,0
.even
.text
mov	$L112,(sp)
jsr	pc,*$_err
L111:jsr	pc,_wait
mov	r0,-32.(r5)
cmp	-32.(r5),-18.(r5)
jeq	L113
.data
L114:.byte 0160,0162,0157,0142,0
.even
.text
mov	$L114,(sp)
jsr	pc,*$_err
L113:jsr	pc,_fork
mov	r0,-18.(r5)
tst	-18.(r5)
jne	L115
tst	_npr
jeq	L116
.data
L117:.byte 057,0142,0151,0156,057,0143,0141,0164,0
.even
.text
.data
L118:.byte 0143,0141,0164,0
.even
.text
clr	(sp)
mov	2.+_name,-(sp)
mov	$L118,-(sp)
mov	$L117,-(sp)
jsr	pc,*$_execl
add	$06,sp
jbr	L119
L116:.data
L120:.byte 057,0142,0151,0156,057,0160,0162,0
.even
.text
.data
L121:.byte 0160,0162,0
.even
.text
.data
L122:.byte 055,063,0
.even
.text
.data
L123:.byte 055,0150,0
.even
.text
.data
L124:.byte 0120,0157,0163,0163,0151,0142,0154,0145,040,0164,0171,0160,0157,047,0163,040,0141,0156,0144,040,0163,0160,0145,0154,0154,0151,0156,0147,040,0145,0162,0162,0157,0162,0163,0
.even
.text
clr	(sp)
mov	2.+_name,-(sp)
mov	$L124,-(sp)
mov	$L123,-(sp)
mov	$L122,-(sp)
mov	$L121,-(sp)
mov	$L120,-(sp)
jsr	pc,*$_execl
add	$014,sp
.data
L125:.byte 0160,0162,0
.even
.text
mov	$L125,(sp)
jsr	pc,*$_err
L119:L115:tst	-18.(r5)
jge	L126
.data
L127:.byte 0146,0157,0162,0153,0
.even
.text
mov	$L127,(sp)
jsr	pc,*$_err
L126:jsr	pc,_wait
mov	r0,-32.(r5)
cmp	-32.(r5),-18.(r5)
jeq	L128
.data
L129:.byte 0160,0162,0157,0142,0
.even
.text
mov	$L129,(sp)
jsr	pc,*$_err
L128:jsr	pc,_unl
L1:jmp	rretrn
.globl	_unl
.text
_unl:
jsr	r5,rsave; 4
mov	$02,-8.(r5)
L131:mov	-8.(r5),r0
dec	-8.(r5)
tst	r0
jeq	L132
mov	-8.(r5),r0
asl	r0
mov	_name(r0),(sp)
jsr	pc,*$_unlink
jbr	L131
L132:jsr	pc,_exit
L130:jmp	rretrn
.globl	_trigram
.text
_trigram:
jsr	r5,rsave; 4
mov	4.(r5),r0
movb	_tab3(r0),r0
bic	$!0377,r0
asl	r0
mov	_logtab(r0),-8.(r5)
sub	-8.(r5),_tot
jbr	L133
L133:jmp	rretrn
.globl	_digram
.text
_digram:
jsr	r5,rsave; 4
mov	4.(r5),r0
asl	r0
mov	_tab2(r0),r0
asl	r0
mov	_logtab(r0),-8.(r5)
add	-8.(r5),_tot
jbr	L134
L134:jmp	rretrn
.globl	_dfile
.text
_dfile:
jsr	r5,rsave; 2
cmpb	4.(r5),$0141
jlt	L136
cmpb	4.(r5),$0153
jgt	L136
clr	r0
jbr	L135
L136:mov	$01,r0
jbr	L135
L135:jmp	rretrn
.globl	_err
.text
_err:
jsr	r5,rsave; 4
.data
L138:.byte 0143,0141,0156,0156,0157,0164,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L138,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$04,-8.(r5)
L139:mov	-8.(r5),r0
dec	-8.(r5)
tst	r0
jeq	L140
mov	-8.(r5),r0
asl	r0
mov	_name(r0),(sp)
jsr	pc,*$_unlink
jbr	L139
L140:jsr	pc,_exit
L137:jmp	rretrn
.globl	_get
.text
_get:
.bss
L142:.=.+20.
.text
jsr	r5,rsave; 2
mov	4.(r5),r0
asl	r0
dec	_ptr(r0)
jeq	L143
mov	4.(r5),r0
asl	r0
mov	L142(r0),-(sp)
inc	L142(r0)
mov	(sp)+,r0
movb	(r0),r0
jbr	L141
L143:mov	$01000,(sp)
mov	4.(r5),r0
asl	r0
mov	_buf(r0),-(sp)
mov	4.(r5),r0
asl	r0
mov	_file(r0),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	4.(r5),r1
asl	r1
mov	r0,_ptr(r1)
jeq	L144
mov	4.(r5),r0
asl	r0
tst	_ptr(r0)
jlt	L145
mov	4.(r5),r0
asl	r0
mov	4.(r5),r1
asl	r1
mov	_buf(r1),L142(r0)
mov	4.(r5),r0
asl	r0
mov	L142(r0),-(sp)
inc	L142(r0)
mov	(sp)+,r0
movb	(r0),r0
jbr	L141
L144:mov	4.(r5),r0
asl	r0
mov	$01,_ptr(r0)
clr	r0
jbr	L141
L145:mov	4.(r5),r0
asl	r0
mov	$01,_ptr(r0)
.data
L146:.byte 0162,0145,0141,0144,040,0145,0162,0162,0157,0162,012,0
.even
.text
mov	$L146,(sp)
jsr	pc,*$_printf
clr	r0
jbr	L141
L141:jmp	rretrn
.globl	_put
.text
_put:
jsr	r5,rsave; 4
L148:mov	8.(r5),r0
dec	8.(r5)
tst	r0
jlt	L149
mov	4.(r5),r0
asl	r0
cmp	_ptr(r0),$01000
jge	L10004
mov	4.(r5),r0
asl	r0
mov	_ptr(r0),-(sp)
inc	_ptr(r0)
mov	(sp)+,r0
jbr	L10005
L10004:mov	$01,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_flsh
tst	(sp)+
L10005:mov	4.(r5),r1
asl	r1
add	_buf(r1),r0
mov	6.(r5),r1
inc	6.(r5)
movb	(r1),(r0)
jbr	L148
L149:jbr	L147
L147:jmp	rretrn
.globl	_flsh
.text
_flsh:
jsr	r5,rsave; 4
mov	4.(r5),r0
asl	r0
mov	_ptr(r0),(sp)
mov	4.(r5),r0
asl	r0
mov	_buf(r0),-(sp)
mov	4.(r5),r0
asl	r0
mov	_file(r0),-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jlt	L151
mov	4.(r5),r0
asl	r0
mov	6.(r5),_ptr(r0)
clr	r0
jbr	L150
L151:.data
L152:.byte 0167,0162,0151,0164,0145,040,0145,0162,0162,0157,0162,040,0157,0156,040,0164,056,045,0144,012,0
.even
.text
mov	$_file,(sp)
mov	$L152,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L150:jmp	rretrn
.globl	_wdval
.text
_wdval:
.bss
L154:.=.+2.
.text
.bss
L155:.=.+2.
.text
jsr	r5,rsave; 4
L156:mov	$0177777,-8.(r5)
cmp	L155,$01
jne	L157
clr	L155
jbr	L158
L157:L159:mov	4.(r5),(sp)
jsr	pc,*$_get
mov	r0,L154
cmp	$012,r0
jeq	L160
tst	L154
jne	L161
clr	r0
jbr	L153
L161:L158:cmp	L154,$045
jne	L10006
cmp	-8.(r5),$0177777
jne	L162
L10006:cmp	L154,$055
jne	L163
mov	4.(r5),(sp)
jsr	pc,*$_get
mov	r0,L154
cmp	$012,r0
jne	L164
L165:mov	4.(r5),(sp)
jsr	pc,*$_get
mov	r0,L154
cmp	$012,r0
jne	L166
jbr	L165
L166:jbr	L158
L164:mov	$01,L155
jbr	L167
L163:cmp	L154,$047
jne	L168
cmp	-8.(r5),$01
jlt	L167
jbr	L162
L168:cmp	L154,$0101
jlt	L167
cmp	L154,$0132
jgt	L169
inc	-8.(r5)
mov	-8.(r5),r0
add	_wd,r0
mov	L154,r1
add	$0177700,r1
movb	r1,(r0)
mov	-8.(r5),r0
mov	L154,r1
add	$040,r1
movb	r1,_nwd(r0)
jbr	L162
L169:cmp	L154,$0141
jlt	L167
cmp	L154,$0172
jgt	L167
inc	-8.(r5)
mov	-8.(r5),r0
add	_wd,r0
mov	L154,r1
add	$0177640,r1
movb	r1,(r0)
mov	-8.(r5),r0
movb	L154,_nwd(r0)
L162:jbr	L159
L160:L170:mov	4.(r5),(sp)
jsr	pc,*$_get
mov	r0,L154
cmp	$056,r0
jeq	L10007
cmp	L154,$047
jne	L171
L10007:L172:mov	4.(r5),(sp)
jsr	pc,*$_get
mov	r0,L154
cmp	$012,r0
jeq	L173
jbr	L172
L173:jbr	L170
L171:mov	$01,L155
L167:cmp	-8.(r5),$01
jlt	L156
mov	-8.(r5),r0
movb	$012,1.+_nwd(r0)
mov	_wd,r0
add	-8.(r5),r0
clrb	1.(r0)
mov	-8.(r5),r0
jbr	L153
L153:jmp	rretrn
.globl	_conf
.text
_conf:
jsr	r5,rsave; 6
mov	6.(r5),-8.(r5)
L175:mov	-8.(r5),r0
dec	-8.(r5)
tst	r0
jeq	L176
mov	8.(r5),r0
add	-8.(r5),r0
movb	$040,(r0)
jbr	L175
L176:mov	4.(r5),r1
sxt	r0
div	$012,r0
mov	r0,-10.(r5)
jeq	L10008
mov	8.(r5),(sp)
dec	6.(r5)
mov	6.(r5),-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_conf
cmp	(sp)+,(sp)+
jbr	L10009
L10008:dec	6.(r5)
mov	6.(r5),r0
L10009:add	8.(r5),r0
mov	4.(r5),r3
sxt	r2
div	$012,r2
add	$060,r3
movb	r3,(r0)
inc	6.(r5)
mov	6.(r5),r0
jbr	L174
L174:jmp	rretrn
.globl	_incr
.text
_incr:
.bss
L178:.=.+2.
.text
jsr	r5,rsave; 4
tst	L178
jne	L179
movf	$040200,r0
movfo	r0,_inctab
mov	$0177766,_logtab
mov	$01,-8.(r5)
L180:cmp	-8.(r5),$0400
jge	L181
.data
L183:041363;0174733;021320;0162540
.text
mov	-8.(r5),r0
neg	r0
movif	r0,r0
divf	L183,r0
movf	r0,-(sp)
jsr	pc,_exp
add	$010,sp
mov	-8.(r5),r1
ash	$02,r1
movfo	r0,_inctab(r1)
.data
L184:040204;041454;0122572;074160
.text
movif	-8.(r5),r0
addf	$00,r0
movf	r0,-(sp)
movf	L184,r0
movf	r0,-(sp)
jsr	pc,_pow
add	$020,sp
mulf	$041360,r0
subf	$041360,r0
movf	r0,-(sp)
jsr	pc,_log
add	$010,sp
addf	$040000,r0
mov	-8.(r5),r1
asl	r1
movfi	r0,r0
mov	r0,_logtab(r1)
L182:inc	-8.(r5)
jbr	L180
L181:mov	$0177766,2.+_logtab
mov	$01,L178
L179:jsr	pc,_rand
movif	r0,r0
divf	$044000,r0
mov	4.(r5),r1
ash	$02,r1
movof	_inctab(r1),r1
cmpf	r0,r1
cfcc
jge	L185
inc	4.(r5)
L185:cmp	4.(r5),$0377
jle	L186
mov	$0377,4.(r5)
L186:mov	4.(r5),r0
jbr	L177
L177:jmp	rretrn
.globl	fltused
