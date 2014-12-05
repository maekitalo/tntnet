pkglib_LTLIBRARIES = calcmvc.la
AUTOMAKE_OPTIONS = subdir-objects

calcmvc_la_SOURCES = \
	calcmvc.ecpp \
	view/accumulator.ecpp \
	view/calc.ecpp \
	controller/accumulator.cpp \
	controller/calc.cpp

nobase_include_HEADERS = \
	model/accumulator.h \
	model/calc.h

AM_CPPFLAGS = -I$(top_srcdir)/framework/common -I.
calcmvc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calcmvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

dist_noinst_DATA = tntnet.xml
CLEANFILES = calcmvc.cpp view/accumulator.cpp view/calc.cpp

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -p -I $(srcdir) $<
