


struct mtab {
	char	file[ 32 ];
	char	spec[ 32 ];
} mtab[ 16 ];

main(argc, argv)
char **argv;
{
	register struct mtab *mp;
	register char *p1, *p2;
	int mf;

	mf = open("/etc/mtab", 0);
	read(mf, mtab,  16 *2* 32 );
	if(argc != 2) {
		printf("arg count\n");
		return;
	}
	if (umount(argv[1]) < 0) {
		perror("umount");
		return;
	}
	p1 = argv[1];
	while(*p1++)
		;
	p1--;
	while(*--p1 == '/')
		*p1 = '\0';
	while(p1 > argv[1] && *--p1 != '/')
		;
	if(*p1 == '/')
		p1++;
	argv[1] = p1;
	for (mp = mtab; mp < &mtab[ 16 ]; mp++) {
		p1 = argv[1];
		p2 = &mp->spec[0];
		while (*p1++ == *p2)
			if (*p2++ == 0) {
				for (p1 = mp->file; p1 < &mp->file[ 32 *2];)
					*p1++ = 0;
				mp = &mtab[ 16 ];
				while ((--mp)->file[0] == 0);
				mf = creat("/etc/mtab", 0644);
				write(mf, mtab, (mp-mtab+1)*2* 32 );
				return;
			}
	}
	printf("%s not in mount table\n", argv[1]);
}
