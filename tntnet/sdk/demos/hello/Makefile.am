pkglib_LTLIBRARIES = hello.la

nodist_hello_la_SOURCES = \
	hello.cpp \
	tntnet.cpp

EXTRA_DIST = \
	hello.ecpp \
	tntnet.jpg

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
hello_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
hello_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = hello.cpp tntnet.cpp tntnet.conf tntnet.properties

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .jpg .cpp
.ecpp.cpp:
	$(ECPPC) $<
.jpg.cpp:
	$(ECPPC) -b -m image/jpeg $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=hello >$@
