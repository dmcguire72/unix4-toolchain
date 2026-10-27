.globl	_ibuf
.comm	_ibuf,01006
.globl	_obuf
.comm	_obuf,01006
.globl	_file
.comm	_file,02
.globl	_filep
.comm	_filep,02
.globl	_nfiles
.comm	_nfiles,02
.globl	_nlines
.comm	_nlines,02
.globl	_ntext
.comm	_ntext,02
.globl	_lspace
.comm	_lspace,02
.globl	_tspace
.comm	_tspace,02
.globl	_aflg
.comm	_aflg,02
.globl	_dflg
.comm	_dflg,02
.globl	_mflg
.comm	_mflg,02
.globl	_nflg
.comm	_nflg,02
.globl	_rflg
.data
_rflg=.
1.
.globl	_sfield
.data
_sfield=.
-1.
.globl	_schar
.comm	_schar,02
.globl	_cmp
.comm	_cmp,00
.globl	_map
.comm	_map,0400
.globl	_term
.comm	_term,00
.globl	_outfil
.comm	_outfil,02
.globl	_eargc
.comm	_eargc,02
.globl	_eargv
.comm	_eargv,02
.globl	_main
.text
_main:
jsr	r5,rsave; 4
mov	6.(r5),_eargv
L2:dec	4.(r5)
jle	L3
add	$02,6.(r5)
mov	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L4
mov	*6.(r5),-8.(r5)
L5:inc	-8.(r5)
mov	-8.(r5),r0
movb	(r0),r0
jbr	L8
L9:mov	-8.(r5),r0
movb	-1.(r0),r0
cmp	$055,r0
jne	L10
.data
L11:.byte 055,0
.even
.text
mov	_eargc,r0
inc	_eargc
asl	r0
add	_eargv,r0
mov	$L11,(r0)
L10:jbr	L7
L12:inc	_aflg
jbr	L5
L13:inc	_dflg
jbr	L5
L14:inc	_mflg
jbr	L5
L15:inc	_nflg
jbr	L5
L16:dec	4.(r5)
jle	L17
add	$02,6.(r5)
mov	6.(r5),r0
mov	(r0),_outfil
L17:jbr	L5
L18:mov	$0177777,_rflg
jbr	L5
L19:inc	*6.(r5)
mov	*6.(r5),(sp)
jsr	pc,*$_number
mov	r0,_sfield
jbr	L7
jbr	L7
L8:mov	$L10001,r1
mov	r0,L10002
L10003:cmp	r0,(r1)+
jne	L10003
jmp	*L10002-L10001(r1)
.data
L10001:0
141
144
155
156
157
162
L10002:..
L9
L12
L13
L14
L15
L16
L18
L19
.text
/esw
L7:jbr	L6
jbr	L5
L6:jbr	L20
L4:mov	6.(r5),r0
movb	*(r0),r0
cmp	$053,r0
jne	L21
inc	*6.(r5)
mov	*6.(r5),(sp)
jsr	pc,*$_number
mov	r0,_schar
jbr	L22
L21:mov	_eargc,r0
inc	_eargc
asl	r0
add	_eargv,r0
mov	*6.(r5),(r0)
L22:L20:jbr	L2
L3:tst	_eargc
jne	L23
.data
L24:.byte 055,0
.even
.text
mov	_eargc,r0
inc	_eargc
asl	r0
add	_eargv,r0
mov	$L24,(r0)
L23:clr	r4
L25:cmp	r4,$0200
jge	L26
movb	r4,128.+_map(r4)
L27:inc	r4
jbr	L25
L26:tst	_aflg
jne	L28
mov	$0141,r4
L29:cmp	r4,$0172
jgt	L30
movb	96.+_map(r4),128.+_map(r4)
L31:inc	r4
jbr	L29
L30:L28:clrb	138.+_map
tst	_rflg
jge	L32
clr	r4
L33:cmp	r4,$0200
jge	L34
mov	$0177,r0
movb	128.+_map(r4),r1
sub	r1,r0
movb	r0,128.+_map(r4)
L35:inc	r4
jbr	L33
L34:L32:tst	_dflg
jeq	L36
clr	r4
L37:cmp	r4,$060
jge	L38
cmp	r4,$012
jeq	L40
cmp	r4,$040
jeq	L40
cmp	r4,$011
jeq	L40
movb	$0177777,128.+_map(r4)
L40:L39:inc	r4
jbr	L37
L38:mov	$0100,(sp)
mov	$072,-(sp)
jsr	pc,*$_ignore
tst	(sp)+
mov	$0140,(sp)
mov	$0133,-(sp)
jsr	pc,*$_ignore
tst	(sp)+
mov	$0177,(sp)
mov	$0173,-(sp)
jsr	pc,*$_ignore
tst	(sp)+
L36:clr	(sp)
mov	$0177600,-(sp)
jsr	pc,*$_ignore
tst	(sp)+
movb	$0177777,255.+_map
mov	$0100000,r4
L41:mov	r4,(sp)
jsr	pc,*$_brk
cmp	$0177777,r0
jne	L42
jbr	L41
L42:sub	$01000,r4
sub	$01000,r4
mov	r4,r0
mov	r0,(sp)
jsr	pc,*$_brk
mov	$_end,_lspace
sub	$_end,r4
mov	r4,r0
add	$0177000,r0
asr	r0
bic	$!077777,r0
mov	r0,_nlines
mov	_nlines,r1
sxt	r0
div	$05,r0
mov	r0,_nlines
mov	_nlines,r0
ash	$03,r0
mov	r0,_ntext
mov	_nlines,r0
asl	r0
add	_lspace,r0
mov	r0,_tspace
.data
L43:.byte 057,0165,0163,0162,057,0164,0155,0160,057,0163,0164,0155,0130,0141,0141,0
.even
.text
mov	$L43,_file
L44:mov	_file,_filep
L45:cmpb	*_filep,$0130
jeq	L46
inc	_filep
jbr	L45
L46:movb	$0141,*_filep
L47:mov	_lspace,(sp)
mov	_file,-(sp)
jsr	pc,*$_stat
tst	(sp)+
tst	r0
jge	L50
mov	$0600,(sp)
mov	_file,-(sp)
jsr	pc,*$_creat
tst	(sp)+
mov	r0,r4
tst	r4
jge	L48
L50:cmpb	*_filep,$0172
jne	L51
mov	_file,r0
movb	1.(r0),r0
cmp	$0164,r0
jeq	L52
.data
L53:.byte 057,0164,0155,0160,057,0163,0164,0155,0130,0141,0141,0
.even
.text
mov	$L53,_file
jbr	L44
L52:.data
L54:.byte 0103,0141,0156,0156,0157,0164,040,0154,0157,0143,0141,0164,0145,040,0164,0145,0155,0160,012,0
.even
.text
mov	$L54,(sp)
jsr	pc,*$_mess
jsr	pc,_exit
L51:L49:incb	*_filep
jbr	L47
L48:mov	r4,(sp)
jsr	pc,*$_close
inc	_filep
mov	$01,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
bit	$01,r0
jne	L55
mov	$_term,(sp)
mov	$02,-(sp)
jsr	pc,*$_signal
tst	(sp)+
L55:mov	_eargc,_nfiles
tst	_mflg
jne	L56
mov	$0177777,_ibuf
jsr	pc,_sort
clr	(sp)
jsr	pc,*$_close
L56:tst	_mflg
jeq	L10004
clr	r0
jbr	L10005
L10004:mov	_eargc,r0
L10005:mov	r0,r4
L57:mov	r4,r0
add	$07,r0
cmp	_nfiles,r0
jle	L58
jsr	pc,_newfile
mov	r4,(sp)
add	$07,(sp)
mov	r4,-(sp)
jsr	pc,*$_merge
tst	(sp)+
L59:add	$07,r4
jbr	L57
L58:cmp	r4,_nfiles
jeq	L60
jsr	pc,_oldfile
mov	_nfiles,(sp)
mov	r4,-(sp)
jsr	pc,*$_merge
tst	(sp)+
L60:jsr	pc,_term
L1:jmp	rretrn
.globl	_sort
.text
_sort:
jsr	r5,rsave; 10
clr	-8.(r5)
clr	-10.(r5)
L64:mov	_tspace,r4
mov	_lspace,r3
L65:mov	_nlines,r0
asl	r0
add	_lspace,r0
cmp	r3,r0
jhis	L66
mov	_tspace,r0
add	_ntext,r0
cmp	r4,r0
jhis	L66
mov	r4,(r3)+
L67:mov	$_ibuf,(sp)
jsr	pc,*$_getc
mov	r0,r2
movb	r0,(r4)+
cmp	$012,r0
jeq	L68
tst	r2
jge	L67
dec	r4
mov	_ibuf,(sp)
jsr	pc,*$_close
cmp	-10.(r5),_eargc
jge	L69
mov	-10.(r5),(sp)
inc	-10.(r5)
jsr	pc,*$_setfil
mov	r0,-12.(r5)
jne	L70
clr	_ibuf
jbr	L71
L70:mov	$_ibuf,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L72
mov	-12.(r5),(sp)
jsr	pc,*$_cant
L72:L71:jbr	L73
L69:jbr	L68
L73:jbr	L67
L68:tst	r2
jge	L74
inc	-8.(r5)
sub	$02,r3
jbr	L66
L74:jbr	L65
L66:mov	$_cmp,(sp)
mov	$02,-(sp)
mov	r3,r0
sub	_lspace,r0
asr	r0
mov	r0,-(sp)
mov	_lspace,-(sp)
jsr	pc,*$_qsort
add	$06,sp
tst	-8.(r5)
jeq	L10006
tst	_nfiles
jeq	L75
L10006:jsr	pc,_newfile
jbr	L76
L75:jsr	pc,_oldfile
L76:L77:cmp	r3,_lspace
jlos	L78
mov	-(r3),r4
L81:mov	$_obuf,(sp)
movb	*r4,r0
mov	r0,-(sp)
jsr	pc,*$_putc
tst	(sp)+
L79:cmpb	(r4)+,$012
jne	L81
L80:jbr	L77
L78:mov	$_obuf,(sp)
jsr	pc,*$_fflush
mov	_obuf,(sp)
jsr	pc,*$_close
L62:tst	-8.(r5)
jeq	L64
L63:L61:jmp	rretrn
.globl	_merge
.text
_merge:
jsr	r5,rsave; 6
mov	_lspace,r4
clr	-10.(r5)
mov	4.(r5),r2
L83:cmp	r2,6.(r5)
jge	L84
mov	r2,(sp)
jsr	pc,*$_setfil
mov	r0,-8.(r5)
tst	-8.(r5)
jne	L86
clr	(sp)
jsr	pc,*$_dup
mov	r0,512.(r4)
jbr	L87
L86:mov	r4,(sp)
add	$01000,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_fopen
tst	(sp)+
tst	r0
jge	L88
mov	-8.(r5),(sp)
jsr	pc,*$_cant
L88:L87:mov	-10.(r5),r0
asl	r0
mov	r4,_ibuf(r0)
mov	r4,(sp)
jsr	pc,*$_rline
tst	r0
jne	L89
inc	-10.(r5)
L89:add	$02006,r4
L85:inc	r2
jbr	L83
L84:mov	-10.(r5),r2
mov	$_cmp,(sp)
mov	$02,-(sp)
mov	r2,-(sp)
mov	$_ibuf,-(sp)
jsr	pc,*$_qsort
add	$06,sp
tst	r2
jle	L90
L91:mov	r2,r0
asl	r0
mov	-2.+_ibuf(r0),r3
L95:mov	$_obuf,(sp)
movb	*r3,r0
mov	r0,-(sp)
jsr	pc,*$_putc
tst	(sp)+
L93:cmpb	(r3)+,$012
jne	L95
L94:mov	r2,r0
asl	r0
mov	-2.+_ibuf(r0),(sp)
jsr	pc,*$_rline
tst	r0
jeq	L96
dec	r2
tst	r2
jeq	L92
L96:mov	r2,r0
asl	r0
add	$_ibuf,r0
mov	r0,r3
L97:sub	$02,r3
mov	r3,r0
cmp	$_ibuf,r0
jhis	L98
mov	r3,(sp)
add	$0177776,(sp)
mov	r3,-(sp)
jsr	pc,*$_cmp
tst	(sp)+
tst	r0
jge	L98
mov	(r3),r4
mov	-2.(r3),(r3)
mov	r4,-2.(r3)
jbr	L97
L98:jbr	L91
L92:L90:mov	_lspace,r4
mov	4.(r5),r2
L99:cmp	r2,6.(r5)
jge	L100
mov	512.(r4),(sp)
jsr	pc,*$_close
add	$02006,r4
cmp	r2,_eargc
jlt	L102
mov	r2,(sp)
jsr	pc,*$_setfil
mov	r0,(sp)
jsr	pc,*$_creat
mov	r0,(sp)
jsr	pc,*$_close
L102:L101:inc	r2
jbr	L99
L100:mov	$_obuf,(sp)
jsr	pc,*$_fflush
mov	_obuf,(sp)
jsr	pc,*$_close
L82:jmp	rretrn
.globl	_rline
.text
_rline:
jsr	r5,rsave; 2
mov	4.(r5),r0
add	$01000,r0
mov	r0,r3
mov	4.(r5),r4
L106:mov	r3,(sp)
jsr	pc,*$_getc
mov	r0,r2
tst	r2
jge	L107
mov	$01,r0
jbr	L103
L107:movb	r2,(r4)+
L104:cmp	r2,$012
jne	L106
L105:clr	r0
jbr	L103
L103:jmp	rretrn
.globl	_newfile
.text
_newfile:
jsr	r5,rsave; 2
mov	$_obuf,(sp)
mov	_nfiles,-(sp)
jsr	pc,_setfil
tst	(sp)+
mov	r0,-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jge	L109
.data
L110:.byte 0103,0141,0156,047,0164,040,0143,0162,0145,0141,0164,0145,040,0164,0145,0155,0160,012,0
.even
.text
mov	$L110,(sp)
jsr	pc,*$_mess
jsr	pc,_term
L109:inc	_nfiles
L108:jmp	rretrn
.globl	_setfil
.text
_setfil:
jsr	r5,rsave; 2
cmp	4.(r5),_eargc
jge	L112
mov	4.(r5),r0
asl	r0
add	_eargv,r0
movb	*(r0),r0
cmp	$055,r0
jne	L113
mov	4.(r5),r0
asl	r0
add	_eargv,r0
mov	(r0),r0
tstb	1.(r0)
jne	L113
clr	r0
jbr	L111
L113:mov	4.(r5),r0
asl	r0
add	_eargv,r0
mov	(r0),r0
jbr	L111
L112:sub	_eargc,4.(r5)
mov	4.(r5),r1
sxt	r0
div	$032,r0
add	$0141,r0
movb	r0,*_filep
mov	_filep,r0
mov	4.(r5),r3
sxt	r2
div	$032,r2
add	$0141,r3
movb	r3,1.(r0)
mov	_file,r0
jbr	L111
L111:jmp	rretrn
.globl	_oldfile
.text
_oldfile:
jsr	r5,rsave; 2
tst	_outfil
jeq	L115
mov	$_obuf,(sp)
mov	_outfil,-(sp)
jsr	pc,*$_fcreat
tst	(sp)+
tst	r0
jge	L116
.data
L117:.byte 0103,0141,0156,047,0164,040,0143,0162,0145,0141,0164,0145,040,0157,0165,0164,0160,0165,0164,012,0
.even
.text
mov	$L117,(sp)
jsr	pc,*$_mess
jsr	pc,_term
L116:jbr	L118
L115:mov	$01,_obuf
L118:L114:jmp	rretrn
.globl	_cant
.text
_cant:
jsr	r5,rsave; 2
.data
L120:.byte 0103,0141,0156,047,0164,040,0157,0160,0145,0156,040,0
.even
.text
mov	$L120,(sp)
jsr	pc,*$_mess
mov	4.(r5),(sp)
jsr	pc,*$_mess
.data
L121:.byte 012,0
.even
.text
mov	$L121,(sp)
jsr	pc,*$_mess
jsr	pc,_term
L119:jmp	rretrn
.globl	_term
.text
_term:
jsr	r5,rsave; 2
tst	_nfiles
jne	L123
inc	_nfiles
L123:mov	_eargc,r4
L124:cmp	r4,_nfiles
jge	L125
mov	r4,(sp)
jsr	pc,*$_setfil
mov	r0,(sp)
jsr	pc,*$_unlink
L126:inc	r4
jbr	L124
L125:jsr	pc,_exit
L122:jmp	rretrn
.globl	_cmp
.text
_cmp:
jsr	r5,rsave; 22
mov	*4.(r5),r2
mov	*6.(r5),-8.(r5)
tst	_sfield
jlt	L128
mov	r2,(sp)
jsr	pc,*$_skip
mov	r0,r2
mov	-8.(r5),(sp)
jsr	pc,*$_skip
mov	r0,-8.(r5)
L128:tst	_schar
jeq	L129
mov	r2,(sp)
jsr	pc,*$_slip
mov	r0,r2
mov	-8.(r5),(sp)
jsr	pc,*$_slip
mov	r0,-8.(r5)
L129:tst	_nflg
jeq	L130
mov	r5,r0
add	$0177762,r0
mov	r0,(sp)
mov	r2,-(sp)
jsr	pc,*$_snip
tst	(sp)+
mov	r0,r2
mov	r5,r0
add	$0177760,r0
mov	r0,(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_snip
tst	(sp)+
mov	r0,-8.(r5)
cmp	-14.(r5),-16.(r5)
jeq	L10007
clr	r1
jbr	L10008
L10007:mov	$01,r1
L10008:mul	-14.(r5),r1
mov	r1,-22.(r5)
mov	r2,-10.(r5)
L131:movb	*-10.(r5),r0
mov	r0,(sp)
jsr	pc,*$_digit
tst	r0
jeq	L132
L133:inc	-10.(r5)
jbr	L131
L132:mov	-8.(r5),-12.(r5)
L134:movb	*-12.(r5),r0
mov	r0,(sp)
jsr	pc,*$_digit
tst	r0
jeq	L135
L136:inc	-12.(r5)
jbr	L134
L135:mov	-10.(r5),-18.(r5)
mov	-12.(r5),-20.(r5)
clr	r4
tst	-22.(r5)
jeq	L137
L138:cmp	-10.(r5),r2
jlos	L139
cmp	-12.(r5),-8.(r5)
jlos	L139
dec	-12.(r5)
mov	-12.(r5),r0
movb	(r0),r0
dec	-10.(r5)
mov	-10.(r5),r1
movb	(r1),r1
sub	r1,r0
mov	r0,r3
jeq	L140
mov	r3,r4
L140:jbr	L138
L139:L137:L141:cmp	-10.(r5),r2
jlos	L142
dec	-10.(r5)
mov	-10.(r5),r0
movb	(r0),r0
cmp	$060,r0
jeq	L143
tst	-22.(r5)
jeq	L10009
mov	-22.(r5),r0
neg	r0
jbr	L10010
L10009:mov	-14.(r5),r0
neg	r0
L10010:jbr	L127
L143:jbr	L141
L142:L144:cmp	-12.(r5),-8.(r5)
jlos	L145
dec	-12.(r5)
mov	-12.(r5),r0
movb	(r0),r0
cmp	$060,r0
jeq	L146
tst	-22.(r5)
jeq	L10011
mov	-22.(r5),r0
jbr	L10012
L10011:mov	-16.(r5),r0
L10012:jbr	L127
L146:jbr	L144
L145:tst	r4
jeq	L147
mov	r4,r1
mul	-22.(r5),r1
mov	r1,r0
jbr	L127
L147:mov	-18.(r5),r0
mov	r0,r2
movb	(r0),r0
cmp	$056,r0
jne	L148
inc	r2
L148:mov	-20.(r5),r0
mov	r0,-8.(r5)
movb	(r0),r0
cmp	$056,r0
jne	L149
inc	-8.(r5)
L149:L150:movb	*r2,r0
mov	r0,(sp)
jsr	pc,*$_digit
tst	r0
jeq	L151
movb	*-8.(r5),r0
mov	r0,(sp)
jsr	pc,*$_digit
tst	r0
jeq	L151
mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
movb	(r2)+,r1
sub	r1,r0
mov	r0,r4
jeq	L152
tst	-22.(r5)
jeq	L10013
mov	r4,r1
mul	-22.(r5),r1
jbr	L10014
L10013:mov	-16.(r5),r1
L10014:mov	r1,r0
jbr	L127
L152:jbr	L150
L151:L153:movb	*r2,r0
mov	r0,(sp)
jsr	pc,*$_digit
tst	r0
jeq	L154
cmpb	(r2)+,$060
jeq	L155
tst	-22.(r5)
jeq	L10015
mov	-22.(r5),r0
neg	r0
jbr	L10016
L10015:mov	-14.(r5),r0
neg	r0
L10016:jbr	L127
L155:jbr	L153
L154:L156:movb	*-8.(r5),r0
mov	r0,(sp)
jsr	pc,*$_digit
tst	r0
jeq	L157
mov	-8.(r5),r0
inc	-8.(r5)
movb	(r0),r0
cmp	$060,r0
jeq	L158
tst	-22.(r5)
jeq	L10017
mov	-22.(r5),r0
jbr	L10018
L10017:mov	-16.(r5),r0
L10018:jbr	L127
L158:jbr	L156
L157:L130:L159:L160:movb	(r2),r0
movb	128.+_map(r0),r4
jge	L161
inc	r2
jbr	L160
L161:L162:movb	*-8.(r5),r0
movb	128.+_map(r0),r3
jge	L163
inc	-8.(r5)
jbr	L162
L163:cmp	r4,r3
jne	L164
cmpb	(r2)+,$012
jeq	L165
inc	-8.(r5)
jbr	L159
L165:mov	*4.(r5),r2
mov	*6.(r5),-8.(r5)
L166:cmpb	(r2),*-8.(r5)
jne	L167
cmpb	(r2)+,$012
jne	L168
clr	r0
jbr	L127
L168:inc	-8.(r5)
jbr	L166
L167:movb	*-8.(r5),r0
movb	(r2),r1
sub	r1,r0
mov	r0,r1
mul	_rflg,r1
mov	r1,r0
jbr	L127
L164:mov	r3,r0
sub	r4,r0
jbr	L127
L127:jmp	rretrn
.globl	_skip
.text
_skip:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	_sfield,r4
jeq	L170
L173:L174:cmpb	(r3),$040
jeq	L10019
cmpb	(r3),$011
jne	L175
L10019:inc	r3
jbr	L174
L175:L176:cmpb	(r3),$040
jeq	L177
cmpb	(r3),$011
jeq	L177
cmpb	(r3),$012
jeq	L177
inc	r3
jbr	L176
L177:L171:dec	r4
jne	L173
L172:L170:L178:cmpb	(r3),$040
jeq	L10020
cmpb	(r3),$011
jne	L179
L10020:inc	r3
jbr	L178
L179:mov	r3,r0
jbr	L169
L169:jmp	rretrn
.globl	_slip
.text
_slip:
jsr	r5,rsave; 2
mov	4.(r5),r3
mov	_schar,r4
L183:cmpb	(r3),$012
jeq	L184
inc	r3
L184:L181:dec	r4
jne	L183
L182:mov	r3,r0
jbr	L180
L180:jmp	rretrn
.globl	_snip
.text
_snip:
jsr	r5,rsave; 2
mov	4.(r5),r4
L186:cmpb	(r4),$040
jeq	L10021
cmpb	(r4),$011
jne	L187
L10021:inc	r4
jbr	L186
L187:mov	_rflg,r3
cmpb	(r4),$055
jne	L188
mov	r3,r0
neg	r0
mov	r0,r3
inc	r4
L188:mov	r3,*6.(r5)
mov	r4,r0
jbr	L185
L185:jmp	rretrn
.globl	_number
.text
_number:
jsr	r5,rsave; 2
mov	4.(r5),r3
clr	r4
L190:movb	*r3,r0
mov	r0,(sp)
jsr	pc,*$_digit
tst	r0
jeq	L191
mov	r4,r1
mul	$012,r1
mov	r1,r0
movb	(r3)+,r1
add	r1,r0
add	$0177720,r0
mov	r0,r4
jbr	L190
L191:mov	r4,r0
jbr	L189
L189:jmp	rretrn
.globl	_digit
.text
_digit:
jsr	r5,rsave; 2
cmp	4.(r5),$071
jgt	L193
cmp	4.(r5),$060
jlt	L193
mov	$01,r0
jbr	L192
L193:clr	r0
jbr	L192
L192:jmp	rretrn
.globl	_mess
.text
_mess:
jsr	r5,rsave; 2
L195:tstb	*4.(r5)
jeq	L196
mov	$01,(sp)
mov	4.(r5),-(sp)
inc	4.(r5)
mov	$02,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jbr	L195
L196:L194:jmp	rretrn
.globl	_ignore
.text
_ignore:
jsr	r5,rsave; 2
mov	4.(r5),r4
L198:cmp	r4,6.(r5)
jgt	L199
movb	$0177777,128.+_map(r4)
L200:inc	r4
jbr	L198
L199:L197:jmp	rretrn
