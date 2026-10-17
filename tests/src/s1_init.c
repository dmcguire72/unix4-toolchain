





char	shell[]	"/bin/sh";
char	minus[]	"-";
char	runc[]	"/etc/rc";
char	init[]	"/etc/init";
char	ifile[]	"/etc/ttys";
char	utmp[]	"/tmp/utmp";
char	wtmpf[]	"/usr/adm/wtmp";
char	ctty[]	"/dev/tty8";
char	*com[ 1 ]
{
	"/etc/getty",
};
int	fi;
struct
{
	int	flag;
	int	line;
	int	comn;
} line;
struct	tab
{
	int	pid;
	int	line;
	int	comn;
} itab[ 20 ];

struct {
	char	name[8];
	char	tty;
	char	fill;
	int	time[2];
	int	wfill;
} wtmp;

main()
{
	register i;
	register struct tab *p, *q;
	int reset();




	

	if(getcsw() !=  0173030 ) {
		i = fork();
		if(i == 0) {
			execl(shell, shell, runc, 0);
			exit();
		}
		while(wait() != i);
		close(creat(utmp, 0644));
		if ((i = open(wtmpf, 1)) >= 0) {
			seek(i, 0, 2);
			wtmp.tty = '~';
			time(wtmp.time);
			write(i, &wtmp, 16);
			close(i);
		}
	}





	

	setexit();
	signal(1, reset);
	for(i=0; i<10; i++)
		close(i);
	switch(getcsw()) {

	case  0173030 :
	error:
		termall();
		i = fork();
		if(i == 0) {
			open(ctty, 2);
			dup(0);
			execl(shell, minus, 0);
			exit();
		}
		while(wait() != i);

	case  0173040 :
		termall();
		execl(init, minus, 0);
		reset();
	}



	

	fi = open(ifile, 0);
	q = &itab[0];
	while(rline()) {
		if(line.flag == '0')
			continue;
		for( p = &itab[0]; p < &itab[20]; p++ )
			if(p->line==line.line || p->line==0) {
				if(p >= q) {
					i = p->pid;
					p->pid = q->pid;
					q->pid = i;
					p->line = q->line;
					p->comn = q->comn;
					q->line = line.line;
					q->comn = line.comn;
					q++;
				}
				break;
			}
	}
	close(fi);
	if(q == &itab[0])
		goto error;
	for(; q < &itab[ 20 ]; q++)
		term(q);
	for( p = &itab[0]; p < &itab[20]; p++ )
		if(p->line != 0 && p->pid == 0)
			dfork(p);
	for( ;; ) {
		i = wait();
		for( p = &itab[0]; p < &itab[20]; p++ )
			if(p->pid == i) {
				rmut(p);
				dfork(p);
			}
	}
}

termall()
{
	register struct tab *p;

	for( p = &itab[0]; p < &itab[20]; p++ )
		term(p);
}

term(ap)
struct tab *ap;
{
	register struct tab *p;

	p = ap;
	if(p->pid != 0) {
		rmut(p);
		kill(p->pid, 9);
	}
	p->pid = 0;
	p->line = 0;
}

rline()
{
	static char c[4];

	if(read(fi, c, 4) != 4 || c[3] != '\n')
		return(0);
	line.flag = c[0];
	line.line = c[1];
	line.comn = c[2] - '0';
	if(line.comn<0 || line.comn>= 1 )
		line.comn = 0;
	return(1);
}

dfork(ap)
struct tab *ap;
{
	register i;
	register char *tty;
	register struct tab *p;

	p = ap;
	i = fork();
	if(i == 0) {
		signal(1, 0);
		tty = "/dev/ttyx";
		tty[8] = p->line;
		chown(tty, 0);
		chmod(tty, 0622);
		open(tty, 2);
		dup(0);
		execl(com[p->comn], minus, 0);
		exit();
	}
	p->pid = i;
}

rmut(p)
struct tab *p;
{
	register i, f;
	static char zero[16];

	f = open(utmp, 1);
	if(f >= 0) {
		i = p->line;
		if(i >= 'a')
			i =+ '0' + 10 - 'a';
		seek(f, (i-'0')*16, 0);
		write(f, zero, 16);
		close(f);
	}
	f = open(wtmpf, 1);
	if (f >= 0) {
		wtmp.tty = p->line;
		time(wtmp.time);
		seek(f, 0, 2);
		write(f, &wtmp, 16);
		close(f);
	}
}
