bin_PROGRAMS = ecppc

INCLUDES = -I../common -I$(top_builddir)/framework/common

ecppc_SOURCES = \
	ecppc.cpp \
	ecppGenerator.cpp \
	ecppGenerator.h

ecppc_LDADD = $(top_builddir)/framework/common/libtntnet.la $(top_builddir)/sdk/tools/common/libtntnet_sdk.la
