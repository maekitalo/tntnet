pkglib_LTLIBRARIES = hello.la

hello_la_SOURCES = \
	hello.ecpp

hello_la_LDFLAGS = -module
hello_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = hello.h hello.cpp tntnet.conf tntnet.properties

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	$(top_builddir)/tntnet-config --config >$@
