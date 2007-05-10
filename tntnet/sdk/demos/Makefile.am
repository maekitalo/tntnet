pkglib_LTLIBRARIES = alldemos.la

nodist_alldemos_la_SOURCES = \
	alldemos.cpp

EXTRA_DIST = \
	alldemos.ecpp \
	tntnet.conf \
	demos.txt

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
alldemos_la_LDFLAGS = -module $(PTHREAD_LIBS)
alldemos_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.properties
CLEANFILES = alldemos.cpp tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	cxxtools-config --properties tntnet >$@
