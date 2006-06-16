pkglib_LTLIBRARIES = calcajax.la

calcajax_la_SOURCES = \
	calcajax.ecpp \
	calcajax_js.js \
	calc.ecpp

calcajax_la_LDFLAGS = -module
calcajax_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calcajax.cpp calc.cpp tntnet.conf tntnet.properties

SUFFIXES=.js
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=calcajax >$@
