lib_LTLIBRARIES = libtntnet_cgi.la

libtntnet_cgi_la_SOURCES = \
	cgi.cpp \
	tnt/cgi.h

libtntnet_cgi_la_CXXFLAGS = $(PTHREAD_CFLAGS)
libtntnet_cgi_la_LDFLAGS = -version-info @sonumber@ $(PTHREAD_LIBS)
libtntnet_cgi_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
