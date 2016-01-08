#!/usr/bin/env bash

OS=win
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
BUILD_HELPER="$BUILD_DIR/build.bat"

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
    for FROM in "$SRC_DIR"/*; do
        echo $FROM
        mv "$FROM" "$SRC_DIR/$PROJECT-$VERSION"
        break
    done
}

function createHelper {
	# VS 2015
	echo -ne '@echo off\r\n\r\ncall "%VS140COMNTOOLS%..\\..\\VC\\bin\\amd64\\vcvars64.bat"\r\n' > "$BUILD_HELPER"
	echo -ne '\r\n' >> "$BUILD_HELPER"
	# release
	echo -ne 'nmake -f win32/Makefile.msc clean\r\n' >> "$BUILD_HELPER"
	echo -ne 'nmake -f win32/Makefile.msc CFLAGS="-nologo -MT  -W3 -Ox -Zi -Fd"zlib" $(LOC)" AS=ml64 LOC="-DASMV -DASMINF -I." OBJA="inffasx64.obj gvmat64.obj inffas8664.obj" zlib.lib\r\n' >> "$BUILD_HELPER"
	echo -ne 'copy zlib.lib release/zlib.lib\r\n' >> "$BUILD_HELPER"
	# debug
	echo -ne 'nmake -f win32/Makefile.msc clean\r\n' >> "$BUILD_HELPER"
	echo -ne 'nmake -f win32/Makefile.msc CFLAGS="-nologo -MTd -W3 -Od -Zi -Fd"zlib" $(LOC)" AS=ml64 LOC="-DASMV -DASMINF -I." OBJA="inffasx64.obj gvmat64.obj inffas8664.obj" zlib.lib\r\n' >> "$BUILD_HELPER"
	echo -ne 'copy zlib.lib debug/zlib.lib\r\n' >> "$BUILD_HELPER"

    chmod +x "$BUILD_HELPER"
}

function doBuild {
	createHelper
	
    # debug and release
    cd "$BUILD_DIR"
	mkdir debug
	mkdir release
	"$BUILD_HELPER"
}

function doCopy {
    mkdir -p "$TARGET_DIR/bin/$OS/debug"
    mkdir -p "$TARGET_DIR/bin/$OS/release"
    mkdir -p "$TARGET_DIR/include"
    cp "$BUILD_DIR/debug/zlib.lib" "$TARGET_DIR/bin/$OS/debug/zlib.lib"
    cp "$BUILD_DIR/release/zlib.lib" "$TARGET_DIR/bin/$OS/release/zlib.lib"
    cp "$BUILD_DIR/zlib.h" "$TARGET_DIR/include"
    cp "$BUILD_DIR/zconf.h" "$TARGET_DIR/include"
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
