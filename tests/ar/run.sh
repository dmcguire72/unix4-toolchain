#!/bin/sh
mode=${1:-check}
here=$(cd "$(dirname "$0")" && pwd)
top=$here/../..
BIN=${BIN:-$top/bin}
V4ROOT=${V4ROOT:-$top/v4}
T0=1700000000
t=$(mktemp -d) || exit 2
trap 'rm -rf "$t"' EXIT INT TERM
umask 0

${CC:-cc} -o "$t/ardump" "$here/ardump.c" || exit 2

mk() { yes "$1" | head -c "$2" > "$3/$1"; }
fixtures() {
	d=$1; mkdir -p "$d/sub"
	mk a.o 1 "$d"; mk bb.o 17 "$d"; mk ccc.o 511 "$d"; mk dddddddd.o 512 "$d"
	mk eeeeeeeeee.o 513 "$d"; mk f.o 1023 "$d"; mk g.o 2000 "$d"
	mk odd.o 1 "$d"; mk even.o 2 "$d"; : > "$d/empty.o"
	mk big.o 30001 "$d"; mk h.o 32766 "$d"
	printf 'int x;\n' > "$d/x.c"; printf 'subfile!!' > "$d/sub/deep.txt"
	cp "$here/../ld/lib.a" "$d/lib.a"
	for s in h1 h2 h3 h4; do mk $s.o 40 "$d"; done
	find "$d" -type f -exec chmod 644 {} +
	find "$d" -type f -exec touch -d @$T0 {} +
}

state() {
	( cd "$1" && for f in $(ls -1 | grep -v '^sub$'; ls -1 sub 2>/dev/null | sed 's|^|sub/|'); do
		case $f in
		*.a) echo "$f: $("$t/ardump" "$f" | tr '\n' ';')" ;;
		*) echo "$f: $(cksum < "$f")" ;;
		esac
	done )
}

if [ "$mode" = gen ]; then
	[ -f "$BIN/v4linux" ] || { echo "need $BIN/v4linux (make emulator)" >&2; exit 2; }
	[ -f "$V4ROOT/bin/ar" ] || { echo "need the V4 image in $V4ROOT" >&2; exit 2; }
	export V4LINUX_TIME_SHIFT_YEARS=0 V4LINUX_TZ=0
fi

fail=0
for sc in "$here"/scenarios/*.txt; do
	name=$(basename "$sc" .txt)
	if [ "$mode" = gen ]; then
		rm -rf "$t/root"; cp -r "$V4ROOT" "$t/root"
		w=$t/root/tmp/w; mkdir -p "$w"; tmpd=$t/root/tmp
	else
		w=$t/w; tmpd=$t/tmp; rm -rf "$w" "$tmpd"; mkdir -p "$w" "$tmpd"
	fi
	fixtures "$w"
	out=$t/$name.txt; : > "$out"
	while read -r cmd args; do
		[ -z "$cmd" ] && continue
		case $cmd in
		ar)
			echo "\$ ar $args" >> "$out"
			if [ "$mode" = gen ]; then
				( cd /tmp && "$BIN/v4linux" -r "$t/root" --chdir /tmp/w /bin/ar $args ) >> "$out" 2>&1 </dev/null
			else
				( cd "$w" && TMPDIR=$tmpd "$BIN/ar" $args ) >> "$out" 2>&1 </dev/null
			fi
			;;
		touch) set -- $args; echo "\$ touch $1 $2" >> "$out"; touch -d @"$1" "$w/$2";;
		rm) echo "\$ rm $args" >> "$out"; rm -f "$w/$args";;
		esac
		echo "temp files left: $(ls "$tmpd" | grep -c '^vtm')" >> "$out"
		state "$w" >> "$out"
		find "$w" -type f ! -name '*.a' -exec touch -d @$T0 {} +
	done < "$sc"
	if [ "$mode" = gen ]; then
		cp "$out" "$here/expected/$name.txt"; echo "gen $name"
	elif cmp -s "$out" "$here/expected/$name.txt"; then
		echo "ok $name"
	else
		echo "FAIL $name"; diff "$here/expected/$name.txt" "$out" | head -10 | cat -v
		fail=1
	fi
done
exit $fail
