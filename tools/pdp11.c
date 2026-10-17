#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>

static uint8_t mem[65536];
static uint16_t R[8];
static int fN, fZ, fV, fC;
static const char *root = "";
static int stop_at_exec;
static int fdtab[16];
static uint16_t sigtab[64];
static int trace;

#define SP R[6]
#define PC R[7]

static void die(const char *m, ...) __attribute__((noreturn));
static void die(const char *m, ...)
{
	fprintf(stderr, "pdp11: %s (pc=%06o)\n", m, PC);
	exit(99);
}

static uint16_t rdw(uint16_t a)
{
	if (a & 1) die("odd address read");
	return mem[a] | mem[a + 1] << 8;
}
static void wrw(uint16_t a, uint16_t v)
{
	if (a & 1) die("odd address write");
	mem[a] = v; mem[a + 1] = v >> 8;
}

static int regdst;
static uint16_t ea;

static void getops(int mode, int reg, int size)
{
	int inc = (reg >= 6 && size < 2) ? 2 : size;
	uint16_t x;

	regdst = -1;
	switch (mode) {
	case 0: regdst = reg; return;
	case 1: ea = R[reg]; return;
	case 2: ea = R[reg]; R[reg] += inc; return;
	case 3: ea = rdw(R[reg]); R[reg] += 2; return;
	case 4: R[reg] -= inc; ea = R[reg]; return;
	case 5: R[reg] -= 2; ea = rdw(R[reg]); return;
	case 6: x = rdw(PC); PC += 2; ea = R[reg] + x; return;
	case 7: x = rdw(PC); PC += 2; ea = rdw(R[reg] + x); return;
	}
}
static void getop(int mode, int reg, int byte)
{
	getops(mode, reg, byte ? 1 : 2);
}
static uint16_t load(int byte)
{
	if (regdst >= 0) return byte ? R[regdst] & 0377 : R[regdst];
	return byte ? mem[ea] : rdw(ea);
}
static void store(uint16_t v, int byte)
{
	if (regdst >= 0) {
		if (byte) R[regdst] = (R[regdst] & 0177400) | (v & 0377);
		else R[regdst] = v;
	} else if (byte) mem[ea] = v;
	else wrw(ea, v);
}
static void storeb_sext(uint16_t v)
{
	if (regdst >= 0) R[regdst] = (int8_t)v;
	else mem[ea] = v;
}
static void nz(uint16_t v, int byte)
{
	if (byte) { fN = (v & 0200) != 0; fZ = (v & 0377) == 0; }
	else { fN = (v & 0100000) != 0; fZ = v == 0; }
}

static void mappath(uint16_t a, char *out)
{
	char tmp[512]; int i = 0;
	while (mem[(uint16_t)(a + i)] && i < 500) { tmp[i] = mem[(uint16_t)(a + i)]; i++; }
	tmp[i] = 0;

	if (root[0] == '\0') {
		snprintf(out, 600, "%s", tmp);
	} else {
		if (tmp[0] == '/') {
			snprintf(out, 600, "%s%s", root, tmp);
		} else {
			if (root[strlen(root)-1] == '/') {
				snprintf(out, 600, "%s%s", root, tmp);
			} else {
				snprintf(out, 600, "%s/%s", root, tmp);
			}
		}
	}
}

static int allocfd(int hostfd)
{
	int i;
	for (i = 3; i < 16; i++)
		if (fdtab[i] < 0) { fdtab[i] = hostfd; return i; }
	close(hostfd);
	return -1;
}

static int load_image(const char *path, int argc, char **argv);

static void seterr(int e) { fC = 1; R[0] = e; }

static int open_v4_dir(const char *path) {
	DIR *dir = opendir(path);
	if (!dir) return -1;
	
	char tmptpl[] = "/tmp/v4dirXXXXXX";
	int fd = mkstemp(tmptpl);
	if (fd < 0) { closedir(dir); return -1; }
	
	unlink(tmptpl); 
	
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
		uint8_t buf[16] = {0};
		uint16_t ino = ent->d_ino ? (ent->d_ino & 0xffff) : 1;
		if (ino == 0) ino = 1;
		
		buf[0] = ino & 0xff;
		buf[1] = (ino >> 8) & 0xff;
		size_t namelen = strlen(ent->d_name);
		if (namelen > 14) namelen = 14;
		memcpy(buf + 2, ent->d_name, namelen);
		write(fd, buf, 16);
	}
	closedir(dir);

	lseek(fd, 0, SEEK_SET);
	return fd;
}

static void v4sys(int n)
{
	static const int nargs[64] = {
		0,0,0,2,2,2,0,0,2,2,1,2,1,0,3,2,2,1,2,2,0,3,1,0,0,0,0,0,1,0,1,1,
		1,0,0,0,0,1,0,0,0,0,0,1,4,0,0,0,2 };
	uint16_t a[4]; int i; char p[600]; int r, fd;

	{
		uint16_t ap;

		if (n == 0) {
			uint16_t t = rdw(PC);
			PC += 2;
			n = rdw(t) & 077;
			ap = t + 2;
			for (i = 0; i < nargs[n]; i++) { a[i] = rdw(ap); ap += 2; }
		} else {
			for (i = 0; i < nargs[n]; i++) { a[i] = rdw(PC); PC += 2; }
		}
	}
	fC = 0;
	switch (n) {
	case 1:		/* exit */
		fflush(stdout);
		exit(R[0] & 0377);
	case 2: {	/* fork */
		r = fork();
		if (r < 0) { seterr(11); return; } /* EAGAIN */
		if (r == 0) { 
			R[0] = getppid(); 
		} else { 
			R[0] = r; 
			PC += 2; 
		}
		return;
	}
	case 3:		/* read */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		r = read(fdtab[fd], mem + a[0], a[1]);
		if (r < 0) seterr(5); else R[0] = r;
		return;
	case 4:		/* write */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		r = write(fdtab[fd], mem + a[0], a[1]);
		if (r < 0) seterr(5); else R[0] = r;
		return;
	case 5: { /* open */
		struct stat st;
		mappath(a[0], p);
		if (stat(p, &st) == 0 && S_ISDIR(st.st_mode)) {
			r = open_v4_dir(p);
		} else {
			r = open(p, a[1] == 0 ? O_RDONLY : a[1] == 1 ? O_WRONLY : O_RDWR);
		}
		if (r < 0) { seterr(2); return; }
		fd = allocfd(r);
		if (fd < 0) seterr(24); else R[0] = fd;
		return;
	}
	case 7: {	/* wait */
		int st;
		r = wait(&st);
		if (r < 0) { seterr(10); return; } /* ECHILD */
		R[0] = r;
		R[1] = st;
		return;
	}
	case 8:		/* creat */
		mappath(a[0], p);
		r = open(p, O_RDWR | O_CREAT | O_TRUNC, 0600);
		if (r < 0) { seterr(2); return; }
		chmod(p, a[1] & 0777 ? (a[1] & 0777) | 0200 : 0600);
		fd = allocfd(r);
		if (fd < 0) seterr(24); else R[0] = fd;
		return;
	case 6:		/* close */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		if (fd > 2) close(fdtab[fd]);
		fdtab[fd] = -1;
		return;
	case 9: {	/* link */
		char q[600];
		mappath(a[0], p); mappath(a[1], q);
		if (link(p, q) < 0) seterr(errno == EEXIST ? 17 : 2);
		return;
	}
	case 10:	/* unlink */
		mappath(a[0], p);
		if (unlink(p) < 0) seterr(2);
		return;
	case 15:	/* chmod */
		mappath(a[0], p);
		if (chmod(p, a[1] & 0777) < 0) seterr(2);
		return;
	case 17:	/* break */
		return;
	case 18: {	/* stat */
		struct stat st;
		mappath(a[0], p);
		if (stat(p, &st) < 0) { seterr(2); return; }
		memset(mem + a[1], 0, 34);
		
		uint16_t v4_mode = 0100000;
		if (S_ISDIR(st.st_mode)) v4_mode |= 040000;
		else if (S_ISCHR(st.st_mode)) v4_mode |= 020000;
		else if (S_ISBLK(st.st_mode)) v4_mode |= 060000;
		v4_mode |= (st.st_mode & 0777);
		
		wrw(a[1] + 4, v4_mode);
		mem[a[1] + 6] = st.st_nlink;
		mem[a[1] + 7] = st.st_uid;
		mem[a[1] + 8] = st.st_gid;
		wrw(a[1] + 12, st.st_size >> 16);
		wrw(a[1] + 14, st.st_size & 0xffff);
		return;
	}
	case 19: {	/* seek */
		int w = a[1]; long off;
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		off = (w > 2) ? (long)a[0] * 512 : (w == 0 || w == 3) ? (long)a[0] : (int16_t)a[0];
		r = lseek(fdtab[fd], off, (w % 3) == 0 ? SEEK_SET : (w % 3) == 1 ? SEEK_CUR : SEEK_END);
		if (r < 0) seterr(22);
		return;
	}
	case 24:	/* getuid */
		R[0] = 0; return;
	case 48: {	/* signal */
		uint16_t old = sigtab[a[0] & 63];
		sigtab[a[0] & 63] = a[1];
		R[0] = old;
		return;
	}
		case 12: {	/* chdir */
		mappath(a[0], p);
		if (chdir(p) < 0) seterr(2); /* ENOENT */
		return;
	}
	case 13: {	/* time */
		time_t t = time(NULL);
		R[0] = (t >> 16) & 0xffff;
		R[1] = t & 0xffff;
		return;
	}
	case 23:	/* setuid */
	case 46:	/* setgid */
		return; 
	case 28: {	/* fstat */
		struct stat st;
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		if (fstat(fdtab[fd], &st) < 0) { seterr(2); return; }
		memset(mem + a[0], 0, 34);
		
		uint16_t v4_mode = 0100000;
		if (S_ISDIR(st.st_mode)) v4_mode |= 040000;
		else if (S_ISCHR(st.st_mode)) v4_mode |= 020000;
		else if (S_ISBLK(st.st_mode)) v4_mode |= 060000;
		v4_mode |= (st.st_mode & 0777);
		
		wrw(a[0] + 4, v4_mode);
		mem[a[0] + 6] = st.st_nlink;
		mem[a[0] + 7] = st.st_uid;
		mem[a[0] + 8] = st.st_gid;
		wrw(a[0] + 12, st.st_size >> 16);
		wrw(a[0] + 14, st.st_size & 0xffff);
		return;
	}
	case 31:	/* stty */
	case 32:	/* gtty */
		seterr(25); 
		return;
	case 36:	/* sync */
		sync();
		return;
	case 41:	/* dup */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		r = dup(fdtab[fd]);
		if (r < 0) { seterr(24); return; } /* EMFILE */
		fd = allocfd(r);
		if (fd < 0) seterr(24); else R[0] = fd;
		return;
	case 42: {	/* pipe */
		int pfd[2];
		if (pipe(pfd) < 0) { seterr(24); return; } /* EMFILE */
		int fd0 = allocfd(pfd[0]);
		int fd1 = allocfd(pfd[1]);
		if (fd0 < 0 || fd1 < 0) { seterr(24); return; }
		R[0] = fd0;
		R[1] = fd1;
		return;
	}
	case 47:	/* getgid */
		R[0] = 0; 
		return;
	case 11: {	/* exec */
		char *av[40]; int na = 0; uint16_t ap = a[1], s;
		static char strs[40][512];
		mappath(a[0], p);
		while ((s = rdw(ap)) != 0 && s != 0177777) {
			int k = 0;
			while (mem[(uint16_t)(s + k)] && k < 510) { strs[na][k] = mem[(uint16_t)(s + k)]; k++; }
			strs[na][k] = 0; av[na] = strs[na]; na++; ap += 2;
		}
		if (stop_at_exec) {
			printf("EXEC %s", p);
			for (i = 0; i < na; i++) printf(" %s", av[i]);
			printf("\n");
			fflush(stdout);
			exit(0);
		}
		if (load_image(p, na, av) < 0) { seterr(2); return; }
		return;
	}
	default:
		fprintf(stderr, "pdp11: unimplemented syscall %d at %06o\n", n, PC);
		exit(98);
	}
}

static int load_image(const char *path, int argc, char **argv)
{
	FILE *f = fopen(path, "rb");
	uint8_t h[16]; size_t n; int i, k; uint16_t sp, magic, ts, ds, bs;
	int nc = 0;

	if (!f) return -1;
	if (fread(h, 1, 16, f) != 16) { fclose(f); return -1; }
	magic = h[0] | h[1] << 8; ts = h[2] | h[3] << 8; ds = h[4] | h[5] << 8;
	bs = h[6] | h[7] << 8; (void)bs;
	if (magic != 0407 && magic != 0410) { fclose(f); return -1; }
	memset(mem, 0, sizeof mem);
	if (magic == 0410) {
		size_t dbase = ((size_t)ts + 8191) & ~(size_t)8191;

		n = fread(mem, 1, ts, f);
		if (dbase + ds + bs > 0160000) { fclose(f); die("image too large"); }
		n = fread(mem + dbase, 1, ds, f);
	} else {
		n = fread(mem, 1, (size_t)ts + ds, f);
	}
	(void)n;
	fclose(f);
	for (i = 0; i < argc; i++) nc += strlen(argv[i]) + 1;
	if (nc & 1) nc++;
	sp = (uint16_t)(-nc - argc * 2 - 4);
	SP = sp;
	wrw(sp, argc);
	k = (uint16_t)-nc;
	{
		uint16_t ap = sp, c = (uint16_t)-nc;
		for (i = 0; i < argc; i++) {
			ap += 2; wrw(ap, c);
			for (const char *s = argv[i]; ; s++) { mem[c++] = *s; if (!*s) break; }
		}
		wrw(ap + 2, 0177777);
	}
	(void)k;
	for (i = 0; i < 6; i++) R[i] = 0;
	PC = 0;
	for (i = 0; i < 64; i++) if (!(sigtab[i] & 1)) sigtab[i] = 0;
	fN = fZ = fV = fC = 0;
	return 0;
}

static void setcc_add(uint32_t s, uint32_t d, uint32_t r, int byte)
{
	uint32_t m = byte ? 0x80 : 0x8000, mask = byte ? 0xff : 0xffff;
	r &= 0x1ffff; (void)mask;
	fV = ((s & m) == (d & m)) && ((r & m) != (s & m));
	fC = (r >> (byte ? 8 : 16)) & 1;
}

static void branch(int cond, int off)
{
	if (cond) PC += 2 * (int8_t)off;
}

static double ac[8];
static int fpFD, fpFL, fpFT, fpN, fpZ, fpV, fpC;

static double fget(uint16_t a, int dbl)
{
	uint16_t w0 = rdw(a);
	int e = (w0 >> 7) & 0xff;
	uint64_t m;
	double v;

	if (e == 0) return 0.0;
	m = 0x80 | (w0 & 0x7f);
	m = (m << 16) | rdw(a + 2);
	if (dbl) {
		m = (m << 16) | rdw(a + 4);
		m = (m << 16) | rdw(a + 6);
		v = ldexp((double)m, e - 128 - 56);
	} else
		v = ldexp((double)m, e - 128 - 24);
	return (w0 & 0x8000) ? -v : v;
}

static void fput(uint16_t a, double v, int dbl)
{
	int e, sign = v < 0;
	double m;
	uint64_t mant;
	uint16_t w0;

	if (v == 0 || !isfinite(v)) {
		wrw(a, 0); wrw(a + 2, 0);
		if (dbl) { wrw(a + 4, 0); wrw(a + 6, 0); }
		return;
	}
	if (sign) v = -v;
	m = frexp(v, &e);		/* v = m * 2^e, 0.5 <= m < 1 */
	e += 128;
	if (dbl) mant = (uint64_t)ldexp(m, 56);
	else mant = (uint64_t)floor(ldexp(m, 24) + (fpFT ? 0 : 0.5));
	if (!dbl && mant >> 24) { mant >>= 1; e++; }
	if (e <= 0 || e > 255) {
		wrw(a, 0); wrw(a + 2, 0);
		if (dbl) { wrw(a + 4, 0); wrw(a + 6, 0); }
		return;
	}
	if (dbl) {
		w0 = (sign << 15) | (e << 7) | ((mant >> 48) & 0x7f);
		wrw(a, w0); wrw(a + 2, mant >> 32); wrw(a + 4, mant >> 16); wrw(a + 6, mant);
	} else {
		w0 = (sign << 15) | (e << 7) | ((mant >> 16) & 0x7f);
		wrw(a, w0); wrw(a + 2, mant);
	}
}

static void fcc(double v) { fpN = v < 0; fpZ = v == 0; fpV = fpC = 0; }

static double fsrc(int mode, int reg, int dbl)
{
	if (mode == 0) return ac[reg & 7];
	getops(mode, reg, dbl ? 8 : 4);
	return fget(ea, dbl);
}

static void fstep(uint16_t ir)
{
	int a = (ir >> 6) & 3, dm = (ir >> 3) & 7, dr = ir & 7, dbl = fpFD;
	double v;

	switch (ir & 0177700) {
	case 0170100: {		/* LDFPS */
		uint16_t w; getop(dm, dr, 0); w = load(0);
		fpFD = (w >> 7) & 1; fpFL = (w >> 6) & 1; fpFT = (w >> 5) & 1;
		fpN = (w >> 3) & 1; fpZ = (w >> 2) & 1; fpV = (w >> 1) & 1; fpC = w & 1;
		return;
	}
	case 0170200: case 0170300: {	/* STFPS / STST */
		uint16_t w = fpFD << 7 | fpFL << 6 | fpFT << 5 | fpN << 3 | fpZ << 2 | fpV << 1 | fpC;
		getop(dm, dr, 0); store(w, 0); return;
	}
	case 0170400:		/* CLRF */
		if (dm == 0) ac[dr & 7] = 0;
		else { getops(dm, dr, dbl ? 8 : 4); fput(ea, 0, dbl); }
		fcc(0); return;
	case 0170500:		/* TSTF */
		v = fsrc(dm, dr, dbl); fcc(v); return;
	case 0170600: case 0170700:	/* ABSF NEGF */
		if (dm == 0) { v = ac[dr & 7]; v = (ir & 0100) ? -v : fabs(v); ac[dr & 7] = v; }
		else { getops(dm, dr, dbl ? 8 : 4); v = fget(ea, dbl); v = (ir & 0100) ? -v : fabs(v); fput(ea, v, dbl); }
		fcc(v); return;
	}
	switch (ir & 0177400) {
	case 0171000: v = fsrc(dm, dr, dbl); ac[a] *= v; fcc(ac[a]); return;	/* MULF */
	case 0172000: v = fsrc(dm, dr, dbl); ac[a] += v; fcc(ac[a]); return;	/* ADDF */
	case 0172400: v = fsrc(dm, dr, dbl); ac[a] = v; fcc(v); return;	/* LDF */
	case 0173000: v = fsrc(dm, dr, dbl); ac[a] -= v; fcc(ac[a]); return;	/* SUBF */
	case 0174400: v = fsrc(dm, dr, dbl);					/* DIVF */
		if (v == 0) { fpV = 1; return; }
		ac[a] /= v; fcc(ac[a]); return;
	case 0173400:		/* CMPF */
		v = fsrc(dm, dr, dbl) - ac[a];
		fpN = v < 0; fpZ = v == 0; fpV = fpC = 0; return;
	case 0174000:		/* STF */
		if (dm == 0) ac[dr & 7] = ac[a];
		else { getops(dm, dr, dbl ? 8 : 4); fput(ea, ac[a], dbl); }
		return;
	case 0177000: {		/* LDCIF */
		int sz = fpFL ? 4 : 2; int32_t n;
		getops(dm, dr, sz);
		if (regdst >= 0) n = (int16_t)R[regdst];
		else if (sz == 2) n = (int16_t)rdw(ea);
		else n = (int32_t)((uint32_t)rdw(ea) << 16 | rdw(ea + 2));
		ac[a] = n; fcc(ac[a]); return;
	}
	case 0175400: {		/* STCFI */
		double t = trunc(ac[a]); int sz = fpFL ? 4 : 2;
		getops(dm, dr, sz);
		fpV = fpC = 0;
		if ((sz == 2 && (t > 32767 || t < -32768)) || (sz == 4 && (t > 2147483647.0 || t < -2147483648.0))) {
			fpC = 1; t = 0;
		}
		fpN = t < 0; fpZ = t == 0;
		if (regdst >= 0) R[regdst] = (int16_t)t;
		else if (sz == 2) wrw(ea, (int16_t)t);
		else { wrw(ea, (uint32_t)(int32_t)t >> 16); wrw(ea + 2, (uint32_t)(int32_t)t); }
		return;
	}
	case 0177400:		/* LDCDF (LDCFD): load other precision */
		v = fsrc(dm, dr, !dbl); ac[a] = v; fcc(v); return;
	case 0176000:		/* STCFD (STCDF): store in other precision */
		if (dm == 0) ac[dr & 7] = ac[a];
		else { getops(dm, dr, dbl ? 4 : 8); fput(ea, ac[a], !dbl); }
		return;
	case 0175000: {		/* STEXP */
		int e = 0; double m = ac[a];
		if (m != 0) { frexp(m, &e); e += 128; }
		e -= 128; getop(dm, dr, 0); store(e, 0); fcc(e); fpN = e < 0; fpZ = e == 0; return;
	}
	case 0176400: {		/* LDEXP */
		int e; double m;
		getop(dm, dr, 0); e = (int16_t)load(0);
		m = frexp(ac[a], &(int){0}); if (ac[a] == 0) { fcc(0); return; }
		ac[a] = ldexp(m, e); fcc(ac[a]); return;
	}
	}
	if (ir == 0170000) { fN = fpN; fZ = fpZ; fV = fpV; fC = fpC; return; }	/* CFCC */
	if (ir == 0170001) { fpFD = 0; return; }	/* SETF */
	if (ir == 0170002) { fpFL = 0; return; }	/* SETI */
	if (ir == 0170011) { fpFD = 1; return; }	/* SETD */
	if (ir == 0170012) { fpFL = 1; return; }	/* SETL */
	die("illegal FP instruction");
}

static void step(void)
{
	uint16_t ir = rdw(PC), s, d, r;
	int sm, sr, dm, dr, op, byte;
	uint16_t at = PC;
	(void)at;
	PC += 2;
	op = ir >> 12; byte = (ir >> 15) & 1;
	sm = (ir >> 9) & 7; sr = (ir >> 6) & 7; dm = (ir >> 3) & 7; dr = ir & 7;

	switch (op & 7) {
	case 1:	/* MOV / MOVB */
		getop(sm, sr, byte); s = load(byte);
		getop(dm, dr, byte);
		nz(s, byte); fV = 0;
		if (byte) storeb_sext(s); else store(s, 0);
		return;
	case 2:	/* CMP */
		getop(sm, sr, byte); s = load(byte);
		getop(dm, dr, byte); d = load(byte);
		r = s - d; nz(r, byte);
		{ uint32_t m = byte ? 0x80 : 0x8000;
		  fV = ((s & m) != (d & m)) && ((r & m) == (d & m));
		  fC = byte ? ((s & 0xff) < (d & 0xff)) : (s < d); }
		return;
	case 3:	/* BIT */
		getop(sm, sr, byte); s = load(byte);
		getop(dm, dr, byte); d = load(byte);
		nz(s & d, byte); fV = 0; return;
	case 4:	/* BIC */
		getop(sm, sr, byte); s = load(byte);
		getop(dm, dr, byte); d = load(byte);
		r = d & ~s; nz(r, byte); fV = 0; store(r, byte); return;
	case 5:	/* BIS */
		getop(sm, sr, byte); s = load(byte);
		getop(dm, dr, byte); d = load(byte);
		r = d | s; nz(r, byte); fV = 0; store(r, byte); return;
	case 6:	/* ADD / SUB */
		getop(sm, sr, 0); s = load(0);
		getop(dm, dr, 0); d = load(0);
		if (!byte) {
			uint32_t t = (uint32_t)s + d; r = t;
			nz(r, 0); setcc_add(s, d, t, 0);
		} else {
			r = d - s; nz(r, 0);
			fV = ((s & 0x8000) != (d & 0x8000)) && ((r & 0x8000) == (s & 0x8000));
			fC = d < s;
		}
		store(r, 0); return;
	}
	/* op & 7 == 0 or 7 */
	if (op == 7) {	/* EIS */
		int rg = (ir >> 6) & 7;
		switch ((ir >> 9) & 7) {
		case 0: {	/* MUL */
			int32_t a, res;
			getop(dm, dr, 0); s = load(0);
			a = (int16_t)R[rg]; res = a * (int16_t)s;
			if (rg & 1) R[rg] = res; else { R[rg] = res >> 16; R[rg | 1] = res; }
			fN = res < 0; fZ = res == 0; fV = 0; fC = res < -32768 || res > 32767;
			return;
		}
		case 1: {	/* DIV */
			int32_t dv, q, rm; int16_t sv;
			getop(dm, dr, 0); sv = load(0);
			if (sv == 0) { fV = fC = 1; return; }
			dv = (int32_t)((uint32_t)R[rg] << 16 | R[rg | 1]);
			q = dv / sv; rm = dv % sv;
			if (q > 32767 || q < -32768) { fV = 1; fC = 0; return; }
			R[rg] = q; R[rg | 1] = rm;
			fN = q < 0; fZ = q == 0; fV = 0; fC = 0;
			return;
		}
		case 2: {	/* ASH */
			int n_; int16_t v = R[rg]; int32_t res;
			getop(dm, dr, 0); n_ = load(0) & 077; if (n_ & 040) n_ -= 64;
			fV = 0; fC = 0;
			if (n_ > 0) {
				int32_t t = v; int k;
				for (k = 0; k < n_; k++) { fC = (t >> 15) & 1; t = (int16_t)(t << 1); }
				res = t; fV = ((int16_t)res < 0) != (v < 0);
			} else if (n_ < 0) {
				int32_t t = v; int k;
				for (k = 0; k < -n_; k++) { fC = t & 1; t >>= 1; }
				res = t;
			} else res = v;
			R[rg] = res; nz(R[rg], 0);
			return;
		}
		case 3: {	/* ASHC */
			int n_; int32_t v = (int32_t)((uint32_t)R[rg] << 16 | R[rg | 1]);
			getop(dm, dr, 0); n_ = load(0) & 077; if (n_ & 040) n_ -= 64;
			fV = 0; fC = 0;
			if (n_ > 0) {
				int k; int32_t t = v;
				for (k = 0; k < n_; k++) { fC = (t >> 31) & 1; t = (int32_t)((uint32_t)t << 1); }
				fV = (t < 0) != (v < 0); v = t;
			} else if (n_ < 0) {
				int k;
				for (k = 0; k < -n_; k++) { fC = v & 1; v >>= 1; }
			}
			R[rg] = (uint32_t)v >> 16; R[rg | 1] = v;
			fN = v < 0; fZ = v == 0;
			return;
		}
		case 4:	/* XOR */
			getop(dm, dr, 0); d = load(0);
			r = d ^ R[rg]; nz(r, 0); fV = 0; store(r, 0);
			return;
		case 7:	/* SOB */
			if (--R[rg] != 0) PC -= 2 * (ir & 077);
			return;
		}
		die("bad EIS instruction");
	}
	if ((ir & 0170000) == 0170000) { fstep(ir); return; }
	/* op == 0 or 8 (byte forms of single-operand/branch/trap) */
	if (!byte) {
		switch (ir >> 6) {
		case 0:
			if (ir == 0) die("HALT");
			if (ir == 5 || ir == 6 || ir == 1 || ir == 2 || ir == 3 || ir == 4) die("unsupported privileged instruction");
			break;
		case 1:	/* JMP */
			getop(dm, dr, 0);
			if (regdst >= 0) die("JMP to register");
			PC = ea; return;
		case 2:
			if ((ir & 070) == 0) {	/* RTS */
				PC = R[dr]; R[dr] = rdw(SP); SP += 2; return;
			}
			if ((ir & 0177760) == 0000240 || (ir & 0177760) == 0000260) {	/* CCC/SCC */
				int set = ir & 020;
				if (ir & 1) fC = set != 0;
				if (ir & 2) fV = set != 0;
				if (ir & 4) fZ = set != 0;
				if (ir & 010) fN = set != 0;
				return;
			}
			break;
		case 3:	/* SWAB */
			getop(dm, dr, 0); d = load(0);
			r = d >> 8 | d << 8; store(r, 0);
			fN = (r & 0x80) != 0; fZ = (r & 0xff) == 0; fV = fC = 0;
			return;
		}
	}
	if (!byte && (ir >> 8) >= 001 && (ir >> 8) <= 003) { /* BR..BLE handled below via branch table */ }
	{
		int hi = (ir >> 8) & 0377;
		/* branches: 0004xx-0003xx and 1000xx-1034xx */
		switch (ir & 0177400) {
		case 0000400: branch(1, ir); return;
		case 0001000: branch(!fZ, ir); return;
		case 0001400: branch(fZ, ir); return;
		case 0002000: branch(!(fN ^ fV), ir); return;
		case 0002400: branch(fN ^ fV, ir); return;
		case 0003000: branch(!(fZ | (fN ^ fV)), ir); return;
		case 0003400: branch(fZ | (fN ^ fV), ir); return;
		case 0100000: branch(!fN, ir); return;
		case 0100400: branch(fN, ir); return;
		case 0101000: branch(!(fC | fZ), ir); return;
		case 0101400: branch(fC | fZ, ir); return;
		case 0102000: branch(!fV, ir); return;
		case 0102400: branch(fV, ir); return;
		case 0103000: branch(!fC, ir); return;
		case 0103400: branch(fC, ir); return;
		case 0104400: v4sys(ir & 0377); return;	/* TRAP */
		}
		(void)hi;
	}
	if (!byte && (ir & 0177000) == 0004000) {	/* JSR */
		int rg = (ir >> 6) & 7;
		getop(dm, dr, 0);
		if (regdst >= 0) die("JSR to register");
		d = ea;
		SP -= 2; wrw(SP, R[rg]);
		R[rg] = PC; PC = d; return;
	}
	/* single operand group (byte flag selects b forms): 0050xx-0063xx, 0067xx */
	{
		int sub = (ir >> 6) & 077;	/* with byte bit in ir>>15 */
		uint16_t v; int c;
		switch (sub) {
		case 050: getop(dm, dr, byte); store(0, byte); fN = 0; fZ = 1; fV = fC = 0; return;
		case 051: getop(dm, dr, byte); v = load(byte); v = ~v; store(v, byte); nz(v, byte); fV = 0; fC = 1; return;
		case 052: getop(dm, dr, byte); v = load(byte);
			  { uint16_t m = byte ? 0x7f : 0x7fff; fV = (v & (byte?0xff:0xffff)) == m; }
			  v++; store(v, byte); nz(v, byte); return;
		case 053: getop(dm, dr, byte); v = load(byte);
			  { uint16_t m = byte ? 0x80 : 0x8000; fV = (v & (byte?0xff:0xffff)) == m; }
			  v--; store(v, byte); nz(v, byte); return;
		case 054: getop(dm, dr, byte); v = load(byte);
			  { uint16_t m = byte ? 0x80 : 0x8000; fV = (v & (byte?0xff:0xffff)) == m; }
			  v = -v; store(v, byte); nz(v, byte); fC = (v & (byte?0xff:0xffff)) != 0; return;
		case 055: getop(dm, dr, byte); v = load(byte); c = fC;
			  { uint16_t m = byte ? 0x7f : 0x7fff; fV = c && (v & (byte?0xff:0xffff)) == m; fC = c && ((v & (byte?0xff:0xffff)) == (byte?0xff:0xffff)); }
			  v += c; store(v, byte); nz(v, byte); return;
		case 056: getop(dm, dr, byte); v = load(byte); c = fC;
			  { uint16_t m = byte ? 0x80 : 0x8000; fV = (v & (byte?0xff:0xffff)) == m; fC = (v & (byte?0xff:0xffff)) == 0 && c; }
			  v -= c; store(v, byte); nz(v, byte); return;
		case 057: getop(dm, dr, byte); v = load(byte); nz(v, byte); fV = fC = 0; return;
		case 060: getop(dm, dr, byte); v = load(byte);
			  { uint16_t sb = byte ? 0x80 : 0x8000; int nc = v & 1; v = (v & (byte?0xff:0xffff)) >> 1; if (fC) v |= sb;
			    store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 061: getop(dm, dr, byte); v = load(byte);
			  { uint16_t sb = byte ? 0x80 : 0x8000; int nc = (v & sb) != 0; v = v << 1; if (fC) v |= 1;
			    store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 062: getop(dm, dr, byte); v = load(byte);
			  { uint16_t sb = byte ? 0x80 : 0x8000; int nc = v & 1; v = (v & (byte?0xff:0xffff)) >> 1 | (v & sb);
			    store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 063: getop(dm, dr, byte); v = load(byte);
			  { uint16_t sb = byte ? 0x80 : 0x8000; int nc = (v & sb) != 0; v = v << 1;
			    store(v, byte); nz(v, byte); fC = nc; fV = fN ^ fC; } return;
		case 067:
			if (!byte) {	/* SXT */
				getop(dm, dr, 0); store(fN ? 0xffff : 0, 0); fZ = !fN; fV = 0; return;
			}
		}
	}
	die("illegal instruction");
}

int main(int argc, char **argv)
{
	int i, ai = 1;
	for (i = 0; i < 16; i++) fdtab[i] = -1;
	fdtab[0] = 0; fdtab[1] = 1; fdtab[2] = 2;
	while (ai < argc && argv[ai][0] == '-') {
		if (!strcmp(argv[ai], "-r") && ai + 1 < argc) { root = argv[ai + 1]; ai += 2; }
		else if (!strcmp(argv[ai], "-x")) { stop_at_exec = 1; ai++; }
		else if (!strcmp(argv[ai], "-t")) { trace = 1; ai++; }
		else break;
	}
	if (ai >= argc) { fprintf(stderr, "usage: pdp11 [-r root] [-x] prog args...\n"); return 2; }
	{
		char p[600];
		if (argv[ai][0] == '/') snprintf(p, sizeof p, "%s%s", root, argv[ai]);
		else snprintf(p, sizeof p, "%s", argv[ai]);
		if (load_image(p, argc - ai, argv + ai) < 0) { fprintf(stderr, "pdp11: cannot load %s\n", p); return 2; }
	}
	for (;;) step();
}
