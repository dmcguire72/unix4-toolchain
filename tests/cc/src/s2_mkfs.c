
struct
{
	char	*s_isize;
	char	*s_fsize;
	int	s_nfree;
	int	s_free[100];
	int	s_ninode;
	int	s_inode[100];
	char	s_flock;
	char	s_ilock;
	char	s_fmod;
	int	s_time[2];
} filsys;

struct inode
{
	int	i_number;
	int	i_mode;
	char	i_nlink;
	char	i_uid;
	char	i_gid;
	char	i_size0;
	char	*i_size1;
	int	i_addr[8];
	int	i_time[4];
};














int	utime[2];
int	fin;
int	fsi;
int	fso;
char	*charp;
int	buf[256];
char	string[50];
char	*fsys;
char	*proto;

main(argc, argv)
char **argv;
{
	int f, n;



	

	time(utime);
	if(argc != 3) {
		printf("arg count\n");
		exit();
	}
	fsys = argv[1];
	proto = argv[2];
	fso = creat(fsys, 0666);
	if(fso < 0) {
		printf("%s: cannot create\n", fsys);
		exit();
	}
	fsi = open(fsys, 0);
	if(fsi < 0) {
		printf("%s: cannot open\n", fsys);
		exit();
	}
	fin = open(proto, 0);
	if(fin < 0) {
		n = 0;
		for(f=0; proto[f]; f++) {
			if(proto[f]<'0' || proto[f]>'9') {
				printf("%s: cannot open\n", proto);
				exit();
			}
			n = n*10 + proto[f]-'0';
		}
		filsys.s_fsize = n;
		filsys.s_isize = ldiv(0, n, 43+ldiv(0, n, 1000));
		printf("isize = %d\n", filsys.s_isize);
		charp = "d--777 0 0 $ ";
		goto f3;
	}




	

	getstr();
	f = open(string, 0);
	if(f < 0) {
		printf("%s: cannot  open init\n", string);
		goto f2;
	}
	read(f, buf, 020);
	if(buf[0] != 0407) {
		printf("%s: bad format\n", string);
		goto f1;
	}
	n = buf[1]+buf[2];
	if(n > 512) {
		printf("%s: too big\n", string);
		goto f1;
	}
	read(f, buf, n);
	wtfs(0, buf);

f1:
	close(f);




	

f2:
	filsys.s_fsize = getnum();
	filsys.s_isize = getnum();

f3:
	if(filsys.s_isize > filsys.s_fsize ||
	   filsys.s_fsize-filsys.s_isize-2 < filsys.s_isize) {
		printf("%l/%l: bad ratio\n", filsys.s_fsize, filsys.s_isize);
		exit();
	}
	free(0);
	for(n=filsys.s_fsize-1; n!=filsys.s_isize+1; n--)
		free(n);



	

	for(n=0; n<256; n++)
		buf[n] = 0;
	for(n=0; n!=filsys.s_isize; n++)
		wtfs(n+2, buf);
	cfile(0);



	

	for(n=0; n<256; n++)
		buf[n] = 0;
	filsys.s_time[0] = utime[0];
	filsys.s_time[1] = utime[1];
	wtfs(1, &filsys);
}

cfile(par)
struct inode *par;
{
	struct inode in;
	int db[256], ib[256];
	int dbc, ibc;
	static ino;
	int i, f, *p1, *p2;



	

	getstr();
	in.i_mode =  0100000 ;
	in.i_mode =| gmode(string[0], "bcd",  060000 ,  020000 ,  040000 );
	in.i_mode =| gmode(string[1], "u",  04000 );
	in.i_mode =| gmode(string[2], "g",  02000 );
	for(i=3; i<6; i++) {
		if(string[i]<'0' || string[i]>'7') {
			printf("%c/%s: bad digit\n", string[i], string);
			exit();
		}
		in.i_mode =| (string[i]-'0')<<(15-3*i);
	}
	in.i_uid = getnum();
	in.i_gid = getnum();




	

	ino++;
	in.i_number = ino;
	if(ldiv(0, ino, 16) > filsys.s_isize) {
		printf("too many inodes\n");
		exit();
	}
	in.i_nlink = 1;
	in.i_size0 = 0;
	in.i_size1 = 0;
	for(i=0; i<8; i++)
		in.i_addr[i] = 0;
	for(i=0; i<256; i++) {
		db[i] = 0;
		ib[i] = 0;
	}
	if(par == 0) {
		par = &in;
		in.i_nlink--;
	}
	dbc = 0;
	ibc = 0;
	switch(in.i_mode& 060000 ) {

	case 0:



		

		getstr();
		f = open(string, 0);
		if(f < 0) {
			printf("%s: cannot open\n", string);
			break;
		}
		while((i=read(f, db, 512)) > 0) {
			in.i_size1 =+ i;
			newblk(&dbc, db, &ibc, ib);
		}
		close(f);
		break;

	case  060000 :
	case  020000 :



		

		in.i_addr[0] = getnum()<<8;
		in.i_addr[0] =| getnum();
		break;

	case  040000 :





		

		par->i_nlink++;
		entry(par->i_number, "..", &dbc, db, &ibc, ib);
		in.i_nlink++;
		entry(in.i_number, ".", &dbc, db, &ibc, ib);
		in.i_size1 = 32;
		for(;;) {
			getstr();
			if(string[0]=='$' && string[1]=='\0')
				break;
			entry(ino+1, string, &dbc, db, &ibc, ib);
			in.i_size1 =+ 16;
			cfile(&in);
		}
		break;
	}
	if(dbc != 0)
		newblk(&dbc, db, &ibc, ib);
	if(ibc > 8) {
		in.i_mode =|  010000 ;
		dbc = alloc();
		wtfs(dbc, ib);
		in.i_addr[0] = dbc;
	} else
	for(i=0; i<ibc; i++)
		in.i_addr[i] = ib[i];
	in.i_time[0] = in.i_time[2] = utime[0];
	in.i_time[1] = in.i_time[3] = utime[1];
	i = in.i_number + 31;
	dbc = ldiv(0, i, 16);
	p1 = &buf[lrem(0, i, 16)*16];
	p2 = &in.i_mode;
	rdfs(dbc, buf);
	for(i=0; i<16; i++)
		*p1++ = *p2++;
	wtfs(dbc, buf);
}

gmode(c, s, m0, m1, m2, m3)
char c, *s;
{
	int i;

	for(i=0; s[i]!='\0'; i++)
		if(c == s[i])
			return((&m0)[i]);
	if(c == '-')
		return(0);
	printf("%c/%s: bad mode\n", c, string);
	exit();
}

getnum()
{
	int n, i;

	getstr();
	n = 0;
	i = 0;
	for(i=0; string[i]!='\0'; i++) {
		if(string[i]<'0' || string[i]>'9') {
			printf("%s: bad number\n", string);
			exit();
		}
		n = n*10 + string[i] - '0';
	}
	return(n);
}

getstr()
{
	int i, c;

loop:
	switch(c=getch()) {

	case ' ':
	case '\t':
	case '\n':
		goto loop;

	case '\0':
		printf("EOF\n");
		exit();

	case ':':
		while(getch() != '\n');
		goto loop;

	}
	i = 0;

	do {
		string[i++] = c;
		c = getch();
	} while(c!=' '&&c!='\t'&&c!='\n'&&c!='\0');
	string[i] = '\0';
}

rdfs(bno, bf)
{
	int n;

	seek(fsi, bno, 3);
	n = read(fsi, bf, 512);
	if(n != 512) {
		printf("read error: %l\n", bno);
		exit();
	}
}

wtfs(bno, bf)
{
	int n;

	seek(fso, bno, 3);
	n = write(fso, bf, 512);
	if(n != 512) {
		printf("write error: %l\n", bno);
		exit();
	}
}

alloc()
{
	int bno, i;

	filsys.s_nfree--;
	bno = filsys.s_free[filsys.s_nfree];
	filsys.s_free[filsys.s_nfree] = 0;
	if(bno == 0) {
		printf("out of free space\n");
		exit();
	}
	if(filsys.s_nfree <= 0) {
		rdfs(bno, buf);
		filsys.s_nfree = buf[0];
		for(i=0; i<100; i++)
			filsys.s_free[i] = buf[i+1];
	}
	return(bno);
}

free(bno)
{
	int i;

	if(filsys.s_nfree >= 100) {
		buf[0] = filsys.s_nfree;
		for(i=0; i<100; i++)
			buf[i+1] = filsys.s_free[i];
		wtfs(bno, buf);
		filsys.s_nfree = 0;
	}
	filsys.s_free[filsys.s_nfree] = bno;
	filsys.s_nfree++;
}

entry(ino, str, adbc, db, aibc, ib)
char *str;
int *adbc, *db, *aibc, *ib;
{
	char *s;
	int i;

	db[*adbc] = ino;
	(*adbc)++;
	s = &db[*adbc];
	for(i=0; i<14; i++) {
		*s++ = *str;
		if(*str != '\0')
			str++;
	}
	*adbc =+ 7;
	if(*adbc >= 256)
		newblk(adbc, db, aibc, ib);
}

newblk(adbc, db, aibc, ib)
int *adbc, *db, *aibc, *ib;
{
	int bno, i;

	bno = alloc();
	wtfs(bno, db);
	for(i=0; i<256; i++)
		db[i] = 0;
	*adbc = 0;
	ib[*aibc] = bno;
	(*aibc)++;
	if(*aibc >= 256) {
		printf("indirect block full\n");
		exit();
	}
}

getch()
{

	if(charp)
		return(*charp++);
	return(getchar());
}
