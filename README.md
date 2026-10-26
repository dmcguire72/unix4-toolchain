# Modern Unix Fourth Edition Toolchain (PDP-11)

This repository contains a few components:

- A. CC for Linux (targetting Unix v4 for the PDP-11)
- B. AS for Linux (targetting Unix v4 for the PDP-11)
- C. LD for Linux (targetting Unix v4 for the PDP-11) [soon]
- D. A Unix v4 userspace emulation component using binfmt_misc, syscall translation, and a minimal PDP-11 CPU/FPU emulator.
- E. A Disassembler for binaries compiled with this toolchain, or the stock Unix 4 toolchain.

## A. CC

The CC Compiler is built of many pieces, as the original had memory constraints:

- A. cc / cc11
- B. c0 / cc0
- C. c1 / cc1
- D. c2 / cc2

### A. cc11

```bash
cc [-c] [-S] [-O] [-P] [-p] [-2 | -20] [-t] [options/files...]
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

A port for LD is currently being planned

## D. Userspace emulator

The userspace emulator translates Unix V4 syscalls to POSIX, and handles host integrations.

Like any other executable format, do not trust unknown PDP-11 a.out executables or run them on your system. They can exec/fork other non PDP-11 binaries on your system, use file descriptors, and properly inherit root privileges if launched as root or via `sudo`.

### Setting up the emulator

To get started:

> [!NOTE]
>
> [https://danielm.pages.dev/useful/unix_v4_debug.tar.xz](https://danielm.pages.dev/useful/unix_v4_debug.tar.xz) contains unstripped symbol rebuilds for most* of the binaries. Symbols allow both hook based patches to work (fixing time properly), and for more readable stacktraces in most cases of failure. The URL below contains the original image, modified only to remove root nesting to limit the amount of commands to extract.
>
> <sub>*All binaries with identical code segments when rebuilt were kept.</sub>

```bash
mkdir -p v4
curl -fL https://danielm.pages.dev/useful/unix_v4.tar.xz -o /tmp/unix_v4.tar.xz
tar -xJf /tmp/unix_v4.tar.xz -C ./v4
make emulator
sudo ./tools/emulator/linux/pdp11-setup.sh ./v4 ./bin
```

If all goes well:

```bash
$ sudo ./tools/emulator/linux/pdp11-setup.sh ./v4 ./bin
Installing persistence...
Created symlink '/etc/systemd/system/multi-user.target.wants/pdp11.service' → '/etc/systemd/system/pdp11.service'.
Persistence installed via systemd.
setup complete.
  V4 lower root: /opt/v4/sys
  Emulator:      /opt/v4/bin/v4linux
  Guest root:    /var/run/pdp11-v4 (/)
  Host view:     /var/run/pdp11-v4/host (/host)
  Home view:     /var/run/pdp11-v4/home (/home)
  Dispatcher:    /usr/local/libexec/unix4-dispatch
  Magic 0407:    registered (07 01)
  Magic 0410:    registered (08 01)
```

From there, you can use Unix v4 programs inside Linux:

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

---

This product includes software developed or owned by Caldera International, Inc.
