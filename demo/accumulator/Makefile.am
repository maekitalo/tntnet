pkglib_LTLIBRARIES = accumulator.la

accumulator_la_SOURCES = \
	accumulator.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
accumulator_la_LDFLAGS = -module @SHARED_LIB_FLAG@
accumulator_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = accumulator.cpp tntnet.xml

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .cpp

.ecpp.cpp: $(ECPPC)
	$(AM_V_GEN)$(ECPPC) -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/tntnet-config --config=accumulator|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
