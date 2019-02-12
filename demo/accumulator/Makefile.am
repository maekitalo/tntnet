bin_PROGRAMS = accumulator

accumulator_SOURCES = \
	accumulator.ecpp \
	main.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
accumulator_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
CLEANFILES = accumulator.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .cpp

.ecpp.cpp: $(ECPPC)
	$(AM_V_GEN)$(ECPPC) -o $@ $<
