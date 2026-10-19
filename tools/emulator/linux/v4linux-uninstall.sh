#!/usr/bin/env bash
set -Eeuo pipefail

V4_DEST=/opt/v4/sys
PDP11_DEST=/opt/v4/bin
DISPATCH_DEST=/usr/local/libexec/v4linux-dispatch
GUEST_ROOT=/var/run/v4
HOST_MOUNT="$GUEST_ROOT/host"
HOME_MOUNT="$GUEST_ROOT/home"
STATE_DIR=/var/run/v4-overlay
HOST_STAGING=/var/run/v4-host-staging
BINFMT_DIR=/proc/sys/fs/binfmt_misc
BINFMT_MARKER=/run/v4-binfmt-mounted-by-v4linux
RUNTIME_SCRIPT="$PDP11_DEST/v4linux-runtime"
SERVICE_FILE=/etc/systemd/system/v4linux.service

PURGE_V4=0
PURGE_BIN=0

usage() {
    cat <<'USAGE'
Usage: sudo ./v4linux-uninstall.sh [--purge-v4]

Unregister v4linux handlers, unmount the V4 host/overlay mounts, and
remove the installed dispatcher.

  --purge-v4   Also delete /opt/v4
USAGE
}

die() { echo "v4linux-uninstall: $*" >&2; exit 1; }
[[ $EUID -eq 0 ]] || die "run this script as root (for example, with sudo)."

while (($#)); do
    case "$1" in
        --purge-v4) PURGE_V4=1 ;;
        --purge-bin) PURGE_BIN=1 ;;
        --purge-all) PURGE_V4=1; PURGE_BIN=1 ;;
        -h|--help) usage; exit 0 ;;
        *) usage >&2; die "unknown option: $1" ;;
    esac
    shift
done

if systemctl list-unit-files v4linux.service &>/dev/null || [[ -f "$SERVICE_FILE" ]]; then
    systemctl disable --now v4linux.service 2>/dev/null || true
    rm -f "$SERVICE_FILE"
    systemctl daemon-reload
fi

rm -f "$RUNTIME_SCRIPT"

if mountpoint -q "$BINFMT_DIR"; then
    for name in v4linux-aout-0407 v4linux-aout-0410; do
        if [[ -e "$BINFMT_DIR/$name" ]]; then
            printf '%s\n' -1 > "$BINFMT_DIR/$name"
        fi
    done
fi

if mountpoint -q "$HOST_MOUNT"; then
    umount -R -- "$HOST_MOUNT"
fi

if mountpoint -q "$HOME_MOUNT"; then
    umount -R -- "$HOME_MOUNT"
fi

if mountpoint -q "$GUEST_ROOT"; then
    umount -- "$GUEST_ROOT"
fi

if mountpoint -q "$HOST_STAGING"; then
    umount -R -- "$HOST_STAGING"
fi

rm -rf -- "$STATE_DIR"
rmdir "$HOST_STAGING" 2>/dev/null || true
rmdir "$GUEST_ROOT" 2>/dev/null || true
rm -f -- "$DISPATCH_DEST"

if [[ -e "$BINFMT_MARKER" ]] && mountpoint -q "$BINFMT_DIR"; then
    other_handler=0
    for entry in "$BINFMT_DIR"/*; do
        [[ -e "$entry" ]] || continue
        name=${entry##*/}
        case "$name" in
            register|status|features|new_namespace|v4linux-aout-0407|v4linux-aout-0410) ;;
            *) other_handler=1; break ;;
        esac
    done
    if (( other_handler == 0 )); then
        umount "$BINFMT_DIR"
        rm -f -- "$BINFMT_MARKER"
    else
        echo "Leaving $BINFMT_DIR mounted because other binary format handlers are registered."
    fi
else
    rm -f -- "$BINFMT_MARKER"
fi

if (( PURGE_V4 )); then
    rm -rf -- "$V4_DEST"
    echo "Removed $V4_DEST"
fi

echo "v4linux binfmt integration uninstalled."
