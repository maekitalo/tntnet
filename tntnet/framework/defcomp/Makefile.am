pkglib_LTLIBRARIES = tntnet.la

tntnet_la_SOURCES = \
	error.cpp \
	mime.cpp \
	mimehandler.cpp \
	redirect.cpp \
	static.cpp \
	unzipcomp.cpp \
	mime.h \
	mimehandler.h \
	static.h

tntnet_la_LDFLAGS = -module -version-info @sonumber@
tntnet_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
