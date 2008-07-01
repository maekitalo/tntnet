pkglib_LTLIBRARIES = calc.la

nodist_calc_la_SOURCES = \
	calc.cpp

EXTRA_DIST = \
	calc.ecpp

if INSTALL_DEMOS
    doc_DATA = $(EXTRA_DIST)
endif

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calc_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@ @MODULE_LDFLAGS@
calc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calc.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( /bin/sh $(top_builddir)/tntnet-config --config=calc; echo CompPath .libs ) >$@
