pkglib_LTLIBRARIES = sprintf.la

nodist_sprintf_la_SOURCES = \
	sprintf.cpp \
	subcomp.cpp

EXTRA_DIST = \
	sprintf.ecpp \
	subcomp.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
sprintf_la_LDFLAGS = -module @SHARED_LIB_FLAG@
sprintf_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = sprintf.cpp subcomp.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=sprintf|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
