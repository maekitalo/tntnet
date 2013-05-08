pkglib_LTLIBRARIES = config.la

EXTRA_DIST = \
	config.ecpp

nodist_config_la_SOURCES = \
	config.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
config_la_LDFLAGS = -module @SHARED_LIB_FLAG@
config_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = config.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=config|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>\n\n  <dburl>some_config_value</dburl>\n  <values>\n    <value>5</value>\n    <value>9</value>\n  </values>"} {print}' >$@
