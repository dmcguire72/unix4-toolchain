#!/usr/bin/env bash
set -Eeuo pipefail

VERBOSE=false
ARGS=()

while (($#)); do
    case "$1" in
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --)
            shift
            ARGS+=("$@")
            break
            ;;
        *)
            ARGS+=("$1")
            shift
            ;;
    esac
done

set -- "${ARGS[@]}"

if [[ "$VERBOSE" == true ]]; then
    export PS4='+ [${BASH_SOURCE##*/}:${LINENO}] '
    set -x
fi

PARENT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

MAN_SOURCE=""
MAN_DEST=/opt/v4/man
MAN_LINK_ROOT=/usr/local/share/man
MAN_INSTALLATION="$MAN_DEST/installation"

V4_DEST=/opt/v4/sys
PDP11_DEST=/opt/v4/bin
DISPATCH_DEST=/usr/local/libexec/v4linux-dispatch
GUEST_ROOT=/var/run/v4
HOST_MOUNT="$GUEST_ROOT/host"
HOME_MOUNT="$GUEST_ROOT/home"
TMP_MOUNT="$GUEST_ROOT/tmp"
STATE_DIR=/var/run/v4-overlay
UPPER_DIR="$STATE_DIR/upper"
WORK_DIR="$STATE_DIR/work"
HOST_STAGING=/var/run/v4-host-staging
BINFMT_DIR=/proc/sys/fs/binfmt_misc
BINFMT_MARKER=/run/v4-binfmt-mounted-by-v4linux
RUNTIME_SCRIPT="$PDP11_DEST/v4linux-runtime"

REQUIRED_TOOLS=(as as2 cc0 cc1 cc11 ar ld strip v4dis)

STOCK_URL="https://danielm.pages.dev/useful/unix_v4.tar.xz"
SYMBOLS_URL="https://danielm.pages.dev/useful/unix_v4_debug.tar.xz"

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
DOWNLOAD_SH="$SCRIPT_DIR/download.sh"
[[ -f "$DOWNLOAD_SH" ]] || DOWNLOAD_SH="./download.sh"

CLEANUP_DIRS=()
cleanup() {
    for item in "${CLEANUP_DIRS[@]}"; do
        rm -rf "$item" 2>/dev/null || true
    done
}
trap cleanup EXIT

usage() {
    cat <<USAGE
Usage: sudo ./v4linux-setup.sh [-v|--verbose] /path/to/v4linux-bin-dir [/path/or/url/to/extracted-v4-root/]

Options:
  -v, --verbose   Log executed commands with file and line numbers.

You can get Unix v4 from ${STOCK_URL}.
If symbols are needed: ${SYMBOLS_URL}.

The first argument must be the bin folder from 'make emulator'.
If the second argument is omitted, you will be prompted to download Unix v4.
USAGE
}

die() {
    echo "v4linux-setup: $*" >&2
    exit 1
}

is_url() {
    case "$1" in
        http://*|https://*|ftp://*) return 0 ;;
        *) return 1 ;;
    esac
}

resolve_root_dir() {
    local dir=$1
    if [[ ! -d "$dir/bin" ]]; then
        for sub in "$dir"/*; do
            if [[ -d "$sub/bin" ]]; then
                echo "$sub"
                return 0
            fi
        done
    fi
    echo "$dir"
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

install_man_pages() {
    local section_dir page section base rel dest_dir dest_file link_dir link_file
    local old_rel old_section old_link_dir old_link_file expected_target actual_target
    local resolved_root resolved_dir
    local -a sources=() relatives=()
    local -A seen=()

    for section_dir in "$MAN_SOURCE"/man*; do
        [[ -d "$section_dir" ]] || continue
        [[ ! -L "$section_dir" ]] || die "refusing symlinked man source directory: $section_dir"
        section=${section_dir##*/}

        for page in "$section_dir"/*; do
            [[ -f "$page" ]] || continue
            base=${page##*/}
            if [[ "$base" == *.gz ]]; then
                rel="$section/$base"
            else
                rel="$section/$base.gz"
            fi
            valid_man_link_relative "$rel" || die "invalid man page path: $rel"
            [[ -z "${seen[$rel]+present}" ]] || die "multiple source man pages map to $rel"
            seen["$rel"]=1
            sources+=("$page")
            relatives+=("$rel")
        done
    done

    ((${#sources[@]} > 0)) || die "no man pages found under $MAN_SOURCE/man*/*"

    [[ ! -L "$MAN_DEST" ]] || die "refusing symlinked man destination: $MAN_DEST"
    [[ ! -L "$MAN_LINK_ROOT" ]] || die "refusing symlinked system man directory: $MAN_LINK_ROOT"
    install -d -m 0755 "$MAN_DEST" "$MAN_LINK_ROOT"

    resolved_root=$(realpath -e -- "$MAN_LINK_ROOT") || die "cannot resolve $MAN_LINK_ROOT"
    [[ "$resolved_root" == "$MAN_LINK_ROOT" ]] || die "refusing non-canonical man directory: $MAN_LINK_ROOT"
    resolved_root=$(realpath -e -- "$MAN_DEST") || die "cannot resolve $MAN_DEST"
    [[ "$resolved_root" == "$MAN_DEST" ]] || die "refusing non-canonical man destination: $MAN_DEST"

    [[ ! -L "$MAN_INSTALLATION" ]] || die "refusing symlinked installation manifest: $MAN_INSTALLATION"
    if [[ -e "$MAN_INSTALLATION" && ! -f "$MAN_INSTALLATION" ]]; then
        die "man installation manifest is not a regular file: $MAN_INSTALLATION"
    fi

    if [[ -f "$MAN_INSTALLATION" ]]; then
        old_rel=""
        while IFS= read -r -d '' old_rel || [[ -n "$old_rel" ]]; do
            valid_man_link_relative "$old_rel" || die "invalid path in $MAN_INSTALLATION"
            old_section=${old_rel%%/*}
            old_link_dir="$MAN_LINK_ROOT/$old_section"
            [[ ! -L "$old_link_dir" ]] || die "refusing symlinked man section directory: $old_link_dir"
            old_link_file="$MAN_LINK_ROOT/$old_rel"
            if [[ -L "$old_link_file" ]]; then
                actual_target=$(readlink -- "$old_link_file") || die "cannot read symlink: $old_link_file"
                expected_target="$MAN_DEST/$old_rel"
                if [[ "$actual_target" == "$expected_target" ]]; then
                    rm -f -- "$old_link_file"
                fi
            fi
        done < "$MAN_INSTALLATION"
    fi

    : > "$MAN_INSTALLATION"
    chmod 0644 "$MAN_INSTALLATION"

    for index in "${!sources[@]}"; do
        page=${sources[$index]}
        rel=${relatives[$index]}
        section=${rel%%/*}
        dest_dir="$MAN_DEST/$section"
        dest_file="$MAN_DEST/$rel"
        link_dir="$MAN_LINK_ROOT/$section"
        link_file="$MAN_LINK_ROOT/$rel"

        [[ ! -L "$dest_dir" ]] || die "refusing symlinked man destination section: $dest_dir"
        [[ ! -L "$link_dir" ]] || die "refusing symlinked man section directory: $link_dir"
        install -d -m 0755 "$dest_dir" "$link_dir"
        resolved_dir=$(realpath -e -- "$link_dir") || die "cannot resolve $link_dir"
        [[ "$resolved_dir" == "$link_dir" ]] || die "refusing non-canonical man section directory: $link_dir"

        if [[ "${page##*/}" == *.gz ]]; then
            install -m 0644 -- "$page" "$dest_file"
        else
            gzip -c -- "$page" > "$dest_file"
            chmod 0644 -- "$dest_file"
        fi

        if [[ -L "$link_file" ]]; then
            actual_target=$(readlink -- "$link_file") || die "cannot read symlink: $link_file"
            [[ "$actual_target" == "$dest_file" ]] || die "man page link already exists with a different target: $link_file"
        elif [[ -e "$link_file" ]]; then
            die "refusing to replace existing non-symlink man page: $link_file"
        else
            printf '%s\0' "$rel" >> "$MAN_INSTALLATION"
            ln -s -- "$dest_file" "$link_file"
            continue
        fi

        printf '%s\0' "$rel" >> "$MAN_INSTALLATION"
    done

    echo "Installed ${#sources[@]} compressed man page(s) under $MAN_DEST."
}

[[ $EUID -eq 0 ]] || die "run this script as root (for example, with sudo)."
[[ $# -ge 1 && $# -le 2 ]] || {
    usage >&2
    exit 2
}

BIN_SOURCE=$(realpath -e -- "$1") || die "cannot resolve bin source: $1"
[[ -d "$BIN_SOURCE" ]] || die "bin source is not a directory: $BIN_SOURCE"

# Man pages live in ../man relative to the supplied bin directory.
MAN_SOURCE="$(dirname -- "$BIN_SOURCE")/man"

[[ -x "$BIN_SOURCE/v4linux" && -f "$BIN_SOURCE/v4linux" ]] ||
    die "expected executable $BIN_SOURCE/v4linux"

[[ -x "$BIN_SOURCE/v4linux-dispatch" && -f "$BIN_SOURCE/v4linux-dispatch" ]] ||
    die "expected executable $BIN_SOURCE/v4linux-dispatch"

for tool in "${REQUIRED_TOOLS[@]}"; do
    [[ -f "$BIN_SOURCE/$tool" ]] ||
        die "expected file $BIN_SOURCE/$tool"
done

V4_INPUT="${2:-}"

if [[ -n "$V4_INPUT" ]]; then
    if is_url "$V4_INPUT"; then
        [[ -f "$DOWNLOAD_SH" ]] || die "cannot find $DOWNLOAD_SH to download $V4_INPUT"
        chmod +x "$DOWNLOAD_SH" 2>/dev/null || true

        TMP_ARCHIVE=$(mktemp /tmp/v4_dl.XXXXXX.tar.xz)
        TMP_EXTRACT=$(mktemp -d /tmp/v4_extracted.XXXXXX)
        CLEANUP_DIRS+=("$TMP_ARCHIVE" "$TMP_EXTRACT")

        echo "Downloading Unix v4 from $V4_INPUT..."
        "$DOWNLOAD_SH" "$V4_INPUT" "$TMP_ARCHIVE"

        echo "Extracting archive..."
        tar -xf "$TMP_ARCHIVE" -C "$TMP_EXTRACT"
        rm -f "$TMP_ARCHIVE"

        V4_SOURCE=$(resolve_root_dir "$TMP_EXTRACT")
    else
        V4_SOURCE=$(realpath -e -- "$V4_INPUT") || die "cannot resolve V4 root source: $V4_INPUT"
        [[ -d "$V4_SOURCE" ]] || die "V4 root source is not a directory: $V4_SOURCE"
    fi
else
    echo "No Unix v4 root specified. Would you like to download it?"
    echo "  0 - No, exit"
    echo "  1 - Stock"
    echo "  2 - With symbols"
    read -r -p "Selection [0-2]: " choice

    case "$choice" in
        0)
            echo "Exiting."
            exit 0
            ;;
        1)
            VARIANT="stock"
            TARGET_URL="$STOCK_URL"
            ;;
        2)
            VARIANT="symbols"
            TARGET_URL="$SYMBOLS_URL"
            ;;
        *)
            die "invalid selection: $choice"
            ;;
    esac

    TARGET_DIR="./.v4/$VARIANT"
    DO_DOWNLOAD=true

    if [[ -d "$TARGET_DIR" && -n "$(ls -A "$TARGET_DIR" 2>/dev/null)" ]]; then
        read -r -p "'$TARGET_DIR' already exists. Redownload? [y/N]: " redownload
        case "$redownload" in
            [yY]|[yY][eE][sS])
                rm -rf "$TARGET_DIR"
                ;;
            *)
                DO_DOWNLOAD=false
                ;;
        esac
    fi

    if [[ "$DO_DOWNLOAD" == true ]]; then
        [[ -f "$DOWNLOAD_SH" ]] || die "cannot find $DOWNLOAD_SH to download $TARGET_URL"
        chmod +x "$DOWNLOAD_SH" 2>/dev/null || true

        mkdir -p "$TARGET_DIR"
        TMP_ARCHIVE=$(mktemp /tmp/v4_dl.XXXXXX.tar.xz)
        CLEANUP_DIRS+=("$TMP_ARCHIVE")

        echo "Downloading Unix v4 ($VARIANT)..."
        "$DOWNLOAD_SH" "$TARGET_URL" "$TMP_ARCHIVE"

        echo "Extracting into $TARGET_DIR..."
        tar -xf "$TMP_ARCHIVE" -C "$TARGET_DIR"
        rm -f "$TMP_ARCHIVE"
    fi

    V4_RESOLVED=$(realpath -e -- "$TARGET_DIR") || die "cannot resolve $TARGET_DIR"
    V4_SOURCE=$(resolve_root_dir "$V4_RESOLVED")
fi

check_not_nested() {
    local src=$1
    local dst=$2
    local label=$3

    [[ "$src" == "$dst" ]] && return 0

    case "$src/" in
        "$dst/"*)
            die "$label source is inside destination: $src"
            ;;
    esac

    case "$dst/" in
        "$src/"*)
            die "$label destination is inside source: $dst"
            ;;
    esac
}

if mountpoint -q "$GUEST_ROOT"; then
    die "$GUEST_ROOT is already mounted; run v4linux-uninstall.sh first, then set it up again."
fi

if mountpoint -q "$HOST_STAGING"; then
    die "$HOST_STAGING is already a mountpoint; inspect it and run the uninstall script."
fi

install -d -m 0755 "$V4_DEST" "$PDP11_DEST" /usr/local/libexec

install_man_pages

UNINSTALL_SCRIPT="$PARENT_DIR/v4linux-uninstall.sh"
UNINSTALL_BOOT="$PARENT_DIR/v4linux-uninstall-boot.sh"

install -d -m 0755 /opt/v4/bin

install -m 0755 "$UNINSTALL_SCRIPT" /opt/v4/bin/uninstall-script
install -m 0755 "$UNINSTALL_BOOT" /opt/v4/bin/uninstall

ln -sfn /opt/v4/bin/uninstall /usr/bin/v4-uninstall

if [[ "$V4_SOURCE" != "$V4_DEST" ]]; then
    check_not_nested "$V4_SOURCE" "$V4_DEST" "V4 root"
    cp -a -- "$V4_SOURCE/." "$V4_DEST/"
fi

if [[ "$BIN_SOURCE" != "$PDP11_DEST" ]]; then
    check_not_nested "$BIN_SOURCE" "$PDP11_DEST" "v4linux bin"
    cp -a -- "$BIN_SOURCE/." "$PDP11_DEST/"
fi

[[ -x "$PDP11_DEST/v4linux" ]] ||
    die "installed emulator is not executable: $PDP11_DEST/v4linux"

install -m 0755 "$BIN_SOURCE/v4linux-dispatch" "$DISPATCH_DEST"

for tool in "${REQUIRED_TOOLS[@]}"; do
    case "$tool" in
        cc11)  link_name="v4-cc" ;;
        v4dis) link_name="v4-disasm" ;;
        *)     link_name="v4-$tool" ;;
    esac
    ln -sf "$PDP11_DEST/$tool" "/usr/bin/$link_name"
done

if ! mountpoint -q "$BINFMT_DIR"; then
    modprobe binfmt_misc 2>/dev/null || true
    mount -t binfmt_misc binfmt_misc "$BINFMT_DIR"
    touch "$BINFMT_MARKER"
fi

install -d -m 0755 \
    "$GUEST_ROOT" \
    "$HOST_STAGING" \
    "$STATE_DIR" \
    "$UPPER_DIR" \
    "$WORK_DIR"

mount --bind "$HOST_STAGING" "$HOST_STAGING"
mount --make-private "$HOST_STAGING"
mount --rbind / "$HOST_STAGING"
mount --make-rslave "$HOST_STAGING"

if ! mount -t overlay overlay \
    -o "lowerdir=$V4_DEST,upperdir=$UPPER_DIR,workdir=$WORK_DIR" \
    "$GUEST_ROOT"; then
    umount -R "$HOST_STAGING" || true
    die "failed to mount OverlayFS. Check kernel OverlayFS support and the upper/work directories."
fi

mount --make-private "$GUEST_ROOT"

install -d -m 1777 "$TMP_MOUNT"

if ! mount -t tmpfs -o mode=1777 tmpfs "$TMP_MOUNT"; then
    umount "$GUEST_ROOT" || true
    umount -R "$HOST_STAGING" || true
    die "failed to mount tmpfs on $TMP_MOUNT"
fi

mount --make-private "$TMP_MOUNT"

install -d -m 0755 "$HOME_MOUNT"

if ! mount --bind /home "$HOME_MOUNT"; then
    umount "$TMP_MOUNT" || true
    umount "$GUEST_ROOT" || true
    umount -R "$HOST_STAGING" || true
    die "failed to bind host /home into $GUEST_ROOT/home"
fi

mount --make-private "$HOME_MOUNT"

install -d -m 0755 "$HOST_MOUNT"

if ! mount --move "$HOST_STAGING" "$HOST_MOUNT"; then
    umount "$HOME_MOUNT" || true
    umount "$TMP_MOUNT" || true
    umount "$GUEST_ROOT" || true
    umount -R "$HOST_STAGING" || true
    die "failed to move host root mount into $HOST_MOUNT"
fi

umount "$HOST_STAGING" 2>/dev/null || true
rmdir "$HOST_STAGING" 2>/dev/null || true

for name in v4linux-aout-0407 v4linux-aout-0410; do
    if [[ -e "$BINFMT_DIR/$name" ]]; then
        printf '%s\n' -1 > "$BINFMT_DIR/$name"
    fi
done

printf '%s\n' ':v4linux-aout-0407:M::\x07\x01::/usr/local/libexec/v4linux-dispatch:' \
    > "$BINFMT_DIR/register"

printf '%s\n' ':v4linux-aout-0410:M::\x08\x01::/usr/local/libexec/v4linux-dispatch:' \
    > "$BINFMT_DIR/register"

echo "Installing persistence..."

cat <<EOF > "$RUNTIME_SCRIPT"
#!/usr/bin/env bash
set -Eeuo pipefail

V4_DEST=$V4_DEST
GUEST_ROOT=$GUEST_ROOT
HOST_MOUNT="$GUEST_ROOT/host"
HOME_MOUNT="$GUEST_ROOT/home"
TMP_MOUNT="$GUEST_ROOT/tmp"
STATE_DIR=$STATE_DIR
UPPER_DIR="$STATE_DIR/upper"
WORK_DIR="$STATE_DIR/work"
HOST_STAGING=$HOST_STAGING
BINFMT_DIR=$BINFMT_DIR

if ! mountpoint -q "\$BINFMT_DIR"; then
    modprobe binfmt_misc 2>/dev/null || true
    mount -t binfmt_misc binfmt_misc "\$BINFMT_DIR"
fi

install -d -m 0755 "\$GUEST_ROOT" "\$HOST_STAGING" "\$STATE_DIR" "\$UPPER_DIR" "\$WORK_DIR"

mount --bind "\$HOST_STAGING" "\$HOST_STAGING"
mount --make-private "\$HOST_STAGING"
mount --rbind / "\$HOST_STAGING"
mount --make-rslave "\$HOST_STAGING"

mount -t overlay overlay -o "lowerdir=\$V4_DEST,upperdir=\$UPPER_DIR,workdir=\$WORK_DIR" "\$GUEST_ROOT"
mount --make-private "\$GUEST_ROOT"

install -d -m 1777 "\$TMP_MOUNT"
mount -t tmpfs -o mode=1777 tmpfs "\$TMP_MOUNT"
mount --make-private "\$TMP_MOUNT"

install -d -m 0755 "\$HOME_MOUNT"
mount --bind /home "\$HOME_MOUNT"
mount --make-private "\$HOME_MOUNT"

install -d -m 0755 "\$HOST_MOUNT"
mount --move "\$HOST_STAGING" "\$HOST_MOUNT"

for name in v4linux-aout-0407 v4linux-aout-0410; do
    if [[ -e "\$BINFMT_DIR/\$name" ]]; then
        printf '%s\n' -1 > "\$BINFMT_DIR/\$name"
    fi
done
printf '%s\n' ':v4linux-aout-0407:M::\x07\x01::/usr/local/libexec/v4linux-dispatch:' > "\$BINFMT_DIR/register"
printf '%s\n' ':v4linux-aout-0410:M::\x08\x01::/usr/local/libexec/v4linux-dispatch:' > "\$BINFMT_DIR/register"
EOF

chmod +x "$RUNTIME_SCRIPT"

cat <<EOF > /etc/systemd/system/v4linux.service
[Unit]
Description=PDP-11 Unix v4 Persistence
After=local-fs.target

[Service]
Type=oneshot
RemainAfterExit=yes
ExecStart=$RUNTIME_SCRIPT
ExecStop=/bin/umount -R $GUEST_ROOT

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable v4linux.service

echo "Persistence installed via systemd."

cat <<EOF
setup complete.
  V4 lower root: $V4_DEST
  Emulator:      $PDP11_DEST/v4linux
  Guest root:    $GUEST_ROOT (/)
  Guest /tmp:    tmpfs at $TMP_MOUNT
  Host view:     $HOST_MOUNT (/host)
  Home view:     $HOME_MOUNT (/home)
  Dispatcher:    $DISPATCH_DEST
  Magic 0407:    registered (07 01)
  Magic 0410:    registered (08 01)
EOF