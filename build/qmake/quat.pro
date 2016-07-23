
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
win32-msvc*: include( $${PRI_DIR}/win.pri )
win32-g++: include( $${PRI_DIR}/mingw.pri )

macx: PLATFORM=mac
linux: PLATFORM=linux
win32: PLATFORM=win

win32: RC_FILE = $${MAIN_DIR}/resources/win/quat.rc
win32: DEFINES += 'IDI_ICON1=101'
macx: ICON = $${MAIN_DIR}/resources/mac/logo.icns
macx: QMAKE_POST_LINK += macdeployqt $${DESTDIR}/$${TARGET}.$${TEMPLATE};

win32 {
    CRLF = $$escape_expand(\n\t)
    DESTDIR_WIN = $${DESTDIR}
    DESTDIR_WIN ~= s,/,\\,g

    DEFINES += '"DOCDIR=\\\"manual\\\""'
    DEFINES += '"INIPATH=\\\"/examples\\\""'
}

win32-msvc* {
    QMAKE_POST_LINK += $$quote( echo \"Copy manual...\" & xcopy /s /i /y \"$${MAIN_DIR}/docs/manual\" \"$${DESTDIR_WIN}/manual\" $$CRLF )
    QMAKE_POST_LINK += $$quote( echo \"Copy examples...\" & xcopy /s /i /y \"$${MAIN_DIR}/docs/examples\" \"$${DESTDIR_WIN}/examples\" $$CRLF )
}

win32-g++ {
    QMAKE_POST_LINK += $$quote( echo \"Copy manual...\" & cmd /C \"xcopy /s /i /y \\\"$${MAIN_DIR}/docs/manual\\\" \\\"$${DESTDIR_WIN}/manual\\\" \" $$CRLF )
    QMAKE_POST_LINK += $$quote( echo \"Copy examples...\" & cmd /C \"xcopy /s /i /y \\\"$${MAIN_DIR}/docs/examples\\\" \\\"$${DESTDIR_WIN}/examples\\\" \" $$CRLF )
}

linux {
    DEFINES += '"DOCDIR=\\\"manual\\\""'
    DEFINES += '"INIPATH=\\\"/examples\\\""'
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/manual\" \"$$DESTDIR/\";
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/examples\" \"$$DESTDIR/\";
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/resources/inkscape/logo.png\" \"$$DESTDIR/\";
}

macx {
    DEFINES += '"DOCDIR=\\\"/../manual\\\""'
    DEFINES += '"INIPATH=\\\"/../examples\\\""'
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/manual\" \"$$DESTDIR/$${TARGET}.app/Contents/\";
    QMAKE_POST_LINK += cp -r \"$${MAIN_DIR}/docs/examples\" \"$$DESTDIR/$${TARGET}.app/Contents/\";
}

include( $${PRI_DIR}/libs.pri )
include( $${PRI_DIR}/src.pri )
include( $${PRI_DIR}/gui.pri )
