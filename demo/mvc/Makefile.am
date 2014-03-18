pkglib_LTLIBRARIES = mvc.la
AUTOMAKE_OPTIONS = subdir-objects

mvc_la_SOURCES = \
	controller/counter.cpp \
	main.ecpp \
	view/counter.ecpp

nobase_include_HEADERS = \
	model/counter.h

AM_CPPFLAGS = -I$(top_srcdir)/framework/common -I$(srcdir)/model
mvc_la_LDFLAGS = -module @SHARED_LIB_FLAG@
mvc_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -p $<
