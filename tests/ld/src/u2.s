.globl	_foo, _bar, _data1
.globl	_comm1
.comm	_comm1,6
.text
_foo:	mov	$_comm1,r0
	br	.+4
_bar:	mov	_data1,r1
	rts	pc
.data
loc:	1;2;3
.globl	_etext, _edata, _end
	_etext
	_edata
	_end
