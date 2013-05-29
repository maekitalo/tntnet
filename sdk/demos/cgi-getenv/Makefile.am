noinst_PROGRAMS = getenv

nodist_getenv_SOURCES = \
	getenv.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
getenv_LDADD = $(top_builddir)/framework/cgi/libtntnet_cgi.la

CLEANFILES = getenv.cpp

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
