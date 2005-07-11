pkglib_LTLIBRARIES = cookie.la

cookie_la_SOURCES = \
	cookie.ecpp

cookie_la_LDFLAGS = -module
cookie_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = cookie.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=cookie >$@
