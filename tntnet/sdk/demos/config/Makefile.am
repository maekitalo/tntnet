pkglib_LTLIBRARIES = config.la

config_la_SOURCES = \
	config.ecpp

config_la_LDFLAGS = -module
config_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = config.h config.cpp tntnet.conf tntnet.properties

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( $(top_builddir)/tntnet-config --config=config; echo dburl "some_config_value" ) >$@
