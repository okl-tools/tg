#!/bin/bash

set -e

APP_NAME="tg"
VERSION="1.0.0"
ARCH=$(dpkg --print-architecture)
BUILD_DIR="build"
DEB_DIR="deb_pkg"

BIN_PATH="$BUILD_DIR/$APP_NAME"
OUT_BIN="usr/bin/$APP_NAME"

# Prüfe, ob Binary existiert
if [ ! -f "$BIN_PATH" ]; then
    echo "❌ Binary $BIN_PATH nicht gefunden. Bitte zuerst mit make_tg.sh bauen."
    exit 1
fi

# Verzeichnisse vorbereiten
rm -rf "$DEB_DIR"
mkdir -p "$DEB_DIR/DEBIAN"
mkdir -p "$DEB_DIR/usr/bin"

# Binary kopieren und strippen
cp "$BIN_PATH" "$DEB_DIR/$OUT_BIN"
strip "$DEB_DIR/$OUT_BIN"

# Control-Datei erstellen
cat > "$DEB_DIR/DEBIAN/control" <<EOF
Package: $APP_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: $ARCH
Depends: $(ldd "$DEB_DIR/$OUT_BIN" | grep "=>" | awk '{print $3}' | xargs dpkg -S | cut -d: -f1 | sort -u | tr '\n' ',' | sed 's/,$//')
Maintainer: $(whoami)
Description: Kleines Qt-Tool $APP_NAME
EOF

# Paket bauen
DEB_FILE="${APP_NAME}_${VERSION}_${ARCH}.deb"
dpkg-deb --build "$DEB_DIR" "$DEB_FILE"

echo
echo "✅ Fertig: $DEB_FILE"
echo "Installierbar mit:"
echo "    sudo apt install ./$DEB_FILE"
