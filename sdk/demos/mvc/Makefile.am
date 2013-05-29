pkglib_LTLIBRARIES = mvc.la
AUTOMAKE_OPTIONS = subdir-objects

nodist_mvc_la_SOURCES = \
	controller/counter.cpp \
	view/counter.ecpp

EXTRA_DIST = \
	mvc.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common -Imodel
mvc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
mvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -p $<
