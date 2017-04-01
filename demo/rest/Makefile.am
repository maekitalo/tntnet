noinst_PROGRAMS = rest putdata getdata deldata
noinst_DATA = tntnet.properties
dist_noinst_DATA = README

rest_SOURCES = \
	getvalue.ecpp \
	putvalue.ecpp \
	delvalue.ecpp \
	rest.cpp

putdata_SOURCES = \
	putdata.cpp

getdata_SOURCES = \
	getdata.cpp

deldata_SOURCES = \
	deldata.cpp

EXTRA_DIST = \
	global.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
rest_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools
putdata_LDADD = -lcxxtools -lcxxtools-http
getdata_LDADD = -lcxxtools -lcxxtools-http
deldata_LDADD = -lcxxtools -lcxxtools-http

CLEANFILES = tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.properties:
	$(AM_V_GEN)echo "rootlogger=INFO" > $@
