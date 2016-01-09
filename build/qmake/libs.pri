
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
