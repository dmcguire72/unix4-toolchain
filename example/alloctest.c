main()
{
	char *str;

	sinit();
	
	str = strdup("Hi!");
	if (str == 0) {
		eputs("out of memory");
		exit(1);
	}
	puts(str);
	free(str);
}
