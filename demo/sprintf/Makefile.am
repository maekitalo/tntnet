pkglib_LTLIBRARIES = sprintf.la

sprintf_la_SOURCES = \
	sprintf.ecpp \
	subcomp.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
sprintf_la_LDFLAGS = -module @SHARED_LIB_FLAG@
sprintf_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = sprintf.cpp subcomp.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=sprintf|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
