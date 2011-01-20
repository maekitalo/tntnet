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
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
comp_la_LDFLAGS = -module @SHARED_LIB_FLAG@
comp_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)
extlib_la_LDFLAGS = -module
extlib_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = comp.cpp subcomp.cpp extcomp.cpp \
    tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties comp >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=comp; echo CompPath .libs ) >$@
