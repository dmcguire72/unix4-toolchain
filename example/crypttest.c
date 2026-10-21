main(argc, argv)
char *argv[];
{
	char *str;

	if (argc<=1 || argc>2) {
		printf("arg count\n");
		return 1;
	}

	str = malloc(9);
	if (str == 0) {
		printf("out of memory\n");
		return 1;
	}

	memcpy(str, crypt(argv[1]), 8);
	str[8] = '\0';

	printf("%s\n", str);

	free(str);
	return 0;
}