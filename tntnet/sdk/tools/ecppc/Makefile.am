bin_PROGRAMS = ecppc

ecppc_SOURCES = \
	ecppc.cpp \
	ecppGenerator.cpp \
	ecppGenerator.h

ecppc_LDADD = $(top_builddir)/framework/common/libtntnet.la $(top_builddir)/sdk/tools/common/libtntnet_sdk.la

AM_CPPFLAGS = -I$(top_builddir)/sdk/tools/common @CXXTOOLS_CXXFLAGS@
