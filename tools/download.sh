#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
    printf 'Usage: %s URL OUTFILE\n' "$0" >&2
    exit 2
fi

url=$1
outfile=$2

if command -v wget >/dev/null 2>&1; then
    exec wget -O "$outfile" "$url"
elif command -v curl >/dev/null 2>&1; then
    exec curl --fail --location --output "$outfile" "$url"
elif command -v python3 >/dev/null 2>&1; then
    py=python3
elif command -v python >/dev/null 2>&1; then
    py=python
else
    exit 1
fi

"$py" - "$url" "$outfile" <<'PY'
import shutil
import sys
from urllib.request import urlopen

url, outfile = sys.argv[1:3]

with urlopen(url) as response, open(outfile, "wb") as output:
    shutil.copyfileobj(response, output)
PY