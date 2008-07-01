pkglib_LTLIBRARIES = comp.la extlib.la

nodist_comp_la_SOURCES = \
	comp.cpp \
    subcomp.cpp

nodist_extlib_la_SOURCES = \
	extcomp.cpp

EXTRA_DIST = \
	comp.ecpp \
    subcomp.ecpp \
	extcomp.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
comp_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@ @MODULE_LDFLAGS@
comp_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
extlib_la_LDFLAGS = -module $(PTHREAD_LIBS)
extlib_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = comp.cpp subcomp.cpp extcomp.cpp \
    tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( /bin/sh $(top_builddir)/tntnet-config --config=comp; echo CompPath .libs ) >$@
