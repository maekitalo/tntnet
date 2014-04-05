pkglib_LTLIBRARIES = strings.la

strings_la_SOURCES = \
	strings.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
strings_la_LDFLAGS = -module @SHARED_LIB_FLAG@
strings_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = strings.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=strings|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
