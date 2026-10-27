.globl	_main, _foo, _bar
.globl	_comm1
.text
_main:
	mov	$_foo,r0
	jsr	pc,_bar
	mov	_data1,r1
	mov	$bss1,r2
	rts	pc
.data
_data1:	_foo
	_comm1
.bss
bss1:	.=.+10
.comm	_comm1,4
