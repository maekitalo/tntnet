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
calcmvc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calcmvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = calcController.cpp calcModel.cpp calcView.cpp calcmvc.cpp \
	tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -I $(srcdir) $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=calcmvc|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
