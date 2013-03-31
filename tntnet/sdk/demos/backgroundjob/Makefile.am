pkglib_LTLIBRARIES = backgroundjob.la

backgroundjob_la_SOURCES = \
	ajax.js \
	backgroundjob.ecpp \
	status.ecpp

EXTRA_DIST = \
	global.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
backgroundjob_la_LDFLAGS = -module @SHARED_LIB_FLAG@
backgroundjob_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = ajax.cpp backgroundjob.cpp status.cpp tntnet.xml

SUFFIXES=.ecpp .js .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -I$(srcdir) $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -b -m application/x-javascript -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=backgroundjob|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
