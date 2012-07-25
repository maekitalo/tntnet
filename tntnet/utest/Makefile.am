AM_CPPFLAGS = -I$(top_srcdir)/framework/common -I$(top_srcdir)/sdk/tools/common

noinst_PROGRAMS = utest

utest_SOURCES = \
    ecpptest.cpp \
    messageheadertest.cpp \
    strutest.cpp \
    testmain.cpp

utest_LDADD = $(top_builddir)/framework/common/libtntnet.la $(top_builddir)/sdk/tools/common/libtntnet_sdk.la -lcxxtools -lcxxtools-unit
