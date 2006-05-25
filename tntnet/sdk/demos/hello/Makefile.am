pkglib_LTLIBRARIES = hello.la

hello_la_SOURCES = \
	hello.ecpp \
	tntnet.jpg

hello_la_LDFLAGS = -module
hello_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = hello.cpp tntnet.cpp tntnet.conf tntnet.properties

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .jpg
.ecpp.cpp:
	$(ECPPC) $<
.jpg.cpp:
	$(ECPPC) -b $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=hello >$@
