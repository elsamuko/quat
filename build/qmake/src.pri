
INCLUDEPATH += $${SRC_DIR}/kernel

HEADERS += $$files($${SRC_DIR}/kernel/*.h)
SOURCES += $$files($${SRC_DIR}/kernel/*.c)

SOURCES -= $${SRC_DIR}/kernel/textver.c

HEADERS += $$files($${SRC_DIR}/gui/*.h)
SOURCES += $$files($${SRC_DIR}/gui/*.cxx)
