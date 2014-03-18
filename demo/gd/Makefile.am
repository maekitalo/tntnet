pkglib_LTLIBRARIES = gd.la

gd_la_SOURCES = \
	gd++.cpp \
	gd.ecpp \
	imgstr.ecpp

include_HEADERS = \
	gd++.h

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
gd_la_LDFLAGS = -module @SHARED_LIB_FLAG@ -lgd
gd_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = gd.cpp tntnet.cpp tntnet.xml

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .jpg .cpp
.ecpp.cpp:
	$(ECPPC) -o $@ $<
.jpg.cpp:
	$(ECPPC) -b -m image/jpeg -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=gd|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
