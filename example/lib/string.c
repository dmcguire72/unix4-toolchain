#define NULL    0

strlen(str)
char *str;
{
	register int l;
	l=0;

	while(str[l]) l++;

	return l;
}

char*
strcpy(dst, src)
char *dst;
char *src;
{
	register int l;

	if (dst == NULL || src == NULL) 
		return NULL;

	l=strlen(src);
	l++;

	memcpy(dst, src, l);
	return dst;
}

char*
strdup(src)
char *src;
{
	register char *dst;
	register int l;

	if (src == NULL) return NULL;

	l=strlen(src);
	l++;

	dst = malloc(l);
	if (dst == NULL) return NULL;
	memcpy(dst, src, l);

	return dst;
}
