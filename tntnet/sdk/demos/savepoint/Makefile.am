pkglib_LTLIBRARIES = savepoint.la

nodist_savepoint_la_SOURCES = \
	savepoint.cpp

EXTRA_DIST = \
	savepoint.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
savepoint_la_LDFLAGS = -module @SHARED_LIB_FLAG@
savepoint_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = savepoint.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=savepoint|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
