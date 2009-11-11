pkglib_LTLIBRARIES = strings.la

nodist_strings_la_SOURCES = \
	strings.cpp

EXTRA_DIST = \
	strings.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
strings_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
strings_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = strings.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties strings >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=strings; echo CompPath .libs ) >$@
