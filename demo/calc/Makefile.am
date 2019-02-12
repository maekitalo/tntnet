bin_PROGRAMS = calc

calc_SOURCES = \
	calc.ecpp \
	main.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
calc_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
dist_noinst_DATA = README
CLEANFILES = calc.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .cpp

.ecpp.cpp: $(ECPPC)
	$(AM_V_GEN)$(ECPPC) -o $@ $<
