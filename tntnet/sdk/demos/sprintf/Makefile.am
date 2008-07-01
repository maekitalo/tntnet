pkglib_LTLIBRARIES = sprintf.la

nodist_sprintf_la_SOURCES = \
	sprintf.cpp \
	subcomp.cpp

EXTRA_DIST = \
	sprintf.ecpp \
	subcomp.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
sprintf_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@ @MODULE_LDFLAGS@
sprintf_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = sprintf.cpp subcomp.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( /bin/sh $(top_builddir)/tntnet-config --config=sprintf; echo CompPath .libs ) >$@
