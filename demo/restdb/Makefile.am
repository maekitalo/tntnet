AUTOMAKE_OPTIONS = subdir-objects
noinst_PROGRAMS = restdb
noinst_DATA = tntnet.properties

restdb_SOURCES = \
	person/all.ecpp \
	person/delete.ecpp \
	person/get.ecpp \
	person/getbyname.ecpp \
	person/insert.ecpp \
	person/update.ecpp \
	main.cpp

EXTRA_DIST = \
	global.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
restdb_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools -ltntdb

CLEANFILES = delperson.cpp getperson.cpp postperson.cpp putperson.cpp tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -p $<

tntnet.properties:
	echo "rootlogger=INFO" > $@
