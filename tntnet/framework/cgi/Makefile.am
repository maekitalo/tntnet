pkglib_LTLIBRARIES = libtntnet-cgi.la

libtntnet_cgi_la_SOURCES = \
	cgi.cpp \
	tnt/cgi.h

libtntnet_cgi_la_LDFLAGS = -version-info @sonumber@
libtntnet_cgi_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_PROGRAMS = getenv

getenv_SOURCES = getenv.ecpp
getenv_LDADD = $(top_builddir)/framework/cgi/libtntnet-cgi.la

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
