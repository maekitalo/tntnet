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
calcajax_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calcajax_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = calcajax.cpp calcajax_js.cpp docalc.cpp tntnet.xml

SUFFIXES=.ecpp .js .cpp

.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -m application/x-javascript -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=calcajax|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
