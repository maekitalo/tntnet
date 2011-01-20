pkglib_LTLIBRARIES = sprintf.la

nodist_sprintf_la_SOURCES = \
	sprintf.cpp \
	subcomp.cpp

EXTRA_DIST = \
	sprintf.ecpp \
	subcomp.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
sprintf_la_LDFLAGS = -module @SHARED_LIB_FLAG@
sprintf_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = sprintf.cpp subcomp.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties sprintf >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=sprintf; echo CompPath .libs ) >$@
