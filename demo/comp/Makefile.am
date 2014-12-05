pkglib_LTLIBRARIES = comp.la extlib.la

comp_la_SOURCES = \
	comp.ecpp \
    subcomp.ecpp

extlib_la_SOURCES = \
	extcomp.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
comp_la_LDFLAGS = -module @SHARED_LIB_FLAG@
comp_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
extlib_la_LDFLAGS = -module
extlib_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = comp.cpp subcomp.cpp extcomp.cpp \
    tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=comp|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
