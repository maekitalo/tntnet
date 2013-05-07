pkglib_LTLIBRARIES = comp.la extlib.la

nodist_comp_la_SOURCES = \
	comp.cpp \
    subcomp.cpp

nodist_extlib_la_SOURCES = \
	extcomp.cpp

EXTRA_DIST = \
	comp.ecpp \
    subcomp.ecpp \
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
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=comp|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
