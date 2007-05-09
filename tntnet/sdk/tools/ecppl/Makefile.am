bin_PROGRAMS = ecppl

ecppl_SOURCES = \
	ecppl.cpp \
	ecpplang.cpp \
	ecpplang.h

ecppl_LDADD = $(top_builddir)/sdk/tools/common/libtntnet_sdk.la

AM_CPPFLAGS = -I$(top_builddir)/sdk/tools/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = -lcxxtools $(PTHREAD_LIBS)
