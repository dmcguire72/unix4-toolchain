int sbuf[259];

sinit()
{
	sbuf[0] = 2;
	sbuf[1] = 0;
	sbuf[2] = 0;
}

eputs(s)
char *s;
{
	register char *p;
	register int n, k;

	if (s == 0)
		return(-1);
	p = s;
	while (*p)
		p++;
	n = p - s;
	while (n > 0) {
		k = write(2, s, n);
		if (k <= 0)
			return(-1);
		s =+ k;
		n =- k;
	}
	if (write(2, "\n", 1) != 1)
		return(-1);
	return(0);
}

puts(s)
char *s;
{
	while (*s)
		putc(*s++, sbuf);
	putc('\n', sbuf);
	fflush(sbuf);
}