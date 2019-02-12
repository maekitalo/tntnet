bin_PROGRAMS = calcajax

calcajax_SOURCES = \
	calcajax.ecpp \
	calcajax_js.js \
	docalc.ecpp \
	jquery.js \
	main.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
calcajax_LDFLAGS =
calcajax_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
CLEANFILES = calcajax.cpp calcajax_js.cpp docalc.cpp

SUFFIXES=.ecpp .js .cpp

.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
.js.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -m application/x-javascript -b -o $@ $<
