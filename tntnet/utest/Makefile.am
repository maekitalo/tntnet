AM_CPPFLAGS = -I$(top_srcdir)/framework/common

noinst_PROGRAMS = utest

utest_SOURCES = \
    messageheadertest.cpp \
    testmain.cpp \
    strutest.cpp

utest_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools-unit
