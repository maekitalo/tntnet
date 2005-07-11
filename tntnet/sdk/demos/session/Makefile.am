pkglib_LTLIBRARIES = session.la

session_la_SOURCES = \
	session.ecpp appsession.ecpp

session_la_LDFLAGS = -module
session_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = session.cpp appsession.cpp \
	tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=session >$@
