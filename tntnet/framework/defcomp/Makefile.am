pkglib_LTLIBRARIES = tntnet.la

tntnet_la_SOURCES = \
	error.cpp \
	mime.cpp \
	redirect.cpp \
	static.cpp \
	fstatic.cpp \
	unzipcomp.cpp \
	static.h \
	fstatic.h

tntnet_la_LDFLAGS = -module -version-info @sonumber@
tntnet_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
