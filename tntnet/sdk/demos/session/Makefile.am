pkglib_LTLIBRARIES = session.la

session_la_SOURCES = \
	session.ecpp

session_la_LDFLAGS = -module
session_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = session.h session.cpp tntnet.conf tntnet.properties

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

session.cpp: $(top_builddir)/sdk/tools/ecppc/ecppc

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	$(top_builddir)/tntnet-config --config=session >$@
