bin_PROGRAMS = gd

gd_SOURCES = \
	gd++.cpp \
	gd.ecpp \
	imgstr.ecpp \
	main.cpp

include_HEADERS = \
	gd++.h

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
gd_LDFLAGS = -lgd
gd_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
dist_noinst_DATA = README
CLEANFILES = gd.cpp imgstr.cpp tntnet.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .cpp

.ecpp.cpp: $(ECPPC)
	$(AM_V_GEN)$(ECPPC) -o $@ $<
