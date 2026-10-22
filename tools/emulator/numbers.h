#ifndef NUMBERS_H
#define NUMBERS_H

#pragma region CPU

#define OP_MOV             1
#define OP_CMP             2
#define OP_BIT             3
#define OP_BIC             4
#define OP_BIS             5
#define OP_ADD             6

#define EIS_MUL            0
#define EIS_DIV            1
#define EIS_ASH            2
#define EIS_ASHC           3
#define EIS_XOR            4
#define EIS_SOB            7

#define SOP_CLR         0050
#define SOP_COM         0051
#define SOP_INC         0052
#define SOP_DEC         0053
#define SOP_NEG         0054
#define SOP_ADC         0055
#define SOP_SBC         0056
#define SOP_TST         0057
#define SOP_ROR         0060
#define SOP_ROL         0061
#define SOP_ASR         0062
#define SOP_ASL         0063
#define SOP_MARK        0064
#define SOP_MTPS    SOP_MARK
#define SOP_MFPI        0065
#define SOP_MFPD    SOP_MFPI
#define SOP_MTPI        0066
#define SOP_MTPD    SOP_MTPI
#define SOP_SXT         0067
#define SOP_MFPS     SOP_SXT

#define MISC_GRP_SPECIAL   0
#define MISC_GRP_JMP       1
#define MISC_GRP_RTS       2
#define MISC_GRP_SWAB      3

#define OP_HALT      0000000
#define OP_WAIT      0000001
#define OP_RTI       0000002
#define OP_BPT       0000003
#define OP_IOT       0000004
#define OP_RESET     0000005
#define OP_RTT       0000006

#define OP_RTS_MASK   0000070
#define OP_RTS_VAL    0000000
#define OP_SPL_MASK  00177770
#define OP_SPL_VAL    0000230
#define OP_CC_MASK   00177740
#define OP_CC_VAL     0000240

#define GRP_MISC  0
#define GRP_MOV   1
#define GRP_CMP   2
#define GRP_BIT   3
#define GRP_BIC   4
#define GRP_BIS   5
#define GRP_ADD   6
#define GRP_EIS   7
#define GRP_MOVB  9
#define GRP_CMPB  10
#define GRP_BITB  11
#define GRP_BICB  12
#define GRP_BISB  13
#define GRP_SUB   14
#define GRP_FP    15

#define MISC_SPECIAL_LIMIT  0000400
#define BRANCH_JSR_LIMIT    0004000

#define BR_BR    0000400
#define BR_BNE   0001000
#define BR_BEQ   0001400
#define BR_BGE   0002000
#define BR_BLT   0002400
#define BR_BGT   0003000
#define BR_BLE   0003400

#define OP_JSR_MASK  00177000
#define OP_JSR_VAL   0004000

#define BR_BPL   0100000
#define BR_BMI   0100400
#define BR_BHI   0101000
#define BR_BLOS  0101400
#define BR_BVC   0102000
#define BR_BVS   0102400
#define BR_BCC   0103000
#define BR_BCS   0103400
#define OP_EMT   0104000
#define OP_TRAP  0104400
#define OP_TRAP_VEC_MASK  0000077

#define PROF_TICK_RATE   8192
#define OP_LOW_MASK      0177400

#pragma endregion
#pragma region FPU

#define FP_NUM_AC               8
#define PDP11_WORD_SIZE         2
#define PDP11_PC_REG            7
#define PDP11_MODE_DIRECT       0
#define PDP11_MODE_AUTOINC      2

#define FP_SINGLE_BYTES         4
#define FP_DOUBLE_BYTES         8
#define INT_SHORT_BYTES         2
#define INT_LONG_BYTES          4

#define FPS_CC_MASK             017
#define FPS_CC_C_SHIFT          0
#define FPS_CC_V_SHIFT          1
#define FPS_CC_Z_SHIFT          2
#define FPS_CC_N_SHIFT          3

#define FP_SIGN_BIT             0x8000
#define FP_SIGN_SHIFT           15

#define FP_EXP_BIAS             128
#define FP_EXP_MAX              255
#define FP_EXP_SHIFT            7
#define FP_EXP_MASK             0xFF

#define FP_FRAC_HIGH_MASK       0x7F
#define FP_HIDDEN_BIT           0x80

#define FP_FRAC_BITS_SINGLE     24
#define FP_FRAC_BITS_DOUBLE     56

#define FP_MANT_SHIFT_WORD0_DBL 48
#define FP_MANT_SHIFT_WORD0_SNG 16
#define FP_MANT_SHIFT_WORD1     32
#define FP_MANT_SHIFT_WORD2     16

#define IEEE754_DOUBLE_SIGN_SHIFT       63
#define IEEE754_DOUBLE_SIGN_MASK        (1ULL << IEEE754_DOUBLE_SIGN_SHIFT)
#define IEEE754_DOUBLE_EXP_BIAS         1023
#define IEEE754_DOUBLE_FRAC_BITS        52
#define IEEE754_DOUBLE_FRAC_MASK        ((1ULL << IEEE754_DOUBLE_FRAC_BITS) - 1)
#define IEEE754_DOUBLE_HIDDEN_BIT       (1ULL << IEEE754_DOUBLE_FRAC_BITS)

#define FP_FAST_DBL_MANT_SHIFT          3
#define FP_FAST_SNG_ROUND_BIT           (1ULL << 28)
#define FP_FAST_SNG_TRUNC_MASK          ((1ULL << 29) - 1)
#define FP_FAST_SNG_FRAC_SHIFT          29
#define FP_FAST_SNG_HIDDEN_BIT          (1ULL << 23)
#define FP_FAST_SNG_FRAC_MASK           0x7FFFFF
#define FP_FAST_EXP_BIAS_OFFSET         894

#define FP_ROUND_NEAREST_BIAS           0.5

#define FP_INT16_MAX            32767
#define FP_INT16_MIN            -32768
#define FP_INT32_MAX_F          2147483647.0
#define FP_INT32_MIN_F          -2147483648.0

#define FP_IR_AC_SHIFT          6
#define FP_IR_AC_MASK           3
#define FP_IR_DM_SHIFT          3
#define FP_IR_DM_MASK           7
#define FP_IR_DR_MASK           7
#define FP_IR_NEG_BIT           0100

#define FP_OP_MASK_GROUP1       0177700
#define FP_OP_MASK_GROUP2       0177400

#define OP_LDFPS                0170100
#define OP_STFPS                0170200
#define OP_STST                 0170300
#define OP_CLRF                 0170400
#define OP_TSTF                 0170500
#define OP_ABSF                 0170600
#define OP_NEGF                 0170700

#define OP_MULF                 0171000
#define OP_MODF                 0171400
#define OP_ADDF                 0172000
#define OP_LDF                  0172400
#define OP_SUBF                 0173000
#define OP_CMPF                 0173400
#define OP_STF                  0174000
#define OP_DIVF                 0174400
#define OP_STEXP                0175000
#define OP_STCFI                0175400
#define OP_STCFD                0176000
#define OP_LDEXP                0176400
#define OP_LDCIF                0177000
#define OP_LDCDF                0177400

#define OP_CFCC                 0170000
#define OP_SETF                 0170001
#define OP_SETI                 0170002
#define OP_SETD                 0170011
#define OP_SETL                 0170012

#pragma endregion
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