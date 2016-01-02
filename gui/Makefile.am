INCLUDES=-I$(top_srcdir)/kernel

SUFFIXES=".c .cxx .h .fl .o"

if FLTK
bin_PROGRAMS=quat
quat_SOURCES=main.cxx MandelPreview.cxx JuliaPreview.cxx ImageWid.cxx \
   ViewSelector.cxx ColorPreview.cxx ColorClipboard.cxx ColorEditor.cxx \
   ObjectEditor.cxx ViewEditor.cxx IntersecEditor.cxx OtherEditor.cxx \
   ParameterEditor.cxx ScrollWid.cxx PixWid.cxx about.cxx structs.cxx \
   WriteIni.cxx MainWindow.cxx CReplacements.cxx
noinst_HEADERS=MandelPreview.h JuliaPreview.h ImageWid.h ViewSelector.h \
   ColorPreview.h ColorClipboard.h ColorEditor.h ObjectEditor.h ViewEditor.h\
   IntersecEditor.h OtherEditor.h ParameterEditor.h ScrollWid.h PixWid.h \
   about.h ChildWindow.h structs.h WriteIni.h MainWindow.h CReplacements.h \
   dragWindow.h
#quat_LDADD=-L$(top_builddir)/kernel -lquat
quat_LDADD=$(top_builddir)/kernel/libquat.a
quat_DEPENDENCIES=$(top_builddir)/kernel/libquat.a

# The following pattern rule works well, but not on DEC OSF1
# where it would say 'No rule to make main.fl'
# %.cxx %.h: %.fl
#	$(FLUID) -c $<

# This is a suffix rule
.fl.cxx .fl.h:
	$(FLUID) -c $<

endif

if WIN32
quat_DEPENDENCIES=resources.o $(top_builddir)/kernel/libquat.a
quat_LDFLAGS=resources.o
resources.o: quat.rc
	windres quat.rc resources.o
endif

BUILT_SOURCES=about.cxx about.h ParameterEditor.cxx ParameterEditor.h \
  ObjectEditor.cxx ObjectEditor.h ViewEditor.cxx ViewEditor.h \
  ColorEditor.cxx ColorEditor.h IntersecEditor.cxx IntersecEditor.h \
  OtherEditor.cxx OtherEditor.h WriteIni.cxx WriteIni.h

EXTRA_DIST=$(top_srcdir)/gui/ParameterEditor.fl $(top_srcdir)/gui/about.fl \
  $(top_srcdir)/gui/title.xpm $(top_srcdir)/gui/icon.xpm \
  $(top_srcdir)/gui/ObjectEditor.fl $(top_srcdir)/gui/ViewEditor.fl \
  $(top_srcdir)/gui/ColorEditor.fl $(top_srcdir)/gui/IntersecEditor.fl \
  $(top_srcdir)/gui/OtherEditor.fl $(top_srcdir)/gui/WriteIni.fl \
  $(top_srcdir)/gui/icon1.ico $(top_srcdir)/gui/icon2.ico \
  $(top_srcdir)/gui/quat.rc $(top_srcdir)/gui/resources.h \
  $(top_srcdir)/gui/icon3.ico $(top_srcdir)/gui/icon.xbm 

