
# zlib
INCLUDEPATH += $$MAIN_DIR/libs/zlib/include
# fltk
INCLUDEPATH += $$MAIN_DIR/libs/fltk/include

unix {
    QMAKE_CXXFLAGS += -isystem $$MAIN_DIR/libs/zlib/include
    QMAKE_CXXFLAGS += -isystem $$MAIN_DIR/libs/fltk/include
    QMAKE_CFLAGS += -isystem $$MAIN_DIR/libs/zlib/include
    QMAKE_CFLAGS += -isystem $$MAIN_DIR/libs/fltk/include
}

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

win32-msvc* {
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

win32-g++ {
 LIBS += /mingw64/lib/libfltk_images.a -lpng -lz -ljpeg /mingw64/lib/libfltk.a -lgdi32 -luser32 -lole32 -luuid -lcomctl32 -lcomdlg32
    #LIBS += /mingw64/lib/libfltk.a /mingw64/lib/libfltk_images.a /mingw64/lib/libjpeg.a /mingw64/lib/libz.a
}

