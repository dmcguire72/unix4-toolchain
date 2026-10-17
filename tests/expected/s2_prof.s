.globl	_nl
.comm	_nl,025060
.globl	_cbuf
.comm	_cbuf,02260
.globl	_buf
.comm	_buf,042
.globl	_i
.comm	_i,02
.globl	_j
.comm	_j,02
.globl	_highpc
.comm	_highpc,02
.globl	_lowpc
.comm	_lowpc,02
.globl	_ccnt
.comm	_ccnt,02
.globl	_pcl
.comm	_pcl,02
.globl	_pch
.comm	_pch,02
.globl	_bufs
.comm	_bufs,02
.globl	_nname
.comm	_nname,02
.globl	_time
.comm	_time,010
.globl	_totime
.comm	_totime,010
.globl	_maxtime
.comm	_maxtime,010
.globl	_scale
.comm	_scale,010
.globl	_lastx
.comm	_lastx,010
.globl	_lasty
.comm	_lasty,010
.globl	_np
.comm	_np,02
.globl	_npe
.comm	_npe,02
.globl	_aflg
.comm	_aflg,02
.globl	_vflg
.comm	_vflg,02
.globl	_lflg
.comm	_lflg,02
.globl	_symoff
.comm	_symoff,02
.globl	_symsiz
.comm	_symsiz,02
.globl	_vf
.comm	_vf,02
.globl	_etext
.comm	_etext,02
.globl	_fout
.comm	_fout,02
.globl	_ncount
.comm	_ncount,02
.globl	_main
.text
_main:
jsr	r5,rsave; 24
mov	$01,(sp)
jsr	pc,*$_dup
mov	r0,_fout
add	$02,6.(r5)
.data
L2:.byte 0141,056,0157,0165,0164,0
.even
.text
mov	$L2,-8.(r5)
L3:cmp	4.(r5),$01
jle	L4
mov	6.(r5),r0
movb	*(r0),r0
cmp	$055,r0
jne	L5
inc	*6.(r5)
mov	*6.(r5),r0
movb	(r0),r0
cmp	$0154,r0
jne	L6
inc	_lflg
L6:mov	6.(r5),r0
movb	*(r0),r0
cmp	$0141,r0
jne	L7
mov	$040,_aflg
L7:mov	6.(r5),r0
movb	*(r0),r0
cmp	$0166,r0
jne	L8
inc	_vflg
L8:jbr	L9
L5:mov	*6.(r5),-8.(r5)
L9:dec	4.(r5)
add	$02,6.(r5)
jbr	L3
L4:clr	(sp)
mov	-8.(r5),-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-10.(r5)
jge	L10
.data
L11:.byte 0103,0141,0156,047,0164,040,0146,0151,0156,0144,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L11,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_done
L10:mov	$020,(sp)
mov	$_buf,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	_buf,$0407
jeq	L12
cmp	_buf,$0410
jeq	L12
.data
L13:.byte 0102,0141,0144,040,0146,0157,0162,0155,0141,0164,072,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L13,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_done
L12:mov	8.+_buf,_symsiz
mov	2.+_buf,r0
add	4.+_buf,r0
mov	r0,_symoff
cmp	14.+_buf,$01
jeq	L14
asl	_symoff
L14:clr	(sp)
mov	_symoff,-(sp)
add	$020,(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
.data
L15:.byte 0155,0157,0156,056,0157,0165,0164,0
.even
.text
clr	(sp)
mov	$L15,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,-12.(r5)
jge	L16
.data
L17:.byte 0116,0157,040,0155,0157,0156,056,0157,0165,0164,012,0
.even
.text
mov	$L17,(sp)
jsr	pc,*$_printf
jsr	pc,_done
L16:mov	$_buf,(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_fstat
tst	(sp)+
mov	$02,(sp)
mov	$_lowpc,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	$02,(sp)
mov	$_highpc,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	$02,(sp)
mov	$_ncount,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	10.+_buf,r0
asr	r0
mov	_ncount,r1
inc	r1
mul	$03,r1
sub	r1,r0
mov	r0,_bufs
mov	_ncount,r1
mul	$06,r1
mov	r1,(sp)
mov	$_cbuf,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	_lowpc,r0
asr	r0
bic	$!077777,r0
mov	r0,_lowpc
mov	_highpc,r0
asr	r0
bic	$!077777,r0
mov	r0,_highpc
mov	$_nl,_npe
clr	_nname
L18:tst	_symsiz
jle	L19
mov	$014,(sp)
mov	$_buf,-(sp)
mov	-10.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	8.+_buf,r0
bis	_aflg,r0
cmp	$042,r0
jne	L20
mov	10.+_buf,r0
asr	r0
bic	$!077777,r0
mov	r0,10.+_buf
mov	_npe,r0
mov	10.+_buf,8.(r0)
clr	_i
L21:cmp	_i,$010
jge	L22
mov	_npe,r0
add	_i,r0
mov	_i,r1
movb	_buf(r1),(r0)
L23:inc	_i
jbr	L21
L22:add	$022,_npe
inc	_nname
L20:sub	$014,_symsiz
jbr	L18
L19:tst	_nname
jne	L24
.data
L25:.byte 0116,0157,040,0163,0171,0155,0142,0157,0154,0163,072,040,045,0163,012,0
.even
.text
mov	-8.(r5),(sp)
mov	$L25,-(sp)
jsr	pc,*$_printf
tst	(sp)+
jsr	pc,_done
L24:mov	_npe,r0
mov	$077777,8.(r0)
add	$022,_npe
mov	$_cbuf,-24.(r5)
L26:mov	_ncount,r1
mul	$06,r1
add	$_cbuf,r1
cmp	-24.(r5),r1
jhis	L27
mov	$_nl,_np
L29:cmp	_np,_npe
jhis	L30
mov	*-24.(r5),r0
add	$0177772,r0
mov	_np,r1
mov	8.(r1),r1
asl	r1
cmp	r0,r1
jne	L32
mov	_np,r0
mov	-24.(r5),r1
mov	2.(r1),14.(r0)
mov	_np,r0
mov	-24.(r5),r1
mov	4.(r1),16.(r0)
jbr	L30
L32:L31:add	$022,_np
jbr	L29
L30:L28:add	$06,-24.(r5)
jbr	L26
L27:mov	$_valcmp,(sp)
mov	$022,-(sp)
mov	_nname,-(sp)
mov	$_nl,-(sp)
jsr	pc,*$_qsort
add	$06,sp
mov	_highpc,r0
sub	_lowpc,r0
movif	r0,r0
movif	_bufs,r1
addf	$00,r1
divf	r1,r0
movf	r0,_scale
mov	$0177777,_i
L33:mov	$02,(sp)
mov	$_ccnt,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$02,r0
jne	L34
tst	_ccnt
jeq	L35
movif	_ccnt,r0
movf	r0,_time
tst	_ccnt
jge	L36
movf	$044200,r0
movf	r0,-(sp)
movf	_time,r0
addf	(sp)+,r0
movf	r0,_time
L36:movf	_time,r0
movf	r0,-(sp)
movf	_totime,r0
addf	(sp)+,r0
movf	r0,_totime
movf	_time,r0
cmpf	_maxtime,r0
cfcc
jge	L37
movf	_time,r0
movf	r0,_maxtime
L37:movif	_lowpc,r0
movif	_i,r1
mulf	_scale,r1
addf	r1,r0
movfi	r0,r0
mov	r0,_pcl
movif	_lowpc,r0
mov	_i,r1
inc	r1
movif	r1,r1
mulf	_scale,r1
addf	r1,r0
movfi	r0,r0
mov	r0,_pch
clr	_j
L38:cmp	_j,_nname
jge	L39
mov	_j,r1
mul	$022,r1
cmp	8.+_nl(r1),_pch
jgt	L39
mov	_j,r1
mul	$022,r1
cmp	26.+_nl(r1),_pcl
jle	L40
mov	_j,r1
mul	$022,r1
mov	8.+_nl(r1),(sp)
mov	_pcl,-(sp)
jsr	pc,*$_max
tst	(sp)+
mov	r0,-(sp)
mov	_j,r1
mul	$022,r1
mov	26.+_nl(r1),-(sp)
mov	_pch,-(sp)
jsr	pc,_min
cmp	(sp)+,(sp)+
sub	(sp)+,r0
mov	r0,-14.(r5)
movif	-14.(r5),r0
mulf	_time,r0
divf	_scale,r0
movf	r0,-(sp)
mov	_j,r1
mul	$022,r1
movof	10.+_nl(r1),r2
addf	(sp)+,r2
movfo	r2,10.+_nl(r1)
movf	r2,r1
L40:inc	_j
jbr	L38
L39:L35:inc	_i
jbr	L33
L34:tstf	_totime
cfcc
jne	L41
.data
L42:.byte 0116,0157,040,0164,0151,0155,0145,040,0141,0143,0143,0165,0155,0165,0154,0141,0164,0145,0144,012,0
.even
.text
mov	$L42,(sp)
jsr	pc,*$_printf
jsr	pc,_done
L41:tst	_vflg
jeq	L43
.data
L44:.byte 057,0144,0145,0166,057,0166,0164,060,0
.even
.text
mov	$01,(sp)
mov	$L44,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_vf
tst	_vf
jge	L45
.data
L46:.byte 0103,0141,0156,0156,0157,0164,040,0157,0160,0145,0156,040,0166,0164,012,0
.even
.text
mov	$L46,(sp)
jsr	pc,*$_printf
jsr	pc,_done
L45:mov	_vf,_fout
mov	$01,(sp)
jsr	pc,*$_vtch
mov	$01,(sp)
jsr	pc,*$_vtch
mov	$03,(sp)
jsr	pc,*$_vtch
movf	$0143000,r0
movf	r0,-(sp)
movf	$0143000,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
movf	$043000,r0
movf	r0,-(sp)
movf	$0143000,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
mov	$03,(sp)
jsr	pc,*$_vtch
movf	$0143000,r0
movf	r0,-(sp)
clrf	-(sp)
jsr	pc,_point
add	$020,sp
movf	$043000,r0
movf	r0,-(sp)
clrf	-(sp)
jsr	pc,_point
add	$020,sp
clr	_j
L47:cmp	_j,$011
jge	L48
mov	$03,(sp)
jsr	pc,*$_vtch
movf	$043000,r0
movif	_j,r1
mulf	$042400,r1
subf	r1,r0
movf	r0,-(sp)
movf	$0143000,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
movf	$043000,r0
movif	_j,r1
mulf	$042400,r1
subf	r1,r0
movf	r0,-(sp)
clrf	-(sp)
jsr	pc,_point
add	$020,sp
L49:inc	_j
jbr	L47
L48:clrf	_lastx
movf	$043000,r0
movf	r0,_lasty
movf	$043200,r0
mov	_i,r1
add	$0177777,r1
movif	r1,r1
divf	r1,r0
movf	r0,_scale
clr	(sp)
mov	_ncount,r1
inc	r1
mul	$06,r1
mov	r1,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_seek
cmp	(sp)+,(sp)+
L50:mov	$02,(sp)
mov	$_ccnt,-(sp)
mov	-12.(r5),-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
cmp	$02,r0
jne	L51
movif	_ccnt,r0
movf	r0,_time
tst	_ccnt
jge	L52
movf	$044200,r0
movf	r0,-(sp)
movf	_time,r0
addf	(sp)+,r0
movf	r0,_time
L52:mov	$03,(sp)
jsr	pc,*$_vtch
movf	_lasty,r0
movf	r0,-(sp)
movf	_lastx,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
movf	_time,r0
negf	r0
mulf	$042772,r0
divf	_maxtime,r0
movf	r0,_lastx
movf	_lasty,r0
movf	r0,-(sp)
movf	_lastx,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
mov	$03,(sp)
jsr	pc,*$_vtch
movf	_lasty,r0
movf	r0,-(sp)
movf	_lastx,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
movf	_scale,r0
movf	r0,-(sp)
movf	_lasty,r0
subf	(sp)+,r0
movf	r0,_lasty
movf	_lasty,r0
movf	r0,-(sp)
movf	_lastx,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
jbr	L50
L51:movf	$043200,r0
mov	_highpc,r1
sub	_lowpc,r1
movif	r1,r1
divf	r1,r0
movf	r0,_scale
movf	$041510,r0
movf	r0,_lastx
mov	$_nl,_np
L53:cmp	_np,_npe
jhis	L54
mov	_np,r0
cmp	8.(r0),_lowpc
jlt	L55
mov	_np,r0
cmp	8.(r0),_highpc
jge	L55
mov	_np,r0
movof	10.(r0),r0
divf	_totime,r0
movf	r0,_time
movf	$043000,r0
mov	_np,r1
mov	8.(r1),r1
sub	_lowpc,r1
movif	r1,r1
mulf	_scale,r1
subf	r1,r0
movf	r0,_lasty
mov	$03,(sp)
jsr	pc,*$_vtch
movf	_lasty,r0
movf	r0,-(sp)
clrf	-(sp)
jsr	pc,_point
add	$020,sp
movf	_lasty,r0
movf	r0,-(sp)
movf	$041510,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
mov	$03,(sp)
jsr	pc,*$_vtch
movf	_lasty,r0
movf	r0,-(sp)
movf	_lastx,r0
subf	$041510,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
movf	_lasty,r0
movf	r0,-(sp)
movf	_lastx,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
mov	$011,(sp)
jsr	pc,*$_vtch
movf	_lasty,r0
addf	$041560,r0
movf	r0,-(sp)
movf	_lastx,r0
addf	$041040,r0
movf	r0,-(sp)
jsr	pc,_point
add	$020,sp
mov	$01,(sp)
jsr	pc,*$_vtch
mov	$03,(sp)
jsr	pc,*$_vtch
clr	_j
L56:cmp	_j,$010
jge	L57
mov	_np,r0
add	_j,r0
movb	(r0),r0
cmp	$0137,r0
jeq	L59
mov	_np,r0
add	_j,r0
movb	(r0),r0
mov	r0,(sp)
jsr	pc,*$_vtch
L59:L58:inc	_j
jbr	L56
L57:clr	(sp)
jsr	pc,*$_vtch
movf	$042372,r0
movf	r0,-(sp)
movf	_lastx,r0
addf	(sp)+,r0
movf	r0,_lastx
movf	_lastx,r0
cmpf	$042772,r0
cfcc
jge	L60
movf	$041510,r0
movf	r0,_lastx
L60:L55:add	$022,_np
jbr	L53
L54:jsr	pc,_done
L43:.data
L61:.byte 040,040,040,040,0156,0141,0155,0145,040,045,045,0164,0151,0155,0145,040,043,0143,0141,0154,0154,040,040,0155,0163,057,0143,0141,0154,0154,012,0
.even
.text
mov	$L61,(sp)
jsr	pc,*$_printf
tst	_lflg
jne	L62
mov	$_timcmp,(sp)
mov	$022,-(sp)
mov	_nname,-(sp)
mov	$_nl,-(sp)
jsr	pc,*$_qsort
add	$06,sp
L62:mov	$_nl,_np
L63:mov	_npe,r0
add	$0177756,r0
cmp	_np,r0
jhis	L64
mov	_np,r0
movof	10.(r0),r0
divf	_totime,r0
movf	r0,_time
.data
L66:.byte 045,070,056,070,0163,045,066,056,061,0146,0
.even
.text
movf	$041710,r0
mulf	_time,r0
movf	r0,-(sp)
mov	_np,-(sp)
mov	$L66,-(sp)
jsr	pc,_printf
add	$014,sp
mov	_np,(sp)
add	$016,(sp)
jsr	pc,*$_ltod
movf	r0,-22.(r5)
tstf	-22.(r5)
cfcc
jeq	L67
.data
L68:.byte 045,066,0163,0
.even
.text
mov	_np,r0
mov	16.(r0),(sp)
mov	_np,r0
mov	14.(r0),-(sp)
jsr	pc,*$_locv
tst	(sp)+
mov	r0,(sp)
mov	$L68,-(sp)
jsr	pc,*$_printf
tst	(sp)+
.data
L69:.byte 040,045,067,056,062,0146,012,0
.even
.text
.data
L70:037165;0141217;056050;0172700
.text
mov	_np,r0
movof	10.(r0),r0
movf	-22.(r5),r1
mulf	L70,r1
divf	r1,r0
movf	r0,-(sp)
mov	$L69,-(sp)
jsr	pc,_printf
add	$012,sp
jbr	L71
L67:.data
L72:.byte 012,0
.even
.text
mov	$L72,(sp)
jsr	pc,*$_printf
L71:L65:add	$022,_np
jbr	L63
L64:jsr	pc,_done
L1:jmp	rretrn
.globl	_min
.text
_min:
jsr	r5,rsave; 2
cmp	4.(r5),6.(r5)
jge	L74
mov	4.(r5),r0
jbr	L73
L74:mov	6.(r5),r0
jbr	L73
L73:jmp	rretrn
.globl	_max
.text
_max:
jsr	r5,rsave; 2
cmp	4.(r5),6.(r5)
jle	L76
mov	4.(r5),r0
jbr	L75
L76:mov	6.(r5),r0
jbr	L75
L75:jmp	rretrn
.globl	_valcmp
.text
_valcmp:
jsr	r5,rsave; 2
mov	4.(r5),r0
mov	8.(r0),r0
mov	6.(r5),r1
sub	8.(r1),r0
jbr	L77
L77:jmp	rretrn
.globl	_timcmp
.text
_timcmp:
jsr	r5,rsave; 6
mov	6.(r5),r0
movof	10.(r0),r0
mov	4.(r5),r1
movof	10.(r1),r1
subf	r1,r0
movfo	r0,-10.(r5)
movof	-10.(r5),r0
cfcc
jle	L79
mov	$01,r0
jbr	L78
L79:movof	-10.(r5),r0
cfcc
jge	L80
mov	$0177777,r0
jbr	L78
L80:clr	r0
jbr	L78
L78:jmp	rretrn
.globl	_vtch
.text
_vtch:
jsr	r5,rsave; 2
mov	4.(r5),(sp)
bic	$!0377,(sp)
jsr	pc,*$_putchar
L81:jmp	rretrn
.globl	_point
.text
_point:
jsr	r5,rsave; 2
movf	4.(r5),r0
movf	r0,-(sp)
jsr	pc,_point1
add	$010,sp
movf	12.(r5),r0
movf	r0,-(sp)
jsr	pc,_point1
add	$010,sp
L82:jmp	rretrn
.globl	_point1
.text
_point1:
jsr	r5,rsave; 4
.data
L84:042777;0160000;00;00
.text
movf	4.(r5),r0
cmpf	L84,r0
cfcc
jge	L85
.data
L86:042777;0160000;00;00
.text
movf	L86,r0
movf	r0,4.(r5)
L85:movf	4.(r5),r0
cmpf	$0143000,r0
cfcc
jle	L87
movf	$0143000,r0
movf	r0,4.(r5)
L87:movf	4.(r5),r0
movfi	r0,r0
mov	r0,-8.(r5)
movb	-8.(r5),r0
mov	r0,(sp)
jsr	pc,*$_vtch
movb	1.+-8.(r5),r0
mov	r0,(sp)
jsr	pc,*$_vtch
L83:jmp	rretrn
.globl	_done
.text
_done:
jsr	r5,rsave; 2
jsr	pc,_flush
jsr	pc,_exit
L88:jmp	rretrn
.globl	fltused
