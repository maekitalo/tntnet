noinst_PROGRAMS = calc

nodist_calc_SOURCES = \
	main.cpp \
	calc.cpp

EXTRA_DIST = \
	calc.ecpp

calc_LDADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.properties
CLEANFILES = calc.cpp tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties calc >$@
