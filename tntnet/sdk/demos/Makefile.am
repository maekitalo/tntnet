pkglib_LTLIBRARIES = alldemos.la

alldemos_la_SOURCES = \
	alldemos.ecpp

alldemos_la_LDFLAGS = -module
alldemos_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

EXTRA_DIST = tntnet.conf
noinst_DATA = tntnet.properties
CLEANFILES = alldemos.cpp tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
