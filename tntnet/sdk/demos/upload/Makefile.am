pkglib_LTLIBRARIES = upload.la

nodist_upload_la_SOURCES = \
	upload.cpp

EXTRA_DIST = \
	upload.ecpp

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
upload_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
upload_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = upload.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties upload >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=upload; echo CompPath .libs ) >$@
