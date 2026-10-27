#!/bin/sh
mode=${1:-check}
here=$(cd "$(dirname "$0")" && pwd)
top=$here/../..
BIN=${BIN:-$top/bin}
V4ROOT=${V4ROOT:-$top/v4}
t=$(mktemp -d) || exit 2
trap 'rm -rf "$t"' EXIT INT TERM
src=$t/src; mkdir "$src"

AS2=$BIN/as2; export AS2

prep() {
	for s in "$here"/src/*.s; do
		n=$(basename "$s" .s)
		rm -rf "$t/as"; mkdir "$t/as"
		cp "$s" "$t/as/$n.s"
		( cd "$t/as" && TMPDIR=$t/as "$BIN/as" "$n.s" >/dev/null 2>&1 )
		[ -f "$t/as/a.out" ] || { echo "assemble failed: $n.s" >&2; exit 2; }
		mv "$t/as/a.out" "$src/$n.o"
	done
	cp "$here/lib.a" "$src/lib.a"
	echo hello > "$src/junk.o"
	head -c 100 "$src/f.o" > "$src/trunc.o"
	head -c 20 "$src/f.o" > "$src/trunc2.o"
	head -c 10 "$src/f.o" > "$src/trunc3.o"
	head -c 200 "$src/lib.a" > "$src/tlib.a"
}

prep

if [ "$mode" = gen ]; then
	[ -f "$BIN/v4linux" ] || { 
		echo "need $BIN/v4linux (make emulator)" >&2; 
		exit 2; 
	}
	[ -f "$V4ROOT/bin/ld" ] || { 
		echo "need the V4 image in $V4ROOT" >&2; 
		exit 2; 
	}
	cp -r "$V4ROOT" "$t/root"; mkdir -p "$t/root/tmp/ldt"
	ld_run() {
		( cd /tmp && "$BIN/v4linux" -r "$t/root" --chdir /tmp/ldt /bin/ld "$@" -Zpad ) 2>&1
	}
	rm -rf "$t/root/tmp/ldt"/*; cp "$src"/* "$t/root/tmp/ldt/"
	ld_run m.o f.o >/dev/null; cp "$t/root/tmp/ldt/a.out" "$src/linked.o"
else
	( cd "$src" && "$BIN/ld" m.o f.o ) >/dev/null 2>&1
	cp "$src/a.out" "$t/linked.o"; rm -f "$src/a.out"; mv "$t/linked.o" "$src/linked.o"
fi

fail=0
while IFS='|' read -r name args; do
	name=$(echo "$name" | tr -d ' \t')
	case $name in ''|'#'*) continue;; esac
	if [ "$mode" = gen ]; then
		rm -rf "$t/root/tmp/ldt"/*; cp "$src"/* "$t/root/tmp/ldt/"
		ld_run $args > "$here/expected/$name.err"
		if [ -f "$t/root/tmp/ldt/a.out" ]; then
			cp "$t/root/tmp/ldt/a.out" "$here/expected/$name.out"
		else
			rm -f "$here/expected/$name.out"
		fi
		echo "gen $name"
	else
		w=$t/work; rm -rf "$w"; mkdir "$w"; cp "$src"/* "$w"/
		( cd "$w" && "$BIN/ld" $args >out.txt 2>&1 )
		ok=1
		if [ -f "$here/expected/$name.out" ]; then
			cmp -s "$w/a.out" "$here/expected/$name.out" || ok=0
		else
			[ ! -f "$w/a.out" ] || ok=0
		fi
		cmp -s "$w/out.txt" "$here/expected/$name.err" || ok=0
		[ -f "$w/l.out" ] && ok=0
		if [ $ok = 1 ]; then echo "ok $name"; else echo "FAIL $name"; fail=1; fi
	fi
done < "$here/cases.txt"
exit $fail
