.globl	_main
.text
_main:
.bss
L2:.=.+34.
.text
jsr	r5,rsave; 2
cmp	4.(r5),$02
jge	L3
.data
L4:.byte 0125,0163,0141,0147,0145,072,040,0154,0156,040,0164,0141,0162,0147,0145,0164,040,0133,040,0156,0145,0167,0156,0141,0155,0145,040,0135,012,0
.even
.text
mov	$035,(sp)
mov	$L4,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L3:cmp	4.(r5),$02
jne	L5
mov	6.(r5),r0
mov	2.(r0),r4
L6:tstb	(r4)+
jeq	L7
jbr	L6
L7:L8:cmpb	-(r4),$057
jeq	L9
mov	6.(r5),r0
cmp	2.(r0),r4
jhis	L9
jbr	L8
L9:inc	r4
mov	6.(r5),r0
mov	r4,4.(r0)
L5:mov	$L2,(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_stat
tst	(sp)+
mov	2.+L2,r0
bic	$!060000,r0
cmp	$040000,r0
jne	L10
.data
L11:.byte 0116,0157,040,0144,0151,0162,0145,0143,0164,0157,0162,0171,040,0154,0151,0156,0153,012,0
.even
.text
mov	$022,(sp)
mov	$L11,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L10:mov	6.(r5),r0
mov	4.(r0),(sp)
mov	6.(r5),r0
mov	2.(r0),-(sp)
jsr	pc,*$_link
tst	(sp)+
tst	r0
jge	L12
.data
L13:.byte 0103,0141,0156,047,0164,040,0154,0151,0156,0153,012,0
.even
.text
mov	$013,(sp)
mov	$L13,-(sp)
mov	$01,-(sp)
jsr	pc,*$_write
cmp	(sp)+,(sp)+
mov	$01,(sp)
jsr	pc,*$_exit
L12:clr	(sp)
jsr	pc,*$_exit
L1:jmp	rretrn
