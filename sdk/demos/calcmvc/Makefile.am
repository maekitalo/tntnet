pkglib_LTLIBRARIES = calcmvc.la
AUTOMAKE_OPTIONS = subdir-objects

calcmvc_la_SOURCES = \
	calcmvc.ecpp \
	view/accumulator.ecpp \
	view/calc.ecpp \
	controller/accumulator.cpp \
	controller/calc.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common -I.
calcmvc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calcmvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = calcmvc.cpp view/calc.cpp controller/calc.cpp

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -p -I $(srcdir) $<
