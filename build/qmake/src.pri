
unix: QMAKE_CXXFLAGS += -Wno-unused-parameter
unix: QMAKE_CFLAGS += -Wno-unused-parameter

INCLUDEPATH += $${SRC_DIR}/kernel

HEADERS += $$files($${SRC_DIR}/kernel/*.h)
SOURCES += $$files($${SRC_DIR}/kernel/*.c)

SOURCES -= $${SRC_DIR}/kernel/textver.c
