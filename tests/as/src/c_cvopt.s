.globl	_nofloat
.comm	_nofloat,02
.globl	_peekc
.comm	_peekc,02
.globl	_main
.text
_main:
jsr	r5,rsave; 20
clr	r0
mov	r0,-20.(r5)
mov	r0,-10.(r5)
mov	r0,-12.(r5)
mov	r0,-16.(r5)
cmp	4.(r5),$01
jle	L2
clr	(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_fin
jge	L3
mov	$05077,(sp)
jsr	pc,*$_putchar
jbr	L1
L3:L2:cmp	4.(r5),$02
jle	L4
mov	$0666,(sp)
mov	6.(r5),r0
mov	4.(r0),-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,_fout
jge	L5
mov	$05077,(sp)
jsr	pc,*$_putchar
jbr	L1
L5:L4:L6:jsr	pc,_getc
mov	r0,-8.(r5)
cmp	-8.(r5),$012
jeq	L7
cmp	-8.(r5),$011
jeq	L7
clr	-12.(r5)
L7:tst	-20.(r5)
jeq	L8
cmp	-8.(r5),$045
jeq	L8
clr	-20.(r5)
.data
L9:.byte 056,0144,0141,0164,0141,012,061,072,074,0
.even
.text
mov	$L9,(sp)
jsr	pc,*$_printf
L8:mov	-8.(r5),r0
jbr	L11
L12:.data
L13:.byte 056,0164,0145,0170,0164,073,040,060,012,0
.even
.text
mov	$L13,(sp)
jsr	pc,*$_printf
jsr	pc,_flush
jbr	L1
L14:tst	-16.(r5)
jne	L15
.data
L16:.byte 075,056,053,062,073,040,060,0
.even
.text
mov	$L16,(sp)
jsr	pc,*$_printf
jbr	L17
L15:mov	$072,(sp)
jsr	pc,*$_putchar
L17:jbr	L6
L18:jsr	pc,_getc
mov	r0,-8.(r5)
cmp	$061,r0
jeq	L10000
cmp	-8.(r5),$062
jne	L19
L10000:mov	-8.(r5),(sp)
add	$020,(sp)
jsr	pc,*$_putchar
jbr	L6
L19:mov	$0117,(sp)
jsr	pc,*$_putchar
mov	-8.(r5),_peekc
jbr	L6
L20:jsr	pc,_getc
jbr	L22
L23:mov	$0103,(sp)
jsr	pc,*$_putchar
jbr	L6
L24:mov	$0104,(sp)
jsr	pc,*$_putchar
jbr	L6
L25:mov	$0114,(sp)
jsr	pc,*$_putchar
jbr	L6
L26:mov	$0120,(sp)
jsr	pc,*$_putchar
jbr	L6
jbr	L21
L22:mov	$L10002,r1
mov	r0,L10003
L10004:cmp	r0,(r1)+
jne	L10004
jmp	*L10003-L10002(r1)
.data
L10002:61
62
105
106
L10003:..
L23
L24
L25
L26
L27
.text
/esw
L21:L27:mov	$077,(sp)
jsr	pc,*$_putchar
jbr	L6
L28:jsr	pc,_getc
mov	r0,(sp)
add	$024,(sp)
jsr	pc,*$_putchar
jbr	L6
L29:mov	$0107,(sp)
jsr	pc,*$_putchar
jbr	L30
L31:jsr	pc,_getc
mov	r0,-8.(r5)
cmp	$061,r0
jne	L32
mov	$0112,(sp)
jsr	pc,*$_putchar
jbr	L33
L32:mov	$0111,(sp)
jsr	pc,*$_putchar
mov	-8.(r5),_peekc
L33:jbr	L6
L34:mov	$0110,(sp)
jsr	pc,*$_putchar
jbr	L30
L35:mov	$0115,(sp)
jsr	pc,*$_putchar
jbr	L6
L36:mov	$0126,(sp)
jsr	pc,*$_putchar
inc	-10.(r5)
jbr	L6
L37:mov	$0113,(sp)
jsr	pc,*$_putchar
L30:mov	$01,-10.(r5)
mov	$0101,-14.(r5)
L38:jsr	pc,_getc
mov	r0,-8.(r5)
jbr	L40
L41:inc	-14.(r5)
jbr	L38
L42:add	$02,-14.(r5)
jbr	L38
L43:add	$04,-14.(r5)
jbr	L38
L44:add	$010,-14.(r5)
jbr	L38
L45:add	$020,-14.(r5)
jbr	L38
jbr	L39
L40:mov	$L10006,r1
mov	r0,L10007
L10008:cmp	r0,(r1)+
jne	L10008
jmp	*L10007-L10006(r1)
.data
L10006:52
61
62
103
123
L10007:..
L41
L44
L45
L43
L42
L46
.text
/esw
L39:L46:mov	-8.(r5),_peekc
mov	-14.(r5),(sp)
jsr	pc,*$_putchar
jbr	L6
L47:jsr	pc,_getc
cmp	$061,r0
jne	L48
mov	$043,(sp)
jsr	pc,*$_putchar
jbr	L49
L48:mov	$042,(sp)
jsr	pc,*$_putchar
L49:jbr	L6
L50:tst	-16.(r5)
jeq	L51
.data
L52:.byte 056,0164,0145,0170,0164,073,0
.even
.text
mov	$L52,(sp)
jsr	pc,*$_printf
L51:L53:jsr	pc,_getc
mov	r0,-8.(r5)
jbr	L55
L56:mov	$020,-18.(r5)
jsr	pc,_flag
mov	r0,-14.(r5)
jbr	L57
L58:mov	$073,(sp)
jsr	pc,*$_putchar
jbr	L53
L59:mov	$014,-18.(r5)
jsr	pc,_flag
mov	r0,-14.(r5)
jbr	L57
L60:mov	$04,-18.(r5)
jsr	pc,_flag
mov	r0,-14.(r5)
jbr	L57
L61:mov	$05,-18.(r5)
jsr	pc,_flag
mov	r0,-14.(r5)
jbr	L57
L62:clr	-14.(r5)
mov	$010,-18.(r5)
jbr	L57
L63:jsr	pc,_flag
mov	r0,-14.(r5)
mov	$024,-18.(r5)
jbr	L57
L64:jsr	pc,_flag
mov	r0,-14.(r5)
mov	$077,-18.(r5)
L57:jsr	pc,_getc
mov	r0,-8.(r5)
cmp	$052,r0
jne	L65
add	$0100,-18.(r5)
jbr	L66
L65:mov	-8.(r5),_peekc
L66:.data
L67:.byte 056,0142,0171,0164,0145,040,045,0157,054,045,0157,0
.even
.text
mov	-14.(r5),(sp)
mov	-18.(r5),-(sp)
mov	$L67,-(sp)
jsr	pc,*$_printf
cmp	(sp)+,(sp)+
jbr	L53
L68:.data
L69:.byte 073,061,0146,012,0
.even
.text
mov	$L69,(sp)
jsr	pc,*$_printf
mov	$01,-20.(r5)
mov	$01,-12.(r5)
mov	$01,-16.(r5)
jbr	L6
jbr	L54
L55:jsr	r2,hsw; 2; L10010
jmp	*L10013-L10011(r1)
.data
L10010:L10011
L10012
L10013
L10011:..
5
26
67
75
L10012:..
30
60
61
62
64
L10013:L70
L68
L58
L64
L60
L70
L61
L56
L62
L63
L59
.text
/esw
L54:L70:mov	-8.(r5),(sp)
jsr	pc,*$_putchar
jbr	L53
L71:tst	-12.(r5)
jeq	L72
clr	-12.(r5)
jbr	L6
L72:mov	$011,(sp)
jsr	pc,*$_putchar
jbr	L6
L73:tst	-16.(r5)
jne	L74
mov	$012,(sp)
jsr	pc,*$_putchar
jbr	L6
L74:tst	-12.(r5)
jeq	L75
clr	-12.(r5)
.data
L76:.byte 0134,060,076,012,056,0164,0145,0170,0164,012,0
.even
.text
mov	$L76,(sp)
jsr	pc,*$_printf
clr	-16.(r5)
jbr	L6
L75:tst	-10.(r5)
jne	L77
.data
L78:.byte 0134,0156,0
.even
.text
mov	$L78,(sp)
jsr	pc,*$_printf
L77:clr	-10.(r5)
.data
L79:.byte 076,012,074,0
.even
.text
mov	$L79,(sp)
jsr	pc,*$_printf
mov	$01,-12.(r5)
jbr	L6
L80:L81:L82:inc	-10.(r5)
jbr	L10
L11:jsr	r2,hsw; 4; L10015
jmp	*L10020-L10016(r1)
.data
L10015:L10016
L10017
L10018
L10019
L10020
L10016:..
0
22
25
26
L10017:..
2
11
20
22
26
L10018:..
2
16
20
21
24
25
L10019:..
10
20
24
L10020:L83
L12
L34
L82
L80
L83
L71
L50
L18
L35
L81
L83
L73
L14
L20
L29
L31
L36
L83
L47
L28
L37
.text
/esw
L10:L83:mov	-8.(r5),(sp)
jsr	pc,*$_putchar
jbr	L6
L1:jmp	rretrn
.globl	_getc
.text
_getc:
jsr	r5,rsave; 6
clr	-10.(r5)
L85:tst	_peekc
jeq	L86
mov	_peekc,-8.(r5)
clr	_peekc
jbr	L87
L86:jsr	pc,_getchar
mov	r0,-8.(r5)
L87:tst	-8.(r5)
jne	L88
clr	r0
jbr	L84
L88:cmp	-8.(r5),$0173
jne	L89
inc	-10.(r5)
jsr	pc,_getchar
mov	r0,-8.(r5)
L89:cmp	-8.(r5),$0175
jne	L90
jsr	pc,_getc
mov	r0,-8.(r5)
dec	-10.(r5)
jne	L91
cmp	-8.(r5),$012
jne	L92
jsr	pc,_getc
mov	r0,-8.(r5)
L92:L91:L90:tst	-10.(r5)
jeq	L10021
tst	_nofloat
jne	L85
L10021:mov	-8.(r5),r0
jbr	L84
L84:jmp	rretrn
.globl	_flag
.text
_flag:
jsr	r5,rsave; 2
clr	r3
L94:jsr	pc,_getc
mov	r0,r4
jbr	L96
L97:mov	$01,r3
jbr	L94
L98:mov	$02,r3
jbr	L94
L99:mov	$03,r3
jbr	L94
L100:mov	$04,r3
jbr	L94
L101:mov	$05,r3
jbr	L94
L102:mov	$06,r3
jbr	L94
L103:add	$020,r3
jbr	L94
jbr	L95
L96:sub	$0142,r0
cmp	r0,$025
jhi	L104
asl	r0
jmp	*.+4(r0)
L99
L104
L101
L104
L100
L104
L104
L98
L104
L104
L104
L104
L104
L104
L103
L104
L104
L102
L104
L104
L104
L97
/esw
L95:L104:mov	r4,_peekc
mov	r3,r0
jbr	L93
L93:jmp	rretrn
