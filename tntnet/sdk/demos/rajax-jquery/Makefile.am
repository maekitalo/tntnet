pkglib_LTLIBRARIES = message.la

nodist_message_la_SOURCES = \
	get.cpp \
	jquery.cpp \
	message.cpp \
	put.cpp

EXTRA_DIST = \
	get.ecpp \
	jquery.js \
	message.ecpp \
	put.ecpp

if INSTALL_DEMOS
    doc_DATA = $(EXTRA_DIST)
endif

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
message_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
message_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = get.cpp jquery.cpp message.cpp put.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -m application/x-javascript -b $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( /bin/sh $(top_builddir)/tntnet-config --config=message; echo CompPath .libs ) >$@
