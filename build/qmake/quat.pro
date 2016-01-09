
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


include( $${PRI_DIR}/libs.pri )
include( $${PRI_DIR}/src.pri )
