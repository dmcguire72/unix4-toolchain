


struct mtab {
	char	file[ 32 ];
	char	spec[ 32 ];
} mtab[ 16 ];

main(argc, argv)
char **argv;
{
	register int ro;
	register struct mtab *mp;
	register char *np;
	int n, mf;

	mf = open("/etc/mtab", 0);
	read(mf, mtab,  16 *2* 32 );
	if (argc==1) {
		for (mp = mtab; mp < &mtab[ 16 ]; mp++)
			if (mp->file[0])
				printf("%s on %s\n", mp->spec, mp->file);
		return;
	}
	if(argc < 3) {
		printf("arg count\n");
		return;
	}
	ro = 0;
	if(argc > 3)
		ro++;
	if(mount(argv[1], argv[2], ro) < 0) {
		perror("mount");
		return;
	}
	np = argv[1];
	while(*np++)
		;
	np--;
	while(*--np == '/')
		*np = '\0';
	while(np > argv[1] && *--np != '/')
		;
	if(*np == '/')
		np++;
	argv[1] = np;
	for (mp = mtab; mp < &mtab[ 16 ]; mp++) {
		if (mp->file[0] == 0) {
			for (np = mp->spec; np < &mp->spec[ 32 -1];)
				if ((*np++ = *argv[1]++) == 0)
					argv[1]--;
			for (np = mp->file; np < &mp->file[ 32 -1];)
				if ((*np++ = *argv[2]++) == 0)
					argv[2]--;
			mp = &mtab[ 16 ];
			while ((--mp)->file[0] == 0);
			mf = creat("/etc/mtab", 0644);
			write(mf, mtab, (mp-mtab+1)*2* 32 );
			return;
		}
	}
}
