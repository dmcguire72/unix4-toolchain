#ifndef NUMBERS_H
#define NUMBERS_H

#pragma region SIGNALS

#define V4_SIGHUP    1
#define V4_SIGINT    2
#define V4_SIGQIT    3
#define V4_SIGINS    4
#define V4_SIGTRC    5
#define V4_SIGIOT    6
#define V4_SIGEMT    7
#define V4_SIGFPT    8
#define V4_SIGKIL    9
#define V4_SIGBUS   10
#define V4_SIGSEG   11
#define V4_SIGSYS   12

#pragma endregion
#pragma region ERRORS

// @TODO update these comments to match public docs
#define V4_EPERM     1   /* Not owner */
#define V4_ENOENT    2   /* No such file or directory */
#define V4_ESRCH     3   /* No such process */
#define V4_EIO       5   /* I/O error */
#define V4_ENXIO     6   /* No such device or address */
#define V4_E2BIG     7   /* Arg list too long */
#define V4_ENOEXEC   8   /* Exec format error */
#define V4_EBADF     9   /* Bad file number */
#define V4_ECHILD   10   /* No children */
#define V4_EAGAIN   11   /* No more processes */
#define V4_ENOMEM   12   /* Not enough core */
#define V4_EACCES   13   /* Permission denied */
#define V4_EFAULT   14   /* Bad address */
#define V4_ENOTBLK  15   /* Block device required */
#define V4_EBUSY    16   /* Mount device busy */
#define V4_EEXIST   17   /* File exists */
#define V4_EXDEV    18   /* Cross-device link */
#define V4_ENODEV   19   /* No such device */
#define V4_ENOTDIR  20   /* Not a directory */
#define V4_EISDIR   21   /* Is a directory */
#define V4_EINVAL   22   /* Invalid argument */
#define V4_ENFILE   23   /* File table overflow */
#define V4_EMFILE   24   /* Too many open files */
#define V4_ENOTTY   25   /* Not a typewriter */
#define V4_ETXTBSY  26   /* Text file busy */
#define V4_EFBIG    27   /* File too large */
#define V4_ENOSPC   28   /* No space left on device */
#define V4_ESPIPE   29   /* Illegal seek */
#define V4_EROFS    30   /* Read-only file system */
#define V4_ENOSYS  100   /* Bad system call */

#pragma endregion
#pragma region SYSCALLS

#define V4_NULLSYS   0  /* () */
#define V4_EXIT      1  /* (R0: status) */
#define V4_FORK      2  /* () */
#define V4_READ      3  /* (R0: fd, a[0]: buf, a[1]: nbytes) */
#define V4_WRITE     4  /* (R0: fd, a[0]: buf, a[1]: nbytes) */
#define V4_OPEN      5  /* (a[0]: path, a[1]: mode) */
#define V4_CLOSE     6  /* (R0: fd) */
#define V4_WAIT      7  /* () */
#define V4_CREAT     8  /* (a[0]: path, a[1]: mode) */
#define V4_LINK      9  /* (a[0]: oldpath, a[1]: newpath) */
#define V4_UNLINK   10  /* (a[0]: path) */
#define V4_EXEC     11  /* (a[0]: path, a[1]: argv) */
#define V4_CHDIR    12  /* (a[0]: path) */
#define V4_TIME     13  /* () */
#define V4_MKNOD    14  /* (a[0]: path, a[1]: mode, a[2]: dev) */
#define V4_CHMOD    15  /* (a[0]: path, a[1]: mode) */
#define V4_CHOWN    16  /* (a[0]: path, a[1]: uid) */
#define V4_BREAK    17  /* (a[0]: addr) */
#define V4_STAT     18  /* (a[0]: path, a[1]: statbuf) */
#define V4_SEEK     19  /* (R0: fd, a[0]: offset, a[1]: whence) */
#define V4_MOUNT    21  /* (a[0]: special, a[1]: dir, a[2]: rwflag) */
#define V4_UMOUNT   22  /* (a[0]: special) */
#define V4_SETUID   23  /* (R0: uid) */
#define V4_GETUID   24  /* () */
#define V4_STIME    25  /* (R0: time_hi, R1: time_lo) */
#define V4_FSTAT    28  /* (R0: fd, a[0]: statbuf) */
#define V4_SMDATE   30  /* () */
#define V4_STTY     31  /* (R0: fd, a[0]: ttybuf) */
#define V4_GTTY     32  /* (R0: fd, a[0]: ttybuf) */
#define V4_NICE     34  /* (R0: prio) */
#define V4_SLEEP    35  /* (R0: seconds) */
#define V4_SYNC     36  /* () */
#define V4_KILL     37  /* (R0: pid, a[0]: sig) */
#define V4_GETSWIT  38  /* () */
#define V4_DUP      41  /* (R0: fd) */
#define V4_PIPE     42  /* () */
#define V4_TIMES    43  /* (a[0]: tmsbuf) */
#define V4_PROFIL   44  /* (a[0]: buf, a[1]: size, a[2]: offset, a[3]: scale) */
#define V4_SETGID   46  /* (R0: gid) */
#define V4_GETGID   47  /* () */
#define V4_SIG      48  /* (a[0]: signum, a[1]: handler) */

#pragma endregion
#pragma region MISC

#define V4_AOUT_OMAGIC  0407
#define V4_AOUT_NMAGIC  0410
#define V4_MAX_ARGS_LEN 510
#define V4_MEM_MAX      0160000
#define V4_MEM_LOGICAL  0200000
#define V4_NOFILE	    15
#define V4_NSIG		    13

#pragma endregion

#endif /* NUMBERS_H */