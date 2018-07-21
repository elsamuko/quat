#!/usr/bin/env bash

OS=win
PROJECT=fltk
VERSION="1.3.4"
PATCH="2"
DL_URL="http://fltk.org/pub/fltk/$VERSION/fltk-$VERSION-$PATCH-source.tar.gz"

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
MAIN_DIR="$SCRIPT_DIR/.."
TARGET_DIR="$MAIN_DIR/libs/$PROJECT"
PROJECT_DIR="$MAIN_DIR/tmp/$PROJECT"
DOWNLOAD="$PROJECT_DIR/$PROJECT-$VERSION-$PATCH.tar.gz"
SRC_DIR="$PROJECT_DIR/src"
BUILD_DIR="$SRC_DIR/$PROJECT-$VERSION-$PATCH"
BUILD_HELPER_DEBUG="$BUILD_DIR/ide/VisualC2010/build_debug.bat"
BUILD_HELPER_RELEASE="$BUILD_DIR/ide/VisualC2010/build_release.bat"

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
    tar xzf "$DOWNLOAD" -C "$SRC_DIR"
    for FROM in "$SRC_DIR"/*; do
        echo $FROM
        mv "$FROM" "$SRC_DIR/$PROJECT-$VERSION"
        break
    done
}

function create_helper {
	# VS 2015	
	echo -ne '@echo off\r\n\r\ncall "%VS140COMNTOOLS%..\\..\\VC\\bin\\amd64\\vcvars64.bat"\r\n' > "$BUILD_HELPER_DEBUG"
	echo -ne '\r\n' >> "$BUILD_HELPER_DEBUG"
	echo -ne 'msbuild fltk.sln /p:configuration=debug /p:platform=x64 /p:PlatformToolset=v140 /p:PreferredToolArchitecture=x64\r\n' >> "$BUILD_HELPER_DEBUG"
    chmod +x "$BUILD_HELPER_DEBUG"

	echo -ne '@echo off\r\n\r\ncall "%VS140COMNTOOLS%..\\..\\VC\\bin\\amd64\\vcvars64.bat"\r\n' > "$BUILD_HELPER_RELEASE"
	echo -ne '\r\n' >> "$BUILD_HELPER_RELEASE"
	echo -ne 'msbuild fltk.sln /p:configuration=release /p:platform=x64 /p:PlatformToolset=v140 /p:PreferredToolArchitecture=x64\r\n' >> "$BUILD_HELPER_RELEASE"
    chmod +x "$BUILD_HELPER_RELEASE"
}

function doBuild {
	create_helper
	
	# preconfigured project files
	cp -R "$SCRIPT_DIR/fltk_win"/* "$BUILD_DIR/"

    # debug and release
    cd "$BUILD_DIR"
	rm -r lib
	cmd /c "cd ide/VisualC2010 & build_debug.bat"
	mv lib lib_debug
	cmd /c "cd ide/VisualC2010 & build_release.bat"
	mv lib lib_release
}

function doCopy {
    mkdir -p "$TARGET_DIR/bin/$OS/debug"
    mkdir -p "$TARGET_DIR/bin/$OS/release"
    mkdir -p "$TARGET_DIR/include"
    cp -r "$BUILD_DIR/lib_debug"/* "$TARGET_DIR/bin/$OS/debug"
    cp -r "$BUILD_DIR/lib_release"/* "$TARGET_DIR/bin/$OS/release"
    cp -r "$BUILD_DIR/FL" "$TARGET_DIR/include"
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
