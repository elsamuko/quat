if FLTK 
#lib_LIBRARIES=libquat.a
noinst_LIBRARIES=libquat.a
else
bin_PROGRAMS=quat-txt
quat_txt_LDADD=
endif

if FLTK 
libquat_a_SOURCES=calculat.c calculat.h colors.c colors.h \
	common.h files.c files.h iter.c iter.h png.c png.h \
	qmath.c qmath.h quat.c quat.h vector.h
#INCLUDES=@FLTK_DEF@
else
quat_txt_SOURCES=calculat.c calculat.h colors.c colors.h \
	common.h files.c files.h iter.c iter.h png.c png.h \
	qmath.c qmath.h quat.c quat.h vector.h textver.c \
	textver.h
#INCLUDES=-DTEXTVER
endif
EXTRA_DIST=textver.c textver.h 

