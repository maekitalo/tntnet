pkglib_LTLIBRARIES = sprintf.la

sprintf_la_SOURCES = \
	sprintf.ecpp \
	subcomp.ecpp

sprintf_la_LDFLAGS = -module
sprintf_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = sprintf.cpp subcomp.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=sprintf >$@
