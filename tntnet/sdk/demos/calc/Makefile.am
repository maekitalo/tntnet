pkglib_LTLIBRARIES = calc.la

nodist_calc_la_SOURCES = \
	calc.cpp

EXTRA_DIST = \
	calc.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calc.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties calc >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=calc; echo CompPath .libs ) >$@
