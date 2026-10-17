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

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/times.h>


static void mappath(uint16_t a, char *out)
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

static int allocfd(int hostfd)
{
	int i;
	for (i = 3; i < 16; i++) {
		if (fdtab[i] < 0) {
			fdtab[i] = hostfd;
			return i;
		}
	}
	close(hostfd);
	return -1;
}

static int open_v4_dir(const char *path)
{
	DIR *dir = opendir(path);
	if (!dir) return -1;

	char tmptpl[] = "/tmp/v4dirXXXXXX";
	int fd = mkstemp(tmptpl);
	if (fd < 0) {
		closedir(dir);
		return -1;
	}

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

static void seterr(int e)
{
	fC = 1;
	R[0] = e;
}

void v4sys(int n)
{
	static const int nargs[64] = {
		0,0,0,2,2,2,0,0,2,2,1,2,1,0,3,2,2,1,2,2,0,3,1,0,0,0,0,0,1,0,1,1,
		1,0,0,0,0,1,0,0,0,0,0,1,4,0,0,0,2
	};
	uint16_t a[4];
	int i;
	char p[PATH_MAX * 2];
	int r, fd;

	{
		uint16_t ap;

		if (n == 0) {
			uint16_t t = rdw(PC);
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
	}
	fC = 0;
	switch (n) {
	case 1: /* exit */
		fflush(stdout);
		exit(R[0] & 0377);
	case 2: { /* fork */
		r = fork();
		if (r < 0) {
			seterr(11);
			return;
		}
		if (r == 0) {
			R[0] = getppid();
		} else {
			R[0] = r;
			PC += 2;
		}
		return;
	}
	case 3: /* read */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		r = read(fdtab[fd], mem + a[0], a[1]);
		if (r < 0) seterr(5); else R[0] = r;
		return;
	case 4: /* write */
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
	case 7: { /* wait */
		int st;
		r = wait(&st);
		if (r < 0) { seterr(10); return; }
		R[0] = r;
		R[1] = st;
		return;
	}
	case 8: /* creat */
		mappath(a[0], p);
		r = open(p, O_RDWR | O_CREAT | O_TRUNC, 0600);
		if (r < 0) { seterr(2); return; }
		chmod(p, a[1] & 0777 ? (a[1] & 0777) | 0200 : 0600);
		fd = allocfd(r);
		if (fd < 0) seterr(24); else R[0] = fd;
		return;
	case 6: /* close */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		if (fd > 2) close(fdtab[fd]);
		fdtab[fd] = -1;
		return;
	case 9: { /* link */
		char q[PATH_MAX * 2];
		mappath(a[0], p);
		mappath(a[1], q);
		if (link(p, q) < 0) seterr(errno == EEXIST ? 17 : 2);
		return;
	}
	case 10: /* unlink */
		mappath(a[0], p);
		if (unlink(p) < 0) seterr(2);
		return;
	case 14: /* mknod (sys2.c) */
		mappath(a[0], p);
		if (mknod(p, a[1], a[2]) < 0) seterr(errno == EEXIST ? 17 : 2);
		return;
	case 16: /* chown (sys3.c) */
		mappath(a[0], p);
		if (chown(p, a[1], -1) < 0) seterr(2);
		return;
	case 25: /* stime (sys3.c) */
	case 30: /* smdate (sysent.c) */
	case 34: /* nice (sys3.c) */
	case 44: /* profil (sys4.c) */
		return;
	case 35: /* sleep (sys2.c) */
		sleep(R[0]);
		return;
	case 37: /* kill (sys4.c) */
		if (kill(R[0], a[0]) < 0) seterr(errno == ESRCH ? 3 : 1);
		return;
	case 38: /* getswit (sys3.c) */
		R[0] = 0;
		return;
	case 15: /* chmod */
		mappath(a[0], p);
		if (chmod(p, a[1] & 0777) < 0) seterr(2);
		return;
	case 17: /* break */
		return;
	case 18: { /* stat */
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
	case 19: /* seek */ {
		int w = a[1];
		long off;
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		off = (w > 2) ? (long)a[0] * 512 : (w == 0 || w == 3) ? (long)a[0] : (int16_t)a[0];
		r = lseek(fdtab[fd], off, (w % 3) == 0 ? SEEK_SET : (w % 3) == 1 ? SEEK_CUR : SEEK_END);
		if (r < 0) seterr(22);
		return;
	}
	case 24: /* getuid */
		R[0] = 0;
		return;
	case 48: { /* signal */
		uint16_t old = sigtab[a[0] & 63];
		sigtab[a[0] & 63] = a[1];
		R[0] = old;
		return;
	}
	case 12: { /* chdir */
		mappath(a[0], p);
		if (chdir(p) < 0) { seterr(2); return; }
		if (root[0] != '\0' && strcmp(root, "/") != 0) {
			char cwd[PATH_MAX];
			if (getcwd(cwd, sizeof cwd)) {
				size_t rlen = strlen(root);
				if (strncmp(cwd, root, rlen) != 0 ||
					(cwd[rlen] != '\0' && cwd[rlen] != '/'))
					chdir(root);
			}
		}
		return;
	}
	case 13: { /* time */
		time_t t = time(NULL);
		R[0] = (t >> 16) & 0xffff;
		R[1] = t & 0xffff;
		return;
	}
	case 23: /* setuid */
	case 43: { /* times */
		struct tms t;
		times(&t);
		long hz = sysconf(_SC_CLK_TCK);
		if (hz <= 0) hz = 60;

		uint32_t utime  = (uint64_t)t.tms_utime  * 60 / hz;
		uint32_t stime  = (uint64_t)t.tms_stime  * 60 / hz;
		uint32_t cutime = (uint64_t)t.tms_cutime * 60 / hz;
		uint32_t cstime = (uint64_t)t.tms_cstime * 60 / hz;

		wrw(a[0] + 0, utime & 0xffff);
		wrw(a[0] + 2, stime & 0xffff);
		wrw(a[0] + 4, (cutime >> 16) & 0xffff);
		wrw(a[0] + 6, cutime & 0xffff);
		wrw(a[0] + 8, (cstime >> 16) & 0xffff);
		wrw(a[0] + 10, cstime & 0xffff);
		return;
	}
	case 46: /* setgid */
		return;
	case 28: { /* fstat */
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
	case 31: /* stty */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		if (!isatty(fdtab[fd])) { seterr(25); return; }
		return;
	case 32: /* gtty */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		if (!isatty(fdtab[fd])) { seterr(25); return; }
		wrw(a[0] + 0, 0);
		wrw(a[0] + 2, ('@' << 8) | '#');
		wrw(a[0] + 4, 030);
		return;
	case 36: /* sync */
		sync();
		return;
	case 41: /* dup */
		fd = R[0];
		if (fd < 0 || fd > 15 || fdtab[fd] < 0) { seterr(9); return; }
		r = dup(fdtab[fd]);
		if (r < 0) { seterr(24); return; }
		fd = allocfd(r);
		if (fd < 0) seterr(24); else R[0] = fd;
		return;
	case 42: { /* pipe */
		int pfd[2];
		if (pipe(pfd) < 0) { seterr(24); return; }
		int fd0 = allocfd(pfd[0]);
		int fd1 = allocfd(pfd[1]);
		if (fd0 < 0 || fd1 < 0) { seterr(24); return; }
		R[0] = fd0;
		R[1] = fd1;
		return;
	}
	case 47: /* getgid */
		R[0] = 0;
		return;
	case 11: { /* exec */
		char *av[40];
		int na = 0;
		uint16_t ap = a[1], s;
		static char strs[40][512];
		mappath(a[0], p);
		while ((s = rdw(ap)) != 0 && s != 0177777) {
			int k = 0;
			while (mem[(uint16_t)(s + k)] && k < 510) {
				strs[na][k] = mem[(uint16_t)(s + k)];
				k++;
			}
			strs[na][k] = 0;
			av[na] = strs[na];
			na++;
			ap += 2;
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
