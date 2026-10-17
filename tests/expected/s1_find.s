.globl	_randlast
.comm	_randlast,02
.globl	_pathname
.comm	_pathname,02
.globl	_verbose
.comm	_verbose,02
.globl	_node
.comm	_node,01130
.globl	_nn
.comm	_nn,02
.globl	_fname
.comm	_fname,02
.globl	_path
.comm	_path,02
.globl	_now
.comm	_now,04
.globl	_ap
.comm	_ap,02
.globl	_ac
.comm	_ac,02
.globl	_av
.comm	_av,02
.globl	_statb
.comm	_statb,044
.globl	_main
.text
_main:
jsr	r5,rsave; 4
mov	$_now,(sp)
jsr	pc,*$_time
mov	4.(r5),_ac
mov	6.(r5),_av
mov	$02,_ap
mov	6.(r5),r0
mov	2.(r0),_pathname
.data
L2:.byte 053,0
.even
.text
mov	$L2,(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L3
inc	_verbose
inc	_ap
mov	6.(r5),r0
mov	4.(r0),_pathname
jbr	L4
L3:clr	_verbose
L4:mov	4.(r5),r0
asl	r0
add	6.(r5),r0
clr	(r0)
cmp	4.(r5),$03
jge	L5
.data
L6:.byte 0111,0156,0163,0165,0146,0146,0151,0143,0151,0145,0156,0164,040,0141,0162,0147,0163,012,0
.even
.text
mov	$L6,(sp)
jsr	pc,*$_printf
mov	$011,(sp)
jsr	pc,*$_exit
L5:jsr	pc,_exp
mov	r0,-8.(r5)
jne	L7
.data
L8:.byte 0117,0144,0144,040,0165,0163,0141,0147,0145,012,0
.even
.text
mov	$L8,(sp)
jsr	pc,*$_printf
mov	$011,(sp)
jsr	pc,*$_exit
L7:cmp	_ap,4.(r5)
jge	L9
.data
L10:.byte 0115,0151,0163,0163,0151,0156,0147,040,0143,0157,0156,0152,0165,0156,0143,0164,0151,0157,0156,012,0
.even
.text
mov	$L10,(sp)
jsr	pc,*$_printf
mov	$011,(sp)
jsr	pc,*$_exit
L9:mov	-8.(r5),(sp)
mov	$_find,-(sp)
mov	$0146,-(sp)
mov	_pathname,-(sp)
jsr	pc,*$_descend
add	$06,sp
L1:jmp	rretrn
.globl	_exp
.text
_exp:
jsr	r5,rsave; 4
jsr	pc,_e1
mov	r0,-8.(r5)
.data
L12:.byte 055,0157,0
.even
.text
mov	$L12,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L13
dec	_randlast
jsr	pc,_exp
mov	r0,(sp)
mov	-8.(r5),-(sp)
mov	$_or,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L11
L13:dec	_ap
mov	-8.(r5),r0
jbr	L11
L11:jmp	rretrn
.globl	_e1
.text
_e1:
jsr	r5,rsave; 4
jsr	pc,_e2
mov	r0,-8.(r5)
.data
L15:.byte 055,0141,0
.even
.text
mov	$L15,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L16
dec	_randlast
jsr	pc,_e1
mov	r0,(sp)
mov	-8.(r5),-(sp)
mov	$_and,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L14
L16:dec	_ap
mov	-8.(r5),r0
jbr	L14
L14:jmp	rretrn
.globl	_e2
.text
_e2:
jsr	r5,rsave; 2
tst	_randlast
jeq	L18
.data
L19:.byte 0157,0160,0145,0162,0141,0156,0144,040,0146,0157,0154,0154,0157,0167,0163,040,0157,0160,0145,0162,0141,0156,0144,056,012,0
.even
.text
mov	$L19,(sp)
jsr	pc,*$_printf
mov	$011,(sp)
jsr	pc,*$_exit
L18:inc	_randlast
.data
L20:.byte 041,0
.even
.text
mov	$L20,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L21
clr	(sp)
jsr	pc,_e3
mov	r0,-(sp)
mov	$_not,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L17
L21:dec	_ap
jsr	pc,_e3
jbr	L17
L17:jmp	rretrn
.globl	_e3
.text
_e3:
jsr	r5,rsave; 14
jsr	pc,_nxtarg
mov	r0,-12.(r5)
.data
L23:.byte 050,0
.even
.text
mov	$L23,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L24
dec	_randlast
jsr	pc,_exp
mov	r0,-8.(r5)
jsr	pc,_nxtarg
mov	r0,-12.(r5)
.data
L25:.byte 051,0
.even
.text
mov	$L25,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L26
mov	-8.(r5),r0
jbr	L22
L24:jsr	pc,_nxtarg
mov	r0,-14.(r5)
movb	*-14.(r5),-16.(r5)
cmpb	*-14.(r5),$053
jne	L10000
mov	$01,r0
jbr	L10001
L10000:clr	r0
L10001:add	r0,-14.(r5)
.data
L27:.byte 055,0156,0141,0155,0145,0
.even
.text
mov	$L27,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L28
clr	(sp)
mov	-14.(r5),-(sp)
mov	$_glob,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
L28:.data
L29:.byte 055,0155,0164,0151,0155,0145,0
.even
.text
mov	$L29,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L30
movb	-16.(r5),r0
mov	r0,(sp)
mov	-14.(r5),-(sp)
jsr	pc,_atof
tst	(sp)+
movfi	r0,r0
mov	r0,-10.(r5)
mov	r0,-(sp)
mov	$_mtime,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
L30:.data
L31:.byte 055,0141,0164,0151,0155,0145,0
.even
.text
mov	$L31,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L32
movb	-16.(r5),r0
mov	r0,(sp)
mov	-14.(r5),-(sp)
jsr	pc,_atof
tst	(sp)+
movfi	r0,r0
mov	r0,-10.(r5)
mov	r0,-(sp)
mov	$_atime,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
L32:.data
L33:.byte 055,0165,0163,0145,0162,0
.even
.text
mov	$L33,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L34
mov	-14.(r5),(sp)
jsr	pc,*$_getunum
mov	r0,-10.(r5)
cmp	$0177777,r0
jne	L35
.data
L36:.byte 0103,0141,0156,0156,0157,0164,040,0146,0151,0156,0144,040,0165,0163,0145,0162,040,074,045,0163,076,012,0
.even
.text
mov	-14.(r5),(sp)
mov	$L36,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$011,(sp)
jsr	pc,*$_exit
L35:movb	-16.(r5),r0
mov	r0,(sp)
mov	-10.(r5),-(sp)
mov	$_user,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
jbr	L37
L34:.data
L38:.byte 055,0160,0162,0151,0156,0164,0
.even
.text
mov	$L38,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L39
dec	_ap
clr	(sp)
clr	-(sp)
mov	$_print,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
jbr	L40
L39:.data
L41:.byte 055,0147,0162,0157,0165,0160,0
.even
.text
mov	$L41,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L42
movb	-16.(r5),r0
mov	r0,(sp)
mov	-14.(r5),-(sp)
jsr	pc,_atof
tst	(sp)+
movfi	r0,r0
mov	r0,-10.(r5)
mov	r0,-(sp)
mov	$_group,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
L42:.data
L43:.byte 055,0163,0151,0172,0145,0
.even
.text
mov	$L43,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L44
movb	-16.(r5),r0
mov	r0,(sp)
mov	-14.(r5),-(sp)
jsr	pc,_atof
tst	(sp)+
movfi	r0,r0
mov	r0,-10.(r5)
mov	r0,-(sp)
mov	$_size,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
L44:.data
L45:.byte 055,0154,0151,0156,0153,0163,0
.even
.text
mov	$L45,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L46
movb	-16.(r5),r0
mov	r0,(sp)
mov	-14.(r5),-(sp)
jsr	pc,_atof
tst	(sp)+
movfi	r0,r0
mov	r0,-10.(r5)
mov	r0,-(sp)
mov	$_links,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
L46:.data
L47:.byte 055,0160,0145,0162,0155,0
.even
.text
mov	$L47,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L48
L49:tstb	*-14.(r5)
jeq	L50
cmpb	*-14.(r5),$055
jeq	L51
mov	-10.(r5),r0
ash	$03,r0
mov	r0,-10.(r5)
movb	*-14.(r5),r0
add	-10.(r5),r0
add	$0177720,r0
mov	r0,-10.(r5)
L51:inc	-14.(r5)
jbr	L49
L50:movb	-16.(r5),r0
mov	r0,(sp)
mov	-10.(r5),-(sp)
bic	$!03777,(sp)
mov	$_perm,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
jbr	L52
L48:.data
L53:.byte 055,0145,0170,0145,0143,0
.even
.text
mov	$L53,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L54
mov	_ap,r0
add	$0177777,r0
mov	r0,-10.(r5)
L55:.data
L57:.byte 073,0
.even
.text
mov	$L57,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jeq	L56
jbr	L55
L56:clr	(sp)
mov	-10.(r5),-(sp)
mov	$_exeq,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
jbr	L58
L54:.data
L59:.byte 055,0157,0153,0
.even
.text
mov	$L59,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L60
mov	_ap,r0
add	$0177777,r0
mov	r0,-10.(r5)
L61:.data
L63:.byte 073,0
.even
.text
mov	$L63,(sp)
jsr	pc,_nxtarg
mov	r0,-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jeq	L62
jbr	L61
L62:clr	(sp)
mov	-10.(r5),-(sp)
mov	$_ok,-(sp)
jsr	pc,*$_mk
cmp	(sp)+,(sp)+
jbr	L22
L60:L58:L52:L40:L37:L26:.data
L64:.byte 0105,0162,0162,0157,0162,040,074,045,0163,040,057,040,045,0163,076,012,0
.even
.text
mov	-14.(r5),(sp)
mov	-12.(r5),-(sp)
mov	$L64,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
mov	$011,(sp)
jsr	pc,*$_exit
L22:jmp	rretrn
.globl	_mk
.text
_mk:
jsr	r5,rsave; 2
mov	_nn,r1
mul	$06,r1
mov	4.(r5),_node(r1)
mov	_nn,r1
mul	$06,r1
mov	6.(r5),2.+_node(r1)
mov	_nn,r1
mul	$06,r1
mov	8.(r5),4.+_node(r1)
mov	_nn,r1
inc	_nn
mul	$06,r1
add	$_node,r1
mov	r1,r0
jbr	L65
L65:jmp	rretrn
.globl	_nxtarg
.text
_nxtarg:
jsr	r5,rsave; 2
cmp	_ap,_ac
jle	L67
clr	r0
jbr	L66
L67:mov	_ap,r0
inc	_ap
asl	r0
add	_av,r0
mov	(r0),r0
jbr	L66
L66:jmp	rretrn
.globl	_find
.text
_find:
jsr	r5,rsave; 2
mov	6.(r5),_path
tst	_verbose
jeq	L69
.data
L70:.byte 045,0163,0
.even
.text
mov	_path,(sp)
mov	$L70,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L69:clr	r4
L71:mov	6.(r5),r0
add	r4,r0
tstb	(r0)
jeq	L72
mov	6.(r5),r0
add	r4,r0
movb	(r0),r0
cmp	$057,r0
jne	L74
mov	6.(r5),r0
add	r4,r0
inc	r0
mov	r0,_fname
L74:L73:inc	r4
jbr	L71
L72:mov	4.(r5),-(sp)
mov	*4.(r5),r0
jsr	pc,(r0)
tst	(sp)+
mov	r0,r4
tst	_verbose
jeq	L75
tst	r4
jeq	L76
.data
L77:.byte 056,012,0
.even
.text
mov	$L77,(sp)
jsr	pc,*$_printf
jbr	L78
L76:.data
L79:.byte 012,0
.even
.text
mov	$L79,(sp)
jsr	pc,*$_printf
L78:L75:L68:jmp	rretrn
.globl	_and
.text
_and:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	4.(r5),r0
mov	*2.(r0),r0
jsr	pc,(r0)
tst	(sp)+
tst	r0
jeq	L10002
mov	4.(r5),r0
mov	4.(r0),-(sp)
mov	4.(r5),r0
mov	*4.(r0),r0
jsr	pc,(r0)
tst	(sp)+
tst	r0
jeq	L10002
mov	$01,r0
jbr	L10003
L10002:clr	r0
L10003:jbr	L80
L80:jmp	rretrn
.globl	_or
.text
_or:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	4.(r5),r0
mov	*2.(r0),r0
jsr	pc,(r0)
tst	(sp)+
tst	r0
jne	L10005
mov	4.(r5),r0
mov	4.(r0),-(sp)
mov	4.(r5),r0
mov	*4.(r0),r0
jsr	pc,(r0)
tst	(sp)+
tst	r0
jeq	L10004
L10005:mov	$01,r0
jbr	L10006
L10004:clr	r0
L10006:jbr	L81
L81:jmp	rretrn
.globl	_not
.text
_not:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	4.(r5),r0
mov	*2.(r0),r0
jsr	pc,(r0)
tst	(sp)+
tst	r0
beq	1f
clr	r0
br	2f
1:	mov	$1,r0
2:
jbr	L82
L82:jmp	rretrn
.globl	_glob
.text
_glob:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	2.(r0),(sp)
mov	_fname,-(sp)
jsr	pc,*$_gmatch
tst	(sp)+
jbr	L83
L83:jmp	rretrn
.globl	_print
.text
_print:
jsr	r5,rsave; 2
.data
L85:.byte 045,0163,012,0
.even
.text
mov	_path,(sp)
mov	$L85,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$01,r0
jbr	L84
L84:jmp	rretrn
.globl	_mtime
.text
_mtime:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	4.(r0),(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	_now,r1
sub	32.+_statb,r1
mul	$03,r1
ash	$-02,r1
mov	r1,-(sp)
jsr	pc,*$_scomp
cmp	(sp)+,(sp)+
jbr	L86
L86:jmp	rretrn
.globl	_atime
.text
_atime:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	4.(r0),(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	_now,r1
sub	28.+_statb,r1
mul	$03,r1
ash	$-02,r1
mov	r1,-(sp)
jsr	pc,*$_scomp
cmp	(sp)+,(sp)+
jbr	L87
L87:jmp	rretrn
.globl	_user
.text
_user:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	4.(r0),(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
movb	7.+_statb,r0
mov	r0,-(sp)
jsr	pc,*$_scomp
cmp	(sp)+,(sp)+
jbr	L88
L88:jmp	rretrn
.globl	_group
.text
_group:
jsr	r5,rsave; 2
mov	4.(r5),r0
movb	8.+_statb,r1
cmp	2.(r0),r1
jeq	L10007
clr	r0
jbr	L10008
L10007:mov	$01,r0
L10008:jbr	L89
L89:jmp	rretrn
.globl	_links
.text
_links:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	4.(r0),(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
movb	6.+_statb,r0
mov	r0,-(sp)
jsr	pc,*$_scomp
cmp	(sp)+,(sp)+
jbr	L90
L90:jmp	rretrn
.globl	_size
.text
_size:
jsr	r5,rsave; 2
movb	9.+_statb,r0
ash	$07,r0
mov	r0,r4
mov	10.+_statb,r0
ash	$-011,r0
bic	$!0777,r0
bis	r0,r4
mov	4.(r5),r0
mov	4.(r0),(sp)
mov	4.(r5),r0
mov	2.(r0),-(sp)
mov	r4,-(sp)
jsr	pc,*$_scomp
cmp	(sp)+,(sp)+
jbr	L91
L91:jmp	rretrn
.globl	_perm
.text
_perm:
jsr	r5,rsave; 4
mov	4.(r5),r0
cmp	4.(r0),$055
jne	L10009
mov	4.(r5),r0
mov	2.(r0),r0
jbr	L10010
L10009:mov	$03777,r0
L10010:mov	r0,-8.(r5)
mov	4.+_statb,r0
mov	-8.(r5),r1
com	r1
bic	r1,r0
mov	4.(r5),r1
cmp	2.(r1),r0
jeq	L10011
clr	r0
jbr	L10012
L10011:mov	$01,r0
L10012:jbr	L92
L92:jmp	rretrn
.globl	_exeq
.text
_exeq:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_doex
jbr	L93
L93:jmp	rretrn
.globl	_ok
.text
_ok:
jsr	r5,rsave; 6
clr	-10.(r5)
.data
L95:.byte 041,045,0163,041,040,040,0146,0157,0162,040,074,045,0163,076,077,040,040,040,040,040,0
.even
.text
mov	_path,(sp)
mov	4.(r5),r0
mov	2.(r0),r0
asl	r0
add	_av,r0
mov	(r0),-(sp)
mov	$L95,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jsr	pc,_getchar
movb	r0,-8.(r5)
cmp	$0171,r0
jne	L96
mov	$01,-10.(r5)
L96:L97:cmpb	-8.(r5),$012
jeq	L98
jsr	pc,_getchar
movb	r0,-8.(r5)
jbr	L97
L98:tst	-10.(r5)
jeq	L99
mov	4.(r5),r0
mov	2.(r0),(sp)
jsr	pc,*$_doex
jbr	L94
L99:clr	r0
jbr	L94
L94:jmp	rretrn
.globl	_scomp
.text
_scomp:
jsr	r5,rsave; 2
cmpb	8.(r5),$053
jne	L101
cmp	4.(r5),6.(r5)
jgt	L10013
clr	r0
jbr	L10014
L10013:mov	$01,r0
L10014:jbr	L100
L101:cmpb	8.(r5),$055
jne	L102
mov	6.(r5),r1
mul	$0177777,r1
cmp	4.(r5),r1
jlt	L10015
clr	r0
jbr	L10016
L10015:mov	$01,r0
L10016:jbr	L100
L102:cmp	4.(r5),6.(r5)
jeq	L10017
clr	r0
jbr	L10018
L10017:mov	$01,r0
L10018:jbr	L100
L100:jmp	rretrn
.globl	_doex
.text
_doex:
jsr	r5,rsave; 162
clr	r0
mov	r0,-10.(r5)
mov	r0,-8.(r5)
L104:mov	4.(r5),r0
inc	4.(r5)
asl	r0
add	_av,r0
mov	(r0),-118.(r5)
jeq	L105
.data
L106:.byte 073,0
.even
.text
mov	$L106,(sp)
mov	-118.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jeq	L105
.data
L107:.byte 0173,0175,0
.even
.text
mov	$L107,(sp)
mov	-118.(r5),-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L108
mov	-10.(r5),r0
inc	-10.(r5)
asl	r0
add	r5,r0
mov	_path,-114.(r0)
jbr	L109
L108:mov	-10.(r5),r0
inc	-10.(r5)
asl	r0
add	r5,r0
mov	-118.(r5),-114.(r0)
L109:jbr	L104
L105:mov	-10.(r5),r0
asl	r0
add	r5,r0
clr	-114.(r0)
tst	-10.(r5)
jne	L110
mov	$011,r0
jbr	L103
L110:jsr	pc,_fork
tst	r0
jeq	L111
mov	r5,r0
add	$0177770,r0
mov	r0,(sp)
jsr	pc,*$_wait
jbr	L112
L111:mov	-10.(r5),(sp)
mov	r5,r0
add	$0177616,r0
mov	r0,-(sp)
mov	-114.(r5),-(sp)
jsr	pc,*$_execv
cmp	(sp)+,(sp)+
clr	-12.(r5)
.data
L113:.byte 057,0165,0163,0162,057,0142,0151,0156,057,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0170,0
.even
.text
mov	$L113,-116.(r5)
L114:mov	-114.(r5),r0
add	-12.(r5),r0
movb	(r0),r0
mov	r0,-14.(r5)
jeq	L115
mov	-12.(r5),r0
inc	-12.(r5)
add	-116.(r5),r0
movb	-14.(r5),9.(r0)
jbr	L114
L115:mov	-116.(r5),r0
add	-12.(r5),r0
clrb	9.(r0)
mov	-10.(r5),(sp)
mov	r5,r0
add	$0177616,r0
mov	r0,-(sp)
mov	-116.(r5),-(sp)
add	$04,(sp)
jsr	pc,*$_execv
cmp	(sp)+,(sp)+
mov	-10.(r5),(sp)
mov	r5,r0
add	$0177616,r0
mov	r0,-(sp)
mov	-116.(r5),-(sp)
jsr	pc,*$_execv
cmp	(sp)+,(sp)+
mov	$011,(sp)
jsr	pc,*$_exit
L112:tst	-8.(r5)
jeq	L10019
clr	r0
jbr	L10020
L10019:mov	$01,r0
L10020:jbr	L103
L103:jmp	rretrn
.globl	_fin
.comm	_fin,01006
.globl	_getunum
.text
_getunum:
jsr	r5,rsave; 34
mov	$0177777,-8.(r5)
.data
L117:.byte 057,0145,0164,0143,057,0160,0141,0163,0163,0167,0144,0
.even
.text
clr	(sp)
mov	$L117,-(sp)
jsr	pc,*$_open
tst	(sp)+
movb	r0,_fin
L118:jsr	pc,_getchar
movb	r0,-32.(r5)
jeq	L119
cmpb	-32.(r5),$012
jne	L120
mov	r5,r0
add	$0177744,r0
mov	r0,-30.(r5)
L121:jsr	pc,_getchar
movb	r0,*-30.(r5)
cmp	$072,r0
jeq	L122
mov	-30.(r5),r0
inc	-30.(r5)
tstb	(r0)
jeq	L123
jbr	L121
L122:clrb	*-30.(r5)
mov	4.(r5),(sp)
mov	r5,r0
add	$0177744,r0
mov	r0,-(sp)
jsr	pc,*$_compstr
tst	(sp)+
tst	r0
jne	L124
L125:jsr	pc,_getchar
movb	r0,-32.(r5)
cmp	$072,r0
jeq	L126
tstb	-32.(r5)
jeq	L123
jbr	L125
L126:mov	r5,r0
add	$0177744,r0
mov	r0,-30.(r5)
L127:jsr	pc,_getchar
movb	r0,*-30.(r5)
cmp	$072,r0
jeq	L128
inc	-30.(r5)
jbr	L127
L128:clrb	*-30.(r5)
mov	r5,r0
add	$0177744,r0
mov	r0,(sp)
jsr	pc,*$_atof
movfi	r0,r0
mov	r0,-8.(r5)
jbr	L119
L124:L120:jbr	L118
L119:L123:mov	$_fin,(sp)
jsr	pc,*$_close
clrb	_fin
mov	-8.(r5),r0
jbr	L116
L116:jmp	rretrn
.globl	_compstr
.text
_compstr:
jsr	r5,rsave; 2
mov	4.(r5),r4
mov	6.(r5),r3
L130:cmpb	(r4),(r3)
jne	L131
tstb	(r4)+
jne	L132
clr	r0
jbr	L129
L132:inc	r3
jbr	L130
L131:cmpb	(r4),(r3)
jle	L10021
mov	$01,r0
jbr	L10022
L10021:mov	$0177777,r0
L10022:jbr	L129
L129:jmp	rretrn
.globl	_descend
.text
_descend:
jsr	r5,rsave; 1212
mov	$_statb,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jge	L134
.data
L135:.byte 055,055,0142,0141,0144,040,0163,0164,0141,0164,0165,0163,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L135,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	r0
jbr	L133
L134:mov	4.+_statb,r0
bic	$!060000,r0
cmp	$040000,r0
jeq	L136
cmpb	6.(r5),$0146
jeq	L10023
cmpb	6.(r5),$0142
jne	L137
L10023:mov	4.(r5),-(sp)
mov	10.(r5),-(sp)
jsr	pc,*8.(r5)
cmp	(sp)+,(sp)+
L137:mov	$01,r0
jbr	L133
jbr	L138
L136:cmpb	6.(r5),$0144
jeq	L10024
cmpb	6.(r5),$0142
jne	L139
L10024:mov	4.(r5),-(sp)
mov	10.(r5),-(sp)
jsr	pc,*8.(r5)
cmp	(sp)+,(sp)+
L139:L138:mov	10.+_statb,-14.(r5)
clr	-10.(r5)
L140:cmp	-10.(r5),-14.(r5)
jge	L141
mov	-14.(r5),r0
sub	-10.(r5),r0
cmp	$01000,r0
jge	L10025
mov	$01000,r0
jbr	L10026
L10025:mov	-14.(r5),r0
sub	-10.(r5),r0
L10026:mov	r0,-12.(r5)
clr	(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-8.(r5)
jge	L143
.data
L144:.byte 055,055,0143,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L144,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	r0
jbr	L133
L143:tst	-10.(r5)
jeq	L145
clr	(sp)
mov	-10.(r5),-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L145:mov	-12.(r5),(sp)
mov	r5,r0
add	$0176762,r0
mov	r0,-(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
tst	r0
jge	L146
.data
L147:.byte 055,055,0143,0141,0156,0156,0157,0164,040,0162,0145,0141,0144,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L147,-(sp)
jsr	pc,*$_printf
tst	(sp)+
clr	r0
jbr	L133
L146:mov	-8.(r5),(sp)
jsr	pc,*$_close
clr	r4
L148:mov	-12.(r5),r0
ash	$-04,r0
cmp	r4,r0
jge	L149
mov	r4,r0
ash	$04,r0
add	r5,r0
tst	-526.(r0)
jeq	L150
mov	r4,r0
ash	$04,r0
add	r5,r0
movb	-524.(r0),r0
cmp	$056,r0
jeq	L150
mov	r4,r0
ash	$04,r0
add	r5,r0
cmp	-526.(r0),$0177777
jeq	L149
clr	r3
L151:mov	r3,r0
add	r5,r0
mov	4.(r5),r1
add	r3,r1
movb	(r1),-654.(r0)
jeq	L152
L153:inc	r3
jbr	L151
L152:mov	r3,r0
add	r5,r0
movb	-655.(r0),r0
cmp	$057,r0
jeq	L154
mov	r3,r0
inc	r3
add	r5,r0
movb	$057,-654.(r0)
L154:clr	r2
L155:mov	r3,r0
inc	r3
add	r5,r0
mov	r4,r1
ash	$04,r1
add	r2,r1
add	r5,r1
movb	-524.(r1),-654.(r0)
jeq	L156
cmp	r2,$015
jge	L156
L157:inc	r2
jbr	L155
L156:mov	r3,r0
add	r5,r0
clrb	-654.(r0)
mov	10.(r5),(sp)
mov	8.(r5),-(sp)
movb	6.(r5),r0
mov	r0,-(sp)
mov	r5,r0
add	$0176562,r0
mov	r0,-(sp)
jsr	pc,*$_descend
add	$06,sp
tst	r0
jne	L158
.data
L159:.byte 055,055,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L159,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L158:L150:inc	r4
jbr	L148
L149:L142:add	$01000,-10.(r5)
jbr	L140
L141:mov	$01,r0
jbr	L133
L133:jmp	rretrn
.globl	_gmatch
.text
_gmatch:
jsr	r5,rsave; 2
cmpb	*4.(r5),$056
jne	L161
cmpb	*6.(r5),$056
jeq	L161
clr	r0
jbr	L160
L161:mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_amatch
tst	(sp)+
jbr	L160
L160:jmp	rretrn
.globl	_amatch
.text
_amatch:
jsr	r5,rsave; 6
movb	*4.(r5),r3
mov	$077777,-10.(r5)
movb	*6.(r5),r0
mov	r0,-8.(r5)
jbr	L164
L165:clr	r2
L166:inc	6.(r5)
mov	6.(r5),r0
movb	(r0),r4
jeq	L167
mov	r4,r0
jbr	L169
L170:tst	r2
jeq	L171
inc	6.(r5)
mov	6.(r5),(sp)
inc	4.(r5)
mov	4.(r5),-(sp)
jsr	pc,*$_amatch
tst	(sp)+
jbr	L162
L171:clr	r0
jbr	L162
L172:mov	6.(r5),r0
movb	1.(r0),r0
mov	r0,r4
cmp	r3,r0
jle	L10027
clr	r0
jbr	L10028
L10027:mov	$01,r0
L10028:cmp	-10.(r5),r3
jle	L10029
clr	r1
jbr	L10030
L10029:mov	$01,r1
L10030:com	r1
bic	r1,r0
bis	r0,r2
jbr	L168
L169:mov	$L10032,r1
mov	r0,L10033
L10034:cmp	r0,(r1)+
jne	L10034
jmp	*L10033-L10032(r1)
.data
L10032:55
135
L10033:..
L172
L170
L173
.text
/esw
L168:L173:mov	r4,r0
mov	r0,-10.(r5)
cmp	r3,r0
jne	L174
inc	r2
L174:jbr	L166
L167:clr	r0
jbr	L162
L175:L176:tst	r3
jeq	L177
inc	6.(r5)
mov	6.(r5),(sp)
inc	4.(r5)
mov	4.(r5),-(sp)
jsr	pc,*$_amatch
tst	(sp)+
jbr	L162
L177:clr	r0
jbr	L162
L178:inc	6.(r5)
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_umatch
tst	(sp)+
jbr	L162
L179:mov	r3,r0
beq	1f
clr	r0
br	2f
1:	mov	$1,r0
2:
jbr	L162
jbr	L163
L164:mov	$L10036,r1
mov	r0,L10037
L10038:cmp	r0,(r1)+
jne	L10038
jmp	*L10037-L10036(r1)
.data
L10036:0
52
77
133
L10037:..
L179
L178
L175
L165
L180
.text
/esw
L163:L180:cmp	-8.(r5),r3
jeq	L176
clr	r0
jbr	L162
L162:jmp	rretrn
.globl	_umatch
.text
_umatch:
jsr	r5,rsave; 2
tstb	*6.(r5)
jne	L182
mov	$01,r0
jbr	L181
L182:L183:tstb	*4.(r5)
jeq	L184
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
inc	4.(r5)
jsr	pc,*$_amatch
tst	(sp)+
tst	r0
jeq	L185
mov	$01,r0
jbr	L181
L185:jbr	L183
L184:clr	r0
jbr	L181
L181:jmp	rretrn
.globl	fltused
