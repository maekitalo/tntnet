pkglib_LTLIBRARIES = calcmvc.la

nodist_calcmvc_la_SOURCES = \
	calcController.cpp \
	calcModel.cpp \
	calcView.cpp \
	calcmvc.cpp

calcmvc_la_SOURCES = \
	calcBean.h \
	calcBean.cpp

EXTRA_DIST = \
	calcController.ecpp \
	calcModel.ecpp \
	calcView.ecpp \
	calcmvc.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calcmvc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calcmvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calcController.cpp calcModel.cpp calcView.cpp calcmvc.cpp \
	tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -I $(srcdir) $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties calcmvc >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=calcmvc; echo CompPath .libs ) >$@
