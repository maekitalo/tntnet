bin_PROGRAMS = newsdb

newsdb_SOURCES = \
	index.ecpp \
	main.cpp \
	news.ecpp \
	newscss.css

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
newsdb_LDFLAGS = -ltntdb
newsdb_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
dist_noinst_DATA = global.ecpp newsdb.sql README
CLEANFILES = index.cpp news.cpp newscss.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc
ECPPFLAGS=-I $(top_srcdir)/demo/newsdb

SUFFIXES=.ecpp .css .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) $(ECPPFLAGS) -o $@ $<
