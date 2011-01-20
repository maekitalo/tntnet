pkglib_LTLIBRARIES = message.la

noinst_HEADERS = \
    message.h \
    modificationtracker.h

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

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
message_la_LDFLAGS = -module @SHARED_LIB_FLAG@
message_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la $(PTHREAD_LIBS)

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = get.cpp jquery.cpp message.cpp put.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .js .cpp

.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -m application/x-javascript -b -o $@ $<

tntnet.properties:
	$(SHELL) $(top_builddir)/tntnet-config --properties message >$@
tntnet.conf:
	( $(SHELL) $(top_builddir)/tntnet-config --config=message; echo CompPath .libs ) >$@
