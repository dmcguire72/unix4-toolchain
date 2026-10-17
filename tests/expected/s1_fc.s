.globl	_tmp
.comm	_tmp,02
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
.globl	_cflag
.comm	_cflag,02
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
.globl	_main
.text
_main:
jsr	r5,rsave; 20
clr	r0
mov	r0,-20.(r5)
mov	r0,-12.(r5)
mov	r0,-10.(r5)
mov	r0,-14.(r5)
L2:inc	-14.(r5)
mov	-14.(r5),r0
cmp	4.(r5),r0
jle	L3
mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L4
mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),r0
movb	1.(r0),r0
jbr	L6
L7:jbr	L8
L9:inc	_pflag
L10:inc	_cflag
jbr	L5
jbr	L5
L6:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:143
160
L10002:..
L10
L9
L7
.text
/esw
L5:jbr	L11
L4:L8:mov	-14.(r5),r0
asl	r0
add	6.(r5),r0
mov	(r0),-8.(r5)
mov	-8.(r5),(sp)
jsr	pc,*$_getsuf
cmp	$0146,r0
jne	L12
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
L12:mov	-8.(r5),(sp)
mov	$_llist,-(sp)
jsr	pc,*$_nodup
tst	(sp)+
tst	r0
jeq	L13
mov	-12.(r5),r0
inc	-12.(r5)
asl	r0
mov	-8.(r5),_llist(r0)
mov	-8.(r5),(sp)
jsr	pc,*$_getsuf
cmp	$0157,r0
jne	L14
inc	-20.(r5)
L14:L13:L11:jbr	L2
L3:tst	-10.(r5)
jeq	L15
mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
bit	$01,r0
jne	L16
mov	$_dexit,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L16:clr	-14.(r5)
L17:cmp	-14.(r5),-10.(r5)
jge	L18
cmp	-10.(r5),$01
jle	L20
.data
L21:.byte 045,0163,072,012,0
.even
.text
mov	-14.(r5),r0
asl	r0
mov	_clist(r0),(sp)
mov	$L21,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L20:clr	_tmp
.data
L22:.byte 0146,0143,061,0
.even
.text
mov	$L22,_av
mov	-14.(r5),r0
asl	r0
mov	_clist(r0),(sp)
jsr	pc,*$_expand
mov	r0,2.+_av
tst	_pflag
jne	L19
tst	_exfail
jne	L19
tst	2.+_av
jne	L23
inc	_cflag
jbr	L19
L23:clr	4.+_av
.data
L24:.byte 057,0165,0163,0162,057,0146,0157,0162,0164,057,0146,0143,061,0
.even
.text
mov	$_av,(sp)
mov	$L24,-(sp)
jsr	pc,*$_callsys
tst	(sp)+
mov	r0,-8.(r5)
tst	_tmp
jeq	L25
mov	_tmp,(sp)
jsr	pc,*$_cunlink
L25:tst	-8.(r5)
jeq	L26
inc	_cflag
jbr	L19
L26:.data
L27:.byte 0141,0163,0
.even
.text
mov	$L27,_av
.data
L28:.byte 055,0
.even
.text
mov	$L28,2.+_av
.data
L29:.byte 0146,056,0164,0155,0160,061,0
.even
.text
mov	$L29,4.+_av
clr	6.+_av
.data
L30:.byte 057,0142,0151,0156,057,0141,0163,0
.even
.text
mov	$_av,(sp)
mov	$L30,-(sp)
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
L31:.byte 0141,056,0157,0165,0164,0
.even
.text
.data
L32:.byte 0141,056,0157,0165,0164,0
.even
.text
mov	-8.(r5),(sp)
mov	$L31,-(sp)
jsr	pc,*$_link
tst	(sp)+
tst	r0
jne	L10004
mov	$L32,(sp)
jsr	pc,*$_cunlink
tst	r0
jeq	L33
L10004:.data
L34:.byte 0155,0157,0166,0145,040,0146,0141,0151,0154,0145,0144,072,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L34,-(sp)
jsr	pc,*$_printf
tst	(sp)+
inc	_cflag
L33:L19:inc	-14.(r5)
jbr	L17
L18:L15:tst	_cflag
jne	L35
tst	-12.(r5)
jeq	L35
clr	-14.(r5)
.data
L36:.byte 0154,0144,0
.even
.text
mov	$L36,_av
.data
L37:.byte 055,0170,0
.even
.text
mov	$L37,2.+_av
.data
L38:.byte 057,0154,0151,0142,057,0146,0162,060,056,0157,0
.even
.text
mov	$L38,4.+_av
mov	$03,-16.(r5)
L39:cmp	-14.(r5),-12.(r5)
jge	L40
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	-14.(r5),r1
inc	-14.(r5)
asl	r1
mov	_llist(r1),_av(r0)
jbr	L39
L40:.data
L41:.byte 055,0154,0146,0
.even
.text
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	$L41,_av(r0)
.data
L42:.byte 057,0154,0151,0142,057,0146,0151,0154,0151,0142,056,0141,0
.even
.text
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	$L42,_av(r0)
.data
L43:.byte 055,0154,0
.even
.text
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
mov	$L43,_av(r0)
mov	-16.(r5),r0
inc	-16.(r5)
asl	r0
clr	_av(r0)
.data
L44:.byte 057,0142,0151,0156,057,0154,0144,0
.even
.text
mov	$_av,(sp)
mov	$L44,-(sp)
jsr	pc,*$_callsys
tst	(sp)+
cmp	-10.(r5),$01
jne	L45
cmp	-20.(r5),$01
jne	L45
mov	$0157,(sp)
mov	_clist,-(sp)
jsr	pc,*$_setsuf
tst	(sp)+
mov	r0,(sp)
jsr	pc,*$_cunlink
L45:L35:jsr	pc,_dexit
L1:jmp	rretrn
.globl	_dexit
.text
_dexit:
jsr	r5,rsave; 2
.data
L47:.byte 0146,056,0164,0155,0160,061,0
.even
.text
mov	$L47,(sp)
jsr	pc,*$_unlink
jsr	pc,_exit
L46:jmp	rretrn
.globl	_expand
.text
_expand:
jsr	r5,rsave; 11252
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
jge	L49
mov	4.(r5),r0
jbr	L48
L49:mov	_ibuf1,(sp)
jsr	pc,*$_getc
cmp	$043,r0
jeq	L50
mov	*_ibuf1,(sp)
jsr	pc,*$_close
mov	4.(r5),r0
jbr	L48
L50:mov	_ibuf1,r0
inc	2.(r0)
mov	_ibuf1,r0
dec	4.(r0)
mov	r5,r0
add	$0174750,r0
mov	r0,_obuf
mov	r5,r0
add	$0171030,r0
mov	r0,_symtab
clr	-4782.(r5)
L51:cmp	-4782.(r5),$0310
jge	L52
mov	-4782.(r5),r1
mul	$012,r1
add	r5,r1
clrb	-3560.(r1)
mov	-4782.(r5),r1
mul	$012,r1
add	r5,r1
clr	-3552.(r1)
L53:inc	-4782.(r5)
jbr	L51
L52:.data
L54:.byte 0144,0145,0146,0151,0156,0145,0
.even
.text
mov	$01,(sp)
mov	$L54,-(sp)
jsr	pc,*$_lookup
tst	(sp)+
mov	r0,_defloc
mov	_defloc,r0
mov	_defloc,8.(r0)
.data
L55:.byte 0151,0156,0143,0154,0165,0144,0145,0
.even
.text
mov	$01,(sp)
mov	$L55,-(sp)
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
mov	$0151,(sp)
mov	4.(r5),-(sp)
jsr	pc,_copy
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_setsuf
tst	(sp)+
mov	r0,_tmp
mov	_obuf,(sp)
mov	_tmp,-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jge	L56
.data
L57:.byte 0103,0141,0156,047,0164,040,0143,0162,0145,0141,0164,040,045,0163,012,0
.even
.text
mov	_tmp,(sp)
mov	$L57,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_dexit
L56:L58:jsr	pc,_getline
tst	r0
jeq	L59
cmp	_ibuf,_ibuf2
jne	L60
mov	_obuf,(sp)
mov	$01,-(sp)
jsr	pc,*$_putc
tst	(sp)+
L60:cmpb	-3756.(r5),$043
jeq	L61
mov	_line,_lp
L62:tstb	*_lp
jeq	L63
mov	_obuf,(sp)
movb	*_lp,r0
mov	r0,-(sp)
jsr	pc,*$_putc
tst	(sp)+
L64:inc	_lp
jbr	L62
L63:L61:mov	_obuf,(sp)
mov	$012,-(sp)
jsr	pc,*$_putc
tst	(sp)+
jbr	L58
L59:mov	_obuf,(sp)
jsr	pc,*$_fflush
mov	*_obuf,(sp)
jsr	pc,*$_close
mov	*_ibuf1,(sp)
jsr	pc,*$_close
mov	_tmp,r0
jbr	L48
L48:jmp	rretrn
.globl	_getline
.text
_getline:
jsr	r5,rsave; 16
cmp	_ibuf,_ibuf1
jne	L66
inc	_lineno
L66:mov	_line,_lp
clrb	*_lp
clr	-12.(r5)
jsr	pc,_getch
mov	r0,-8.(r5)
cmp	$043,r0
jne	L67
mov	$01,-12.(r5)
L67:L68:cmp	-8.(r5),$012
jeq	L69
tst	-8.(r5)
jeq	L69
cmp	-8.(r5),$0141
jlt	L10006
cmp	-8.(r5),$0172
jle	L10005
L10006:cmp	-8.(r5),$0101
jlt	L10007
cmp	-8.(r5),$0132
jle	L10005
L10007:cmp	-8.(r5),$0137
jne	L70
L10005:mov	_lp,-16.(r5)
mov	-8.(r5),(sp)
jsr	pc,*$_sch
L71:jsr	pc,_getch
mov	r0,-8.(r5)
cmp	$0141,r0
jgt	L10009
cmp	-8.(r5),$0172
jle	L10008
L10009:cmp	-8.(r5),$0101
jlt	L10010
cmp	-8.(r5),$0132
jle	L10008
L10010:cmp	-8.(r5),$060
jlt	L10011
cmp	-8.(r5),$071
jle	L10008
L10011:cmp	-8.(r5),$0137
jne	L72
L10008:mov	-8.(r5),(sp)
jsr	pc,*$_sch
jbr	L71
L72:clr	(sp)
jsr	pc,*$_sch
dec	_lp
mov	-12.(r5),(sp)
mov	-16.(r5),-(sp)
jsr	pc,*$_lookup
tst	(sp)+
mov	r0,-14.(r5)
cmp	-12.(r5),$01
jne	L73
cmp	-14.(r5),_defloc
jne	L74
mov	$02,-12.(r5)
jbr	L75
L74:cmp	-14.(r5),_incloc
jne	L76
mov	$03,-12.(r5)
jbr	L77
L76:.data
L78:.byte 0125,0156,0144,0145,0146,0151,0156,0145,0144,040,0143,0157,0156,0164,0162,0157,0154,0
.even
.text
mov	$L78,(sp)
jsr	pc,*$_error
L79:cmp	-8.(r5),$012
jeq	L80
tst	-8.(r5)
jeq	L80
jsr	pc,_getch
mov	r0,-8.(r5)
jbr	L79
L80:mov	-8.(r5),r0
jbr	L65
L77:L75:jbr	L81
L73:cmp	-12.(r5),$02
jne	L82
mov	-14.(r5),r0
mov	_stringbu,8.(r0)
L83:jsr	pc,_getch
mov	r0,-8.(r5)
cmp	$012,r0
jeq	L84
tst	-8.(r5)
jeq	L84
mov	-8.(r5),(sp)
jsr	pc,*$_savch
jbr	L83
L84:clr	(sp)
jsr	pc,*$_savch
mov	$01,r0
jbr	L65
L82:L81:jbr	L68
jbr	L85
L70:mov	-8.(r5),r0
mov	r0,-10.(r5)
cmp	$047,r0
jeq	L10012
cmp	-10.(r5),$042
jne	L86
L10012:mov	-10.(r5),(sp)
jsr	pc,*$_sch
mov	_lp,-18.(r5)
inc	_instring
L87:jsr	pc,_getch
mov	r0,-8.(r5)
cmp	-10.(r5),r0
jeq	L88
cmp	-8.(r5),$012
jeq	L88
tst	-8.(r5)
jeq	L88
mov	-8.(r5),(sp)
jsr	pc,*$_sch
cmp	-8.(r5),$0134
jne	L89
jsr	pc,_getch
mov	r0,(sp)
jsr	pc,*$_sch
L89:jbr	L87
L88:clr	_instring
cmp	-12.(r5),$03
jne	L90
clrb	*_lp
L91:jsr	pc,_getch
mov	r0,-8.(r5)
cmp	$012,r0
jeq	L92
tst	-8.(r5)
jeq	L92
jbr	L91
L92:cmp	_ibuf,_ibuf2
jne	L93
.data
L94:.byte 0116,0145,0163,0164,0145,0144,040,047,0151,0156,0143,0154,0165,0144,0145,047,0
.even
.text
mov	$L94,(sp)
jsr	pc,*$_error
L93:mov	_ibuf2,(sp)
mov	-18.(r5),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L95
.data
L96:.byte 0115,0151,0163,0163,0151,0156,0147,040,0146,0151,0154,0145,040,045,0163,0
.even
.text
mov	-18.(r5),(sp)
mov	$L96,-(sp)
jsr	pc,*$_error
tst	(sp)+
jbr	L97
L95:mov	_ibuf2,_ibuf
L97:mov	-8.(r5),r0
jbr	L65
L90:L86:L85:mov	-8.(r5),(sp)
jsr	pc,*$_sch
jsr	pc,_getch
mov	r0,-8.(r5)
jbr	L68
L69:clr	(sp)
jsr	pc,*$_sch
cmp	-12.(r5),$01
jle	L98
.data
L99:.byte 0103,0157,0156,0164,0162,0157,0154,040,0163,0171,0156,0164,0141,0170,0
.even
.text
mov	$L99,(sp)
jsr	pc,*$_error
L98:mov	-8.(r5),r0
jbr	L65
L65:jmp	rretrn
.globl	_error
.text
_error:
jsr	r5,rsave; 2
.data
L101:.byte 045,0144,072,040,0
.even
.text
mov	_lineno,(sp)
mov	$L101,-(sp)
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
L100:jmp	rretrn
.globl	_sch
.text
_sch:
jsr	r5,rsave; 2
mov	_line,r0
add	$0302,r0
cmp	_lp,r0
jne	L103
.data
L104:.byte 0114,0151,0156,0145,040,0157,0166,0145,0162,0146,0154,0157,0167,0
.even
.text
mov	$L104,(sp)
jsr	pc,*$_error
L103:mov	_lp,r0
inc	_lp
movb	4.(r5),(r0)
mov	_line,r0
add	$0303,r0
cmp	_lp,r0
jlos	L105
mov	_line,r0
add	$0303,r0
mov	r0,_lp
L105:L102:jmp	rretrn
.globl	_savch
.text
_savch:
jsr	r5,rsave; 2
mov	_stringbu,r0
inc	_stringbu
movb	4.(r5),(r0)
L106:jmp	rretrn
.globl	_getch
.text
_getch:
.bss
L108:.=.+2.
.text
jsr	r5,rsave; 4
tst	L108
jeq	L109
mov	L108,-8.(r5)
clr	L108
mov	-8.(r5),r0
jbr	L107
L109:L110:jsr	pc,_getc1
mov	r0,-8.(r5)
cmp	$057,r0
jne	L111
tst	_instring
jne	L111
jsr	pc,_getc1
mov	r0,L108
cmp	$052,r0
jeq	L112
mov	$057,r0
jbr	L107
L112:clr	L108
L113:jsr	pc,_getc1
mov	r0,-8.(r5)
L115:mov	-8.(r5),r0
jbr	L117
L118:clr	r0
jbr	L107
L119:jsr	pc,_getc1
mov	r0,-8.(r5)
cmp	$057,r0
jeq	L110
jbr	L115
L120:cmp	_ibuf,_ibuf1
jne	L121
mov	_obuf,(sp)
mov	$012,-(sp)
jsr	pc,*$_putc
tst	(sp)+
inc	_lineno
L121:jbr	L113
jbr	L116
L117:mov	$L10014,r1
mov	r0,L10015
L10016:cmp	r0,(r1)+
jne	L10016
jmp	*L10015-L10014(r1)
.data
L10014:0
12
52
L10015:..
L118
L120
L119
L122
.text
/esw
L116:L122:jbr	L113
L114:L111:mov	-8.(r5),r0
jbr	L107
L107:jmp	rretrn
.globl	_getc1
.text
_getc1:
jsr	r5,rsave; 4
mov	_ibuf,(sp)
jsr	pc,*$_getc
mov	r0,-8.(r5)
jge	L124
cmp	_ibuf,_ibuf2
jne	L124
mov	*_ibuf2,(sp)
jsr	pc,*$_close
mov	_ibuf1,_ibuf
mov	_obuf,(sp)
mov	$012,-(sp)
jsr	pc,*$_putc
tst	(sp)+
jsr	pc,_getc1
mov	r0,-8.(r5)
L124:tst	-8.(r5)
jge	L125
clr	r0
jbr	L123
L125:mov	-8.(r5),r0
jbr	L123
L123:jmp	rretrn
.globl	_lookup
.text
_lookup:
jsr	r5,rsave; 14
mov	4.(r5),-8.(r5)
clr	-14.(r5)
L127:mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
mov	r0,-16.(r5)
jeq	L128
add	-16.(r5),-14.(r5)
jbr	L127
L128:mov	-14.(r5),r1
sxt	r0
div	_symsiz,r0
mov	r1,-14.(r5)
mov	-14.(r5),r1
mul	$012,r1
add	_symtab,r1
mov	r1,-12.(r5)
L129:tstb	*-12.(r5)
jeq	L130
mov	-12.(r5),-10.(r5)
mov	4.(r5),-8.(r5)
L131:mov	-10.(r5),r0
inc	-10.(r5)
cmpb	(r0),*-8.(r5)
jne	L132
mov	-8.(r5),r0
inc	-8.(r5)
tstb	(r0)
jeq	L10017
mov	4.(r5),r0
add	$010,r0
cmp	-8.(r5),r0
jne	L133
L10017:tst	6.(r5)
jne	L134
mov	-12.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_subst
tst	(sp)+
L134:mov	-12.(r5),r0
jbr	L126
L133:jbr	L131
L132:mov	_symsiz,r1
mul	$012,r1
add	_symtab,r1
mov	-12.(r5),r2
add	$012,-12.(r5)
cmp	r1,r2
jhis	L135
mov	_symtab,-12.(r5)
L135:jbr	L129
L130:tst	6.(r5)
jeq	L136
clr	-14.(r5)
L137:cmp	-14.(r5),$010
jge	L138
mov	-12.(r5),r0
add	-14.(r5),r0
movb	*4.(r5),(r0)
jeq	L140
inc	4.(r5)
L140:L139:inc	-14.(r5)
jbr	L137
L138:L141:tstb	*4.(r5)
jeq	L142
inc	4.(r5)
jbr	L141
L142:L136:mov	-12.(r5),r0
jbr	L126
L126:jmp	rretrn
.globl	_subst
.text
_subst:
jsr	r5,rsave; 4
mov	4.(r5),_lp
mov	6.(r5),r0
mov	8.(r0),-8.(r5)
jeq	L143
mov	$040,(sp)
jsr	pc,*$_sch
L144:tstb	*-8.(r5)
jeq	L145
mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
mov	r0,(sp)
jsr	pc,*$_sch
jbr	L144
L145:mov	$040,(sp)
jsr	pc,*$_sch
L143:jmp	rretrn
.globl	_getsuf
.text
_getsuf:
jsr	r5,rsave; 10
clr	-8.(r5)
mov	4.(r5),-12.(r5)
L147:mov	4.(r5),r0
inc	4.(r5)
movb	(r0),-10.(r5)
jeq	L148
cmpb	-10.(r5),$057
jne	L149
clr	-8.(r5)
jbr	L150
L149:inc	-8.(r5)
L150:jbr	L147
L148:sub	$03,4.(r5)
cmp	-8.(r5),$016
jgt	L151
cmp	-8.(r5),$02
jle	L151
mov	4.(r5),r0
inc	4.(r5)
movb	(r0),r0
cmp	$056,r0
jne	L151
movb	*4.(r5),r0
jbr	L146
L151:clr	r0
jbr	L146
L146:jmp	rretrn
.globl	_setsuf
.text
_setsuf:
jsr	r5,rsave; 4
mov	4.(r5),-8.(r5)
L153:mov	4.(r5),r0
inc	4.(r5)
tstb	(r0)
jeq	L154
jbr	L153
L154:mov	4.(r5),r0
movb	6.(r5),-2.(r0)
mov	-8.(r5),r0
jbr	L152
L152:jmp	rretrn
.globl	_callsys
.text
_callsys:
jsr	r5,rsave; 6
jsr	pc,_fork
mov	r0,-8.(r5)
jne	L156
mov	6.(r5),(sp)
mov	4.(r5),-(sp)
jsr	pc,*$_execv
tst	(sp)+
.data
L157:.byte 0103,0141,0156,047,0164,040,0146,0151,0156,0144,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L157,-(sp)
jsr	pc,*$_printf
tst	(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
jbr	L158
L156:cmp	-8.(r5),$0177777
jne	L159
.data
L160:.byte 0124,0162,0171,040,0141,0147,0141,0151,0156,012,0
.even
.text
mov	$L160,(sp)
jsr	pc,*$_printf
mov	$01,r0
jbr	L155
L159:L158:L161:mov	r5,r0
add	$0177766,r0
mov	r0,(sp)
jsr	pc,*$_wait
cmp	-8.(r5),r0
jeq	L162
jbr	L161
L162:mov	-10.(r5),r0
bic	$!0377,r0
mov	r0,-8.(r5)
jeq	L163
cmp	-8.(r5),$016
jeq	L163
cmp	-8.(r5),$02
jeq	L164
.data
L165:.byte 0106,0141,0164,0141,0154,040,0145,0162,0162,0157,0162,040,0151,0156,040,045,0163,012,0
.even
.text
mov	4.(r5),(sp)
mov	$L165,-(sp)
jsr	pc,*$_printf
tst	(sp)+
L164:jsr	pc,_dexit
L163:mov	-10.(r5),r0
ash	$-010,r0
bic	$!0377,r0
jbr	L155
L155:jmp	rretrn
.globl	_copy
.text
_copy:
jsr	r5,rsave; 4
mov	_tsp,-8.(r5)
L167:mov	_tsp,r0
inc	_tsp
mov	4.(r5),r1
inc	4.(r5)
movb	(r1),(r0)
jeq	L168
jbr	L167
L168:mov	-8.(r5),r0
jbr	L166
L166:jmp	rretrn
.globl	_nodup
.text
_nodup:
jsr	r5,rsave; 10
mov	6.(r5),(sp)
jsr	pc,*$_getsuf
cmp	$0157,r0
jeq	L170
mov	$01,r0
jbr	L169
L170:mov	6.(r5),-10.(r5)
L171:mov	4.(r5),r0
add	$02,4.(r5)
mov	(r0),-8.(r5)
jeq	L172
mov	-10.(r5),6.(r5)
L173:mov	6.(r5),r0
inc	6.(r5)
movb	(r0),-12.(r5)
jeq	L174
movb	-12.(r5),r0
mov	-8.(r5),r1
inc	-8.(r5)
movb	(r1),r1
cmp	r0,r1
jne	L174
jbr	L173
L174:mov	-8.(r5),r0
inc	-8.(r5)
tstb	(r0)
jne	L175
clr	r0
jbr	L169
L175:jbr	L171
L172:mov	$01,r0
jbr	L169
L169:jmp	rretrn
.globl	_cunlink
.text
_cunlink:
jsr	r5,rsave; 2
tst	4.(r5)
jne	L177
clr	r0
jbr	L176
L177:mov	4.(r5),(sp)
jsr	pc,*$_unlink
jbr	L176
L176:jmp	rretrn
