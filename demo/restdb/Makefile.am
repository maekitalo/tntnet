AUTOMAKE_OPTIONS = subdir-objects
noinst_PROGRAMS = restdb
noinst_DATA = tntnet.properties
noinst_HEADERS = friend.h person.h

ecppSources = \
	friend/insert.ecpp \
	friend/delete.ecpp \
	friend/get.ecpp \
	person/all.ecpp \
	person/delete.ecpp \
	person/get.ecpp \
	person/getbyname.ecpp \
	person/insert.ecpp \
	person/search.ecpp \
	person/update.ecpp

restdb_SOURCES = \
	$(ecppSources) \
	main.cpp

EXTRA_DIST = \
	global.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
restdb_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools -ltntdb

CLEANFILES = delperson.cpp getperson.cpp postperson.cpp putperson.cpp tntnet.properties $(ecppSources:.ecpp=.cpp) $(ecppSources:.ecpp=.deps)

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -p $<
.ecpp.deps:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -M $(ECPPFLAGS) $< | $(SED) '1s|\(.*\).cpp:|\1.cpp $@:|' > $@

tntnet.properties:
	echo "rootlogger=INFO" > $@

-include $(ecppSources:.ecpp=.deps)
