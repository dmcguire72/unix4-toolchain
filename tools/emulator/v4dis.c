#include "pdp11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int load_image(const char *path, int argc, char **argv);
extern const char *sym_nearest(uint16_t addr, unsigned *off);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return 1;
    }

    if (load_image(argv[1], 0, NULL) != 0) {
        fprintf(stderr, "Error: Could not load binary %s\n", argv[1]);
        return 1;
    }

    uint16_t pc = 0;
    uint32_t end_pc = V4_MEM_LOGICAL; 
    
    printf("--- Disassembly of %s ---\n", argv[1]);
    printf("%-8s %-12s %-20s\n", "Address", "Symbol", "Instruction");
    printf("----------------------------------------------------\n");

    while (pc < end_pc) {
        char inst_buf[96];
        unsigned off;
        const char *sym = sym_nearest(pc, &off);
        
        uint16_t next_pc = disasm(pc, inst_buf, sizeof(inst_buf));

        if (next_pc == pc) {
            printf("%06o: %-12s .word %06o\n", pc, "", rdw(pc));
            pc += 2; 
            continue;
        }

        if (sym && off == 0) {
            printf("%06o: %-12s %-20s\n", pc, sym, inst_buf);
        } else {
            printf("%06o: %-12s %-20s\n", pc, "", inst_buf);
        }

        pc = next_pc;

        if (pc > 0 && pc < V4_MEM_LOGICAL && rdw(pc) == 0) {
            if (rdw(pc + 2) == 0 && rdw(pc + 4) == 0) {
                break;
            }
        }
    }

    return 0;
}