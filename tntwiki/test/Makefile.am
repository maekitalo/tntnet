noinst_PROGRAMS = \
    tntwiki-test

AM_CPPFLAGS = -I$(top_srcdir)/include

tntwiki_test_SOURCES = \
	testmain.cpp \
    page-test.cpp \
    renderer-test.cpp

tntwiki_test_LDADD = $(top_builddir)/src/lib/libtntwiki.la -ltntdb -lcxxtools-unit
