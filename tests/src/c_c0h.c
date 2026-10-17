

























struct tnode {
	int op;
	int type;
	int dimp;
	struct tnode *tr1, *tr2;
};

struct {
	int fop;
	int ftype;
	char ssp;		
	char lenp;		
};

struct tname {
	int nop;
	int ntype;
	int ndimp;
	int class;
	int offset;
	int nloc;
};


struct tconst {
	int cop;
	int ctype;
	int cdimp;
	int value;
};

struct hshtab {
	int hclass;
	int htype;
	int hdimp;
	int hoffset;
	char name[ 8 ];
};

struct swtab {
	int swlab;
	int swval;
};

char	cvtab[];
int	opdope[];
char	ctab[];
char	symbuf[ 8 ];
int	hshused;
struct	hshtab	hshtab[ 200 ];
int	*space;
int	*cp;
int	cmst[ 40 ];
int	isn;
struct	swtab	swtab[ 200 ];
struct	swtab	*swp;
int	contlab;
int	brklab;
int	retlab;
int	deflab;
int	nauto;
int	autolen;
int	peeksym;
int	peekc;
int	eof;
int	line;
int	*treebase;
int	debug;
struct	hshtab	*defsym;
struct	hshtab	*funcsym;
int	xdflg;
int	proflg;
struct	hshtab	*csym;
int	cval;
double	fcval;
int	nchstr;
int	nerror;
struct	hshtab	*paraml;
struct	hshtab	*parame;
int	strflg;
int	osleft;
int	mosflg;
int	initflg;
int	inhdr;
int	dimtab[ 100 ];
char	binbuf[518];
char	ascbuf[518];
int	dimp;
int	regvar;

























































































































































