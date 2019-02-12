bin_PROGRAMS = calcmvc
AUTOMAKE_OPTIONS = subdir-objects

calcmvc_SOURCES = \
	calcmvc.ecpp \
	controller/accumulator.cpp \
	controller/calc.cpp \
	main.cpp \
	view/accumulator.ecpp \
	view/calc.ecpp

noinst_HEADERS = \
	model/accumulator.h \
	model/calc.h

AM_CPPFLAGS = -I$(top_srcdir)/framework/common -I.
calcmvc_LDFLAGS = -module @SHARED_LIB_FLAG@
calcmvc_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

dist_noinst_DATA = tntnet.xml
CLEANFILES = calcmvc.cpp view/accumulator.cpp view/calc.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -o $@ -p -I $(srcdir) $<
