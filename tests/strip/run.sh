#!/bin/sh

mode=${1:-check}
here=$(cd "$(dirname "$0")" && pwd)
top=$here/../..
BIN=${BIN:-$top/bin}
V4ROOT=${V4ROOT:-$top/v4}
t=$(mktemp -d) || exit 2
trap 'rm -rf "$t"' EXIT INT TERM
umask 022

AS2=$BIN/as2; export AS2

fixtures() {
	d=$1; a=$t/as; rm -rf "$a"; mkdir "$a"
	for x in m f g; do
		cp "$here/../ld/src/$x.s" "$a/"
		( cd "$a" && TMPDIR=$a "$BIN/as" $x.s >/dev/null 2>&1 )
		[ -f "$a/a.out" ] || { echo "assemble failed: $x.s" >&2; exit 2; }
		mv "$a/a.out" "$d/$x.o"
	done
	for v in "prog:" "progn:-n" "progr:-r" "progs:-s" "progx:-x"; do
		( cd "$d" && "$BIN/ld" ${v#*:} m.o f.o >/dev/null 2>&1 ) || true
		mv "$d/a.out" "$d/${v%%:*}"
	done
	printf 'not an object file at all, just text.\n' > "$d/text.txt"
	: > "$d/empty"
	printf '\007\001' > "$d/two"
	head -c 16 "$d/prog" > "$d/hdronly"
	chmod 755 "$d"/prog*; chmod 644 "$d"/*.o "$d"/text.txt "$d"/empty "$d"/two "$d"/hdronly
}

state() {
	( cd "$1" && for f in *; do
		echo "$f $(stat -c %a "$f") $(cksum < "$f")"
	done )
}

if [ "$mode" = gen ]; then
	[ -f "$BIN/v4linux" ] || { echo "need $BIN/v4linux (make emulator)" >&2; exit 2; }
	[ -f "$V4ROOT/bin/strip" ] || { echo "need the V4 image in $V4ROOT" >&2; exit 2; }
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
		echo "\$ $cmd $args" >> "$out"
		case $cmd in
		strip)
			if [ "$mode" = gen ]; then
				( cd /tmp && "$BIN/v4linux" -r "$t/root" --chdir /tmp/w /bin/strip $args ) >> "$out" 2>&1 </dev/null
			else
				( cd "$w" && TMPDIR=$tmpd "$BIN/strip" $args ) >> "$out" 2>&1 </dev/null
			fi
			;;
		trunc) set -- $args; truncate -s "$1" "$w/$2";;
		cp) set -- $args; cp -p "$w/$1" "$w/$2";;
		rm) rm -f "$w/$args";;
		esac
		echo "temp files left: $(ls "$tmpd" | grep -c '^stm')" >> "$out"
		state "$w" >> "$out"
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
