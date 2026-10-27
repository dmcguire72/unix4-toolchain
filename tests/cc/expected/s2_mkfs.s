.globl	_filsys
.comm	_filsys,0640
.globl	_utime
.comm	_utime,04
.globl	_fin
.comm	_fin,02
.globl	_fsi
.comm	_fsi,02
.globl	_fso
.comm	_fso,02
.globl	_charp
.comm	_charp,02
.globl	_buf
.comm	_buf,01000
.globl	_string
.comm	_string,062
.globl	_fsys
.comm	_fsys,02
.globl	_proto
.comm	_proto,02
.globl	_main
.text
_main:
jsr	r5,rsave; 6
mov	$_utime,(sp)
jsr	pc,*$_time
cmp	4.(r5),$03
jeq	L2
.data
L3:.byte 0141,0162,0147,040,0143,0157,0165,0156,0164,012,0
.even
.text
mov	$L3,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L2:mov	6.(r5),r0
mov	2.(r0),_fsys
mov	6.(r5),r0
mov	4.(r0),_proto
mov	$0666,(sp)
mov	_fsys,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,_fso
tst	_fso
jge	L4
.data
L5:.byte 045,0163,072,040,0143,0141,0156,0156,0157,0164,040,0143,0162,0145,0141,0164,0145,012,0
.even
.text
mov	_fsys,(sp)
mov	$L5,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L4:clr	(sp)
mov	_fsys,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fsi
tst	_fsi
jge	L6
.data
L7:.byte 045,0163,072,040,0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,012,0
.even
.text
mov	_fsys,(sp)
mov	$L7,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L6:clr	(sp)
mov	_proto,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fin
tst	_fin
jge	L8
clr	-10.(r5)
clr	-8.(r5)
L9:mov	_proto,r0
add	-8.(r5),r0
tstb	(r0)
jeq	L10
mov	_proto,r0
add	-8.(r5),r0
movb	(r0),r0
cmp	$060,r0
jgt	L10000
mov	_proto,r0
add	-8.(r5),r0
movb	(r0),r0
cmp	$071,r0
jge	L12
L10000:.data
L13:.byte 045,0163,072,040,0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,012,0
.even
.text
mov	_proto,(sp)
mov	$L13,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L12:mov	-10.(r5),r1
mul	$012,r1
mov	_proto,r2
add	-8.(r5),r2
movb	(r2),r2
add	r2,r1
add	$0177720,r1
mov	r1,-10.(r5)
L11:inc	-8.(r5)
jbr	L9
L10:mov	-10.(r5),2.+_filsys
mov	$01750,(sp)
mov	-10.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,(sp)
add	$053,(sp)
mov	-10.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,_filsys
.data
L14:.byte 0151,0163,0151,0172,0145,040,075,040,045,0144,012,0
.even
.text
mov	_filsys,(sp)
mov	$L14,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L15:.byte 0144,055,055,067,067,067,040,060,040,060,040,044,040,0
.even
.text
mov	$L15,_charp
jbr	L16
L8:jsr	pc,_getstr
clr	(sp)
mov	$_string,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-8.(r5)
tst	-8.(r5)
jge	L17
.data
L18:.byte 045,0163,072,040,0143,0141,0156,0156,0157,0164,040,040,0157,0160,0145,0156,040,0151,0156,0151,0164,012,0
.even
.text
mov	$_string,(sp)
mov	$L18,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L19
L17:mov	$020,(sp)
mov	$_buf,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	_buf,$0407
jeq	L20
.data
L21:.byte 045,0163,072,040,0142,0141,0144,040,0146,0157,0162,0155,0141,0164,012,0
.even
.text
mov	$_string,(sp)
mov	$L21,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L22
L20:mov	2.+_buf,r0
add	4.+_buf,r0
mov	r0,-10.(r5)
cmp	-10.(r5),$01000
jle	L23
.data
L24:.byte 045,0163,072,040,0164,0157,0157,040,0142,0151,0147,012,0
.even
.text
mov	$_string,(sp)
mov	$L24,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L22
L23:mov	-10.(r5),(sp)
mov	$_buf,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	$_buf,(sp)
clr	-(sp)
jsr	pc,*$_wtfs
tst	(sp)+
L22:mov	-8.(r5),(sp)
jsr	pc,*$_close
L19:jsr	pc,_getnum
mov	r0,2.+_filsys
jsr	pc,_getnum
mov	r0,_filsys
L16:cmp	_filsys,2.+_filsys
jhi	L10001
mov	2.+_filsys,r0
sub	_filsys,r0
add	$0177776,r0
cmp	_filsys,r0
jlos	L25
L10001:.data
L26:.byte 045,0154,057,045,0154,072,040,0142,0141,0144,040,0162,0141,0164,0151,0157,012,0
.even
.text
mov	_filsys,(sp)
mov	2.+_filsys,-(sp)
mov	$L26,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_exit
L25:clr	(sp)
jsr	pc,*$_free
mov	2.+_filsys,r0
add	$0177777,r0
mov	r0,-10.(r5)
L27:mov	_filsys,r0
inc	r0
cmp	-10.(r5),r0
jeq	L28
mov	-10.(r5),(sp)
jsr	pc,*$_free
L29:dec	-10.(r5)
jbr	L27
L28:clr	-10.(r5)
L30:cmp	-10.(r5),$0400
jge	L31
mov	-10.(r5),r0
asl	r0
clr	_buf(r0)
L32:inc	-10.(r5)
jbr	L30
L31:clr	-10.(r5)
L33:cmp	-10.(r5),_filsys
jeq	L34
mov	$_buf,(sp)
mov	-10.(r5),-(sp)
add	$02,(sp)
jsr	pc,*$_wtfs
tst	(sp)+
L35:inc	-10.(r5)
jbr	L33
L34:clr	(sp)
jsr	pc,*$_cfile
clr	-10.(r5)
L36:cmp	-10.(r5),$0400
jge	L37
mov	-10.(r5),r0
asl	r0
clr	_buf(r0)
L38:inc	-10.(r5)
jbr	L36
L37:mov	_utime,412.+_filsys
mov	2.+_utime,414.+_filsys
mov	$_filsys,(sp)
mov	$01,-(sp)
jsr	pc,*$_wtfs
tst	(sp)+
L1:jmp	rretrn
.globl	_cfile
.text
_cfile:
.bss
L40:.=.+2.
.text
jsr	r5,rsave; 2060
jsr	pc,_getstr
mov	$0100000,2.+-40.(r5)
.data
L41:.byte 0142,0143,0144,0
.even
.text
mov	$040000,(sp)
mov	$020000,-(sp)
mov	$060000,-(sp)
mov	$L41,-(sp)
movb	_string,r0
mov	r0,-(sp)
jsr	pc,*$_gmode
add	$010,sp
bis	r0,2.+-40.(r5)
.data
L42:.byte 0165,0
.even
.text
mov	$04000,(sp)
mov	$L42,-(sp)
movb	1.+_string,r0
mov	r0,-(sp)
jsr	pc,*$_gmode
cmp	(sp)+,(sp)+
bis	r0,2.+-40.(r5)
.data
L43:.byte 0147,0
.even
.text
mov	$02000,(sp)
mov	$L43,-(sp)
movb	2.+_string,r0
mov	r0,-(sp)
jsr	pc,*$_gmode
cmp	(sp)+,(sp)+
bis	r0,2.+-40.(r5)
mov	$03,-1070.(r5)
L44:cmp	-1070.(r5),$06
jge	L45
mov	-1070.(r5),r0
movb	_string(r0),r0
cmp	$060,r0
jgt	L10002
mov	-1070.(r5),r0
movb	_string(r0),r0
cmp	$067,r0
jge	L47
L10002:.data
L48:.byte 045,0143,057,045,0163,072,040,0142,0141,0144,040,0144,0151,0147,0151,0164,012,0
.even
.text
mov	$_string,(sp)
mov	-1070.(r5),r0
movb	_string(r0),r0
mov	r0,-(sp)
mov	$L48,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_exit
L47:mov	-1070.(r5),r0
movb	_string(r0),r0
add	$0177720,r0
mov	$017,r1
mov	-1070.(r5),r3
mul	$03,r3
sub	r3,r1
ash	r1,r0
bis	r0,2.+-40.(r5)
L46:inc	-1070.(r5)
jbr	L44
L45:jsr	pc,_getnum
movb	r0,5.+-40.(r5)
jsr	pc,_getnum
movb	r0,6.+-40.(r5)
inc	L40
mov	L40,-40.(r5)
mov	$020,(sp)
mov	L40,-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
cmp	_filsys,r0
jhis	L49
.data
L50:.byte 0164,0157,0157,040,0155,0141,0156,0171,040,0151,0156,0157,0144,0145,0163,012,0
.even
.text
mov	$L50,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L49:movb	$01,4.+-40.(r5)
clrb	7.+-40.(r5)
clr	8.+-40.(r5)
clr	-1070.(r5)
L51:cmp	-1070.(r5),$010
jge	L52
mov	-1070.(r5),r0
asl	r0
add	r5,r0
clr	-30.(r0)
L53:inc	-1070.(r5)
jbr	L51
L52:clr	-1070.(r5)
L54:cmp	-1070.(r5),$0400
jge	L55
mov	-1070.(r5),r0
asl	r0
add	r5,r0
clr	-552.(r0)
mov	-1070.(r5),r0
asl	r0
add	r5,r0
clr	-1064.(r0)
L56:inc	-1070.(r5)
jbr	L54
L55:tst	4.(r5)
jne	L57
mov	r5,r0
add	$0177730,r0
mov	r0,4.(r5)
decb	4.+-40.(r5)
L57:clr	-1066.(r5)
clr	-1068.(r5)
mov	2.+-40.(r5),r0
bic	$!060000,r0
jbr	L59
L60:jsr	pc,_getstr
clr	(sp)
mov	$_string,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-1072.(r5)
tst	-1072.(r5)
jge	L61
.data
L62:.byte 045,0163,072,040,0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,012,0
.even
.text
mov	$_string,(sp)
mov	$L62,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jbr	L58
L61:L63:mov	$01000,(sp)
mov	r5,r0
add	$0176730,r0
mov	r0,-(sp)
mov	-1072.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-1070.(r5)
jle	L64
add	-1070.(r5),8.+-40.(r5)
mov	r5,r0
add	$0175730,r0
mov	r0,(sp)
mov	r5,r0
add	$0175724,r0
mov	r0,-(sp)
mov	r5,r0
add	$0176730,r0
mov	r0,-(sp)
mov	r5,r0
add	$0175726,r0
mov	r0,-(sp)
jsr	pc,*$_newblk
add	$06,sp
jbr	L63
L64:mov	-1072.(r5),(sp)
jsr	pc,*$_close
jbr	L58
L65:L66:jsr	pc,_getnum
ash	$010,r0
mov	r0,10.+-40.(r5)
jsr	pc,_getnum
bis	r0,10.+-40.(r5)
jbr	L58
L67:mov	4.(r5),r0
incb	4.(r0)
.data
L68:.byte 056,056,0
.even
.text
mov	r5,r0
add	$0175730,r0
mov	r0,(sp)
mov	r5,r0
add	$0175724,r0
mov	r0,-(sp)
mov	r5,r0
add	$0176730,r0
mov	r0,-(sp)
mov	r5,r0
add	$0175726,r0
mov	r0,-(sp)
mov	$L68,-(sp)
mov	*4.(r5),-(sp)
jsr	pc,*$_entry
add	$012,sp
incb	4.+-40.(r5)
.data
L69:.byte 056,0
.even
.text
mov	r5,r0
add	$0175730,r0
mov	r0,(sp)
mov	r5,r0
add	$0175724,r0
mov	r0,-(sp)
mov	r5,r0
add	$0176730,r0
mov	r0,-(sp)
mov	r5,r0
add	$0175726,r0
mov	r0,-(sp)
mov	$L69,-(sp)
mov	-40.(r5),-(sp)
jsr	pc,*$_entry
add	$012,sp
mov	$040,8.+-40.(r5)
L70:jsr	pc,_getstr
cmpb	_string,$044
jne	L10003
tstb	1.+_string
jeq	L71
L10003:mov	r5,r0
add	$0175730,r0
mov	r0,(sp)
mov	r5,r0
add	$0175724,r0
mov	r0,-(sp)
mov	r5,r0
add	$0176730,r0
mov	r0,-(sp)
mov	r5,r0
add	$0175726,r0
mov	r0,-(sp)
mov	$_string,-(sp)
mov	L40,-(sp)
inc	(sp)
jsr	pc,*$_entry
add	$012,sp
add	$020,8.+-40.(r5)
mov	r5,r0
add	$0177730,r0
mov	r0,(sp)
jsr	pc,*$_cfile
jbr	L70
L71:jbr	L58
jbr	L58
L59:mov	$L10005,r1
mov	r0,L10006
L10007:cmp	r0,(r1)+
jne	L10007
jmp	*L10006-L10005(r1)
.data
L10005:0
20000
40000
60000
L10006:..
L60
L66
L67
L65
L72
.text
/esw
L58:L72:tst	-1066.(r5)
jeq	L73
mov	r5,r0
add	$0175730,r0
mov	r0,(sp)
mov	r5,r0
add	$0175724,r0
mov	r0,-(sp)
mov	r5,r0
add	$0176730,r0
mov	r0,-(sp)
mov	r5,r0
add	$0175726,r0
mov	r0,-(sp)
jsr	pc,*$_newblk
add	$06,sp
L73:cmp	-1068.(r5),$010
jle	L74
bis	$010000,2.+-40.(r5)
jsr	pc,_alloc
mov	r0,-1066.(r5)
mov	r5,r0
add	$0175730,r0
mov	r0,(sp)
mov	-1066.(r5),-(sp)
jsr	pc,*$_wtfs
tst	(sp)+
mov	-1066.(r5),10.+-40.(r5)
jbr	L75
L74:clr	-1070.(r5)
L76:cmp	-1070.(r5),-1068.(r5)
jge	L77
mov	-1070.(r5),r0
asl	r0
add	r5,r0
mov	-1070.(r5),r1
asl	r1
add	r5,r1
mov	-1064.(r1),-30.(r0)
L78:inc	-1070.(r5)
jbr	L76
L77:L75:mov	_utime,r0
mov	r0,30.+-40.(r5)
mov	r0,26.+-40.(r5)
mov	2.+_utime,r0
mov	r0,32.+-40.(r5)
mov	r0,28.+-40.(r5)
mov	-40.(r5),r0
add	$037,r0
mov	r0,-1070.(r5)
mov	$020,(sp)
mov	-1070.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_ldiv
cmp	(sp)+,(sp)+
mov	r0,-1066.(r5)
mov	$020,(sp)
mov	-1070.(r5),-(sp)
clr	-(sp)
jsr	pc,*$_lrem
cmp	(sp)+,(sp)+
ash	$05,r0
add	$_buf,r0
mov	r0,-1074.(r5)
mov	r5,r0
add	$0177732,r0
mov	r0,-1076.(r5)
mov	$_buf,(sp)
mov	-1066.(r5),-(sp)
jsr	pc,*$_rdfs
tst	(sp)+
clr	-1070.(r5)
L79:cmp	-1070.(r5),$020
jge	L80
mov	-1074.(r5),r0
add	$02,-1074.(r5)
mov	-1076.(r5),r1
add	$02,-1076.(r5)
mov	(r1),(r0)
L81:inc	-1070.(r5)
jbr	L79
L80:mov	$_buf,(sp)
mov	-1066.(r5),-(sp)
jsr	pc,*$_wtfs
tst	(sp)+
L39:jmp	rretrn
.globl	_gmode
.text
_gmode:
jsr	r5,rsave; 4
clr	-8.(r5)
L83:mov	6.(r5),r0
add	-8.(r5),r0
tstb	(r0)
jeq	L84
movb	4.(r5),r0
mov	6.(r5),r1
add	-8.(r5),r1
movb	(r1),r1
cmp	r0,r1
jne	L86
mov	-8.(r5),r0
asl	r0
add	r5,r0
mov	8.(r0),r0
jbr	L82
L86:L85:inc	-8.(r5)
jbr	L83
L84:cmpb	4.(r5),$055
jne	L87
clr	r0
jbr	L82
L87:.data
L88:.byte 045,0143,057,045,0163,072,040,0142,0141,0144,040,0155,0157,0144,0145,012,0
.even
.text
mov	$_string,(sp)
movb	4.(r5),r0
mov	r0,-(sp)
mov	$L88,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_exit
L82:jmp	rretrn
.globl	_getnum
.text
_getnum:
jsr	r5,rsave; 6
jsr	pc,_getstr
clr	-8.(r5)
clr	-10.(r5)
clr	-10.(r5)
L90:mov	-10.(r5),r0
tstb	_string(r0)
jeq	L91
mov	-10.(r5),r0
movb	_string(r0),r0
cmp	$060,r0
jgt	L10008
mov	-10.(r5),r0
movb	_string(r0),r0
cmp	$071,r0
jge	L93
L10008:.data
L94:.byte 045,0163,072,040,0142,0141,0144,040,0156,0165,0155,0142,0145,0162,012,0
.even
.text
mov	$_string,(sp)
mov	$L94,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L93:mov	-8.(r5),r1
mul	$012,r1
mov	-10.(r5),r2
movb	_string(r2),r2
add	r2,r1
add	$0177720,r1
mov	r1,-8.(r5)
L92:inc	-10.(r5)
jbr	L90
L91:mov	-8.(r5),r0
jbr	L89
L89:jmp	rretrn
.globl	_getstr
.text
_getstr:
jsr	r5,rsave; 6
L96:jsr	pc,_getch
mov	r0,-10.(r5)
jbr	L98
L99:L100:L101:jbr	L96
L102:.data
L103:.byte 0105,0117,0106,012,0
.even
.text
mov	$L103,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L104:L105:jsr	pc,_getch
cmp	$012,r0
jeq	L106
jbr	L105
L106:jbr	L96
jbr	L97
L98:mov	$L10010,r1
mov	r0,L10011
L10012:cmp	r0,(r1)+
jne	L10012
jmp	*L10011-L10010(r1)
.data
L10010:0
11
12
40
72
L10011:..
L102
L100
L101
L99
L104
L107
.text
/esw
L97:L107:clr	-8.(r5)
L110:mov	-8.(r5),r0
inc	-8.(r5)
movb	-10.(r5),_string(r0)
jsr	pc,_getch
mov	r0,-10.(r5)
L108:cmp	-10.(r5),$040
jeq	L10013
cmp	-10.(r5),$011
jeq	L10013
cmp	-10.(r5),$012
jeq	L10013
tst	-10.(r5)
jne	L110
L10013:L109:mov	-8.(r5),r0
clrb	_string(r0)
L95:jmp	rretrn
.globl	_rdfs
.text
_rdfs:
jsr	r5,rsave; 4
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fsi,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fsi,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-8.(r5)
cmp	-8.(r5),$01000
jeq	L112
.data
L113:.byte 0162,0145,0141,0144,040,0145,0162,0162,0157,0162,072,040,045,0154,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L113,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L112:L111:jmp	rretrn
.globl	_wtfs
.text
_wtfs:
jsr	r5,rsave; 4
mov	$03,(sp)
mov	4.(r5),-(sp)
mov	_fso,-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
mov	$01000,(sp)
mov	6.(r5),-(sp)
mov	_fso,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	r0,-8.(r5)
cmp	-8.(r5),$01000
jeq	L115
.data
L116:.byte 0167,0162,0151,0164,0145,040,0145,0162,0162,0157,0162,072,040,045,0154,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L116,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_exit
L115:L114:jmp	rretrn
.globl	_alloc
.text
_alloc:
jsr	r5,rsave; 6
dec	4.+_filsys
mov	4.+_filsys,r0
asl	r0
mov	6.+_filsys(r0),-8.(r5)
mov	4.+_filsys,r0
asl	r0
clr	6.+_filsys(r0)
tst	-8.(r5)
jne	L118
.data
L119:.byte 0157,0165,0164,040,0157,0146,040,0146,0162,0145,0145,040,0163,0160,0141,0143,0145,012,0
.even
.text
mov	$L119,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L118:tst	4.+_filsys
jgt	L120
mov	$_buf,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_rdfs
tst	(sp)+
mov	_buf,4.+_filsys
clr	-10.(r5)
L121:cmp	-10.(r5),$0144
jge	L122
mov	-10.(r5),r0
asl	r0
mov	-10.(r5),r1
asl	r1
mov	2.+_buf(r1),6.+_filsys(r0)
L123:inc	-10.(r5)
jbr	L121
L122:L120:mov	-8.(r5),r0
jbr	L117
L117:jmp	rretrn
.globl	_free
.text
_free:
jsr	r5,rsave; 4
cmp	4.+_filsys,$0144
jlt	L125
mov	4.+_filsys,_buf
clr	-8.(r5)
L126:cmp	-8.(r5),$0144
jge	L127
mov	-8.(r5),r0
asl	r0
mov	-8.(r5),r1
asl	r1
mov	6.+_filsys(r1),2.+_buf(r0)
L128:inc	-8.(r5)
jbr	L126
L127:mov	$_buf,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_wtfs
tst	(sp)+
clr	4.+_filsys
L125:mov	4.+_filsys,r0
asl	r0
mov	4.(r5),6.+_filsys(r0)
inc	4.+_filsys
L124:jmp	rretrn
.globl	_entry
.text
_entry:
jsr	r5,rsave; 6
mov	*8.(r5),r0
asl	r0
add	10.(r5),r0
mov	4.(r5),(r0)
inc	*8.(r5)
mov	*8.(r5),r0
asl	r0
add	10.(r5),r0
mov	r0,-8.(r5)
clr	-10.(r5)
L130:cmp	-10.(r5),$016
jge	L131
mov	-8.(r5),r0
inc	-8.(r5)
movb	*6.(r5),(r0)
tstb	*6.(r5)
jeq	L133
inc	6.(r5)
L133:L132:inc	-10.(r5)
jbr	L130
L131:add	$07,*8.(r5)
cmp	*8.(r5),$0400
jlt	L134
mov	14.(r5),(sp)
mov	12.(r5),-(sp)
mov	10.(r5),-(sp)
mov	8.(r5),-(sp)
jsr	pc,*$_newblk
add	$06,sp
L134:L129:jmp	rretrn
.globl	_newblk
.text
_newblk:
jsr	r5,rsave; 6
jsr	pc,_alloc
mov	r0,-8.(r5)
mov	6.(r5),(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_wtfs
tst	(sp)+
clr	-10.(r5)
L136:cmp	-10.(r5),$0400
jge	L137
mov	-10.(r5),r0
asl	r0
add	6.(r5),r0
clr	(r0)
L138:inc	-10.(r5)
jbr	L136
L137:clr	*4.(r5)
mov	*8.(r5),r0
asl	r0
add	10.(r5),r0
mov	-8.(r5),(r0)
inc	*8.(r5)
cmp	*8.(r5),$0400
jlt	L139
.data
L140:.byte 0151,0156,0144,0151,0162,0145,0143,0164,040,0142,0154,0157,0143,0153,040,0146,0165,0154,0154,012,0
.even
.text
mov	$L140,(sp)
jsr	pc,*$_printf
jsr	pc,_exit
L139:L135:jmp	rretrn
.globl	_getch
.text
_getch:
jsr	r5,rsave; 2
tst	_charp
jeq	L142
mov	_charp,r0
inc	_charp
movb	(r0),r0
jbr	L141
L142:jsr	pc,_getchar
jbr	L141
L141:jmp	rretrn
