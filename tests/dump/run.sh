#!/bin/sh

here=$(cd "$(dirname "$0")" && pwd)
top=$here/../..
BIN=${BIN:-$top/bin}
t=$(mktemp -d) || exit 2
trap 'rm -rf "$t"' EXIT INT TERM
AS2=$BIN/as2; export AS2
fail=0

check() {
	if grep -q -- "$2" "$3"; then echo "ok $1"; else echo "FAIL $1"; fail=1; fi
}

cp "$here/crash.s" "$t/crash.s"
cd "$t" || exit 2
TMPDIR=$t "$BIN/as" crash.s || exit 2
"$BIN/v4linux" ./a.out >/dev/null 2>run.err
check emulator-wrote-dump 'crash dump written' run.err
dump=$(ls v4linux-*.v4dump 2>/dev/null | head -n 1)
[ -n "$dump" ] || { echo "FAIL no dump file"; exit 1; }

"$BIN/dumpinfo" -c "$dump" >/dev/null && echo "ok crc" || { echo "FAIL crc"; fail=1; }
"$BIN/dumpinfo" -a "$dump" >info.txt 2>&1
check signal          'SIGBUS' info.txt
check fault-address   'fault addr  077777  odd address' info.txt
check faulting-inst   '\[012737 000042 077777\]' info.txt
check registers       'r0 123456' info.txt
check break           'break       000300' info.txt
check backtrace       '#0  g(01234)' info.txt
"$BIN/dumpinfo" -y "$dump" | grep -q ' T _g$' && echo "ok symbols" || { echo "FAIL symbols"; fail=1; }

cp "$dump" bad.v4dump
printf '\001' | dd of=bad.v4dump bs=1 seek=300 conv=notrunc 2>/dev/null
"$BIN/dumpinfo" -c bad.v4dump >/dev/null 2>&1 && { echo "FAIL damage"; fail=1; } || echo "ok damage-detected"

"$BIN/dumpinfo" -x aout -o ex.out "$dump" 2>/dev/null
cmp -s -n 72 -i 16:16 a.out ex.out && echo "ok extract-text" || { echo "FAIL extract-text"; fail=1; }
"$BIN/v4dis" ex.out 2>/dev/null | grep -q '^_g:' && echo "ok disassemble" || { echo "FAIL disassemble"; fail=1; }
"$BIN/dumpinfo" -x raw -o ex.raw "$dump" 2>/dev/null
[ "$(wc -c <ex.raw)" = 65536 ] && echo "ok extract-raw" || { echo "FAIL extract-raw"; fail=1; }
exit $fail
