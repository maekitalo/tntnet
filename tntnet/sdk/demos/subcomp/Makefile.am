pkglib_LTLIBRARIES = lib.la extlib.la

lib_la_SOURCES = \
	subcomp.ecpp \
	extcomp.ecpp

extlib_la_SOURCES = \
	extcomp.ecpp

lib_la_LDFLAGS = -module
lib_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la
extlib_la_LDFLAGS = -module
extlib_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = subcomp.h subcomp.cpp extcomp.h extcomp.cpp tntnet.conf tntnet.properties

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

EXTRA_DIST = tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	$(top_builddir)/tntnet-config --config=subcomp >$@
