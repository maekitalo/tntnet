lib_LTLIBRARIES = libecpp.la

libecpp_la_SOURCES = \
	ecpp.cpp

nobase_include_HEADERS = \
	tnt/convert.h \
	tnt/ecpp.h

libecpp_la_LDFLAGS = -L$(top_builddir)/framework/common -ltntnet -version-info @sonumber@
libecpp_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
AM_CPPFLAGS = @CXXTOOLS_CXXFLAGS@
