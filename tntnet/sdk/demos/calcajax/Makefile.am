pkglib_LTLIBRARIES = calcajax.la

nodist_calcajax_la_SOURCES = \
	calcajax.cpp \
	calcajax_js.js \
	docalc.cpp

EXTRA_DIST = \
	calcajax.ecpp \
	calcajax_js.js \
	docalc.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calcajax_la_LDFLAGS = -module $(PTHREAD_LIBS)
calcajax_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calcajax.cpp calcajax_js.cpp docalc.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .js .cpp

.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=calcajax >$@
