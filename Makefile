# Copyright 2026 Daniel McGuire
# Licensed under the MIT license. 

CC	?= cc
CFLAGS	?= -std=c11 -O2 -Wall -Wextra -pedantic -I.

BUILD_DIR ?= build
BIN_DIR	   = $(BUILD_DIR)/bin
OBJ_DIR	   = $(BUILD_DIR)/obj

PASS0_OBJS = $(addprefix $(OBJ_DIR)/, c00.o c01.o c02.o c03.o c0t.o ccmn.o)
PASS1_OBJS = $(addprefix $(OBJ_DIR)/, c10.o c11.o c1t.o regtab.o efftab.o cctab.o sptab.o ccmn.o)
CVOPT_OBJS = $(OBJ_DIR)/cvopt.o

TARGETS	   = $(BIN_DIR)/c0 $(BIN_DIR)/c1 $(BIN_DIR)/cvopt

all: $(TARGETS)

c0: $(BIN_DIR)/c0
c1: $(BIN_DIR)/c1
cvopt: $(BIN_DIR)/cvopt

$(BIN_DIR)/c0: $(PASS0_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(PASS0_OBJS)

$(BIN_DIR)/c1: $(PASS1_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(PASS1_OBJS)

$(BIN_DIR)/cvopt: $(CVOPT_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(CVOPT_OBJS)

$(OBJ_DIR)/%tab.c: %tab.s $(BIN_DIR)/cvopt | $(OBJ_DIR)
	$(BIN_DIR)/cvopt $< $@

$(OBJ_DIR)/%tab.o: $(OBJ_DIR)/%tab.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(addprefix $(OBJ_DIR)/, c00.o c01.o c02.o c03.o c0t.o): c0.h cc.h
$(addprefix $(OBJ_DIR)/, c10.o c11.o c1t.o regtab.o efftab.o cctab.o sptab.o): c1.h cc.h
$(OBJ_DIR)/ccmn.o: cc.h

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

check: all
	@fail=0; for f in tests/src/*.c; do \
	  n=$$(basename $$f .c); \
	  BUILD_DIR=$(BUILD_DIR) ./cc11 $$f /tmp/$$n.$$$$.s || { echo "FAIL $$n (compile)"; fail=1; continue; }; \
	  if cmp -s /tmp/$$n.$$$$.s tests/expected/$$n.s; then echo "ok   $$n"; \
	  else echo "FAIL $$n (output differs)"; fail=1; fi; rm -f /tmp/$$n.$$$$.s; \
	done; exit $$fail

baseline: all
	for f in tests/src/*.c; do \
	  BUILD_DIR=$(BUILD_DIR) ./cc11 $$f tests/expected/$$(basename $$f .c).s; \
	done

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean check baseline c0 c1 cvopt