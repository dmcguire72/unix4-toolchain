





int linect[2];
int alphct[2];
int punct[2];
int wordct[2];
int ctlct[2];

int roffflg 0;
int codeflg 0;

char *code "-lw";

char class[128] {
	 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	 1 , 2 , 16 , 1 , 1 , 1 , 1 , 1 ,
	 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	 2 , 4 , 4 , 4 , 4 , 4 , 4 , 4 ,
	 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 ,
	 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
	 8 , 8 , 4 , 4 , 4 , 4 , 4 , 4 ,
	 4 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
	 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
	 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
	 8 , 8 , 8 , 4 , 4 , 4 , 4 , 8 ,
	 4 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
	 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
	 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 ,
	 8 , 8 , 8 , 4 , 4 , 4 , 4 , 1 
};
char buf[518];

main(argc,argv)
char **argv;
{
	int i, j;
	register int c;
	if(--argc>=1 & **++argv=='-') {	
		for(i=1;c=argv[0][i];i++)
			if(c=='r') roffflg = 1;
			else codeflg = 1;
		if(codeflg) code = argv[0];
		argc--;
		argv++;
	}
	i = 0;
	do {
		if(argc<=0) buf[0] = 0;
		else if(fopen(argv[i],buf)<0) {
			printf("%s: cannot open\n", argv[i]);
			continue;
		}
		count();
		for(j=1;code[j];j++) {
			switch(code[j]) {
			case 'l':
				putd(linect);
				break;
			case 'a':
				putd(alphct);
				break;
			case 'p':
				putd(punct);
				break;
			case 'w':
				putd(wordct);
				break;
			case 'c':
				putd(ctlct);
				break;
			}
		}
		printf("%s\n", argc<=0?"":argv[i]);
	} while(++i<argc);
}

count()
{
	register newline, token, c;
	newline = 1;
	token = 0;
	clear(linect);
	clear(alphct);
	clear(punct);
	clear(wordct);
	clear(ctlct);
	while((c=getc(buf))>=0) {
		c =& 0177;
		if(newline) {
			if(roffflg&&(c=='.'||c=='\'')) {
				inc(ctlct);
				while(((c=getc(buf)>=0))&&c!='\n');
				continue;
			}
			else {
				newline = 0;
				inc(linect);
			}
		}
		if(c<0) continue;
		c = class[c];
		if(c& 1 ) continue;
		if(c&( 8 | 4 )) {
			if(token==c) continue;
			if(token==0) inc(wordct);
			token = c;
			if(c& 8 ) inc(alphct);
			else inc(punct);
			continue;
		}
		if(c&( 2 | 16 )) {
			token = 0;
			if(c& 16 ) newline = 1;
		}
	}
}

clear(ct)
int ct[2];
{
	ct[0] = ct[1] = 0;
}

inc(ct)
int ct[2];
{
	ct[0]++;
	if(ct[0]==0) ct[1]++;
}

putd(ct)
int ct[2];
{
	printf("%7s ",locv(ct[1],ct[0]));
}
