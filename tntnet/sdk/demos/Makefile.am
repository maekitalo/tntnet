pkglib_LTLIBRARIES = alldemos.la

nodist_alldemos_la_SOURCES = \
	alldemos.cpp

EXTRA_DIST = \
	alldemos.ecpp \
	tntnet.conf \
	demos.txt

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
alldemos_la_LDFLAGS = -module @SHARED_LIB_FLAG@
alldemos_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

CLEANFILES = alldemos.cpp

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
