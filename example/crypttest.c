main(argc, argv)
char *argv[];
{
	char *str;

	if (argc<=1 || argc>2) {
		eputs("arg count");
		exit(1);
	}

	str = malloc(9);
	if (str == 0) {
		eputs("out of memory");
		exit(1);
	}

	memcpy(str, crypt(argv[1]), 8);
	str[8] = '\0';

	puts(str);
	free(str);
}