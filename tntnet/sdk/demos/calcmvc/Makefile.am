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

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calcmvc_la_LDFLAGS = -module $(PTHREAD_LIBS)
calcmvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calcController.cpp calcModel.cpp calcView.cpp calcmvc.cpp \
	tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=calcmvc >$@
