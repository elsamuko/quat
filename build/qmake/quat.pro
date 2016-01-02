
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

INCLUDEPATH += $${SRC_DIR}/kernel
LIBS += -lz -lX11
LIBS += -lfltk -lfltk_images

HEADERS += $$files($${SRC_DIR}/kernel/*.h)
SOURCES += $$files($${SRC_DIR}/kernel/*.c)

SOURCES -= $${SRC_DIR}/kernel/textver.c

HEADERS += $$files($${SRC_DIR}/gui/*.h)
SOURCES += $$files($${SRC_DIR}/gui/*.cxx)
