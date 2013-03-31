pkglib_LTLIBRARIES = session.la

nodist_session_la_SOURCES = \
	session.cpp \
	appsession.cpp

EXTRA_DIST = \
	session.ecpp \
	appsession.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
session_la_LDFLAGS = -module @SHARED_LIB_FLAG@
session_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = session.cpp appsession.cpp \
	tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=session|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
