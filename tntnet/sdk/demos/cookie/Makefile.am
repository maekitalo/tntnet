pkglib_LTLIBRARIES = cookie.la

nodist_cookie_la_SOURCES = \
	cookie.cpp

EXTRA_DIST = \
	cookie.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
cookie_la_LDFLAGS = -module @SHARED_LIB_FLAG@
cookie_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = cookie.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties cookie >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=cookie; echo CompPath .libs ) >$@
