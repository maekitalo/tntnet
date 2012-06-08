noinst_PROGRAMS = calc

calc_SOURCES = \
	main.cpp

nodist_calc_SOURCES = \
	calc.cpp

EXTRA_DIST = \
	calc.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calc_LDADD = $(top_builddir)/framework/common/libtntnet.la

CLEANFILES = calc.cpp

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
