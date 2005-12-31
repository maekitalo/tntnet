pkglib_LTLIBRARIES = libtntnet-cgi.la

libtntnet_cgi_la_SOURCES = \
	cgi.cpp

libtntnet_cgi_la_LDFLAGS = -version-info @sonumber@
libtntnet_cgi_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_PROGRAMS = getenv

getenv_SOURCES = getenv.cpp
getenv_LDADD = $(top_builddir)/framework/cgi/libtntnet-cgi.la

getenv.cpp: getenv.ecpp
	$(top_builddir)/sdk/tools/ecppc/ecppc getenv.ecpp
