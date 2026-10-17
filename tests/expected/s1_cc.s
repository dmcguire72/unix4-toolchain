.globl	_tmp0
.comm	_tmp0,02
.globl	_tmp1
.comm	_tmp1,02
.globl	_tmp2
.comm	_tmp2,02
.globl	_tmp3
.comm	_tmp3,02
.globl	_tmp4
.comm	_tmp4,02
.globl	_tmp5
.comm	_tmp5,02
.globl	_ts
.comm	_ts,01750
.globl	_tsp
.data
_tsp=.
_ts
.globl	_av
.comm	_av,0144
.globl	_clist
.comm	_clist,0144
.globl	_llist
.comm	_llist,0144
.globl	_instring
.comm	_instring,02
.globl	_pflag
.comm	_pflag,02
.globl	_sflag
.comm	_sflag,02
.globl	_cflag
.comm	_cflag,02
.globl	_oflag
.comm	_oflag,02
.globl	_proflag
.comm	_proflag,02
.globl	_ibuf
.comm	_ibuf,02
.globl	_ibuf1
.comm	_ibuf1,02
.globl	_ibuf2
.comm	_ibuf2,02
.globl	_obuf
.comm	_obuf,02
.globl	_lp
.comm	_lp,02
.globl	_line
.comm	_line,02
.globl	_lineno
.comm	_lineno,02
.globl	_exfail
.comm	_exfail,02
.globl	_symtab
.comm	_symtab,02
.globl	_symsiz
.data
_symsiz=.
200.
.globl	_defloc
.comm	_defloc,02
.globl	_incloc
.comm	_incloc,02
.globl	_stringbu
.comm	_stringbu,02
.text
L1:.byte 057,0154,0151,0142,057,0143,060,0
.even
.data
.globl	_pass0
.data
_pass0=.
L1
.text
L2:.byte 057,0154,0151,0142,057,0143,061,0
.even
.data
.globl	_pass1
.data
_pass1=.
L2
.text
L3:.byte 057,0154,0151,0142,057,0143,0162,0164,060,056,0157,0
.even
.data
.globl	_pref
.data
_pref=.
L3
.globl	_main
.text
_main:
jsr	r5,rsave; 22
clr	r0
mov	r0,-22.(r5)
mov	r0,-20.(r5)
mov	r0,-12.(r5)
mov	r0,-10.(r5)
mov	r0,-14.(r5)
L5:inc	-14.(r5)
mov	-14.(r5),r0
cmp	4.(r5),r0
jle	L6
mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L7
mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
movb	1.(r0),r0
jbr	L9
L10:jbr	L11
L12:inc	_sflag
inc	_cflag
jbr	L8
L13:inc	_oflag
jbr	L8
L14:inc	_proflag
.data
L15:.byte 057,0154,0151,0142,057,0155,0143,0162,0164,060,056,0157,0
.even
.text
mov	$L15,_pref
jbr	L8
L16:inc	_pflag
L17:inc	_cflag
jbr	L8
L18:mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
tstb	2.(r0)
jne	L19
.data
L20:.byte 057,0154,0151,0142,057,0143,0162,0164,062,056,0157,0
.even
.text
mov	$L20,_pref
jbr	L21
L19:.data
L22:.byte 057,0154,0151,0142,057,0143,0162,0164,062,060,056,0157,0
.even
.text
mov	$L22,_pref
mov	$01,-20.(r5)
L21:jbr	L8
L23:mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
movb	2.(r0),r0
cmp	$060,r0
jne	L24
.data
L25:.byte 057,0165,0163,0162,057,0143,057,0143,060,0
.even
.text
mov	$L25,_pass0
L24:mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
movb	2.(r0),r0
cmp	$061,r0
jne	L26
.data
L27:.byte 057,0165,0163,0162,057,0143,057,0143,061,0
.even
.text
mov	$L27,_pass1
L26:jbr	L8
jbr	L8
L9:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:62
117
120
123
143
160
164
L10002:..
L18
L13
L16
L12
L17
L14
L23
L10
.text
/esw
L8:jbr	L28
L7:L11:mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-8.(r5)
mov	-8.(r5),(sp)
jsr	pc,*$_getsuf
cmp	$0143,r0
jne	L29
mov	-10.(r5),r0
inc	-10.(r5)
asl	r0
mov	-8.(r5),_clist(r0)
mov	$0157,(sp)
mov	-8.(r5),-(sp)
jsr	pc,_copy
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_setsuf
tst	(sp)+
mov	r0,-8.(r5)
L29:mov	-8.(r5),(sp)
mov	$_llist,-(sp)
jsr	pc,*$_nodup
tst	(sp)+
tst	r0
jeq	L30
mov	-12.(r5),r0
inc	-12.(r5)
asl	r0
mov	-8.(r5),_llist(r0)
mov	-8.(r5),(sp)
jsr	pc,*$_getsuf
cmp	$0157,r0
jne	L31
inc	-22.(r5)
L31:L30:L28:jbr	L5
L6:tst	-10.(r5)
jeq	L32
tst	_pflag
jne	L33
.data
L34:.byte 057,0164,0155,0160,057,0143,0164,0155,060,0141,0
.even
.text
mov	$L34,(sp)
jsr	pc,*$_copy
mov	r0,_tmp0
L35:clr	(sp)
mov	_tmp0,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-18.(r5)
jlt	L36
mov	-18.(r5),(sp)
jsr	pc,*$_close
mov	_tmp0,r0
incb	9.(r0)
jbr	L35
L36:L37:mov	$0400,(sp)
mov	_tmp0,-(sp)
jsr	pc,*$_creat
tst	(sp)+
tst	r0
jge	L38
mov	_tmp0,r0
incb	9.(r0)
jbr	L37
L38:L33:mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
bit	$01,r0
jne	L39
mov	$_dexit,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L39:mov	_tmp0,(sp)
jsr	pc,*$_copy
mov	r0,_tmp1
movb	$061,8.(r0)
mov	_tmp0,(sp)
jsr	pc,*$_copy
mov	r0,_tmp2
movb	$062,8.(r0)
mov	_tmp0,(sp)
jsr	pc,*$_copy
mov	r0,_tmp3
movb	$063,8.(r0)
tst	_oflag
jeq	L40
mov	_tmp0,(sp)
jsr	pc,*$_copy
mov	r0,_tmp5
movb	$065,8.(r0)
L40:tst	_pflag
jne	L41
mov	_tmp0,(sp)
jsr	pc,*$_copy
mov	r0,_tmp4
movb	$064,8.(r0)
L41:clr	-14.(r5)
L42:cmp	-14.(r5),-10.(r5)
jge	L43
cmp	-10.(r5),$01
jle	L45
.data
L46:.byte 045,0163,072,012,0
.even
.text
mov	-14.(r5),r0
asl	r0
mov	_clist(r0),(sp)
mov	$L46,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L45:.data
L47:.byte 0143,060,0
.even
.text
mov	$L47,_av
tst	_pflag
jeq	L48
mov	$0151,(sp)
mov	-14.(r5),r0
asl	r0
mov	_clist(r0),-(sp)
jsr	pc,_copy
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_setsuf
tst	(sp)+
mov	r0,_tmp4
L48:mov	-14.(r5),r0
asl	r0
mov	_clist(r0),(sp)
jsr	pc,*$_expand
mov	r0,2.+_av
tst	_pflag
jne	L44
tst	_exfail
jne	L44
tst	2.+_av
jne	L49
inc	_cflag
jbr	L44
L49:mov	_tmp1,4.+_av
mov	_tmp2,6.+_av
tst	_proflag
jeq	L50
.data
L51:.byte 055,0120,0
.even
.text
mov	$L51,8.+_av
clr	10.+_av
jbr	L52
L50:clr	8.+_av
L52:mov	$_av,(sp)
mov	_pass0,-(sp)
jsr	pc,*$_callsys
tst	(sp)+
tst	r0
jeq	L53
inc	_cflag
jbr	L44
L53:.data
L54:.byte 0143,061,0
.even
.text
mov	$L54,_av
mov	_tmp1,2.+_av
mov	_tmp2,4.+_av
tst	_sflag
jeq	L55
mov	$0163,(sp)
mov	-14.(r5),r0
asl	r0
mov	_clist(r0),-(sp)
jsr	pc,_copy
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_setsuf
tst	(sp)+
mov	r0,_tmp3
L55:mov	_tmp3,6.+_av
tst	_oflag
jeq	L56
mov	_tmp5,6.+_av
L56:clr	8.+_av
mov	$_av,(sp)
mov	_pass1,-(sp)
jsr	pc,*$_callsys
tst	(sp)+
tst	r0
jeq	L57
inc	_cflag
jbr	L44
L57:tst	_oflag
jeq	L58
.data
L59:.byte 0143,062,0
.even
.text
mov	$L59,_av
mov	_tmp5,2.+_av
mov	_tmp3,4.+_av
clr	6.+_av
.data
L60:.byte 057,0154,0151,0142,057,0143,062,0
.even
.text
mov	$_av,(sp)
mov	$L60,-(sp)
jsr	pc,*$_callsys
tst	(sp)+
mov	_tmp5,(sp)
jsr	pc,*$_unlink
L58:tst	_sflag
jne	L44
.data
L61:.byte 0141,0163,0
.even
.text
mov	$L61,_av
.data
L62:.byte 055,0
.even
.text
mov	$L62,2.+_av
mov	_tmp3,4.+_av
clr	6.+_av
mov	_tmp1,(sp)
jsr	pc,*$_cunlink
mov	_tmp2,(sp)
jsr	pc,*$_cunlink
mov	_tmp4,(sp)
jsr	pc,*$_cunlink
.data
L63:.byte 057,0142,0151,0156,057,0141,0163,0
.even
.text
mov	$_av,(sp)
mov	$L63,-(sp)
jsr	pc,*$_callsys
tst	(sp)+
mov	$0157,(sp)
mov	-14.(r5),r0
asl	r0
mov	_clist(r0),-(sp)
jsr	pc,*$_setsuf
tst	(sp)+
mov	r0,-8.(r5)
mov	-8.(r5),(sp)
jsr	pc,*$_cunlink
.data
L64:.byte 0141,056,0157,0165,0164,0
.even
.text
.data
L65:.byte 0141,056,0157,0165,0164,0
.even
.text
mov	-8.(r5),(sp)
mov	$L64,-(sp)
jsr	pc,*$_link
tst	(sp)+
tst	r0
jne	L10004
mov	$L65,(sp)
jsr	pc,*$_cunlink
tst	r0
jeq	L66
L10004:.data
L67:.byte 0155,0157,0166,0145,040,0146,0141,0151,0154,0145,0144,072,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L67,-(sp)
jsr	pc,*$_printf
tst	(sp)+
inc	_cflag
L66:L44:inc	-14.(r5)
jbr	L42
L43:L32:tst	_cflag
jne	L68
tst	-12.(r5)
jeq	L68
clr	-14.(r5)
.data
L69:.byte 0154,0144,0
.even
.text
mov	$L69,_av
.data
L70:.byte 055,0170,0
.even
.text
mov	$L70,2.+_av
mov	_pref,4.+_av
mov	$03,-16.(r5)
L71:cmp	-14.(r5),-12.(r5)
jge	L72
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	-14.(r5),r1
inc	-14.(r5)
asl	r1
mov	_llist(r1),_av(r0)
jbr	L71
L72:tst	-20.(r5)
jeq	L73
.data
L74:.byte 055,0154,062,0
.even
.text
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	$L74,_av(r0)
jbr	L75
L73:.data
L76:.byte 055,0154,0143,0
.even
.text
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	$L76,_av(r0)
.data
L77:.byte 055,0154,0
.even
.text
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	$L77,_av(r0)
L75:mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
clr	_av(r0)
.data
L78:.byte 057,0142,0151,0156,057,0154,0144,0
.even
.text
mov	$_av,(sp)
mov	$L78,-(sp)
jsr	pc,*$_callsys
tst	(sp)+
cmp	-10.(r5),$01
jne	L79
cmp	-22.(r5),$01
jne	L79
mov	$0157,(sp)
mov	_clist,-(sp)
jsr	pc,*$_setsuf
tst	(sp)+
mov	r0,(sp)
jsr	pc,*$_cunlink
L79:L68:jsr	pc,_dexit
L4:jmp	rretrn
.globl	_dexit
.text
_dexit:
jsr	r5,rsave; 2
tst	_pflag
jne	L81
mov	_tmp1,(sp)
jsr	pc,*$_cunlink
mov	_tmp2,(sp)
jsr	pc,*$_cunlink
tst	_sflag
jne	L82
mov	_tmp3,(sp)
jsr	pc,*$_cunlink
L82:mov	_tmp4,(sp)
jsr	pc,*$_cunlink
mov	_tmp5,(sp)
jsr	pc,*$_cunlink
mov	_tmp0,(sp)
jsr	pc,*$_cunlink
L81:jsr	pc,_exit
L80:jmp	rretrn
.globl	_expand
.text
_expand:
jsr	r5,rsave; 11250
clr	_exfail
mov	r5,r0
add	$0176764,r0
mov	r0,_ibuf1
mov	r0,_ibuf
mov	r5,r0
add	$0175756,r0
mov	r0,_ibuf2
mov	_ibuf1,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L84
mov	4.(r5),r0
jbr	L83
L84:mov	_ibuf1,(sp)
jsr	pc,*$_getc
cmp	$043,r0
jeq	L85
mov	*_ibuf1,(sp)
jsr	pc,*$_close
mov	4.(r5),r0
jbr	L83
L85:mov	_ibuf1,r0
inc	2.(r0)
mov	_ibuf1,r0
dec	4.(r0)
mov	r5,r0
add	$0174750,r0
mov	r0,_obuf
mov	r5,r0
add	$0171030,r0
mov	r0,_symtab
clr	r4
L86:cmp	r4,$0310
jge	L87
mov	r4,r1
mul	$012,r1
add	r5,r1
clrb	-3560.(r1)
mov	r4,r1
mul	$012,r1
add	r5,r1
clr	-3552.(r1)
L88:inc	r4
jbr	L86
L87:.data
L89:.byte 0144,0145,0146,0151,0156,0145,0
.even
.text
mov	$01,(sp)
mov	$L89,-(sp)
jsr	pc,*$_lookup
tst	(sp)+
mov	r0,_defloc
mov	_defloc,r0
mov	_defloc,8.(r0)
.data
L90:.byte 0151,0156,0143,0154,0165,0144,0145,0
.even
.text
mov	$01,(sp)
mov	$L90,-(sp)
jsr	pc,*$_lookup
tst	(sp)+
mov	r0,_incloc
mov	_incloc,r0
mov	_incloc,8.(r0)
mov	r5,r0
add	$0166524,r0
mov	r0,_stringbu
mov	r5,r0
add	$0170524,r0
mov	r0,_line
clr	_lineno
mov	_obuf,(sp)
mov	_tmp4,-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jge	L91
.data
L92:.byte 0103,0141,0156,047,0164,040,0143,0162,0145,0141,0164,040,045,0163,012,0
.even
.text
mov	_tmp4,(sp)
mov	$L92,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_dexit
L91:L93:jsr	pc,_getline
tst	r0
jeq	L94
cmp	_ibuf,_ibuf2
jne	L95
tst	_pflag
jne	L95
mov	_obuf,(sp)
mov	$01,-(sp)
jsr	pc,*$_putc
tst	(sp)+
L95:cmpb	-3756.(r5),$043
jeq	L96
mov	_line,r3
L97:movb	(r3)+,r4
jeq	L98
mov	_obuf,(sp)
mov	r4,-(sp)
jsr	pc,*$_putc
tst	(sp)+
jbr	L97
L98:L96:mov	_obuf,(sp)
mov	$012,-(sp)
jsr	pc,*$_putc
tst	(sp)+
jbr	L93
L94:mov	_obuf,(sp)
jsr	pc,*$_fflush
mov	*_obuf,(sp)
jsr	pc,*$_close
mov	*_ibuf1,(sp)
jsr	pc,*$_close
mov	_tmp4,r0
jbr	L83
L83:jmp	rretrn
.globl	_getline
.text
_getline:
jsr	r5,rsave; 10
cmp	_ibuf,_ibuf1
jne	L100
inc	_lineno
L100:mov	_line,_lp
clrb	*_lp
clr	r2
jsr	pc,_getch
mov	r0,r4
cmp	$043,r0
jne	L101
mov	$01,r2
L101:L102:cmp	r4,$012
jeq	L103
tst	r4
jeq	L103
cmp	r4,$0141
jlt	L10006
cmp	r4,$0172
jle	L10005
L10006:cmp	r4,$0101
jlt	L10007
cmp	r4,$0132
jle	L10005
L10007:cmp	r4,$0137
jne	L104
L10005:mov	_lp,-10.(r5)
mov	r4,(sp)
jsr	pc,*$_sch
L105:jsr	pc,_getch
mov	r0,r4
cmp	$0141,r0
jgt	L10009
cmp	r4,$0172
jle	L10008
L10009:cmp	r4,$0101
jlt	L10010
cmp	r4,$0132
jle	L10008
L10010:cmp	r4,$060
jlt	L10011
cmp	r4,$071
jle	L10008
L10011:cmp	r4,$0137
jne	L106
L10008:mov	r4,(sp)
jsr	pc,*$_sch
jbr	L105
L106:clr	(sp)
jsr	pc,*$_sch
dec	_lp
mov	r2,(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_lookup
tst	(sp)+
mov	r0,-8.(r5)
cmp	r2,$01
jne	L107
cmp	-8.(r5),_defloc
jne	L108
mov	$02,r2
jbr	L109
L108:cmp	-8.(r5),_incloc
jne	L110
mov	$03,r2
jbr	L111
L110:.data
L112:.byte 0125,0156,0144,0145,0146,0151,0156,0145,0144,040,0143,0157,0156,0164,0162,0157,0154,0
.even
.text
mov	$L112,(sp)
jsr	pc,*$_error
L113:cmp	r4,$012
jeq	L114
tst	r4
jeq	L114
jsr	pc,_getch
mov	r0,r4
jbr	L113
L114:mov	r4,r0
jbr	L99
L111:L109:jbr	L115
L107:cmp	r2,$02
jne	L116
mov	-8.(r5),r0
mov	_stringbu,8.(r0)
L117:jsr	pc,_getch
mov	r0,r4
cmp	$012,r0
jeq	L118
tst	r4
jeq	L118
mov	r4,(sp)
jsr	pc,*$_savch
jbr	L117
L118:clr	(sp)
jsr	pc,*$_savch
mov	$01,r0
jbr	L99
L116:L115:jbr	L102
jbr	L119
L104:mov	r4,r0
mov	r0,r3
cmp	$047,r0
jeq	L10012
cmp	r3,$042
jne	L120
L10012:mov	r3,(sp)
jsr	pc,*$_sch
mov	_lp,-12.(r5)
inc	_instring
L121:jsr	pc,_getch
mov	r0,r4
cmp	r3,r0
jeq	L122
cmp	r4,$012
jeq	L122
tst	r4
jeq	L122
mov	r4,(sp)
jsr	pc,*$_sch
cmp	r4,$0134
jne	L123
jsr	pc,_getch
mov	r0,(sp)
jsr	pc,*$_sch
L123:jbr	L121
L122:clr	_instring
cmp	r2,$03
jne	L124
clrb	*_lp
L125:jsr	pc,_getch
mov	r0,r4
cmp	$012,r0
jeq	L126
tst	r4
jeq	L126
jbr	L125
L126:cmp	_ibuf,_ibuf2
jne	L127
.data
L128:.byte 0116,0145,0163,0164,0145,0144,040,047,0151,0156,0143,0154,0165,0144,0145,047,0
.even
.text
mov	$L128,(sp)
jsr	pc,*$_error
L127:mov	_ibuf2,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L129
.data
L130:.byte 0115,0151,0163,0163,0151,0156,0147,040,0146,0151,0154,0145,040,045,0163,0
.even
.text
mov	-12.(r5),(sp)
mov	$L130,-(sp)
jsr	pc,*$_error
tst	(sp)+
jbr	L131
L129:mov	_ibuf2,_ibuf
L131:mov	r4,r0
jbr	L99
L124:L120:L119:mov	r4,(sp)
jsr	pc,*$_sch
jsr	pc,_getch
mov	r0,r4
jbr	L102
L103:clr	(sp)
jsr	pc,*$_sch
cmp	r2,$01
jle	L132
.data
L133:.byte 0103,0157,0156,0164,0162,0157,0154,040,0163,0171,0156,0164,0141,0170,0
.even
.text
mov	$L133,(sp)
jsr	pc,*$_error
L132:mov	r4,r0
jbr	L99
L99:jmp	rretrn
.globl	_error
.text
_error:
jsr	r5,rsave; 2
.data
L135:.byte 045,0144,072,040,0
.even
.text
mov	_lineno,(sp)
mov	$L135,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$012,(sp)
jsr	pc,*$_putchar
inc	_exfail
inc	_cflag
L134:jmp	rretrn
.globl	_sch
.text
_sch:
jsr	r5,rsave; 2
mov	_lp,r4
mov	_line,r0
add	$0302,r0
cmp	r4,r0
jne	L137
.data
L138:.byte 0114,0151,0156,0145,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L138,(sp)
jsr	pc,*$_error
L137:movb	4.(r5),(r4)+
mov	_line,r0
add	$0303,r0
cmp	r4,r0
jlos	L139
mov	_line,r0
add	$0303,r0
mov	r0,r4
L139:mov	r4,_lp
L136:jmp	rretrn
.globl	_savch
.text
_savch:
jsr	r5,rsave; 2
mov	_stringbu,r0
inc	_stringbu
movb	4.(r5),(r0)
L140:jmp	rretrn
.globl	_getch
.text
_getch:
.bss
L142:.=.+2.
.text
jsr	r5,rsave; 2
tst	L142
jeq	L143
mov	L142,r4
clr	L142
mov	r4,r0
jbr	L141
L143:L144:jsr	pc,_getc1
mov	r0,r4
cmp	$057,r0
jne	L145
tst	_instring
jne	L145
jsr	pc,_getc1
mov	r0,L142
cmp	$052,r0
jeq	L146
mov	$057,r0
jbr	L141
L146:clr	L142
L147:jsr	pc,_getc1
mov	r0,r4
L149:mov	r4,r0
jbr	L151
L152:clr	r0
jbr	L141
L153:jsr	pc,_getc1
mov	r0,r4
cmp	$057,r0
jeq	L144
jbr	L149
L154:cmp	_ibuf,_ibuf1
jne	L155
mov	_obuf,(sp)
mov	$012,-(sp)
jsr	pc,*$_putc
tst	(sp)+
inc	_lineno
L155:jbr	L147
jbr	L150
L151:mov	$L10014,r1
mov	r0,L10015
L10016:cmp	r0,(r1)+
jne	L10016
jmp	*L10015-L10014(r1)
.data
L10014:0
12
52
L10015:..
L152
L154
L153
L156
.text
/esw
L150:L156:jbr	L147
L148:L145:mov	r4,r0
jbr	L141
L141:jmp	rretrn
.globl	_getc1
.text
_getc1:
jsr	r5,rsave; 2
mov	_ibuf,(sp)
jsr	pc,*$_getc
mov	r0,r4
jge	L158
cmp	_ibuf,_ibuf2
jne	L158
mov	*_ibuf2,(sp)
jsr	pc,*$_close
mov	_ibuf1,_ibuf
mov	_obuf,(sp)
mov	$012,-(sp)
jsr	pc,*$_putc
tst	(sp)+
jsr	pc,_getc1
mov	r0,r4
L158:tst	r4
jge	L159
clr	r0
jbr	L157
L159:mov	r4,r0
jbr	L157
L157:jmp	rretrn
.globl	_lookup
.text
_lookup:
jsr	r5,rsave; 6
mov	4.(r5),r4
clr	-8.(r5)
L161:movb	(r4)+,r0
mov	r0,-10.(r5)
jeq	L162
add	-10.(r5),-8.(r5)
jbr	L161
L162:mov	-8.(r5),r1
sxt	r0
div	_symsiz,r0
mov	r1,-8.(r5)
mov	-8.(r5),r1
mul	$012,r1
add	_symtab,r1
mov	r1,r2
L163:tstb	(r2)
jeq	L164
mov	r2,r3
mov	4.(r5),r4
L165:cmpb	(r3)+,(r4)
jne	L166
tstb	(r4)+
jeq	L10017
mov	4.(r5),r0
add	$010,r0
cmp	r4,r0
jne	L167
L10017:tst	6.(r5)
jne	L168
mov	r2,(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_subst
tst	(sp)+
L168:mov	r2,r0
jbr	L160
L167:jbr	L165
L166:mov	_symsiz,r1
mul	$012,r1
add	_symtab,r1
mov	r1,r0
mov	r2,r1
add	$012,r2
cmp	r0,r1
jhis	L169
mov	_symtab,r2
L169:jbr	L163
L164:tst	6.(r5)
jeq	L170
mov	4.(r5),r3
mov	r2,r4
L171:mov	r2,r0
add	$010,r0
cmp	r4,r0
jhis	L172
movb	(r3),(r4)+
jeq	L173
inc	r3
L173:jbr	L171
L172:L170:mov	r2,r0
jbr	L160
L160:jmp	rretrn
.globl	_subst
.text
_subst:
jsr	r5,rsave; 2
mov	4.(r5),_lp
mov	6.(r5),r0
mov	8.(r0),r4
jeq	L174
mov	$040,(sp)
jsr	pc,*$_sch
L175:tstb	(r4)
jeq	L176
mov	r4,r0
inc	r4
movb	(r0),r0
mov	r0,(sp)
jsr	pc,*$_sch
jbr	L175
L176:mov	$040,(sp)
jsr	pc,*$_sch
L174:jmp	rretrn
.globl	_getsuf
.text
_getsuf:
jsr	r5,rsave; 2
mov	4.(r5),r3
clr	r4
L178:movb	(r3)+,r2
jeq	L179
cmp	r2,$057
jne	L180
clr	r4
jbr	L181
L180:inc	r4
L181:jbr	L178
L179:sub	$03,r3
cmp	r4,$016
jgt	L182
cmp	r4,$02
jle	L182
cmpb	(r3)+,$056
jne	L182
movb	(r3),r0
jbr	L177
L182:clr	r0
jbr	L177
L177:jmp	rretrn
.globl	_setsuf
.text
_setsuf:
jsr	r5,rsave; 2
mov	4.(r5),r4
L184:tstb	(r4)+
jeq	L185
jbr	L184
L185:movb	6.(r5),-2.(r4)
mov	4.(r5),r0
jbr	L183
L183:jmp	rretrn
.globl	_callsys
.text
_callsys:
jsr	r5,rsave; 6
jsr	pc,_fork
mov	r0,-8.(r5)
jne	L187
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_execv
tst	(sp)+
.data
L188:.byte 0103,0141,0156,047,0164,040,0146,0151,0156,0144,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L188,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
jbr	L189
L187:cmp	-8.(r5),$0177777
jne	L190
.data
L191:.byte 0124,0162,0171,040,0141,0147,0141,0151,0156,012,0
.even
.text
mov	$L191,(sp)
jsr	pc,*$_printf
mov	$01,r0
jbr	L186
L190:L189:L192:mov	r5,r0
add	$0177766,r0
mov	r0,(sp)
jsr	pc,*$_wait
cmp	-8.(r5),r0
jeq	L193
jbr	L192
L193:mov	-10.(r5),r0
bic	$!0377,r0
mov	r0,-8.(r5)
jeq	L194
cmp	-8.(r5),$016
jeq	L194
cmp	-8.(r5),$02
jeq	L195
.data
L196:.byte 0106,0141,0164,0141,0154,040,0145,0162,0162,0157,0162,040,0151,0156,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L196,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L195:jsr	pc,_dexit
L194:mov	-10.(r5),r0
ash	$-010,r0
bic	$!0377,r0
jbr	L186
L186:jmp	rretrn
.globl	_copy
.text
_copy:
jsr	r5,rsave; 2
mov	_tsp,r4
mov	4.(r5),r3
L198:mov	_tsp,r0
inc	_tsp
movb	(r3)+,(r0)
jeq	L199
jbr	L198
L199:mov	r4,r0
jbr	L197
L197:jmp	rretrn
.globl	_nodup
.text
_nodup:
jsr	r5,rsave; 2
mov	6.(r5),r3
mov	r3,(sp)
jsr	pc,*$_getsuf
cmp	$0157,r0
jeq	L201
mov	$01,r0
jbr	L200
L201:L202:mov	4.(r5),r0
add	$02,4.(r5)
mov	(r0),r4
jeq	L203
L204:movb	(r3)+,r2
jeq	L205
cmpb	(r4)+,r2
jne	L205
jbr	L204
L205:tstb	(r4)+
jne	L206
clr	r0
jbr	L200
L206:mov	6.(r5),r3
jbr	L202
L203:mov	$01,r0
jbr	L200
L200:jmp	rretrn
.globl	_cunlink
.text
_cunlink:
jsr	r5,rsave; 2
tst	4.(r5)
jne	L208
clr	r0
jbr	L207
L208:mov	4.(r5),(sp)
jsr	pc,*$_unlink
jbr	L207
L207:jmp	rretrn
