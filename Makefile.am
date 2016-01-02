SUBDIRS=kernel gui doc
dist_data_DATA=$(top_srcdir)/Makefiles \
   $(top_srcdir)/examples $(top_srcdir)/quat.iss
#EXTRA_DIST=$(top_srcdir)/Makefiles \
#  $(top_srcdir)/examples $(top_srcdir)/quat.iss

dist-hook:
        # delete the CVS directories from distribution
	find $(distdir) -name "CVS" >$(distdir)/rm.tmp
	if test -s $(distdir)/rm.tmp; then rm -r `cat $(distdir)/rm.tmp`; fi;
	rm $(distdir)/rm.tmp
