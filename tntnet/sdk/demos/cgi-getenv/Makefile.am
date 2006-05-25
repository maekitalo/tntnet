noinst_PROGRAMS = getenv

getenv_SOURCES = \
	getenv.ecpp

getenv_LDADD = $(top_builddir)/framework/cgi/libtntnet_cgi.la

CLEANFILES = getenv.cpp

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
