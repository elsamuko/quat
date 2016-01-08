
TEMPLATE = app
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

unix {
    # zlib
    INCLUDEPATH += $$MAIN_DIR/libs/zlib/include
    LIBS += $$MAIN_DIR/libs/zlib/bin/$$PLATFORM/$$COMPILE_MODE/libz.a

    # fltk
    INCLUDEPATH += $$MAIN_DIR/libs/fltk/include
    LIBS += $$MAIN_DIR/libs/fltk/bin/$$PLATFORM/$$COMPILE_MODE/libfltk_images.a \
            $$MAIN_DIR/libs/fltk/bin/$$PLATFORM/$$COMPILE_MODE/libfltk.a

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

INCLUDEPATH += $${SRC_DIR}/kernel

HEADERS += $$files($${SRC_DIR}/kernel/*.h)
SOURCES += $$files($${SRC_DIR}/kernel/*.c)

SOURCES -= $${SRC_DIR}/kernel/textver.c

HEADERS += $$files($${SRC_DIR}/gui/*.h)
SOURCES += $$files($${SRC_DIR}/gui/*.cxx)
