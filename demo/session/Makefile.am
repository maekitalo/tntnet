pkglib_LTLIBRARIES = session.la

session_la_SOURCES = \
	session.ecpp \
	renewsession.ecpp \
	appsession.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
session_la_LDFLAGS = -module @SHARED_LIB_FLAG@
session_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = session.cpp appsession.cpp renewsession.cpp \
	tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-project --config=session|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
