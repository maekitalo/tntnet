.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

pkglib_LTLIBRARIES = calc.la

calc_la_SOURCES = \
	calc.ecpp

calc_la_LDFLAGS = -module
calc_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp
