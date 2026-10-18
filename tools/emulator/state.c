#include "pdp11.h"

uint8_t mem[65536];
uint16_t R[8];
int fN, fZ, fV, fC;
const char *root = "";
int stop_at_exec;
int fdtab[16];
uint16_t sigtab[64];
int trace;

uint16_t prof_buf, prof_len, prof_off, prof_scale;
int load_errno;
