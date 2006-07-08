pkglib_LTLIBRARIES = comp.la extlib.la

comp_la_SOURCES = \
	comp.ecpp \
    subcomp.ecpp \
    nscomp.ecpp

extlib_la_SOURCES = \
	extcomp.ecpp

comp_la_LDFLAGS = -module
comp_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
extlib_la_LDFLAGS = -module
extlib_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

nscomp.cpp: ECPPFLAGS=-N mynamespace -o nscomp.cpp

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = comp.cpp subcomp.cpp extcomp.cpp nscomp.cpp \
    tntnet.conf tntnet.properties

EXTRA_DIST = tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=comp >$@
