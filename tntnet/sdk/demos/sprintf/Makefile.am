pkglib_LTLIBRARIES = sprintf.la

sprintf_la_SOURCES = \
	sprintf.ecpp \
	subcomp.ecpp

sprintf_la_LDFLAGS = -module
sprintf_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = sprintf.h sprintf.cpp tntnet.conf tntnet.properties

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	$(top_builddir)/tntnet-config --config=sprintf >$@
