pkglib_LTLIBRARIES = config.la

EXTRA_DIST = \
	config.ecpp

nodist_config_la_SOURCES = \
	config.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
config_la_LDFLAGS = -module @SHARED_LIB_FLAG@
config_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.xml tntnet.properties
CLEANFILES = config.cpp tntnet.xml tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties config >$@
tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=config|awk '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>\n\n  <appconfig>\n    <dburl>some_config_value</dburl>\n  </appconfig>"} {print}' >$@
