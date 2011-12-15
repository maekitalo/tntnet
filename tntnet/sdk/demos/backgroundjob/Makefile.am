pkglib_LTLIBRARIES = backgroundjob.la

backgroundjob_la_SOURCES = \
	ajax.js \
	backgroundjob.ecpp \
	status.ecpp

EXTRA_DIST = \
	global.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
backgroundjob_la_LDFLAGS = -module @SHARED_LIB_FLAG@
backgroundjob_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = ajax.cpp backgroundjob.cpp status.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -b -m application/x-javascript -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties backgroundjob >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=backgroundjob; echo CompPath .libs ) >$@
