pkglib_LTLIBRARIES = lib.la extlib.la

lib_la_SOURCES = \
	comp.ecpp \
    subcomp.ecpp \
    nscomp.ecpp

extlib_la_SOURCES = \
	extcomp.ecpp

lib_la_LDFLAGS = -module
lib_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
extlib_la_LDFLAGS = -module
extlib_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

nscomp.cpp: ECPPFLAGS=-N mynamespace -o nscomp.cpp

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = subcomp.cpp extcomp.cpp tntnet.conf tntnet.properties

EXTRA_DIST = tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=lib >$@
