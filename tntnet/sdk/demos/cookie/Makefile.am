pkglib_LTLIBRARIES = cookie.la

cookie_la_SOURCES = \
	cookie.ecpp

cookie_la_LDFLAGS = -module
cookie_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = cookie.h cookie.cpp tntnet.conf tntnet.properties

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -d $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	$(top_builddir)/tntnet-config --config >$@
