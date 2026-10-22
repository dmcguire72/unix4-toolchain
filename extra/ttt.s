/ 1 to 1 rewrite of original ttt
/ outputs same binary when stripped

    jsr	r5,sigset
	onint
	jmp	main

onint:
	jsr	pc,nl
	sys	exit

puts:
	movb	(r5)+,r0
	beq	1f
	jsr	pc,putc
	br	puts
1:
	inc	r5
	bic	$1,r5
	rts	r5

gety:
	jsr	pc,getc
	cmp	r0,$171
	bne	1f
	tst	(r5)+
1:
	cmp	r0,$12
	beq	1f
	jsr	pc,skipnl
1:
	rts	r5

getc:
	clr	r0
	sys	read; ibuf; 1
	bcs	1f
	tst	r0
	beq	1f
	mov	ibuf,r0
	rts	pc
1:
	jmp	*handler

cat:
	mov	r5,catnam
	sys	open
catnam:
	0
	0
	bcs	2f
	mov	r0,r1
1:
	mov	r1,r0
	sys	read; ibuf; 1
	tst	r0
	beq	3f
	mov	ibuf,r0
	jsr	pc,putc
	br	1b
3:
	mov	r1,r0
	sys	close
2:
	tstb	(r5)+
	bne	2b
	inc	r5
	bic	$1,r5
	rts	r5

nl:
	mov	$12,r0
putc:
	mov	r0,ibuf
	mov	$1,r0
	sys	write; ibuf; 1
	rts	pc

putnum:
	mov	r1,-(sp)
	jsr	pc,1f
	mov	(sp)+,r1
	rts	pc
1:
	mov	r0,r1
	clr	r0
	div	$12,r0
	mov	r1,-(sp)
	tst	r0
	beq	2f
	jsr	pc,1b
2:
	mov	(sp)+,r0
	add	$60,r0
	jsr	pc,putc
	rts	pc

skipnl:
	jsr	pc,getc
	cmp	r0,$12
	bne	skipnl
	rts	pc

rand:
	mov	r1,-(sp)
	mov	seed,r1
	bne	1f
	sys	time
	mov	r1,seed
1:
	mul	$31425,r1
	add	$15415,r1
	mov	r1,r0
	mov	r0,seed
	bic	$100000,r0
	mov	(r5)+,r1
	asl	r1
	mul	r1,r0
	mov	(sp)+,r1
	rts	r5

sigset:
	mov	(r5)+,handler
	sys	signal; 2
handler:
	0
	rts	r5
ibuf:
	0
seed:
	0

main:
	jsr	r5,puts
	<Tic-Tac-Toe\n\0>
	.even
	jsr	r5,puts
	<Accumulated knowledge? \0>
	.even
	jsr	r5,gety
	br	1f
	sys	open; fname; 0
	bcs	1f
	mov	r0,r1
	sys	read; kbuf; 1750
	mov	r1,r0
	sys	close
1:
	mov	$kbuf,r0
2:
	tst	(r0)+
	bne	2b
	tst	-(r0)
	mov	r0,kend
	sub	$kbuf,r0
	asr	r0
	jsr	pc,putnum
	jsr	r5,puts
	< 'bits' of knowledge\n\0>
	.even
	jsr	r5,sigset
	bye

newgame:
	jsr	r5,puts
	<new game\n\0>
	.even
	mov	$stm,stp
	mov	$board,r0
1:
	clrb	(r0)+
	cmp	r0,$stp
	bcs	1b
show:
	mov	$board,r1
1:
	movb	(r1)+,r0
	beq	2f
	dec	r0
	beq	3f
	mov	$130,r0
	br	4f
3:
	mov	$117,r0
	br	4f
2:
	mov	r1,r0
	sub	$board-60,r0
4:
	jsr	pc,putc
	cmp	r1,$board+3
	beq	5f
	cmp	r1,$board+6
	beq	5f
	cmp	r1,$stp
	bne	1b
5:
	jsr	pc,nl
	cmp	r1,$stp
	bcs	1b
	jsr	pc,nl
	jsr	r5,puts
	<? \0>
	.even
	jsr	pc,getc
	cmp	r0,$12
	beq	move
	sub	$61,r0
	cmp	r0,$10
	bhi	illegal
	tstb	board(r0)
	bne	illegal
	mov	r0,r1
	jsr	pc,getc
	cmp	r0,$12
	bne	illegal
	movb	$2,board(r1)
move:
	jsr	r5,wcheck
	6
	br	youwin
	jsr	pc,fullchk
	clr	cnt
	clr	best
	mov	$board,r4
1:
	tstb	(r4)
	bne	next
	movb	$1,(r4)
	jsr	r5,wcheck
	3
	br	iwin
	jsr	pc,hash
	clrb	(r4)
	mov	$kbuf,r1
2:
	cmp	r1,kend
	bcc	3f
	cmp	r0,(r1)+
	bne	2b
	br	next
3:
	cmp	r0,best
	beq	next
	bcs	4f
	mov	r0,best
	mov	r4,bestc
4:
	inc	cnt
next:
	inc	r4
	cmp	r4,$stp
	bcs	1b
	tst	cnt
	beq	concede
	cmp	cnt,$1
	beq	5f
	mov	$stm,stp
5:
	mov	stp,r0
	mov	best,(r0)+
	mov	r0,stp
	mov	bestc,r0
	movb	$1,(r0)
	jmp	show

illegal:
	jsr	r5,puts
	<Illegal move\n\0>
	.even
	jsr	pc,skipnl
	jmp	show

youwin:
	jsr	r5,puts
	<You win\n\0>
	.even
	br	learn

concede:
	jsr	r5,puts
	<I concede\n\0>
	.even
learn:
	mov	kend,r1
	mov	$stm,r2
1:
	cmp	r2,stp
	bcc	2f
	mov	(r2)+,(r1)+
	br	1b
2:
	mov	r1,kend
	jmp	newgame

iwin:
	jsr	r5,puts
	<I win\n\0>
	.even
	jmp	newgame

hash:
	mov	$perms,r2
	clr	r3
1:
	mov	$11,r1
	mov	r5,-(sp)
	clr	r5
2:
	mul	$3,r5
	movb	(r2)+,r0
	movb	board(r0),r0
	add	r0,r5
	sob	r1,2b
	mov	r5,r0
	mov	(sp)+,r5
	cmp	r0,r3
	bcs	3f
	mov	r0,r3
3:
	cmp	r2,$perms+110
	bcs	1b
	mov	r3,r0
	rts	pc

fullchk:
	mov	$board,r0
1:
	tstb	(r0)+
	beq	2f
	cmp	r0,$stp
	bcs	1b
	jsr	r5,puts
	<Draw\n\0>
	.even
	jmp	newgame
2:
	rts	pc

wcheck:
	mov	$lines,r1
1:
	jsr	pc,cell
	mov	r0,r2
	jsr	pc,cell
	add	r0,r2
	jsr	pc,cell
	add	r0,r2
	cmp	r2,(r5)
	beq	2f
	cmp	r1,$lines+30
	bcs	1b
	tst	(r5)+
2:
	tst	(r5)+
	rts	r5

cell:
	movb	(r1)+,r0
	movb	board(r0),r0
	bne	1f
	mov	$12,r0
1:
	rts	pc

bye:
	sys	open; fname; 0
	bcs	3f
	mov	r0,r1
	mov	kend,1f
	sys	read
1:
	0
	1750
	mov	r1,r0
	sys	close
3:
	mov	$kbuf,r1
	mov	r1,r2
4:
	mov	(r2)+,r0
	beq	7f
	blt	4b
	mov	$kbuf,r3
5:
	tst	(r3)
	beq	6f
	cmp	r0,(r3)+
	bne	5b
	mov	$-1,-2(r3)
	br	5b
6:
	mov	r0,(r1)+
	br	4b
7:
	sub	$kbuf,r1
	mov	r1,wcnt
	jsr	pc,nl
	mov	r1,r0
	asr	r0
	jsr	pc,putnum
	jsr	r5,puts
	< 'bits' returned\n\0>
	.even
	sys	creat; fname; 17
	sys	write; kbuf
wcnt:
	0
	jmp	onint

perms:
	.byte	0,1,2,3,4,5,6,7,8
	.byte	6,3,0,7,4,1,8,5,2
	.byte	8,7,6,5,4,3,2,1,0
	.byte	2,5,8,1,4,7,0,3,6
	.byte	2,1,0,5,4,3,8,7,6
	.byte	8,5,2,7,4,1,6,3,0
	.byte	6,7,8,3,4,5,0,1,2
	.byte	0,3,6,1,4,7,2,5,8
lines:
	.byte	0,1,2
	.byte	3,4,5
	.byte	6,7,8
	.byte	0,3,6
	.byte	1,4,7
	.byte	2,5,8
	.byte	0,4,8
	.byte	2,4,6
fname:
	</usr/games/ttt.k\0>
board:
	.=.+9.
stp:
	.=.+2
stm:
	.=.+18.
kend:
	.=.+2
kbuf:
	.=.+1000.
cnt:
	.=.+2
best:
	.=.+2
bestc:
	.=.+2