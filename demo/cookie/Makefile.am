bin_PROGRAMS = cookie

cookie_SOURCES = \
	cookie.ecpp \
	main.cpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
cookie_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
dist_noinst_DATA = README
CLEANFILES = cookie.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

.ecpp.cpp: $(ECPPC)
	$(AM_V_GEN)$(ECPPC) -o $@ $<
