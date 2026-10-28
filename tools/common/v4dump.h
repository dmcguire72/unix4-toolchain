#ifndef V4DUMP_H
#define V4DUMP_H

#include <stddef.h>
#include <stdint.h>

#define V4D_MAGIC       "\x89V4DUMP\n"
#define V4D_MAGIC_LEN   8
#define V4D_VER_MAJOR   1
#define V4D_VER_MINOR   0

#define V4D_HDR_SIZE    256
#define V4D_SECT_ENT    16
#define V4D_ALIGN       16
#define V4D_MEM_SIZE    0200000u

#define V4D_NREGS       8
#define V4D_NAC         8
#define V4D_NFD         16
#define V4D_SYM_SIZE    12
#define V4D_MMAP_ENT    16
#define V4D_MAX_SECT    64

#define V4D_H_MAGIC         0
#define V4D_H_VER_MAJOR     8
#define V4D_H_VER_MINOR     10
#define V4D_H_HDR_SIZE      12
#define V4D_H_FILE_SIZE     16
#define V4D_H_CRC           20
#define V4D_H_FLAGS         24
#define V4D_H_NSECT         28
#define V4D_H_SECT_OFF      32
#define V4D_H_SECT_ENT      36
#define V4D_H_TIME          40
#define V4D_H_HOST_PID      48
#define V4D_H_PID           52
#define V4D_H_PPID          54
#define V4D_H_SIGNAL        56
#define V4D_H_FAULT_FLAGS   58
#define V4D_H_FAULT_ADDR    60
#define V4D_H_INST_PC       62
#define V4D_H_R             64
#define V4D_H_PS            80
#define V4D_H_AOUT_MAGIC    82
#define V4D_H_TEXT_START    84
#define V4D_H_TEXT_SIZE     88
#define V4D_H_DATA_START    92
#define V4D_H_DATA_SIZE     96
#define V4D_H_BSS_START     100
#define V4D_H_BSS_SIZE      104
#define V4D_H_IMAGE_END     108
#define V4D_H_BRK           112
#define V4D_H_STACK_LO      116
#define V4D_H_STACK_TOP     120
#define V4D_H_INITIAL_SP    124
#define V4D_H_ENTRY         128
#define V4D_H_MEM_SIZE      132
#define V4D_H_NSYMS         136
#define V4D_H_ARGC          140

#define V4D_F_CHILD         0x0001
#define V4D_FF_ADDR         0x0001

#define V4D_ID(a, b, c, d) \
	((uint32_t)(uint8_t)(a) | (uint32_t)(uint8_t)(b) << 8 | \
	 (uint32_t)(uint8_t)(c) << 16 | (uint32_t)(uint8_t)(d) << 24)

#define V4D_SEC_MMAP    V4D_ID('M', 'M', 'A', 'P')
#define V4D_SEC_MDAT    V4D_ID('M', 'D', 'A', 'T')
#define V4D_SEC_FPU     V4D_ID('F', 'P', 'U', ' ')
#define V4D_SEC_SIGS    V4D_ID('S', 'I', 'G', 'S')
#define V4D_SEC_FDS     V4D_ID('F', 'D', 'S', ' ')
#define V4D_SEC_SYMS    V4D_ID('S', 'Y', 'M', 'S')
#define V4D_SEC_ARGV    V4D_ID('A', 'R', 'G', 'V')
#define V4D_SEC_PATH    V4D_ID('P', 'A', 'T', 'H')
#define V4D_SEC_BT      V4D_ID('B', 'T', ' ', ' ')

#define V4D_K_TEXT      1
#define V4D_K_PAD       2
#define V4D_K_DATA      3
#define V4D_K_BSS       4
#define V4D_K_HEAP      5
#define V4D_K_HOLE      6
#define V4D_K_STACK     7
#define V4D_K_ARGS      8

#define V4D_SEG_STORED  0x0001

static inline void v4d_p16(uint8_t *p, uint16_t v)
{
	p[0] = (uint8_t)v;
	p[1] = (uint8_t)(v >> 8);
}

static inline void v4d_p32(uint8_t *p, uint32_t v)
{
	p[0] = (uint8_t)v;
	p[1] = (uint8_t)(v >> 8);
	p[2] = (uint8_t)(v >> 16);
	p[3] = (uint8_t)(v >> 24);
}

static inline void v4d_p64(uint8_t *p, uint64_t v)
{
	v4d_p32(p, (uint32_t)v);
	v4d_p32(p + 4, (uint32_t)(v >> 32));
}

static inline uint16_t v4d_g16(const uint8_t *p)
{
	return (uint16_t)(p[0] | p[1] << 8);
}

static inline uint32_t v4d_g32(const uint8_t *p)
{
	return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
	       (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static inline uint64_t v4d_g64(const uint8_t *p)
{
	return (uint64_t)v4d_g32(p) | (uint64_t)v4d_g32(p + 4) << 32;
}

static inline uint32_t v4d_crc32(uint32_t crc, const void *buf, size_t n)
{
	const uint8_t *p = buf;
	int k;

	crc = ~crc;
	while (n--) {
		crc ^= *p++;
		for (k = 0; k < 8; k++)
			crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1)));
	}
	return ~crc;
}

static inline const char *v4d_signame(unsigned s)
{
	static const char *const nm[] = {
		"0", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP",
		"SIGIOT", "SIGEMT", "SIGFPE", "SIGKILL", "SIGBUS", "SIGSEGV",
		"SIGSYS"
	};

	return s < sizeof nm / sizeof nm[0] ? nm[s] : "SIG?";
}

static inline const char *v4d_sigdesc(unsigned s)
{
	static const char *const d[] = {
		"", "hangup", "interrupt", "quit", "illegal instruction",
		"trace/BPT trap", "IOT trap", "EMT trap", "floating exception",
		"kill", "bus error / odd address", "segmentation violation",
		"bad system call"
	};

	return s < sizeof d / sizeof d[0] ? d[s] : "unknown signal";
}

static inline const char *v4d_kindname(unsigned k)
{
	switch (k) {
	case V4D_K_TEXT:  return "text";
	case V4D_K_PAD:   return "pad";
	case V4D_K_DATA:  return "data";
	case V4D_K_BSS:   return "bss";
	case V4D_K_HEAP:  return "heap";
	case V4D_K_HOLE:  return "unmapped";
	case V4D_K_STACK: return "stack";
	case V4D_K_ARGS:  return "args";
	}
	return "?";
}

#endif /* V4DUMP_H */
