pkglib_LTLIBRARIES = calc.la

nodist_calc_la_SOURCES = \
	calc.cpp

EXTRA_DIST = \
	calc.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
calc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = calc.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=calc|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
