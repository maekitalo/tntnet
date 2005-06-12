pkglib_LTLIBRARIES = hello.la

hello_la_SOURCES = \
	hello.ecpp \
	tntnet.jpg

hello_la_LDFLAGS = -module
hello_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = hello.cpp tntnet.cpp tntnet.conf tntnet.properties

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp
ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .jpg
.ecpp.cpp:
	$(ECPPC) $<
.jpg.cpp:
	$(ECPPC) -b -m image/jpg $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=hello >$@
