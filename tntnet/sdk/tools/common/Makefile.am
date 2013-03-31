AM_CPPFLAGS = -I$(top_srcdir)/framework/common

lib_LTLIBRARIES = libtntnet_sdk.la

libtntnet_sdk_la_SOURCES = \
	datachunks_creator.cpp \
	filename.cpp \
	parsehandler.cpp \
	parser.cpp \
	stringescaper.cpp \
	tnt/datachunks_creator.h \
	tnt/ecpp/parsehandler.h \
	tnt/ecpp/parser.h \
	tnt/ecpp/scopetypes.h \
	tnt/filename.h \
	tnt/stringescaper.h

libtntnet_sdk_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
libtntnet_sdk_la_LIBADD = -lcxxtools
