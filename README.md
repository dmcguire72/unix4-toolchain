# Modern Unix Fourth Edition Toolchain (PDP-11)

This repository contains a few components:

- A. CC for Linux (targetting Unix v4 with PDP-11 assembly)
- B. AS for Linux (targetting Unix v4 with the PDP-11 a.out file format)
- C. LD for Linux (targetting Unix v4 for the PDP-11 a.out file format)
- D. AR for Linux (targetting Unix v4 for the PDP-11 a.out file format)
- E. STRIP for Linux (targetting the Unix v4 symbols in PDP-11 a.out file format)
- F. A Unix v4 subsystem for linux using binfmt_misc, syscall translation, long path handling, and a minimal PDP-11 CPU (11/45 + EIS) and FP-11 FPU emulator.
- G. A Disassembler for binaries compiled with this toolchain, or the stock Unix 4 toolchain.
- H. dumpinfo, an inspector for the crash dumps the emulator writes when a program dies.

## A. CC

The CC Compiler is built of many pieces, as the original had memory constraints:

- A. cc / cc11
- B. c0 / cc0
- C. c1 / cc1
- D. c2 / cc2

### A. cc11

```bash
cc [-c] [-S] [-O] [-P] [-p] [-2 | -20] [-t] [options/files...]
# if installed: 
# v4-cc [-c] [-S] [-O] [-P] [-p] [-2 | -20] [-t] [options/files...]
# man v4-cc
```

#### CC Options

- -c: Compile and assemble into objects (.o), no link.
- -S: Compile into assembly (.s), no assemble or link (implies -c).
- -O: Run the peephole optimizer pass (c2) on the assembly code.
- -P: Preprocess C source files only, saving expanded output into .i files (suppress comp, asm, and link).
- -p: Prepare for profiling; passes -P to pass 0 (cc0) and links with mcrt0.o.
- -2: Link with the alternate runtime startup file crt2.o.
- -20 (or any characters following -2): Link with crt20.o and pass -l2 to the linker.
- -t: Accepted/ignored option.
- Any options left unrecognized (e.g. `-l<lib>`) or files (e.g. .o files) are passed to the linker (ld).

### B. Pass 0

```bash
cc0 <infile> <ascii_out> <binary_out> [-P]
```

#### CC0 Options

- infile: Input C source file (or preprocessor output).
- ascii_out: Output for text assembly directives (data, string literals, function labels, etc.).
- binary_out: Output binary temp file holding serialized intermediate representation (IR) expression trees.
- -P (or any 4th argument, argc > 4): Optional, enables profiling mode (proflg), altering function prologues (jsr r5,mrsave).

### C. Pass 1

```bash
cc1 <ascii_in> <binary_in> <outfile>
```

#### CC1 Options

- ascii_in: Input tree boundary ASCII temp file produced by cc0.
- binary_in: Input IR tree binary temp file produced by cc0.
- outfile: Output where assembly is written.

### D. cvopt

```bash
cvopt [-n] [infile [outfile]]
```

#### CVOPT Options

- -n: Disables fp support (nofloat = 1).
- infile (optional): Input pattern/table description. If omitted, reads from stdin.
- outfile (optional): Output C source to generate. If omitted, writes to stdout.

## B. AS

The AS Assembler is also split like the original:

- A. as / as1
- B. as2

### A. as1 (as)

```bash
as [-] file ...
# if installed: 
# v4-as [-] file ...
# man v4-as
```

#### AS Options

- -: Treat undefined symbols as external/global references rather than errors (passes `-g` to `as2`).
- file ...: One or more assembly (.s) source files to assemble together into a single module.
- Environment:
  - TMPDIR: Directory to create intermediate pass files (`atm1a`–`atm4Z`). Defaults to `/tmp`.
  - AS2: Explicit path to the pass 2 executable (`as2`). If unset, checks the directory containing `as`, then searches `PATH`.
- Outputs: Executes `as2` upon pass 1 completion to generate `a.out` in the current working directory.

### B. as2

```bash
as2 <tmp1> <tmp2> <tmp3> <tmp4> [-g]
```

#### AS2 Options

- tmp1: Tokenized intermediate instruction and code produced by pass 1.
- tmp2: Local numeric label table produced by pass 1.
- tmp3: Serialized user symbol table dump produced by pass 1.
- tmp4: Secondary text overflow/spill temporary file produced by pass 1.
- -g (or any 5th arg, `(argc >= 6)`): Optional, marks undefined symbols as global (`defund = 040`) for later resolution by `ld`.

## C. LD

```bash
ld [-s] [-n] [-x] [-r] [-d] [-u sym] [-e sym] [-l[x]] file ...
# if installed: 
# v4-ld [-s] [-n] [-x] [-r] [-d] [-u sym] [-e sym] [-l[x]] file ...
# man v4-ld
```

### Options

- -s: Squash: write no symbol table at all (implies -x).
- -n: Make the text read-only/shareable: magic 0410, text rounded to 64 bytes, data starting on an 8K boundary.
- -x: Do not enter local symbols in the output symbol table.
- -r: Keep relocation bits so the output can be linked again (partial link). This also happens automatically if undefined symbols remain.
- -d: Define common regions (and `_end`, `_etext`, `_edata`) even when relocation bits are kept.
- -u sym: Enter `sym` as undefined so that an archive member defining it is loaded.
- -e sym: Use `sym` as the entry point.
- -l: Search the library `liba.a`; `-lx` searches `libx.a` (only the first letter after `-l` is used, so `-l2` is `lib2.a` for instance.)
- file ...: Object files (.o) and/or archives (.a). Archive members are only loaded if they define a symbol that is currently undefined, so order matters. Unknown `-` options are ignored, as in the original.

## D. AR

```bash
ar key afile [name ...]
# if installed: 
# v4-ar key afile [name ...]
# man v4-ar
```

### Keys

- r: Replace the named files in the archive, or add them at the end if they are not there. Creates the archive if it does not already exist.
- u: Update: replace a member only if the file of that name is newer than the member. With no names, every member for which a newer file exists in the current directory is replaced. Names that match no member are added.
- d: Delete the named members.
- x: Extract the named members (all of them if none are named) into the current directory.
- t: Print the table of contents (only the named members, if any).
- v: Verbose. Print one line per member that is acted on: `a` added, `r` replaced, `c` copied unchanged, `d` deleted, `x` extracted.

## E. STRIP

```bash
strip file ...
# if installed: 
# v4-strip file ...
# man v4-strip
```

## F. Userspace emulator

The userspace emulator translates Unix V4 syscalls to POSIX, and handles host integrations.

Like any other executable file format, do not trust unknown PDP-11 a.out executables (can be identified via the file command) or run them on your system. They can exec/fork other non PDP-11 binaries on your system, use file descriptors, and properly inherit root privileges if launched as root or via `sudo`.

### Setting up the emulator

To get started:

> [!NOTE]
>
> [https://danielm.pages.dev/useful/unix_v4_debug.tar.xz](https://danielm.pages.dev/useful/unix_v4_debug.tar.xz) contains unstripped symbol rebuilds for most* of the binaries. Symbols allow both hook based patches to work (fixing time properly), and for more readable stacktraces in most cases of failure. The URL below in the code block however contains the original image, modified only to remove root nesting to limit the amount of commands to extract.
>
> <sub>*All binaries with identical code segments when rebuilt were kept.</sub>

```bash
mkdir -p v4
curl -fL https://danielm.pages.dev/useful/unix_v4.tar.xz -o /tmp/unix_v4.tar.xz
tar -xJf /tmp/unix_v4.tar.xz -C ./v4
make -j$(nproc)
sudo make install
```

If all goes well:

```bash
$ # 
$ sudo make install
  INSTALL bin
No Unix v4 root specified. Would you like to download it?
  0 - No, exit
  1 - Stock
  2 - With symbols
Selection [0-2]: 1
'./.v4/stock' already exists. Redownload? [y/N]: n
Installing persistence...
Created symlink '/etc/systemd/system/multi-user.target.wants/v4linux.service' → '/etc/systemd/system/v4linux.service'.
Persistence installed via systemd.
setup complete.
  V4 lower root: /opt/v4/sys
  Emulator:      /opt/v4/bin/v4linux
  Guest root:    /var/run/v4 (/)
  Guest /tmp:    tmpfs at /var/run/v4/tmp
  Host view:     /var/run/v4/host (/host)
  Home view:     /var/run/v4/home (/home)
  Dispatcher:    /usr/local/libexec/v4linux-dispatch
  Magic 0407:    registered (07 01)
  Magic 0410:    registered (08 01)
```

From there, you can use Unix v4 programs inside Linux:

> [!NOTE]
>
>If you experience issues with original V4 binaries, file an issue, and a patch for emulator (or a symbol hook) will be looked into!

```bash
$ /opt/v4/sys/bin/sh
% ls /
bin
dev
etc
home # Host /home
host # Host /
lib
mnt
tmp
unix
usr
% echo hello world > file.txt
% cat file.txt | /host/usr/bin/grep hello
hello world
```

To remove it:

```bash
sudo v4-uninstall [--purge-v4] [--purge-bin] [--purge-all]
```

- --purge-v4      Also delete /opt/v4/sys
- --purge-bin     Also delete /opt/v4/bin
- --purge-all     Delete both /opt/v4/sys and /opt/v4/bin

This also installs the native toolchain:

```bash
$ ls -l /usr/bin/v4*
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-ar -> /opt/v4/bin/ar
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-as -> /opt/v4/bin/as
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-as2 -> /opt/v4/bin/as2
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-cc -> /opt/v4/bin/cc11
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-cc0 -> /opt/v4/bin/cc0
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-cc1 -> /opt/v4/bin/cc1
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-disasm -> /opt/v4/bin/v4dis
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-ld -> /opt/v4/bin/ld
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-strip -> /opt/v4/bin/strip
lrwxrwxrwx 1 root root -- MMM DD --:-- /usr/bin/v4-uninstall -> /opt/v4/bin/uninstall
```

## G. Disassembler

### A. v4dis

```bash
v4dis [-a] [-k] [-n] [-s] [-v] [-L labels] <a.out>
# if installed: 
# v4-disasm [-a] [-k] [-n] [-s] [-v] [-L labels] <a.out>
# man v4-disasm
```

#### Disassembler Options

- `-a`: Enable address and octal instruction word comments aligned to column 40 in disassembly.
- `-k`: Allows privileged/kernel instructions (e.g., `halt`, `spl`, `reset`, `rti`, `rtt`) that are invalid in user-mode binaries.
- `-n`: Disable automatic text-to-data padding. By default, pure text binaries (`0410` NMAGIC) emit `.=.+...` padding to preserve the 8KB boundary for the `.data` segment.
- `-s`: Scans function prologues (for both standard C register-saving routines like `csv` and also manual frame setups) to make inferred C function prototype comments (e.g., `/ int name(int a0, int a1);`).
- `-v`: Emits stats out to `stderr`, which includes label allocation budgets, code/data byte distributions, and speculative walk validation counts.
- `-L <labels>`: Maximum label budget (defaults to 700). Sets the limit on synthesized local labels (`L...`, `F...`, `D...`, `B...`) to avoid crashing the V4 assembler.
- `<a.out>`: Input PDP-11 Unix V4 binary (`0407` executable or `0410` pure executable, use 'file <a.out>' for info).

## H. Crash dumps (dumpinfo)

When a guest program dies from a fatal signal, the emulator writes `v4linux-<pid>.v4dump` in the current directory. It is a V4DUMP file: a 256 byte header (magic `89 'V4DUMP' 0A`, version, CRC-32, the signal, fault address, address of the faulting instruction, r0-r7/PS, and the layout: text, data, bss, break, stack limit, initial sp) followed by sections for the memory map and bytes, FP accumulators, signal dispositions, open files, symbols, arguments, paths and the backtrace at crash time. Only live memory is stored (typically a few KB). The format is defined in `tools/common/v4dump.h`.

```bash
dumpinfo [-a] [-r] [-f] [-l] [-p] [-b] [-y] [-s N] [-m ADDR[,LEN]] [-c] [-x aout|raw -o FILE] <dumpfile>
# if installed:
# v4-dumpinfo [-a] [-r] [-f] [-l] [-p] [-b] [-y] [-s N] [-m ADDR[,LEN]] [-c] [-x aout|raw -o FILE] <dumpfile>
# man v4-dumpinfo
# man 5 v4dump
```

### Dumpinfo Options

- (none): Summary: process, crash location, registers, memory layout.
- `-a`: Everything except symbols and memory.
- `-r`: Registers and condition codes.
- `-f`: FP-11 accumulators and status.
- `-l`: Memory layout, region map (text/data/bss/heap/unmapped/stack/args) and file sections.
- `-p`: Paths, arguments, signal dispositions, open files.
- `-b`: Backtrace recorded when the program died.
- `-y`: Symbol table.
- `-s N`: N words of stack from sp, with symbolized return addresses.
- `-m ADDR[,LEN]`: Hex dump of memory (C number syntax, a leading `0` means octal).
- `-c`: Only verify integrity; exit status 0 if intact.
- `-x FORMAT -o FILE`: Extract the image. `aout` writes a 0407/0410 executable with the original symbols, for `v4-disasm` (text and data are as they were at the time of the crash); `raw` writes the flat 64K memory image.

```bash
v4-dumpinfo v4linux-1234.v4dump
v4-dumpinfo -x aout -o crashed.out v4linux-1234.v4dump && v4-disasm -a crashed.out
```

---

This product includes software developed or owned by Caldera International, Inc.
