x 5;
v[3] 1,2,3;
buf[10];

f(a, b)
char b[]; {
	auto i, c, vec[4];
	static s;
	extern x, v[];

	i = 0;
	while (i < 10 & b[i] != 'a') {
		c = b[i++];
		if (c == 'xy')
			goto done;
		else
			vec[i&3] =+ c * 3;
	}
	switch (a) {
	case 1:
		x =| 07;
		break;
	case 2:
		x = -1;
	default:
		s = a>0 ? v[1] : x;
	}
done:
	return(vec[0] + s);
}
