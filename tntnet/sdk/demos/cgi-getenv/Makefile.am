noinst_PROGRAMS = getenv

nodist_getenv_SOURCES = \
	getenv.cpp

EXTRA_DIST = \
	getenv.ecpp

getenv_LDADD = $(top_builddir)/framework/cgi/libtntnet_cgi.la

CLEANFILES = getenv.cpp

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
