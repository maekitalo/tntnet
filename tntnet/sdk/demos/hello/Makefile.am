.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

pkglib_LTLIBRARIES = hello.la

hello_la_SOURCES = \
	hello.ecpp \
	hello

hello_la_LDFLAGS = -module
hello_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

hello:
	echo "#!/bin/sh" >$@
	echo "${top_builddir}/framework/runtime/tntnet -c tntnet.conf" >>$@

EXTRA_DIST = tntnet.conf tntnet.properties
