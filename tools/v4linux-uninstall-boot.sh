#!/usr/bin/env bash
set -Eeuo pipefail

UNINSTALL_SOURCE="/opt/v4/bin/uninstall-script"

if [[ ! -f "$UNINSTALL_SOURCE" ]]; then
    echo "v4-uninstall: uninstaller not found: $UNINSTALL_SOURCE" >&2
    exit 1
fi

TMP_UNINSTALL="$(mktemp /tmp/v4linux-uninstall.XXXXXX)"

cleanup() {
    rm -f -- "$TMP_UNINSTALL"
}

trap cleanup EXIT

install -m 0755 -- "$UNINSTALL_SOURCE" "$TMP_UNINSTALL"

"$TMP_UNINSTALL" "$@"