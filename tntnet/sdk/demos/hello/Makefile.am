pkglib_LTLIBRARIES = hello.la

hello_la_SOURCES = \
	hello.ecpp

hello_la_LDFLAGS = -module
hello_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

EXTRA_DIST = tntnet.conf tntnet.properties

SUFFIXES=.ecpp .gif .jpg

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

.ecpp.cpp:
	$(ECPPC) $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

.gif.cpp:
	$(ECPPC) -b -m image/gif $(ECPPFLAGS) $<

.jpg.cpp:
	$(ECPPC) -b -m image/jpg $(ECPPFLAGS) $<
