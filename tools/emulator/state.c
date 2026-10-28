#include "pdp11.h"

uint8_t mem[V4_MEM_LOGICAL];
uint16_t R[8];
int fN, fZ, fV, fC;
const char *root = "";
int stop_at_exec;
int fdtab[16];
uint16_t sigtab[64];
int trace;
int crash_trace;

int host_enabled = 0;

uint16_t prof_buf, prof_len, prof_off, prof_scale;
int load_errno;

uint16_t inst_pc;
uint16_t fault_addr;
int fault_valid;
struct image_info img;
