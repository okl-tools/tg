#!/bin/bash

# Script to create a .deb package for the 'tg' application

# Package metadata
PACKAGE_NAME="tg"
VERSION="1.0.0"
ARCH="amd64"
MAINTAINER="Your Name <your.email@example.com>"
DESCRIPTION="Tesseract GUI application for OCR"
SECTION="utils"
PRIORITY="optional"
DEPENDS="libtesseract4, liblept5, qt6-base-dev, tesseract-ocr, libxcb1, libx11-xcb1, libxcb-xfixes0, libxcb-shape0, libxcb-shm0, libxcb-randr0, libxcb-image0, libxcb-keysyms1, libxcb-sync1, libxcb-xinerama0, libxcb-xkb1"

# Directories
BUILD_DIR="build"
DEB_DIR="${PACKAGE_NAME}_${VERSION}_${ARCH}"
INSTALL_DIR="${DEB_DIR}/usr"
BIN_DIR="${INSTALL_DIR}/bin"
SHARE_DIR="${INSTALL_DIR}/share/${PACKAGE_NAME}"
DESKTOP_DIR="${INSTALL_DIR}/share/applications"

# Create directory structure for the .deb package
mkdir -p "${BIN_DIR}"
mkdir -p "${SHARE_DIR}"
mkdir -p "${DESKTOP_DIR}"
mkdir -p "${DEB_DIR}/DEBIAN"

# Copy the compiled binary
cp "${BUILD_DIR}/tg" "${BIN_DIR}/"
chmod 755 "${BIN_DIR}/tg"

# Create a desktop entry
cat > "${DESKTOP_DIR}/${PACKAGE_NAME}.desktop" << EOF
[Desktop Entry]
Name=Tesseract GUI
Exec=tg
Type=Application
Terminal=false
Categories=Utility;
Comment=A GUI for Tesseract OCR
EOF

# Create the control file
cat > "${DEB_DIR}/DEBIAN/control" << EOF
Package: ${PACKAGE_NAME}
Version: ${VERSION}
Architecture: ${ARCH}
Maintainer: ${MAINTAINER}
Depends: ${DEPENDS}
Section: ${SECTION}
Priority: ${PRIORITY}
Description: ${DESCRIPTION}
EOF

# Build the .deb package
dpkg-deb --build "${DEB_DIR}"

# Clean up (optional, comment out if you want to inspect the DEB_DIR)
# rm -rf "${DEB_DIR}"

echo "Debian package ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb created successfully."