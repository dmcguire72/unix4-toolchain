#!/usr/bin/env bash
set -Eeuo pipefail

V4_DEST=/opt/v4/sys
PDP11_DEST=/opt/v4/bin
DISPATCH_DEST=/usr/local/libexec/unix4-dispatch
GUEST_ROOT=/var/run/pdp11-v4
HOST_MOUNT="$GUEST_ROOT/host"
HOME_MOUNT="$GUEST_ROOT/home"
STATE_DIR=/var/run/pdp11-v4-overlay
UPPER_DIR="$STATE_DIR/upper"
WORK_DIR="$STATE_DIR/work"
HOST_STAGING=/var/run/pdp11-v4-host-staging
BINFMT_DIR=/proc/sys/fs/binfmt_misc
BINFMT_MARKER=/run/pdp11-binfmt-mounted-by-pdp11
RUNTIME_SCRIPT="$PDP11_DEST/pdp11-runtime"

usage() {
    cat <<'USAGE'
Usage: sudo ./pdp11-setup.sh /path/to/extracted-v4-root /path/to/pdp11-bin-dir

You can get Unix v4 from https://danielm.pages.dev/useful/unix_v4.tar.xz.

The second argument must be the bin folder from 'make emulator'.
USAGE
}

die() {
    echo "pdp11-setup: $*" >&2
    exit 1
}

[[ $EUID -eq 0 ]] || die "run this script as root (for example, with sudo)."
[[ $# -eq 2 ]] || {
    usage >&2
    exit 2
}

V4_SOURCE=$(realpath -e -- "$1") || die "cannot resolve V4 root source: $1"
BIN_SOURCE=$(realpath -e -- "$2") || die "cannot resolve bin source: $2"

[[ -d "$V4_SOURCE" ]] || die "V4 root source is not a directory: $V4_SOURCE"
[[ -d "$BIN_SOURCE" ]] || die "bin source is not a directory: $BIN_SOURCE"

[[ -x "$BIN_SOURCE/v4linux" && -f "$BIN_SOURCE/v4linux" ]] ||
    die "expected executable $BIN_SOURCE/v4linux"

[[ -x "$BIN_SOURCE/unix4-dispatch" && -f "$BIN_SOURCE/unix4-dispatch" ]] ||
    die "expected executable $BIN_SOURCE/unix4-dispatch"

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
    die "$GUEST_ROOT is already mounted; run pdp11-uninstall.sh first, then set it up again."
fi

if mountpoint -q "$HOST_STAGING"; then
    die "$HOST_STAGING is already a mountpoint; inspect it and run the uninstall script."
fi

install -d -m 0755 "$V4_DEST" "$PDP11_DEST" /usr/local/libexec

if [[ "$V4_SOURCE" != "$V4_DEST" ]]; then
    check_not_nested "$V4_SOURCE" "$V4_DEST" "V4 root"
    cp -a -- "$V4_SOURCE/." "$V4_DEST/"
fi

if [[ "$BIN_SOURCE" != "$PDP11_DEST" ]]; then
    check_not_nested "$BIN_SOURCE" "$PDP11_DEST" "pdp11 bin"
    cp -a -- "$BIN_SOURCE/." "$PDP11_DEST/"
fi

[[ -x "$PDP11_DEST/v4linux" ]] ||
    die "installed emulator is not executable: $PDP11_DEST/v4linux"

install -m 0755 "$BIN_SOURCE/unix4-dispatch" "$DISPATCH_DEST"

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

install -d -m 0755 "$HOME_MOUNT"

if ! mount --bind /home "$HOME_MOUNT"; then
    umount "$GUEST_ROOT" || true
    umount -R "$HOST_STAGING" || true
    die "failed to bind host /home into $GUEST_ROOT/home"
fi

mount --make-private "$HOME_MOUNT"

install -d -m 0755 "$HOST_MOUNT"

if ! mount --move "$HOST_STAGING" "$HOST_MOUNT"; then
    umount "$GUEST_ROOT" || true
    umount -R "$HOST_STAGING" || true
    die "failed to move host root mount into $HOST_MOUNT"
fi

umount "$HOST_STAGING" 2>/dev/null || true
rmdir "$HOST_STAGING" 2>/dev/null || true

for name in pdp11-aout-0407 pdp11-aout-0410; do
    if [[ -e "$BINFMT_DIR/$name" ]]; then
        printf '%s\n' -1 > "$BINFMT_DIR/$name"
    fi
done

printf '%s\n' ':pdp11-aout-0407:M::\x07\x01::/usr/local/libexec/unix4-dispatch:' \
    > "$BINFMT_DIR/register"

printf '%s\n' ':pdp11-aout-0410:M::\x08\x01::/usr/local/libexec/unix4-dispatch:' \
    > "$BINFMT_DIR/register"

echo "Installing persistence..."

cat <<EOF > "$RUNTIME_SCRIPT"
#!/usr/bin/env bash
set -Eeuo pipefail

V4_DEST=$V4_DEST
GUEST_ROOT=$GUEST_ROOT
HOST_MOUNT="$GUEST_ROOT/host"
HOME_MOUNT="$GUEST_ROOT/home"
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

install -d -m 0755 "\$HOME_MOUNT"
mount --bind /home "\$HOME_MOUNT"
mount --make-private "\$HOME_MOUNT"

install -d -m 0755 "\$HOST_MOUNT"
mount --move "\$HOST_STAGING" "\$HOST_MOUNT"

for name in pdp11-aout-0407 pdp11-aout-0410; do
    if [[ -e "\$BINFMT_DIR/\$name" ]]; then
        printf '%s\n' -1 > "\$BINFMT_DIR/\$name"
    fi
done
printf '%s\n' ':pdp11-aout-0407:M::\x07\x01::/usr/local/libexec/unix4-dispatch:' > "\$BINFMT_DIR/register"
printf '%s\n' ':pdp11-aout-0410:M::\x08\x01::/usr/local/libexec/unix4-dispatch:' > "\$BINFMT_DIR/register"
EOF

chmod +x "$RUNTIME_SCRIPT"

cat <<EOF > /etc/systemd/system/pdp11.service
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
systemctl enable pdp11.service

echo "Persistence installed via systemd."

cat <<EOF
setup complete.
  V4 lower root: $V4_DEST
  Emulator:      $PDP11_DEST/v4linux
  Guest root:    $GUEST_ROOT (/)
  Host view:     $HOST_MOUNT (/host)
  Home view:     $HOME_MOUNT (/home)
  Dispatcher:    $DISPATCH_DEST
  Magic 0407:    registered (07 01)
  Magic 0410:    registered (08 01)
EOF
