pkglib_LTLIBRARIES = savepoint.la

nodist_savepoint_la_SOURCES = \
	savepoint.cpp

EXTRA_DIST = \
	savepoint.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
savepoint_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
savepoint_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = savepoint.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties savepoint >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=savepoint; echo CompPath .libs ) >$@
