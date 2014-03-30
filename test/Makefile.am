AM_CPPFLAGS = \
    -I$(top_srcdir)/framework/common \
    -I$(top_srcdir)/sdk/tools/common

noinst_PROGRAMS = tntnet-test

tntnet_test_SOURCES = \
    ecpptest.cpp \
    messageheadertest.cpp \
    qparamtest.cpp \
    strutest.cpp \
    testmain.cpp

tntnet_test_LDADD = \
    $(top_builddir)/framework/common/libtntnet.la \
    $(top_builddir)/sdk/tools/common/libtntnet_sdk.la \
    -lcxxtools \
    -lcxxtools-unit
