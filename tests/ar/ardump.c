#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	FILE *f;
	unsigned char h[16];
	unsigned long sum, n, i;
	int c, k;

	if (argc != 2 || (f = fopen(argv[1], "rb")) == NULL)
		return 2;
	if (fread(h, 1, 2, f) != 2 || h[0] != 0155 || h[1] != 0377) {
		printf("not an archive\n");
		return 0;
	}
	while (fread(h, 1, 16, f) == 16) {
		for (k = 0; k < 16; k++) {
			if (k == 12)
				printf("--");
			else
				printf("%02x", h[k]);
			if (k == 7 || k == 11 || k == 13)
				printf(" ");
		}
		n = h[14] | (h[15] << 8);
		sum = 0;
		for (i = 0; i < n; i++) {
			if ((c = getc(f)) == EOF)
				break;
			sum = sum * 31 + c;
		}
		printf(" data=%lu/%lx", i, sum & 0xffffffff);
		if (n & 1) {
			if ((c = getc(f)) == EOF)
				printf(" nopad");
			else
				printf(" pad=%02x", c);
		}
		printf("\n");
		if (i < n)
			break;
	}
	i = 0;
	while (getc(f) != EOF)
		i++;
	if (i)
		printf("trailing %lu\n", i);
	return 0;
}
