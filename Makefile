CC ?= cc
STRIP = strip
CFLAGS = -std=gnu11 -O3 -Wall -Wno-unused-result
DEPFLAGS = -MMD -MP

USR ?= usr
BIN ?= bin
TMP ?= tmp

C_SRC ?= $(USR)/c
S1_SRC ?= $(USR)/source/s1

BLD = $(TMP)/build
OBJ = $(TMP)/obj
DEP = $(TMP)/deps

TO_CLEANUP = $(TMP) $(BIN)

ifeq ($(V),1)
  Q :=
  ECHO := @:
else
  Q := @
  ECHO := @printf "  %-7s %s\n"
endif

PASS0 = $(C_SRC)/cc0.c $(C_SRC)/cc0t.c $(C_SRC)/ccrt.c
TABLES = $(BLD)/regtab.c $(BLD)/efftab.c $(BLD)/cctab.c $(BLD)/sptab.c
.SECONDARY: $(TABLES)
PASS1 = $(C_SRC)/cc1.c $(C_SRC)/cc1t.c $(C_SRC)/ccrt.c $(TABLES)

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
    tools/emulator/hooks.c \
    tools/emulator/dump.c

EMULATOR_OBJ = $(patsubst tools/emulator/%.c,$(OBJ)/emulator/%.o,$(EMULATOR_SRC))

AS_OBJ = $(OBJ)/s1/as1.o $(OBJ)/s1/astab.o
AS2_OBJ = $(OBJ)/s1/as2.o $(OBJ)/s1/astab.o
PASS0_OBJ = $(OBJ)/c/cc0.o $(OBJ)/c/cc0t.o $(OBJ)/c/ccrt.o
TABLES_OBJ = $(OBJ)/bld/regtab.o $(OBJ)/bld/efftab.o $(OBJ)/bld/cctab.o $(OBJ)/bld/sptab.o
PASS1_OBJ = $(OBJ)/c/cc1.o $(OBJ)/c/cc1t.o $(OBJ)/c/ccrt.o $(TABLES_OBJ)

DEPFILES = $(patsubst $(OBJ)/%.o,$(DEP)/%.d,$(sort \
    $(EMULATOR_OBJ) \
    $(AS_OBJ) \
    $(AS2_OBJ) \
    $(PASS0_OBJ) \
    $(PASS1_OBJ) \
    $(TABLES_OBJ)))

BIN_DEP_NAMES = ar ld tp cvopt cc11 strip v4dis v4linux-dispatch dumpinfo
BIN_DEPS = $(addprefix $(DEP)/bin/,$(addsuffix .d,$(BIN_DEP_NAMES)))

STRIP_TARGETS = $(BIN)/as $(BIN)/as2 $(BIN)/ld \
                $(BIN)/ar $(BIN)/strip $(BIN)/tp \
                $(BIN)/cc0 $(BIN)/cc1 $(BIN)/cc11 \
                $(BIN)/v4linux $(BIN)/v4linux-dispatch $(BIN)/v4dis \
                $(BIN)/dumpinfo

all: $(BIN)/as $(BIN)/as2 $(BIN)/ld \
     $(BIN)/ar $(BIN)/strip $(BIN)/tp \
     $(BIN)/cc0 $(BIN)/cc1 $(BIN)/cvopt $(BIN)/cc11 \
     $(BIN)/v4dis $(BIN)/dumpinfo emulator

emulator: $(BIN)/v4linux $(BIN)/v4linux-dispatch

$(BIN) $(BLD) $(OBJ)/emulator $(OBJ)/s1 $(OBJ)/c $(OBJ)/bld $(DEP)/emulator $(DEP)/s1 $(DEP)/c $(DEP)/bld $(DEP)/bin:
	$(ECHO) MKDIR $@
	$(Q)mkdir -p $@

$(OBJ)/emulator/%.o: tools/emulator/%.c | $(OBJ)/emulator $(DEP)/emulator
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/emulator/$*.d -MT $@ -Itools/emulator -Itools/common -c -o $@ $<

$(OBJ)/s1/%.o: $(S1_SRC)/%.c | $(OBJ)/s1 $(DEP)/s1
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/s1/$*.d -MT $@ -Isrc -c -o $@ $<

$(OBJ)/c/%.o: $(C_SRC)/%.c | $(OBJ)/c $(DEP)/c
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/c/$*.d -MT $@ -I$(C_SRC) -c -o $@ $<

$(OBJ)/bld/%.o: $(BLD)/%.c | $(OBJ)/bld $(DEP)/bld
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bld/$*.d -MT $@ -I$(C_SRC) -c -o $@ $<

$(BIN)/as: $(AS_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(AS_OBJ)

$(BIN)/ar: $(S1_SRC)/ar.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/ar.d -MT $@ -Wno-format-truncation -o $@ $<

$(BIN)/as2: $(AS2_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(AS2_OBJ)

$(BIN)/ld: $(S1_SRC)/ld.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/ld.d -MT $@ -o $@ $<

$(BIN)/cvopt: $(C_SRC)/cvopt.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/cvopt.d -MT $@ -o $@ $<

$(BLD)/%tab.c: tables/%tab.s $(BIN)/cvopt | $(BLD)
	$(ECHO) CVOPT $@
	$(Q)$(BIN)/cvopt $< $@

$(BIN)/cc0: $(PASS0_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(PASS0_OBJ) -lm

$(BIN)/cc1: $(PASS1_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(PASS1_OBJ) -lm

$(BIN)/cc11: $(C_SRC)/cc11.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/cc11.d -MT $@ -Wno-format-truncation -o $@ $<

$(BIN)/strip: $(S1_SRC)/strip.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/strip.d -MT $@ -Wno-format-truncation -o $@ $<

$(BIN)/v4linux: $(EMULATOR_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(EMULATOR_OBJ) -lm

$(BIN)/v4dis: tools/v4dis/v4dis.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/v4dis.d -MT $@ -o $@ $<

$(BIN)/tp: $(S1_SRC)/tp.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/tp.d -MT $@ -o $@ $<

$(BIN)/dumpinfo: tools/dumpinfo/dumpinfo.c tools/common/v4dump.h | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/dumpinfo.d -MT $@ -Itools/common -o $@ $<

$(BIN)/v4linux-dispatch: tools/emulator/linux/v4linux-dispatch.c | $(BIN) $(DEP)/bin
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -MF $(DEP)/bin/v4linux-dispatch.d -MT $@ -o $@ $<

STRIP_TASKS = $(addsuffix -strip,$(STRIP_TARGETS))

strip: $(STRIP_TASKS)

$(STRIP_TASKS): %-strip: %
	$(ECHO) STRIP $<
	$(Q)$(STRIP) $<

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

check-ar: $(BIN)/ar
	$(ECHO) TEST $@
	@BIN=$(CURDIR)/$(BIN) sh tests/ar/run.sh check

check-strip: $(BIN)/as $(BIN)/as2 $(BIN)/ld $(BIN)/strip
	$(ECHO) TEST $@
	@BIN=$(CURDIR)/$(BIN) sh tests/strip/run.sh check

check-dump: $(BIN)/as $(BIN)/as2 $(BIN)/v4linux $(BIN)/dumpinfo $(BIN)/v4dis
	$(ECHO) TEST $@
	@BIN=$(CURDIR)/$(BIN) sh tests/dump/run.sh

check: check-as check-cc check-ld check-ar check-strip check-dump

dis-check: $(BIN)/v4dis $(BIN)/v4linux
	$(ECHO) TEST $@
	@python3 tools/v4dis/roundtrip.py --v4dis $(BIN)/v4dis --emu $(BIN)/v4linux --root v4 --scan v4

install: all
	$(ECHO) INSTALL $(BIN)
	$(Q)./tools/v4linux-setup.py $(if $(filter 1,$(V)),-v,) $(BIN)

uninstall:
	$(ECHO) UNINSTALL
	$(Q)./tools/v4linux-uninstall.sh $(if $(filter 1,$(V)),-v,) --purge-all

clean:
	$(ECHO) CLEAN "$(TO_CLEANUP)"
	$(Q)rm -rf $(TO_CLEANUP)

.PHONY: all emulator strip $(STRIP_TASKS) check check-as check-cc check-ld check-ar check-strip check-dump dis-check install uninstall clean

-include $(DEPFILES) $(BIN_DEPS)