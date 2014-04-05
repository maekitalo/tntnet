pkglib_LTLIBRARIES = calcajax.la

calcajax_la_SOURCES = \
	calcajax.ecpp \
	calcajax_js.js \
	docalc.ecpp \
	jquery.js

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
calcajax_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calcajax_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = calcajax.cpp calcajax_js.cpp docalc.cpp tntnet.xml

SUFFIXES=.ecpp .js .cpp

.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
.js.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -m application/x-javascript -b -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=calcajax|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
