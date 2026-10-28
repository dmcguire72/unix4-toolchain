#!/usr/bin/env bash
set -Eeuo pipefail

V4_DEST=/opt/v4/sys
PDP11_DEST=/opt/v4/bin
MAN_DEST=/opt/v4/man
MAN_LINK_ROOT=/usr/local/share/man
MAN_INSTALLATION="$MAN_DEST/installation"
MAGIC_DEST=/etc/magic
MAGIC_BEGIN='# BEGIN v4linux managed v4dump.magic'
MAGIC_END='# END v4linux managed v4dump.magic'
DISPATCH_DEST=/usr/local/libexec/v4linux-dispatch
GUEST_ROOT=/var/run/v4
HOST_MOUNT="$GUEST_ROOT/host"
HOME_MOUNT="$GUEST_ROOT/home"
TMP_MOUNT="$GUEST_ROOT/tmp"
STATE_DIR=/var/run/v4-overlay
HOST_STAGING=/var/run/v4-host-staging
BINFMT_DIR=/proc/sys/fs/binfmt_misc
BINFMT_MARKER=/run/v4-binfmt-mounted-by-v4linux
RUNTIME_SCRIPT="$PDP11_DEST/v4linux-runtime"
SERVICE_FILE=/etc/systemd/system/v4linux.service
VERBOSE=false
PURGE_V4=0
PURGE_BIN=0
CLEANUP_FILE=""

cleanup() {
    if [[ -n "$CLEANUP_FILE" ]]; then
        rm -f -- "$CLEANUP_FILE" 2>/dev/null || true
    fi
}
trap cleanup EXIT

usage() {
    cat <<'USAGE'
Usage: sudo ./v4linux-uninstall.sh [-v|--verbose] [--purge-v4] [--purge-bin] [--purge-all]
Options:
  -v, --verbose   Trace executed commands with file and line numbers.
  --purge-v4      Also delete /opt/v4/sys
  --purge-bin     Also delete /opt/v4/bin
  --purge-all     Delete both /opt/v4/sys and /opt/v4/bin
  -h, --help      Show this help message
USAGE
}

die() {
    echo "v4linux-uninstall: $*" >&2
    exit 1
}

valid_man_link_relative() {
    local rel=$1
    local section filename
    [[ "$rel" == */* ]] || return 1
    section=${rel%%/*}
    filename=${rel#*/}
    [[ "$section" == man* ]] || return 1
    [[ -n "$filename" && "$filename" != */* ]] || return 1
    [[ "$filename" != "." && "$filename" != ".." ]] || return 1
}

remove_installed_man_pages() {
    local rel section link_dir link_file expected_target actual_target resolved_path
    if [[ ! -e "$MAN_DEST" && ! -L "$MAN_DEST" ]]; then
        return 0
    fi
    [[ ! -L "$MAN_DEST" ]] || die "refusing to remove symlinked man destination: $MAN_DEST"
    [[ -d "$MAN_DEST" ]] || die "man destination is not a directory: $MAN_DEST"
    [[ ! -L "$MAN_LINK_ROOT" ]] || die "refusing to traverse symlinked system man directory: $MAN_LINK_ROOT"
    if [[ -d "$MAN_LINK_ROOT" ]]; then
        resolved_path=$(realpath -e -- "$MAN_LINK_ROOT") || die "cannot resolve $MAN_LINK_ROOT"
        [[ "$resolved_path" == "$MAN_LINK_ROOT" ]] || die "refusing non-canonical man directory: $MAN_LINK_ROOT"
    fi
    [[ ! -L "$MAN_INSTALLATION" ]] || die "refusing symlinked installation manifest: $MAN_INSTALLATION"
    if [[ -e "$MAN_INSTALLATION" ]]; then
        [[ -f "$MAN_INSTALLATION" ]] || die "man installation manifest is not a regular file: $MAN_INSTALLATION"
        rel=""
        while IFS= read -r -d '' rel || [[ -n "$rel" ]]; do
            valid_man_link_relative "$rel" || die "invalid path in $MAN_INSTALLATION"
            section=${rel%%/*}
            link_dir="$MAN_LINK_ROOT/$section"
            [[ ! -L "$link_dir" ]] || die "refusing to traverse symlinked man section directory: $link_dir"
            [[ -d "$link_dir" ]] || continue
            resolved_path=$(realpath -e -- "$link_dir") || die "cannot resolve $link_dir"
            [[ "$resolved_path" == "$link_dir" ]] || die "refusing non-canonical man section directory: $link_dir"
            link_file="$MAN_LINK_ROOT/$rel"
            [[ -L "$link_file" ]] || continue
            actual_target=$(readlink -- "$link_file") || die "cannot read symlink: $link_file"
            expected_target="$MAN_DEST/$rel"
            if [[ "$actual_target" == "$expected_target" ]]; then
                rm -f -- "$link_file"
            else
                echo "Leaving man link with changed target: $link_file" >&2
            fi
        done < "$MAN_INSTALLATION"
    else
        echo "No man installation manifest found; no links under $MAN_LINK_ROOT were removed." >&2
    fi
    rm -rf -- "$MAN_DEST"
}

remove_installed_magic() {
    local tmp owner mode

    if [[ ! -e "$MAGIC_DEST" && ! -L "$MAGIC_DEST" ]]; then
        return 0
    fi
    [[ ! -L "$MAGIC_DEST" ]] || die "refusing to modify symlinked magic database: $MAGIC_DEST"
    [[ -f "$MAGIC_DEST" ]] || die "magic database is not a regular file: $MAGIC_DEST"

    if ! grep -Fqx -- "$MAGIC_BEGIN" "$MAGIC_DEST" &&
       ! grep -Fqx -- "$MAGIC_END" "$MAGIC_DEST"; then
        return 0
    fi

    owner=$(stat -c '%u:%g' -- "$MAGIC_DEST") || die "cannot stat $MAGIC_DEST"
    mode=$(stat -c '%a' -- "$MAGIC_DEST") || die "cannot stat $MAGIC_DEST"
    tmp=$(mktemp /etc/.magic.v4linux-uninstall.XXXXXX) || die "cannot create temporary magic database"
    CLEANUP_FILE="$tmp"

    awk -v begin="$MAGIC_BEGIN" -v end="$MAGIC_END" '
        $0 == begin {
            if (inside || found_begin) { bad = 1; next }
            inside = 1
            found_begin = 1
            next
        }
        $0 == end {
            if (!inside || found_end) { bad = 1; next }
            inside = 0
            found_end = 1
            next
        }
        !inside { print }
        END {
            if (inside || (found_begin != found_end) || bad || !found_begin) exit 2
        }
    ' "$MAGIC_DEST" > "$tmp" || die "malformed v4linux magic block in $MAGIC_DEST; refusing to modify it"

    chown -- "$owner" "$tmp"
    chmod "$mode" "$tmp"
    mv -f -- "$tmp" "$MAGIC_DEST"
    CLEANUP_FILE=""
}

while (($#)); do
    case "$1" in
        -v|--verbose)
            VERBOSE=true
            ;;
        --purge-v4)
            PURGE_V4=1
            ;;
        --purge-bin)
            PURGE_BIN=1
            ;;
        --purge-all)
            PURGE_V4=1
            PURGE_BIN=1
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            usage >&2
            die "unknown option: $1"
            ;;
    esac
    shift
done

if [[ "$VERBOSE" == true ]]; then
    export PS4='+ [${BASH_SOURCE##*/}:${LINENO}] '
    set -x
fi

[[ $EUID -eq 0 ]] || die "run this script as root (for example, with sudo)."

remove_installed_magic
remove_installed_man_pages

if systemctl list-unit-files v4linux.service &>/dev/null || [[ -f "$SERVICE_FILE" ]]; then
    systemctl disable --now v4linux.service 2>/dev/null || true
    rm -f -- "$SERVICE_FILE"
    systemctl daemon-reload
fi

rm -f -- "$RUNTIME_SCRIPT"

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
if mountpoint -q "$TMP_MOUNT"; then
    umount -R -- "$TMP_MOUNT"
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

for tool in as as2 cc0 cc1 cc11 ar ld strip v4dis dumpinfo; do
    case "$tool" in
        cc11)     link_name="v4-cc" ;;
        v4dis)    link_name="v4-disasm" ;;
        dumpinfo) link_name="v4-dumpinfo" ;;
        *)        link_name="v4-$tool" ;;
    esac
    rm -f -- "/usr/bin/$link_name"
done

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

if (( PURGE_BIN )); then
    rm -rf -- "$PDP11_DEST"
    echo "Removed $PDP11_DEST"
fi
if (( PURGE_V4 )); then
    rm -rf -- "$V4_DEST"
    echo "Removed $V4_DEST"
fi

rm -f -- \
    "$PDP11_DEST/uninstall-script" \
    "$PDP11_DEST/uninstall" \
    /usr/bin/v4-uninstall
rmdir /opt/v4 2>/dev/null || true

echo "v4linux uninstalled."
