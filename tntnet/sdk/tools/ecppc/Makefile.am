bin_PROGRAMS = ecppc

INCLUDES = -I../common -I$(top_builddir)/framework/common

ecppc_SOURCES = \
	ecppGenerator.cpp \
	ecppc.cpp

ecppc_LDADD = $(top_builddir)/framework/common/libtntnet.la $(top_builddir)/sdk/tools/common/libtntnet_sdk.la
