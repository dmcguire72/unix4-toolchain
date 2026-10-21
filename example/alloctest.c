main(argc, argv)
char *argv[];
{
	char *str;

	str = calloc(4, 1);
	if (str == 0) {
		printf("out of memory\n");
		return 1;
	}

    str[0] = 'h';
    str[1] = 'i';
    str[2] = '!';
    str[3] = '\0';

	printf("%s\n", str);

	free(str);
	return 0;
}