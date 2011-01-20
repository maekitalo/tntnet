pkglib_LTLIBRARIES = calcajax.la

nodist_calcajax_la_SOURCES = \
	calcajax.cpp \
	calcajax_js.cpp \
	docalc.cpp

EXTRA_DIST = \
	calcajax.ecpp \
	calcajax_js.js \
	docalc.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calcajax_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calcajax_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calcajax.cpp calcajax_js.cpp docalc.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .js .cpp

.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -m application/x-javascript -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties calcajax >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=calcajax; echo CompPath .libs ) >$@
