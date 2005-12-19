pkglib_LTLIBRARIES = tntnet.la

tntnet_la_SOURCES = \
	error.cpp \
	mime.cpp \
	redirect.cpp \
	static.cpp \
	fstatic.cpp \
	unzipcomp/unzip_pp.cpp \
	unzipcomp/unzipcomp.cpp \
	unzipcomp/unzip.c \
	unzipcomp/ioapi.c \
	static.h \
	fstatic.h \
	unzipcomp/crypt.h \
	unzipcomp/ioapi.h \
	unzipcomp/unzip.h \
	unzipcomp/unzip_pp.h

tntnet_la_LDFLAGS = -module
tntnet_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
