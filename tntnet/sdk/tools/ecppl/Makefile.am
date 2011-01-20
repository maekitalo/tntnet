bin_PROGRAMS = ecppl

ecppl_SOURCES = \
	ecppl.cpp \
	ecpplang.cpp \
	ecpplang.h

ecppl_LDADD = $(top_builddir)/sdk/tools/common/libtntnet_sdk.la -lcxxtools $(PTHREAD_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/framework/common -I$(top_srcdir)/sdk/tools/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
