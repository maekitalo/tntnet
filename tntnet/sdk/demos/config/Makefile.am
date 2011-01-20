pkglib_LTLIBRARIES = config.la

EXTRA_DIST = \
	config.ecpp

nodist_config_la_SOURCES = \
	config.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
config_la_LDFLAGS = -module @SHARED_LIB_FLAG@
config_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = config.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties config >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=config; echo dburl "some_config_value" ; echo CompPath .libs ) >$@
