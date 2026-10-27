






















































struct
{
	char	lobyte;
	char	hibyte;
};


struct proc {
	char	p_stat;
	char	p_flag;
	char	p_pri;
	char	p_sig;
	char	p_null;
	char	p_time;
	int	p_ttyp;
	int	p_pid;
	int	p_ppid;
	int	p_addr;
	int	p_size;
	int	p_wchan;
	int	*p_textp;
} proc[ 50 ];















struct clist {
	int	c_cc;
	int	c_cf;
	int	c_cl;
};

struct tty {
	struct	clist t_rawq;
	struct	clist t_canq;
	struct	clist t_outq;
	int	t_flags;
	int	*t_addr;
	char	t_delct;
	char	t_col;
	char	t_intrup;
	char	t_quit;
	char	t_state;
	char	t_char;
	int	t_speeds;
};

char partab[];





































struct user {
	int	u_rsav[2];		
	int	u_fsav[25];		
	char	u_segflg;
	char	u_error;
	char	u_uid;
	char	u_gid;
	char	u_ruid;
	char	u_rgid;
	int	u_procp;
	char	*u_base;
	char	*u_count;
	char	*u_offset[2];
	int	*u_cdir;
	char	u_dbuf[ 14 ];
	char	*u_dirp;
	struct	{
		int	u_ino;
		char	u_name[ 14 ];
	} u_dent;
	int	*u_pdir;
	int	u_uisa[8];
	int	u_uisd[8];
	int	u_ofile[ 15 ];
	int	u_arg[5];
	int	u_tsize;
	int	u_dsize;
	int	u_ssize;
	int	u_qsav[2];
	int	u_ssav[2];
	int	u_signal[ 13 ];
	int	u_utime;
	int	u_stime;
	int	u_cutime[2];
	int	u_cstime[2];
	int	*u_ar0;
	int	u_prof[4];
	char	u_nice;
	char	u_dsleep;
} u;	




































struct {
	char name[8];
	int  type;
	char  *value;
} nl[5];

struct proc proc[1];
struct user u;
int	lflg;
int	kflg;
int	xflg;
int	tflg;
int	aflg;
int	mem;
int	swap;

int	stbuf[257];
char	*coref;

main(argc, argv)
char **argv;
{
	struct proc *p;
	int n, b;
	int i, c, mtty;
	char *ap;
	int hmem;

	if (argc>1) {
		ap = argv[1];
		while (*ap) switch (*ap++) {
		case 'a':
			aflg++;
			break;

		case 't':
			tflg++;
			break;

		case 'x':
			xflg++;
			break;

		case 'l':
			lflg++;
			break;

		case 'k':
			kflg++;
			break;

		}
	}
	setup(&nl[0], "_proc");
	setup(&nl[1], "_dc11");
	setup(&nl[2], "_kl11");
	setup(&nl[3], "_dh11");
	nlist("/unix", nl);
	if (nl[0].type==0) {
		printf("No namelist\n");
		return;
	}
	coref = "/dev/mem";
	if(kflg)
		coref = "/dev/rk0";
	if ((hmem = open(coref, 0)) < 0) {
		printf("No mem\n");
		return;
	}
	mem = open(coref, 0);
	swap = open("/dev/rf0", 0);
	n = &proc[1];
	b = &proc;
	n =- b;
	seek(hmem, nl[0].value, 0);
	mtty = ttyn(0);
	for (i=0; i< 50 ; i++) {
		read(hmem, proc, n);
		if (proc[0].p_stat==0)
			continue;
		if ((c=ttyc(proc[0].p_ttyp))=='?' && xflg==0)
			continue;
		if (mtty!=c && aflg==0)
			continue;
		if (lflg) {
			printf("%3o%5d%5d",
				proc[0].p_flag<<3|proc[0].p_stat,
				proc[0].p_pri,
				proc[0].p_time);
		}
		if (aflg)
			printf("%2c", c);
		printf("%6d", proc[0].p_pid);
		if (lflg) {
			printf("%4d", (proc[0].p_size+7)>>3);
			if (proc[0].p_wchan)
				printf("%8o", proc[0].p_wchan);
			else
				printf("        ");
		}
		prcom();
		printf("\n");
	}
}

ttyc(p)
struct tty *p;
{
	int n;
	struct { struct tty xdc11[]; };

	n = p - nl[1].value->xdc11;
	if (n>=0 && n< 10 ) {
		if (n<8)
			return(n+'0');
		return(n+'a'-8);
	}
	n = p - nl[2].value->xdc11;
	if(n>=0 && n< 4 )
		return('8'+n);
	n = p - nl[3].value->xdc11;
	if(n>=0 && n< 16 )
		return('f'+n);
	return('?');
}

setup(p, s)
char *p, *s;
{
	while (*p++ = *s++);
}

prcom()
{
	int baddr, laddr, mf;
	int *ip;
	char *cp, *cp1;
	int c;

	baddr = 0;
	laddr = 0;
	if (proc[0].p_flag& 01 ) {
		laddr = proc[0].p_addr;
		mf = mem;
	} else {
		baddr = proc[0].p_addr;
		mf = swap;
	}
	laddr =+ proc[0].p_size - 8;
	baddr =+ laddr>>3;
	laddr = (laddr&07)<<6;
	seek(mf, baddr, 3);
	seek(mf, laddr, 1);
	if (read(mf, stbuf, 512) != 512)
		return(0);
	for (ip = &stbuf[256]; ip > &stbuf[0];) {
		if (*--ip == -1) {
			cp = ip+1;
			if (*cp==0)
				cp++;
			for (cp1 = cp; cp1 < &stbuf[256]; cp1++) {
				c = *cp1;
				if (c==0)
					*cp1 = ' ';
				else if (c < ' ' || c > 0176)
					*cp1 = '?';
			}
			printf(" %.32s ", cp);
			return(1);
		}
	}
	return(0);
}
