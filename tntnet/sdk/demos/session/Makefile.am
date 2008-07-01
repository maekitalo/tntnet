pkglib_LTLIBRARIES = session.la

nodist_session_la_SOURCES = \
	session.cpp \
	appsession.cpp

EXTRA_DIST = \
	session.ecpp \
	appsession.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
session_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@ @MODULE_LDFLAGS@
session_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = session.cpp appsession.cpp \
	tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( /bin/sh $(top_builddir)/tntnet-config --config=session; echo CompPath .libs ) >$@
