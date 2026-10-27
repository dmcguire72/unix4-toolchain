.globl	start, _foo, _data1, _comm1, _comm2, _etext, _edata, _end
.text
start:	mov	$_foo,r0
	jsr	pc,_foo
	mov	$_comm1,r1
	mov	$_comm2,r2
	mov	$_etext,r3
	mov	$_edata,r3
	mov	$_end,r3
	mov	_data1,r4
	br	.+4
	rts	pc
.data
x1:	_foo
	_data1
.bss
bb:	.=.+6
