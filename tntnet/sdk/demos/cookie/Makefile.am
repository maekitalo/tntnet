pkglib_LTLIBRARIES = cookie.la

nodist_cookie_la_SOURCES = \
	cookie.ecpp

EXTRA_DIST = \
	cookie.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
cookie_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
cookie_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = cookie.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=cookie >$@
