lib_LTLIBRARIES = libecpp.la

libecpp_la_SOURCES = \
	ecpp.cpp

nobase_include_HEADERS = \
	tnt/ecpp.h

libecpp_la_LDFLAGS = -L../common -ltntnet -version-info @sonumber@
libecpp_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la
#libecpp_la_CXXFLAGS = -I../common
