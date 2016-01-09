
TEMPLATE = app
CONFIG += windows
CONFIG -= console
CONFIG += c++11
CONFIG -= qt

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

# zlib
INCLUDEPATH += $$MAIN_DIR/libs/zlib/include
# fltk
INCLUDEPATH += $$MAIN_DIR/libs/fltk/include

unix {
    # zlib
    LIBS += $$MAIN_DIR/libs/zlib/bin/$$PLATFORM/$$COMPILE_MODE/libz.a

    # fltk
    LIBS += $$MAIN_DIR/libs/fltk/bin/$$PLATFORM/$$COMPILE_MODE/libfltk_images.a
    LIBS += $$MAIN_DIR/libs/fltk/bin/$$PLATFORM/$$COMPILE_MODE/libfltk.a

    LIBS += -ljpeg
}

linux {
    LIBS += -lpng
    LIBS += -ldl -fPIC
    LIBS += -lXcursor -lXfixes -lXext -lXft -lfontconfig -lXinerama -lpthread -ldl -lm -lX11
}

macx {
    LIBS += -L/opt/local/lib
    LIBS += -lpng16
    LIBS += -lc++ -lpthread -framework Cocoa
}

win32 {
    # zlib
    LIBS += $$MAIN_DIR/libs/zlib/bin/$$PLATFORM/$$COMPILE_MODE/zlib.lib

    # fltk
    LIBS += /LIBPATH:$$MAIN_DIR/libs/fltk/bin/$$PLATFORM/$$COMPILE_MODE
    LIBS += fltkimages$${COMPILE_FLAG}.lib
    LIBS += fltk$${COMPILE_FLAG}.lib
    LIBS += fltkjpeg$${COMPILE_FLAG}.lib
    LIBS += fltkpng$${COMPILE_FLAG}.lib

    # windows
    LIBS += Gdi32.lib User32.lib Ole32.lib Advapi32.lib Shell32.lib
}

INCLUDEPATH += $${SRC_DIR}/kernel

HEADERS += $$files($${SRC_DIR}/kernel/*.h)
SOURCES += $$files($${SRC_DIR}/kernel/*.c)

SOURCES -= $${SRC_DIR}/kernel/textver.c

HEADERS += $$files($${SRC_DIR}/gui/*.h)
SOURCES += $$files($${SRC_DIR}/gui/*.cxx)
