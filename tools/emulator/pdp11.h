#ifndef PDP11_H
#define PDP11_H

#include <stdint.h>
#include <stdio.h>

extern uint8_t mem[65536];
extern uint16_t R[8];
extern int fN, fZ, fV, fC;
extern const char *root;
extern int stop_at_exec;
extern int fdtab[16];
extern uint16_t sigtab[64];
extern int trace;

#define SP R[6]
#define PC R[7]

extern int regdst;
extern uint16_t ea;

void die(const char *m, ...) __attribute__((noreturn));
uint16_t rdw(uint16_t a);
void wrw(uint16_t a, uint16_t v);
void getops(int mode, int reg, int size);
void getop(int mode, int reg, int byte);
uint16_t load(int byte);
void store(uint16_t v, int byte);
void storeb_sext(uint16_t v);
void nz(uint16_t v, int byte);

int load_image(const char *path, int argc, char **argv);
void v4sys(int n);
void fstep(uint16_t ir);
void step(void);

#endif
