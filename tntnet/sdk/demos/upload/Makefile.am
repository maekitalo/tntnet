pkglib_LTLIBRARIES = upload.la

upload_la_SOURCES = \
	upload.ecpp

upload_la_LDFLAGS = -module
upload_la_LIBADD = $(top_builddir)/framework/ecpp/libecpp.la

AM_CPPFLAGS = -I$(top_builddir)/framework/ecpp

EXTRA_DIST = tntnet.conf tntnet.properties

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $(ECPPFLAGS) $(ECPPFLAGS_CPP) $<

