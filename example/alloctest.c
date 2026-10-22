char *msg "Hi!\0";
char *err "out of memory\0";

main(argc, argv)
char *argv[];
{
	char *str;

	sinit();
	
	str = strdup(msg);
	if (str == 0) {
		eputs(err);
		return -1;
	}
	puts(str);
	free(str);
	return 0;
}
