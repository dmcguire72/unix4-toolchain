#include "pdp11.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <limits.h>
#include <termios.h>

#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/wait.h>
#include <sys/times.h>

void mappath(uint16_t a, char *out)
{
	char tmp[PATH_MAX];
	int i = 0;
	while (mem[(uint16_t)(a + i)] && i < (PATH_MAX - 1)) {
		tmp[i] = mem[(uint16_t)(a + i)];
		i++;
	}
	tmp[i] = 0;

	if (tmp[0] == '\0') {
		out[0] = '\0';
		return;
	}

	if (root[0] == '\0') {
		snprintf(out, PATH_MAX * 2, "%s", tmp);
		return;
	}

	char cwd_buf[PATH_MAX];
	const char *stack[1024];
	int top = 0;

	if (tmp[0] != '/') {
		if (getcwd(cwd_buf, sizeof cwd_buf) == NULL) {
			snprintf(cwd_buf, sizeof cwd_buf, "%s", root);
		}
		size_t rlen = strlen(root);
		int is_root_slash = (strcmp(root, "/") == 0);
		int inside = 0;

		if (is_root_slash) {
			inside = 1;
		} else if (strncmp(cwd_buf, root, rlen) == 0 &&
		           (cwd_buf[rlen] == '\0' || cwd_buf[rlen] == '/')) {
			inside = 1;
		}

		if (inside) {
			char *rel = is_root_slash ? (cwd_buf + 1) : (cwd_buf + rlen);
			char *saveptr1;
			char *tok = strtok_r(rel, "/", &saveptr1);
			while (tok) {
				if (top < 1024) stack[top++] = tok;
				tok = strtok_r(NULL, "/", &saveptr1);
			}
		}
	}

	char tmp_buf[PATH_MAX];
	memcpy(tmp_buf, tmp, sizeof tmp_buf);
	tmp_buf[sizeof(tmp_buf) - 1] = '\0';
	char *saveptr2;
	char *tok = strtok_r(tmp_buf, "/", &saveptr2);
	while (tok) {
		if (strcmp(tok, ".") == 0 || tok[0] == '\0') {
		} else if (strcmp(tok, "..") == 0) {
			if (top > 0)
				top--;
		} else {
			if (top < 1024)
				stack[top++] = tok;
		}
		tok = strtok_r(NULL, "/", &saveptr2);
	}

	if (strcmp(root, "/") == 0) {
		if (top == 0) {
			strcpy(out, "/");
		} else {
			out[0] = '\0';
			for (int j = 0; j < top; j++) {
				size_t cur = strlen(out);
				snprintf(out + cur, (PATH_MAX * 2) - cur, "/%s", stack[j]);
			}
		}
	} else {
		snprintf(out, PATH_MAX * 2, "%s", root);
		for (int j = 0; j < top; j++) {
			size_t cur = strlen(out);
			snprintf(out + cur, (PATH_MAX * 2) - cur, "/%s", stack[j]);
		}
	}
}

static void seterr(int e)
{
	fC = 1;
	R[0] = e;
}

static int v4errno(int e)
{
	if (e >= 1 && e <= 30 && e != EINTR && e != EFAULT) return e;
	switch (e) {
	case ENOTEMPTY: return 16;
	case ENAMETOOLONG: case ELOOP: return 2;
	default: return 5;
	}
}

static void hosterr(void)
{
	seterr(v4errno(errno));
}

static struct stat dirst[16];
static char fisdir[16];

static int hostfd(int fd)
{
	if (fd < 0 || fd >= V4_NOFILE || fdtab[fd] < 0) {
		seterr(9);
		return -1;
	}
	return fdtab[fd];
}

static int allocfd(int h)
{
	int i;

	for (i = 0; i < V4_NOFILE; i++) {
		if (fdtab[i] < 0) {
			fdtab[i] = h;
			fisdir[i] = 0;
			return i;
		}
	}
	close(h);
	return -1;
}

static void closehost(int h)
{
	if (h > 2) close(h);
}

static uint16_t ino16(ino_t i)
{
	uint16_t v = (uint16_t)(i ^ (i >> 16));

	return v ? v : 1;
}

static int open_v4_dir(const char *path)
{
    DIR *dir = opendir(path);
    char tmptpl[] = "/tmp/v4dirXXXXXX";
    struct dirent *ent;
    struct stat st;
    int fd, pass;

    if (!dir) return -1;

    fd = mkstemp(tmptpl);
    if (fd < 0) {
        closedir(dir);
        return -1;
    }
    unlink(tmptpl);

    for (pass = 0; pass < 2; pass++) {
        rewinddir(dir);

        while ((ent = readdir(dir)) != NULL) {
            uint8_t buf[16] = {0};
            int dots = !strcmp(ent->d_name, ".") ||
                       !strcmp(ent->d_name, "..");
            uint16_t ino;
            size_t n;

            if ((pass == 0) != dots) continue;

            ino = fstatat(dirfd(dir), ent->d_name, &st,
                          AT_SYMLINK_NOFOLLOW) == 0
                ? ino16(st.st_ino)
                : ino16(ent->d_ino);

            buf[0] = ino;
            buf[1] = ino >> 8;

            n = strlen(ent->d_name);
            if (n > 14) n = 14;
            memcpy(buf + 2, ent->d_name, n);

            if (write(fd, buf, 16) != 16) break;
        }
    }

    closedir(dir);
    lseek(fd, 0, SEEK_SET);
    return fd;
}
static long dir_size(const char *path)
{
	DIR *d = opendir(path);
	struct dirent *e;
	long n = 0;

	if (!d) return 32;
	while ((e = readdir(d)) != NULL) n++;
	closedir(d);
	return n * 16;
}

static uint8_t g_uid, g_ruid, g_gid, g_rgid;
static long time_off;
static uint8_t g_nice;

static uint8_t map_uid(uid_t u)
{
	return u == geteuid() ? 0 : (uint8_t)u;
}

static uint8_t map_gid(gid_t g)
{
	return g == getegid() ? 0 : (uint8_t)g;
}

static int suser(void)
{
	if (g_uid == 0) return 1;
	seterr(1);
	return 0;
}

#define MAXKIDS 50
static struct { uint16_t g; pid_t h; } kids[MAXKIDS];
static int nkids;
static uint16_t my_gpid, par_gpid, alt_gpid = 30000;
static pid_t par_hpid;

static int gpid_used(uint16_t g)
{
	int i;

	if (g == my_gpid || g == par_gpid) return 1;
	for (i = 0; i < nkids; i++)
		if (kids[i].g == g) return 1;
	return 0;
}

static uint16_t new_gpid(pid_t h, int commit)
{
	if (h > 1 && h <= 65535 && !gpid_used((uint16_t)h))
		return (uint16_t)h;
	while (gpid_used(alt_gpid)) alt_gpid++;
	if (commit) return alt_gpid++;
	return alt_gpid;
}

uint16_t cur_gpid(void)
{
	return my_gpid;
}

void syscalls_init(void)
{
	pid_t p = getpid();

	my_gpid = (p > 1 && p <= 65535) ? (uint16_t)p : 1;
	par_gpid = 0;
	par_hpid = 0;
}

static int proc_is_us(pid_t p)
{
	char a[64], b[PATH_MAX], c[PATH_MAX];
	ssize_t n1, n2;

	snprintf(a, sizeof a, "/proc/%d/exe", (int)p);
	n1 = readlink(a, b, sizeof b - 1);
	n2 = readlink("/proc/self/exe", c, sizeof c - 1);
	return n1 > 0 && n1 == n2 && memcmp(b, c, n1) == 0;
}

static pid_t resolve_pid(uint16_t g)
{
	int i;

	for (i = 0; i < nkids; i++)
		if (kids[i].g == g) return kids[i].h;
	if (g == my_gpid) return getpid();
	if (g == par_gpid && par_hpid && getppid() == par_hpid) return par_hpid;
	if (g > 1 && proc_is_us(g) && g != 0) return g;
	return 0;
}

static void reap_kid(pid_t h, uint16_t *g)
{
	int i;

	*g = (uint16_t)h;
	for (i = 0; i < nkids; i++) {
		if (kids[i].h == h) {
			*g = kids[i].g;
			kids[i] = kids[--nkids];
			return;
		}
	}
}

static void put16(uint16_t base, int off, uint16_t v)
{
	mem[(uint16_t)(base + off)] = v;
	mem[(uint16_t)(base + off + 1)] = v >> 8;
}

static void putstat(uint16_t ub, const struct stat *st, long size)
{
	uint16_t mode = 0100000;
	int i;

	for (i = 0; i < 36; i++) mem[(uint16_t)(ub + i)] = 0;
	if (S_ISDIR(st->st_mode)) mode |= 040000;
	else if (S_ISCHR(st->st_mode)) mode |= 020000;
	else if (S_ISBLK(st->st_mode)) mode |= 060000;
	else if (size > 4096) mode |= 010000;
	mode |= st->st_mode & 06777;

	put16(ub, 0, (uint16_t)(major(st->st_dev) << 8 | minor(st->st_dev)));
	put16(ub, 2, ino16(st->st_ino));
	put16(ub, 4, mode);
	mem[(uint16_t)(ub + 6)] = st->st_nlink > 255 ? 255 : st->st_nlink;
	mem[(uint16_t)(ub + 7)] = map_uid(st->st_uid);
	mem[(uint16_t)(ub + 8)] = map_gid(st->st_gid);
	mem[(uint16_t)(ub + 9)] = (size >> 16) & 0xff;
	put16(ub, 10, size & 0xffff);
	if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode))
		put16(ub, 12, (uint16_t)(major(st->st_rdev) << 8 |
		    minor(st->st_rdev)));
	put16(ub, 28, (uint32_t)st->st_atime >> 16);
	put16(ub, 30, (uint32_t)st->st_atime & 0xffff);
	put16(ub, 32, (uint32_t)st->st_mtime >> 16);
	put16(ub, 34, (uint32_t)st->st_mtime & 0xffff);
}

#define TTY_ECHO 010
#define TTY_CRMOD 020
#define TTY_RAW 040

static struct termios tty_orig;
static int tty_saved;
static int tty_fd = -1;

void tty_restore(void)
{
	if (tty_saved) tcsetattr(tty_fd, TCSANOW, &tty_orig);
}

static uint16_t tty_get_flags(int h)
{
	struct termios t;
	uint16_t f = 0;

	if (tcgetattr(h, &t) < 0) return TTY_ECHO | TTY_CRMOD;
	if (t.c_lflag & ECHO) f |= TTY_ECHO;
	if (t.c_iflag & ICRNL) f |= TTY_CRMOD;
	if (!(t.c_lflag & ICANON)) f |= TTY_RAW;
	return f;
}

static void tty_set_flags(int h, uint16_t f)
{
	struct termios t;

	if (tcgetattr(h, &t) < 0) return;
	if (!tty_saved) {
		tty_orig = t;
		tty_saved = 1;
		tty_fd = h;
		atexit(tty_restore);
	}
	if (f & TTY_ECHO) t.c_lflag |= ECHO; else t.c_lflag &= ~ECHO;
	if (f & TTY_RAW) {
		t.c_lflag &= ~(ICANON | ISIG);
		t.c_cc[VMIN] = 1;
		t.c_cc[VTIME] = 0;
	} else {
		t.c_lflag |= ICANON | ISIG;
	}
	if (f & TTY_CRMOD) t.c_iflag |= ICRNL; else t.c_iflag &= ~ICRNL;
	tcsetattr(h, TCSANOW, &t);
}

static int ends_with(const char *p, const char *suf)
{
	size_t a = strlen(p), b = strlen(suf);

	return a >= b && !strcmp(p + a - b, suf);
}

static int is_dot_entry(const char *p)
{
	return !strcmp(p, ".") || !strcmp(p, "..") ||
	    ends_with(p, "/.") || ends_with(p, "/..");
}

static void rawpath(uint16_t a, char *out)
{
	int i = 0;

	while (mem[(uint16_t)(a + i)] && i < PATH_MAX - 1) {
		out[i] = mem[(uint16_t)(a + i)];
		i++;
	}
	out[i] = 0;
}

static void sys_nosys(void)
{
	if (sigtab[V4_SIGSYS] & 1) { seterr(100); return; }
	trap_sig(V4_SIGSYS);
}

static void do_sys(int n, uint16_t *a)
{
	int i;
	char p[PATH_MAX * 2];
	int r, fd, h;

	fC = 0;
	switch (n) {
	case 0: /* nullsys */
		return;
	case 1: /* exit */
		exit(R[0] & 0377);
	case 2: { /* fork */
		if (nkids >= MAXKIDS) { seterr(11); return; }
		fflush(NULL);
		r = fork();
		if (r < 0) { hosterr(); return; }
		trace_multi = 1;
		if (r == 0) {
			uint16_t child = new_gpid(getpid(), 0);

			is_child = 1;
			par_gpid = my_gpid;
			par_hpid = getppid();
			my_gpid = child;
			nkids = 0;
			R[0] = par_gpid;
		} else {
			uint16_t child = new_gpid(r, 1);

			kids[nkids].g = child;
			kids[nkids].h = r;
			nkids++;
			R[0] = child;
			PC += 2;
		}
		return;
	}
	case 3: /* read */
		h = hostfd(R[0]);
		if (h < 0) return;
		if ((uint32_t)a[0] + a[1] > 65536) a[1] = 65536 - a[0];
		r = read(h, mem + a[0], a[1]);
		if (r < 0) {
			if (errno == EINTR) return;
			hosterr();
		} else R[0] = r;
		return;
	case 4: /* write */
		h = hostfd(R[0]);
		if (h < 0) return;
		if ((uint32_t)a[0] + a[1] > 65536) a[1] = 65536 - a[0];
		r = write(h, mem + a[0], a[1]);
		if (r < 0) {
			if (errno == EINTR) return;
			if (errno == EPIPE) { R[0] = 0; return; }
			hosterr();
		} else R[0] = r;
		return;
	case 5: { /* open */
		struct stat st;

		mappath(a[0], p);
		if (stat(p, &st) == 0 && S_ISDIR(st.st_mode)) {
			if (a[1] != 0) { seterr(21); return; }	/* EISDIR */
			r = open_v4_dir(p);
		} else {
			r = open(p, a[1] == 0 ? O_RDONLY : a[1] == 1 ? O_WRONLY : O_RDWR);
			st.st_mode = 0;
		}
		if (r < 0) { hosterr(); return; }
		fd = allocfd(r);
		if (fd < 0) { seterr(24); return; }
		if (S_ISDIR(st.st_mode)) {
			fisdir[fd] = 1;
			dirst[fd] = st;
		}
		R[0] = fd;
		return;
	}
	case 6: /* close */
		fd = R[0];
		h = hostfd(fd);
		if (h < 0) return;
		closehost(h);
		fdtab[fd] = -1;
		fisdir[fd] = 0;
		return;
	case 7: { /* wait */
		int st;
		uint16_t g;

		r = wait(&st);
		if (r < 0) {
			if (errno == EINTR) return;
			seterr(10);
			return;
		}
		reap_kid(r, &g);
		R[0] = g;
		if (WIFEXITED(st)) R[1] = WEXITSTATUS(st) << 8;
		else {
			int sg = host_to_guest_sig(WTERMSIG(st));

			R[1] = (sg ? sg : V4_SIGKIL) | (WCOREDUMP(st) ? 0200 : 0);
		}
		return;
	}
	case 8: { /* creat */
		mode_t mode = a[1] & 0777;

		mappath(a[0], p);
		r = open(p, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (r >= 0) {
			fchmod(r, mode);
		} else if (errno == EEXIST) {
			r = open(p, O_RDWR | O_TRUNC);
			if (r < 0) r = open(p, O_WRONLY | O_TRUNC);
		}
		if (r < 0) { hosterr(); return; }
		fd = allocfd(r);
		if (fd < 0) seterr(24); else R[0] = fd;
		return;
	}
	case 9: { /* link */
		char q[PATH_MAX * 2];
		struct stat st;

		char raw[PATH_MAX];

		mappath(a[0], p);
		mappath(a[1], q);
		rawpath(a[1], raw);
		if (is_dot_entry(raw)) {
			tnote("no-op: host directories already have . and ..");
			if (stat(p, &st) < 0) hosterr();
			else if (!S_ISDIR(st.st_mode)) seterr(1);
			return;
		}
		if (link(p, q) < 0) hosterr();
		return;
	}
	case 10: { /* unlink */
		struct stat st;

		char raw[PATH_MAX];

		mappath(a[0], p);
		rawpath(a[0], raw);
		if (is_dot_entry(raw)) {
			tnote("no-op: rmdir dropping . or ..");
			return;
		}
		if (lstat(p, &st) == 0 && S_ISDIR(st.st_mode)) {
			if (!suser()) return;
			tnote("unlink of a directory translated to rmdir");
			if (rmdir(p) < 0) hosterr();
			return;
		}
		if (unlink(p) < 0) hosterr();
		return;
	}
	case 11: { /* exec */
		char *av[260];
		static char strs[640];
		int na = 0, nc = 0;
		uint16_t ap = a[1], s;

		mappath(a[0], p);
		while ((s = rdw(ap)) != 0) {
			if (s == 0177777) { seterr(14); return; }
			av[na++] = strs + nc;
			for (;;) {
				char c = mem[s++];

				if (nc > 510) { seterr(7); return; }
				strs[nc++] = c;
				if (!c) break;
			}
			ap += 2;
			if (na >= 255) { seterr(7); return; }
		}
		if (stop_at_exec) {
			printf("EXEC %s", p);
			for (i = 0; i < na; i++) printf(" %s", av[i]);
			printf("\n");
			fflush(stdout);
			exit(0);
		}
		if (load_image(p, na, av) < 0) seterr(load_errno);
		else tnote("image replaced; signals reset, FP state cleared");
		return;
	}
	case 12: { /* chdir */
		struct stat st;

		mappath(a[0], p);
		if (stat(p, &st) == 0 && !S_ISDIR(st.st_mode)) { seterr(20); return; }
		if (chdir(p) < 0) { hosterr(); return; }
		if (root[0] != '\0' && strcmp(root, "/") != 0) {
			char cwd[PATH_MAX];

			if (getcwd(cwd, sizeof cwd)) {
				size_t rlen = strlen(root);

				if (strncmp(cwd, root, rlen) != 0 ||
				    (cwd[rlen] != '\0' && cwd[rlen] != '/'))
					if (chdir(root) < 0) { }
			}
		}
		return;
	}
	case 13: { /* time */
		uint32_t t = (uint32_t)(time(NULL) + time_off);

		R[0] = t >> 16;
		R[1] = t & 0xffff;
		return;
	}
	case 14: { /* mknod */
		uint16_t m = a[1];

		if (!suser()) return;
		mappath(a[0], p);
		switch (m & 060000) {
		case 040000:
			tnote("mknod(IFDIR) translated to mkdir");
			r = mkdir(p, (m & 0777) ? (m & 0777) : 0755);
			break;
		case 020000:
			r = mknod(p, S_IFCHR | (m & 0777), makedev(a[2] >> 8, a[2] & 0377));
			break;
		case 060000:
			r = mknod(p, S_IFBLK | (m & 0777), makedev(a[2] >> 8, a[2] & 0377));
			break;
		default: {
			int f = open(p, O_WRONLY | O_CREAT | O_EXCL, m & 0777);

			r = f;
			if (f >= 0) close(f);
		}
		}
		if (r < 0) hosterr();
		return;
	}
	case 15: /* chmod */
		mappath(a[0], p);
		if (chmod(p, a[1] & 0777) < 0) hosterr();
		return;
	case 16: { /* chown(path, uid) */
		struct stat st;

		mappath(a[0], p);
		if (stat(p, &st) < 0) { hosterr(); return; }
		if (map_uid(st.st_uid) == (a[1] & 0377)) return;
		if (a[1] == 0) {
			if (chown(p, geteuid(), -1) < 0) hosterr();
			return;
		}
		if (chown(p, a[1], -1) < 0) hosterr();
		return;
	}
	case 17: { /* break */
		uint32_t end = ((uint32_t)a[0] + 63) & ~63u;

		if (end > ((uint32_t)SP & ~63u) && a[0] != 0) seterr(12);

		return;
	}
	case 18: { /* stat */
		struct stat st;

		mappath(a[0], p);
		if (stat(p, &st) < 0) { hosterr(); return; }
		putstat(a[1], &st, S_ISDIR(st.st_mode) ? dir_size(p) : st.st_size);
		return;
	}
	case 19: { /* seek */
		int w = a[1];
		off_t off;

		h = hostfd(R[0]);
		if (h < 0) return;
		off = (w > 2) ? (off_t)a[0] * 512 : (w == 0 || w == 3) ? (off_t)a[0] : (int16_t)a[0];
		if (lseek(h, off, (w % 3) == 0 ? SEEK_SET : (w % 3) == 1 ? SEEK_CUR : SEEK_END) < 0)
			hosterr();
		return;
	}
	case 21: { /* mount(special, name, rwflag) */
		struct stat st;

		mappath(a[0], p);
		if (stat(p, &st) < 0) { hosterr(); return; }
		if (!S_ISBLK(st.st_mode)) { seterr(15); return; }	/* ENOTBLK */
		tnote("host block device has no v4 driver");
		seterr(6);	/* ENXIO: no such block driver in this machine */
		return;
	}
	case 22: { /* umount(special) */
		struct stat st;

		mappath(a[0], p);
		if (stat(p, &st) < 0) { hosterr(); return; }
		if (!S_ISBLK(st.st_mode)) { seterr(15); return; }
		tnote("nothing is ever mounted");
		seterr(22);	/* EINVAL */
		return;
	}
	case 23: /* setuid */
		r = R[0] & 0377;
		if (g_ruid == r || suser()) g_uid = g_ruid = r;
		return;
	case 24: /* getuid */
		R[0] = g_uid << 8 | g_ruid;
		return;
	case 25: /* stime */
		if (!suser()) return;
		tnote("clock offset kept in the emulator only");
		time_off = (long)((uint32_t)R[0] << 16 | R[1]) - (long)time(NULL);
		return;
	case 28: { /* fstat */
		struct stat st;

		fd = R[0];
		h = hostfd(fd);
		if (h < 0) return;
		if (fisdir[fd]) {
			putstat(a[0], &dirst[fd], lseek(h, 0, SEEK_END));
			lseek(h, 0, SEEK_SET);
			return;
		}
		if (fstat(h, &st) < 0) { hosterr(); return; }
		putstat(a[0], &st, st.st_size);
		return;
	}
	case 30: /* smdate */
		return;
	case 31: /* stty(fd, buf) */
		h = hostfd(R[0]);
		if (h < 0) return;
		if (!isatty(h)) { seterr(25); return; }
		tnote("flags applied to host termios (echo/raw/crmod)");
		tty_set_flags(h, rdw(a[0] + 4));
		return;
	case 32: /* gtty(fd, buf) */
		h = hostfd(R[0]);
		if (h < 0) return;
		if (!isatty(h)) { seterr(25); return; }
		wrw(a[0] + 0, 0);
		wrw(a[0] + 2, ('@' << 8) | '#');
		wrw(a[0] + 4, tty_get_flags(h));
		return;
	case 34: { /* nice */
		int v = (int16_t)R[0];

		if (v > 20) v = 20;
		if (v < 0 && !suser()) { fC = 0; v = 0; }
		g_nice = v < 0 ? 0 : v;
		if (v > 0) { errno = 0; if (nice(v) < 0 && errno) { } }
		return;
	}
	case 35: /* sleep */
		sleep(R[0]);
		return;
	case 36: /* sync */
		sync();
		return;
	case 37: { /* kill(pid (R0), sig) */
		pid_t hp;
		int hs;

		hp = resolve_pid(R[0]);
		if (!hp) { seterr(3); return; }
		if (a[0] == 0 || a[0] >= V4_NSIG) return;
		hs = guest_to_host_sig(a[0]);
		if (hp == getpid()) {
			sig_pending |= 1 << a[0];
			return;
		}
		if (kill(hp, hs) < 0) seterr(errno == ESRCH ? 3 : 1);
		return;
	}
	case 38: /* getswit */
		R[0] = 0;
		return;
	case 41: /* dup */
		fd = R[0];
		h = hostfd(fd);
		if (h < 0) return;
		r = dup(h);
		if (r < 0) { seterr(24); return; }
		i = allocfd(r);
		if (i < 0) { seterr(24); return; }
		fisdir[i] = fisdir[fd];
		dirst[i] = dirst[fd];
		R[0] = i;
		return;
	case 42: { /* pipe: R0 = read, R1 = write */
		int pfd[2], fd0, fd1;

		if (pipe(pfd) < 0) { seterr(23); return; }
		fd0 = allocfd(pfd[0]);
		if (fd0 < 0) { close(pfd[1]); seterr(24); return; }
		fd1 = allocfd(pfd[1]);
		if (fd1 < 0) {
			closehost(fdtab[fd0]);
			fdtab[fd0] = -1;
			seterr(24);
			return;
		}
		R[0] = fd0;
		R[1] = fd1;
		return;
	}
	case 43: { /* times */
		struct tms t;
		long hz = sysconf(_SC_CLK_TCK);
		uint32_t utime, stime_, cutime, cstime;

		times(&t);
		if (hz <= 0) hz = 60;
		utime = (uint64_t)t.tms_utime * 60 / hz;
		stime_ = (uint64_t)t.tms_stime * 60 / hz;
		cutime = (uint64_t)t.tms_cutime * 60 / hz;
		cstime = (uint64_t)t.tms_cstime * 60 / hz;
		wrw(a[0] + 0, utime & 0xffff);
		wrw(a[0] + 2, stime_ & 0xffff);
		wrw(a[0] + 4, (cutime >> 16) & 0xffff);
		wrw(a[0] + 6, cutime & 0xffff);
		wrw(a[0] + 8, (cstime >> 16) & 0xffff);
		wrw(a[0] + 10, cstime & 0xffff);
		return;
	}
	case 44: /* profil(buf, size, offset, scale) */
		prof_buf = a[0] & ~1;
		prof_len = a[1];
		prof_off = a[2];
		prof_scale = (a[3] >> 1) & 077777;
		return;
	case 46: /* setgid */
		r = R[0] & 0377;
		if (g_rgid == r || suser()) g_gid = g_rgid = r;
		return;
	case 47: /* getgid */
		R[0] = g_gid << 8 | g_rgid;
		return;
	case 48: { /* sig(signum, handler) */
		int s = a[0];

		if (s <= 0 || s >= V4_NSIG) { seterr(22); return; }
		R[0] = sigtab[s];
		sigtab[s] = a[1];
		sigtab[V4_SIGKIL] = 0;
		sig_pending &= ~(1 << s);
		sig_set(s, a[1]);
		return;
	}
	default: /* 20 26 27 29 33 39 40 45 49-63: nosys */
		sys_nosys();
		return;
	}
}

static void hostnote(int n, const uint16_t *a, char *out)
{
	char raw[PATH_MAX], host[PATH_MAX * 2];
	int which[2] = { -1, -1 }, k;

	out[0] = 0;
	switch (n) {
	case 5: case 8: case 10: case 11: case 12: case 14: case 15: case 16:
	case 18: case 21: case 22:
		which[0] = 0;
		break;
	case 9:
		which[0] = 0;
		which[1] = 1;
		break;
	}
	if (n == 21) which[1] = 1;
	for (k = 0; k < 2; k++) {
		if (which[k] < 0) continue;
		rawpath(a[which[k]], raw);
		mappath(a[which[k]], host);
		if (strcmp(raw, host) != 0)
			snprintf(out + strlen(out), 600 - strlen(out), "%shost %s",
			    out[0] ? ", " : "", host);
	}
}

static void fdnote(int n)
{
	switch (n) {
	case 5: case 8: case 41:
		if (!fC && R[0] < V4_NOFILE && fdtab[R[0]] >= 0)
			tnote("fd %u -> host fd %d%s", R[0], fdtab[R[0]],
			    fisdir[R[0]] ? " (directory synthesised as v4 16-byte entries)" : "");
		break;
	case 42:
		if (!fC && R[0] < V4_NOFILE && R[1] < V4_NOFILE)
			tnote("fds %u,%u -> host fds %d,%d", R[0], R[1], fdtab[R[0]],
			    fdtab[R[1]]);
		break;
	}
}

void v4sys(int n)
{
	static const int nargs[64] = {
		0,0,0,2,2,2,0,0,2,2,1,2,1,0,3,2,2,1,2,2,0,3,1,0,0,0,0,0,1,0,1,1,
		1,0,0,0,0,1,0,0,0,0,0,1,4,0,0,0,2
	};
	uint16_t a[4] = {0, 0, 0, 0};
	uint16_t r0in = R[0], r1in = R[1];
	char pre[1024] = "", hn[640] = "";
	int i, deferred, traced = trace & TR_SYS;

	if (n == 0) {
		uint16_t t = rdw(PC), ap;

		PC += 2;
		n = rdw(t) & 077;
		ap = t + 2;
		for (i = 0; i < nargs[n]; i++) {
			a[i] = rdw(ap);
			ap += 2;
		}
	} else {
		for (i = 0; i < nargs[n]; i++) {
			a[i] = rdw(PC);
			PC += 2;
		}
	}
	if (traced) {
		deferred = n == 3 || n == 18 || n == 28 || n == 32 || n == 43;
		trace_note_reset();
		hostnote(n, a, hn);
		if (!deferred) trace_sys_args(n, a, r0in, r1in, pre, sizeof pre);
		if (n == 1) {
			tr_line("exit(%u) = ?", r0in & 0377);
			tr_line("+++ exited with %u +++", r0in & 0377);
		}
	}
	do_sys(n, a);
	if (traced) {
		fdnote(n);
		trace_sys_line(n, a, r0in, r1in, pre, hn, deferred);
	}
}
