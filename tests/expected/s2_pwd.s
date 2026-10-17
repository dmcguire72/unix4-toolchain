.text
L1:.byte 056,0
.even
.data
.globl	_dot
.data
_dot=.
.text
_dot=L1
.text
L2:.byte 056,056,0
.even
.data
.globl	_dotdot
.data
_dotdot=.
.text
_dotdot=L2
.text
L3:.byte 057,0
.even
.data
.globl	_root
.data
_root=.
.text
_root=L3
.globl	_name
.comm	_name,01000
.globl	_file
.comm	_file,02
.globl	_off
.data
_off=.
-1.
.globl	_x
.comm	_x,050
.globl	_y
.comm	_y,022
.globl	_main
.text
_main:
jsr	r5,rsave; 4
L5:mov	$_x,(sp)
mov	$_dot,-(sp)
jsr	pc,*$_stat
tst	(sp)+
clr	(sp)
mov	$_dotdot,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_file
jge	L6
jsr	pc,_prname
L6:L7:mov	$020,(sp)
mov	$_y,-(sp)
mov	_file,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-8.(r5)
cmp	$020,r0
jle	L8
jsr	pc,_prname
L8:cmp	_y,2.+_x
jne	L7
mov	_file,(sp)
jsr	pc,*$_close
cmp	_y,$01
jne	L9
jsr	pc,_ckroot
L9:jsr	pc,_cat
mov	$_dotdot,(sp)
jsr	pc,*$_chdir
jbr	L5
L4:jmp	rretrn
.globl	_ckroot
.text
_ckroot:
jsr	r5,rsave; 6
mov	$_x,(sp)
mov	$2.+_y,-(sp)
jsr	pc,*$_stat
tst	(sp)+
mov	r0,-10.(r5)
jge	L11
jsr	pc,_prname
L11:mov	_x,-8.(r5)
mov	$_root,(sp)
jsr	pc,*$_chdir
mov	r0,-10.(r5)
jge	L12
jsr	pc,_prname
L12:clr	(sp)
mov	$_root,-(sp)
jsr	pc,*$_open
tst	(sp)+
mov	r0,_file
jge	L13
jsr	pc,_prname
L13:L14:mov	$020,(sp)
mov	$_y,-(sp)
mov	_file,-(sp)
jsr	pc,*$_read
cmp	(sp)+,(sp)+
mov	r0,-10.(r5)
cmp	$020,r0
jle	L15
jsr	pc,_prname
L15:tst	_y
jeq	L14
mov	$_x,(sp)
mov	$2.+_y,-(sp)
jsr	pc,*$_stat
tst	(sp)+
mov	r0,-10.(r5)
jge	L16
jsr	pc,_prname
L16:cmp	_x,-8.(r5)
jne	L14
bic	$!060000,4.+_x
cmp	4.+_x,$040000
jne	L14
jsr	pc,_cat
jsr	pc,_prname
L10:jmp	rretrn
.globl	_prname
.text
_prname:
jsr	r5,rsave; 2
mov	_off,r0
movb	$012,_name(r0)
mov	_off,(sp)
inc	(sp)
mov	$_name,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
jsr	pc,_exit
L17:jmp	rretrn
.globl	_cat
.text
_cat:
jsr	r5,rsave; 6
mov	$0177777,-8.(r5)
L19:inc	-8.(r5)
mov	-8.(r5),r0
tstb	2.+_y(r0)
jeq	L20
jbr	L19
L20:mov	_off,r0
add	-8.(r5),r0
add	$02,r0
cmp	$0777,r0
jge	L21
jsr	pc,_prname
L21:mov	_off,r0
inc	r0
mov	r0,-10.(r5)
L22:tst	-10.(r5)
jlt	L23
mov	-10.(r5),r0
add	-8.(r5),r0
mov	-10.(r5),r1
movb	_name(r1),1.+_name(r0)
L24:dec	-10.(r5)
jbr	L22
L23:mov	-8.(r5),r0
add	_off,r0
inc	r0
mov	r0,_off
mov	-8.(r5),r0
movb	_root,_name(r0)
dec	-8.(r5)
L25:tst	-8.(r5)
jlt	L26
mov	-8.(r5),r0
mov	-8.(r5),r1
movb	2.+_y(r1),_name(r0)
L27:dec	-8.(r5)
jbr	L25
L26:L18:jmp	rretrn
