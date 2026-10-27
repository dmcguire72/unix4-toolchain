CC ?= cc
STRIP = strip
CFLAGS = -std=gnu11 -O3 -Wall -Wno-unused-result
DEPFLAGS = -MMD -MP

USR = usr
BIN = bin
TMP = tmp

C_SRC = $(USR)/c
S1_SRC = $(USR)/source/s1

BLD = $(TMP)/build
OBJ = $(TMP)/obj

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
	tools/emulator/hooks.c

EMULATOR_OBJ = $(patsubst tools/emulator/%.c,$(OBJ)/emulator/%.o,$(EMULATOR_SRC))

AS_OBJ = $(OBJ)/s1/as1.o $(OBJ)/s1/astab.o
AS2_OBJ = $(OBJ)/s1/as2.o $(OBJ)/s1/astab.o
PASS0_OBJ = $(OBJ)/c/cc0.o $(OBJ)/c/cc0t.o $(OBJ)/c/ccrt.o
TABLES_OBJ = $(OBJ)/bld/regtab.o $(OBJ)/bld/efftab.o $(OBJ)/bld/cctab.o $(OBJ)/bld/sptab.o
PASS1_OBJ = $(OBJ)/c/cc1.o $(OBJ)/c/cc1t.o $(OBJ)/c/ccrt.o $(TABLES_OBJ)

STRIP_TARGETS = $(BIN)/as $(BIN)/as2 $(BIN)/ld \
				$(BIN)/ar $(BIN)/strip \
                $(BIN)/cc0 $(BIN)/cc1 $(BIN)/cc11 \
                $(BIN)/v4linux $(BIN)/v4linux-dispatch $(BIN)/v4dis

all: $(BIN)/as $(BIN)/as2 $(BIN)/ld \
	 $(BIN)/ar $(BIN)/strip \
     $(BIN)/cc0 $(BIN)/cc1 $(BIN)/cvopt $(BIN)/cc11 \
     $(BIN)/v4dis emulator

emulator: $(BIN)/v4linux $(BIN)/v4linux-dispatch

$(BIN) $(BLD) $(OBJ)/emulator $(OBJ)/s1 $(OBJ)/c $(OBJ)/bld:
	$(ECHO) MKDIR $@
	$(Q)mkdir -p $@

$(OBJ)/emulator/%.o: tools/emulator/%.c | $(OBJ)/emulator
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -Itools/emulator -c -o $@ $<

$(OBJ)/s1/%.o: $(S1_SRC)/%.c | $(OBJ)/s1
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -Isrc -c -o $@ $<

$(OBJ)/c/%.o: $(C_SRC)/%.c | $(OBJ)/c
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -I$(C_SRC) -c -o $@ $<

$(OBJ)/bld/%.o: $(BLD)/%.c | $(OBJ)/bld
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -I$(C_SRC) -c -o $@ $<

$(BIN)/as: $(AS_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(AS_OBJ)

$(BIN)/ar: $(S1_SRC)/ar.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -Wno-format-truncation -o $@ $<

$(BIN)/as2: $(AS2_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(AS2_OBJ)

$(BIN)/ld: $(S1_SRC)/ld.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $<

$(BIN)/cvopt: $(C_SRC)/cvopt.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $<

$(BLD)/%tab.c: tables/%tab.s $(BIN)/cvopt | $(BLD)
	$(ECHO) CVOPT $@
	$(Q)$(BIN)/cvopt $< $@

$(BIN)/cc0: $(PASS0_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(PASS0_OBJ) -lm

$(BIN)/cc1: $(PASS1_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(PASS1_OBJ) -lm

$(BIN)/cc11: $(C_SRC)/cc11.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -Wno-format-truncation -o $@ $<

$(BIN)/strip: $(S1_SRC)/strip.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -Wno-format-truncation -o $@ $<

$(BIN)/v4linux: $(EMULATOR_OBJ) | $(BIN)
	$(ECHO) LD $@
	$(Q)$(CC) $(CFLAGS) -o $@ $(EMULATOR_OBJ) -lm

$(BIN)/v4dis: tools/v4dis/v4dis.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $<

$(BIN)/v4linux-dispatch: tools/emulator/linux/v4linux-dispatch.c | $(BIN)
	$(ECHO) CC $@
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -o $@ $<

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

check: check-as check-cc check-ld check-ar check-strip

dis-check: $(BIN)/v4dis $(BIN)/v4linux
	$(ECHO) TEST $@
	@python3 tools/v4dis/roundtrip.py --v4dis $(BIN)/v4dis --emu $(BIN)/v4linux --root v4 --scan v4

install: all
	$(ECHO) INSTALL $(BIN)
	$(Q)./tools/v4linux-setup.sh $(if $(filter 1,$(V)),-v,) $(BIN)

uninstall:
	$(ECHO) UNINSTALL
	$(Q)./tools/v4linux-uninstall.sh $(if $(filter 1,$(V)),-v,) --purge-all

clean:
	$(ECHO) CLEAN "$(TO_CLEANUP)"
	$(Q)rm -rf $(TO_CLEANUP)

.PHONY: all emulator strip $(STRIP_TASKS) check check-as check-cc check-ld check-ar check-strip dis-check install uninstall clean

DEPS := $(EMULATOR_OBJ:.o=.d) \
        $(AS_OBJ:.o=.d) $(AS2_OBJ:.o=.d) \
        $(PASS0_OBJ:.o=.d) $(PASS1_OBJ:.o=.d) $(TABLES_OBJ:.o=.d)

DEPS += $(BIN)/ar.d $(BIN)/ld.d $(BIN)/cvopt.d $(BIN)/cc11.d \
        $(BIN)/strip.d $(BIN)/v4dis.d $(BIN)/v4linux-dispatch.d

-include $(DEPS)