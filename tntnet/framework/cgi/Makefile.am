AM_CPPFLAGS = -I$(top_srcdir)/framework/common

lib_LTLIBRARIES = libtntnet_cgi.la

libtntnet_cgi_la_SOURCES = \
	cgi.cpp \
	tnt/cgi.h

libtntnet_cgi_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libtntnet_cgi_la_LDFLAGS = -version-info @sonumber@
libtntnet_cgi_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools $(PTHREAD_LIBS)
