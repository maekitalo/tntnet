pkglib_LTLIBRARIES = cookie.la

cookie_la_SOURCES = \
	cookie.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
cookie_la_LDFLAGS = -module @SHARED_LIB_FLAG@
cookie_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = cookie.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=cookie|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
