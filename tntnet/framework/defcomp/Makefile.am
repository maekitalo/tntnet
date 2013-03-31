AM_CPPFLAGS = -I$(top_srcdir)/framework/common

pkglib_LTLIBRARIES = tntnet.la

tntnet_la_SOURCES = \
	error.cpp \
	mime.cpp \
	mimehandler.cpp \
	proxy.cpp \
	redirect.cpp \
	static.cpp \
	unzipcomp.cpp \
	mime.h \
	mimehandler.h \
	static.h

tntnet_la_LDFLAGS = -module -version-info @sonumber@ @SHARED_LIB_FLAG@ -lcxxtools-http
tntnet_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
