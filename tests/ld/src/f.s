.globl	_foo, _data1, _comm1, _comm2
.comm	_comm1,5
.comm	_comm2,2
.text
_foo:	mov	$_data1,r0
	mov	$loc,r1
	rts	pc
.data
_data1:	7
loc:	8
.bss
bb2:	.=.+9
