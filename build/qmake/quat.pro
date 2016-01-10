
TEMPLATE = app
CONFIG += windows
CONFIG -= console
CONFIG += c++11
CONFIG -= qt
TARGET = Quat

MAIN_DIR = ../..
PRI_DIR  = $${MAIN_DIR}/build/qmake
SRC_DIR  = $${MAIN_DIR}/src

include( $${PRI_DIR}/setup.pri )
DESTDIR  = $${MAIN_DIR}/bin/$${COMPILE_MODE}
linux: include( $${PRI_DIR}/linux.pri )
macx:  include( $${PRI_DIR}/mac.pri )
win32: CONFIG += static
win32: include( $${PRI_DIR}/win.pri )

macx: PLATFORM=mac
linux: PLATFORM=linux
win32: PLATFORM=win

win32: RC_FILE = $${MAIN_DIR}/resources/win/quat.rc
win32: DEFINES += 'IDI_ICON1=101'
macx: ICON = $${MAIN_DIR}/resources/mac/logo.icns
macx: QMAKE_POST_LINK += macdeployqt $${DESTDIR}/$${TARGET}.$${TEMPLATE};

linux | win32 {
    DEFINES += '"DOCDIR=\\\"manual\\\""'
    DEFINES += '"INIPATH=\\\"./examples\\\""'
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/manual\" \"$$DESTDIR/\";
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/examples\" \"$$DESTDIR/\";
}

linux {
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/resources/inkscape/logo.png\" \"$$DESTDIR/\";
}

macx {
    DEFINES += '"DOCDIR=\\\"./../manual\\\""'
    DEFINES += '"INIPATH=\\\"./../examples\\\""'
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/manual\" \"$$DESTDIR/$${TARGET}.app/Contents/\";
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/examples\" \"$$DESTDIR/$${TARGET}.app/Contents/\";
}

include( $${PRI_DIR}/libs.pri )
include( $${PRI_DIR}/src.pri )
