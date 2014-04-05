pkglib_LTLIBRARIES = newsdb.la

newsdb_la_SOURCES = \
	index.ecpp \
	news.ecpp \
	newscss.css

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
newsdb_la_LDFLAGS = -module @SHARED_LIB_FLAG@ -ltntdb
newsdb_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
dist_noinst_DATA = global.ecpp newsdb.sql README
CLEANFILES = newsdb.cpp tntnet.cpp tntnet.xml

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc
ECPPFLAGS=-I $(top_srcdir)/demo/newsdb

SUFFIXES=.ecpp .css .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) $(ECPPFLAGS) -o $@ $<
.css.cpp:
	$(AM_V_GEN)$(ECPPC) -b -m text/css -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=newsdb|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
