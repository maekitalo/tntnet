pkglib_LTLIBRARIES = calcmvc.la

calcmvc_la_SOURCES = \
	calcBean.h \
	calcBean.cpp \
	calcController.ecpp \
	calcModel.ecpp \
	calcView.ecpp \
	calcmvc.ecpp

calcmvc_la_LDFLAGS = -module
calcmvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = calcController.cpp calcModel.cpp calcView.cpp calcmvc.cpp \
	tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=calcmvc >$@
