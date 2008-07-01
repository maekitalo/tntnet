pkglib_LTLIBRARIES = config.la

EXTRA_DIST = \
	config.ecpp

nodist_config_la_SOURCES = \
	config.cpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
config_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@ @MODULE_LDFLAGS@
config_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = config.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( /bin/sh $(top_builddir)/tntnet-config --config=config; echo dburl "some_config_value" ; echo CompPath .libs ) >$@
