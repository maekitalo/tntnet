pkglib_LTLIBRARIES = strings.la

nodist_strings_la_SOURCES = \
	strings.cpp

EXTRA_DIST = \
	strings.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
strings_la_LDFLAGS = -module @SHARED_LIB_FLAG@
strings_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = strings.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=strings|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
