CC = cc
STRIP = strip
CFLAGS = -std=gnu11 -O3 -Wall -Wno-unused-result

SRC = src
BLD = build
BIN = bin

ifeq ($(V),1)
  Q :=
  ECHO := @:
else
  Q := @
  ECHO := @printf "  %-7s %s\n"
endif

PASS0 = $(SRC)/cc0.c $(SRC)/cc0t.c $(SRC)/ccrt.c
TABLES = $(BLD)/regtab.c $(BLD)/efftab.c $(BLD)/cctab.c $(BLD)/sptab.c
PASS1 = $(SRC)/cc1.c $(SRC)/cc1t.c $(SRC)/ccrt.c $(TABLES)

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
	tools/emulator/trace.c \
	tools/emulator/symbols.c \
	tools/emulator/hooks.c

EMULATOR_HDR = tools/emulator/pdp11.h

STRIP_TARGETS = $(BIN)/as $(BIN)/as2 $(BIN)/ld \
                $(BIN)/cc0 $(BIN)/cc1 $(BIN)/cvopt $(BIN)/cc11 \
                $(BIN)/v4linux $(BIN)/v4linux-dispatch $(BIN)/v4dis

all: $(BIN)/as $(BIN)/as2 $(BIN)/ld \
     $(BIN)/cc0 $(BIN)/cc1 $(BIN)/cvopt $(BIN)/cc11 \
     $(BIN)/v4dis emulator

emulator: $(BIN)/v4linux $(BIN)/v4linux-dispatch

$(BIN) $(BLD):
	$(ECHO) MKDIR $@
	$(Q)mkdir -p $@

$(BIN)/as: src/as1.c src/astab.c src/as.h tables/chartab.inc | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -Isrc -o $@ src/as1.c src/astab.c

$(BIN)/as2: src/as2.c src/astab.c src/as.h | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -Isrc -o $@ src/as2.c src/astab.c

$(BIN)/ld: $(SRC)/ld.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -o $@ $<

$(BIN)/cvopt: $(SRC)/cvopt.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -o $@ $<

$(BLD)/%tab.c: tables/%tab.s $(BIN)/cvopt | $(BLD)
	$(ECHO) CVOPT $@
	$(Q)$(BIN)/cvopt $< $@

$(BIN)/cc0: $(PASS0) $(SRC)/cc0.h $(SRC)/ccw.h | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -I$(SRC) -o $@ $(PASS0) -lm

$(BIN)/cc1: $(PASS1) $(SRC)/cc1.h $(SRC)/ccw.h | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -I$(SRC) -o $@ $(PASS1) -lm

$(BIN)/cc11: $(SRC)/cc11.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -Wno-format-truncation -o $@ $<

$(BIN)/v4linux: $(EMULATOR_SRC) $(EMULATOR_HDR) | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -Itools/emulator -o $@ $(EMULATOR_SRC) -lm

$(BIN)/v4dis: tools/v4dis/v4dis.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -o $@ $<

$(BIN)/v4linux-dispatch: tools/emulator/linux/v4linux-dispatch.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) -o $@ $<

strip: all
	$(ECHO) STRIP "$(STRIP_TARGETS)"
	$(Q)$(STRIP) $(STRIP_TARGETS)

check-as: all
	$(ECHO) TEST $@
	@fail=0; t=$$(mktemp -d); \
	for f in tests/as/src/*.s; do \
		n=$$(basename $$f .s); \
		rm -rf $$t/w; mkdir -p $$t/w; cp $$f $$t/w/in.s; \
		(cd $$t/w && TMPDIR=$$t/w $(CURDIR)/$(BIN)/as in.s >out.txt 2>&1); \
		ok=1; \
		if [ -f tests/as/expected/$$n.out ]; then \
			cmp -s $$t/w/a.out tests/as/expected/$$n.out || ok=0; \
		else \
			[ ! -f $$t/w/a.out ] || ok=0; \
		fi; \
		cmp -s $$t/w/out.txt tests/as/expected/$$n.err || ok=0; \
		if [ $$ok = 1 ]; then \
			echo "ok $$n"; \
		else \
			echo "FAIL $$n"; fail=1; \
		fi; \
	done; \
	rm -rf $$t; exit $$fail

check-cc: all
	$(ECHO) TEST $@
	@fail=0; t=$$(mktemp -d); \
	for f in tests/cc/src/*.c; do \
		n=$$(basename $$f .c); \
		cp $$f $$t/$$n.c; \
		(cd $$t && $(CURDIR)/$(BIN)/cc11 -S $$n.c >/dev/null 2>&1); \
		if cmp -s $$t/$$n.s tests/cc/expected/$$n.s; then \
			echo "ok $$n"; \
		else \
			echo "FAIL $$n"; fail=1; \
		fi; \
	done; \
	rm -rf $$t; exit $$fail

check-ld: $(BIN)/as $(BIN)/as2 $(BIN)/ld
	$(ECHO) TEST $@
	@BIN=$(CURDIR)/$(BIN) sh tests/ld/run.sh check

check: check-as check-cc check-ld

dis-check: $(BIN)/v4dis $(BIN)/v4linux
	$(ECHO) TEST $@
	@python3 tools/v4dis/roundtrip.py --v4dis $(BIN)/v4dis --emu $(BIN)/v4linux --root v4 --scan v4

clean:
	$(ECHO) CLEAN "$(BIN) $(BLD)"
	$(Q)rm -rf $(BIN) $(BLD)

.PHONY: all emulator strip check check-as check-cc check-ld dis-check clean