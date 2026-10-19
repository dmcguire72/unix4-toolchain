CC	= cc
CFLAGS	= -std=gnu11 -O2 -Wall -Wno-unused-result
SRC	= src
BLD	= build
BIN	= bin

PASS0	= $(SRC)/cc0.c $(SRC)/cc0t.c $(SRC)/ccrt.c
TABLES	= $(BLD)/regtab.c $(BLD)/efftab.c $(BLD)/cctab.c $(BLD)/sptab.c
PASS1	= $(SRC)/cc1.c $(SRC)/cc1t.c $(SRC)/ccrt.c $(TABLES)

EMULATOR_SRC = \
    tools/emulator/main.c \
	tools/emulator/options.c \
	tools/emulator/program.c \
	tools/emulator/script.c \
	tools/emulator/filesystem.c \
    tools/emulator/state.c \
    tools/emulator/memory.c \
    tools/emulator/loader.c \
    tools/emulator/syscalls.c \
	tools/emulator/signals.c \
    tools/emulator/fpu.c \
    tools/emulator/cpu.c \
	tools/emulator/trace.c

EMULATOR_HDR = tools/emulator/pdp11.h

all: $(BIN)/cc0 $(BIN)/cc1 $(BIN)/cvopt $(BIN)/cc11
emulator: $(BIN)/v4linux $(BIN)/v4linux-dispatch

$(BIN) $(BLD):
	mkdir -p $@

$(BIN)/cvopt: $(SRC)/cvopt.c | $(BIN)
	$(CC) $(CFLAGS) -o $@ $<

$(BLD)/%tab.c: tables/%tab.s $(BIN)/cvopt | $(BLD)
	$(BIN)/cvopt $< $@

$(BIN)/cc0: $(PASS0) $(SRC)/cc0.h $(SRC)/ccw.h | $(BIN)
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $(PASS0) -lm

$(BIN)/cc1: $(PASS1) $(SRC)/cc1.h $(SRC)/ccw.h | $(BIN)
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $(PASS1) -lm

$(BIN)/cc11: $(SRC)/cc11.c | $(BIN)
	$(CC) $(CFLAGS) -Wno-format-truncation -o $@ $<

$(BIN)/v4linux: $(EMULATOR_SRC) $(EMULATOR_HDR) | $(BIN)
	$(CC) $(CFLAGS) -Itools/emulator -o $@ $(EMULATOR_SRC) -lm

$(BIN)/v4linux-dispatch: tools/emulator/linux/v4linux-dispatch.c | $(BIN)
	$(CC) $(CFLAGS) -o $@ $<

check: all
	@fail=0; t=$$(mktemp -d); \
	for f in tests/src/*.c; do n=$$(basename $$f .c); \
	  cp $$f $$t/$$n.c; \
	  (cd $$t && $(CURDIR)/$(BIN)/cc11 -S $$n.c >/dev/null 2>&1); \
	  if cmp -s $$t/$$n.s tests/expected/$$n.s; then echo "ok   $$n"; \
	  else echo "FAIL $$n"; fail=1; fi; \
	done; rm -rf $$t; exit $$fail

clean:
	rm -rf $(BIN) $(BLD)

.PHONY: all emulator check clean