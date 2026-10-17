









































char	cbuf[26];
int	dmsize[12]
{
	31,
	28,
	31,
	30,
	31,
	30,
	31,
	31,
	30,
	31,
	30,
	31
};

int timezone	5*60*60;
int tzname[]
{
	"EST",
	"EDT",
};
int	daylight 1;	
int	nixonflg 1;	











ctime(at)
int *at;
{
	return(asctime(localtime(at)));
}

localtime(tim)
int tim[];
{
	register int *t, *ct, dayno;
	int daylbegin, daylend;
	int copyt[2];

	t = copyt;
	t[0] = tim[0];
	t[1] = tim[1];
	dpadd(t, -timezone);
	ct = gmtime(t);
	dayno = ct[ 7 ];
	if (nixonflg && (ct[ 5 ]>74 || ct[ 5 ]==74 && (dayno > 5 ||
	    dayno==5 && ct[ 2 ]>=2))) {
		daylight =| 1;
		daylbegin = -1;
		daylend = 367;
	} else {
		daylbegin = sunday(ct, 119);	
		daylend = sunday(ct, 303);	
	}
	if (daylight &&
	    (dayno>daylbegin || (dayno==daylbegin && ct[ 2 ]>=2)) &&
	    (dayno<daylend || (dayno==daylend && ct[ 2 ]<1))) {
		dpadd(t, 1*60*60);
		ct = gmtime(t);
		ct[ 8 ]++;
	}
	return(ct);
}

sunday(at, ad)
int *at;
{
	register int *t, d;

	t = at;
	d = ad;
	d = ad + dysize(t[ 5 ]) - 365;
	return(d - (d - t[ 7 ] + t[ 6 ] + 700) % 7);
}

gmtime(tim)
int tim[];
{
	register int d0, d1;
	register *tp;
	static xtime[9];
	extern int ldivr;





	

	d0 = ldiv(tim[0], tim[1], 28800);
	d1 = ldivr;
	tp = &xtime[0];



	

	*tp++ = d1%60;
	d1 =/ 60;
	*tp++ = d1%60;
	d1 =/ 60;
	d1 =+ (d0%3)*8;
	d0 =/ 3;
	*tp++ = d1;




	

	xtime[ 6 ] = (d0+4)%7;



	
	for(d1=70; d0 >= dysize(d1); d1++)
		d0 =- dysize(d1);
	xtime[ 5 ] = d1;
	xtime[ 7 ] = d0;



	

	if (dysize(d1)==366)
		dmsize[1] = 29;
	for(d1=0; d0 >= dmsize[d1]; d1++)
		d0 =- dmsize[d1];
	dmsize[1] = 28;
	*tp++ = d0+1;
	*tp++ = d1;
	xtime[ 8 ] = 0;
	return(xtime);
}

asctime(t)
int *t;
{
	register char *cp, *ncp;
	register int *tp;

	cp = cbuf;
	for (ncp = "Day Mon 00 00:00:00 1900\n"; *cp++ = *ncp++;);
	ncp = &"SunMonTueWedThuFriSat"[3*t[6]];
	cp = cbuf;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp++;
	tp = &t[4];
	ncp = &"JanFebMarAprMayJunJulAugSepOctNovDec"[(*tp)*3];
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp = numb(cp, *--tp);
	cp = numb(cp, *--tp+100);
	cp = numb(cp, *--tp+100);
	cp = numb(cp, *--tp+100);
	cp =+ 2;
	cp = numb(cp, t[ 5 ]);
	return(cbuf);
}

dysize(y)
{
	if((y%4) == 0)
		return(366);
	return(365);
}

numb(acp, n)
{
	register char *cp;

	cp = acp;
	cp++;
	if (n>=10)
		*cp++ = (n/10)%10 + '0';
	else
		*cp++ = ' ';
	*cp++ = n%10 + '0';
	return(cp);
}
