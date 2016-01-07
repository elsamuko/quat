#!/usr/bin/env bash

OS=linux
PROJECT=zlib
VERSION="1.2.8"
DL_URL="http://zlib.net/zlib-$VERSION.tar.xz"

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
MAIN_DIR="$SCRIPT_DIR/.."
TARGET_DIR="$MAIN_DIR/libs/$PROJECT"
PROJECT_DIR="$MAIN_DIR/tmp/$PROJECT"
DOWNLOAD="$PROJECT_DIR/$PROJECT-$VERSION.tar.xz"
SRC_DIR="$PROJECT_DIR/src"
BUILD_DIR="$SRC_DIR/$PROJECT-$VERSION"

function indent {
    sed  's/^/     /'
}

function doPrepare {
    if [ -d "$SRC_DIR" ]; then
        rm -rf "$SRC_DIR"
    fi
    if [ -d "$TARGET_DIR" ]; then
        rm -rf "$TARGET_DIR"
    fi
    mkdir -p "$PROJECT_DIR"
    mkdir -p "$TARGET_DIR"
    mkdir -p "$SRC_DIR"
}

function doDownload {
    if [ ! -f "$DOWNLOAD" ]; then
        curl "$DL_URL" -o "$DOWNLOAD"
    fi
}

function doUnzip {
    tar xJf "$DOWNLOAD" -C "$SRC_DIR"
}

function doConfigure {
    ./configure --help > "$PROJECT_DIR/options.txt"
    ./configure \
            --prefix="$BUILD_DIR" \
            --static \
            --64
}

function doBuild {
    cd "$SRC_DIR/$PROJECT-$VERSION"

    # debug
    (export CXXFLAGS="$CXXFLAGS -g -O0"; \
    export CFLAGS="$CFLAGS -g -O0"; \
    doConfigure)
    make -j8 install prefix="$BUILD_DIR/debug"

    # release
    (export CXXFLAGS="$CXXFLAGS -msse2 -Ofast -finline -ffast-math -funsafe-math-optimizations"; \
    export CFLAGS="$CFLAGS -msse2 -Ofast -finline -ffast-math -funsafe-math-optimizations"; \
    doConfigure)
    make -j8 install prefix="$BUILD_DIR/release"
}

function doCopy {
    mkdir -p "$TARGET_DIR/bin/$OS/debug"
    mkdir -p "$TARGET_DIR/bin/$OS/release"
    mkdir -p "$TARGET_DIR/include"
    cp -r "$BUILD_DIR/debug/lib/libz.a" "$TARGET_DIR/bin/$OS/debug/libz.a"
    cp -r "$BUILD_DIR/release/lib/libz.a" "$TARGET_DIR/bin/$OS/release/libz.a"
    cp -r "$BUILD_DIR/release/include"/* "$TARGET_DIR/include"
}


echo "Prepare"
doPrepare | indent

echo "Download"
doDownload | indent

echo "Unzip"
doUnzip | indent

echo "Build"
doBuild 2>&1 | indent

echo "Copy"
doCopy | indent
