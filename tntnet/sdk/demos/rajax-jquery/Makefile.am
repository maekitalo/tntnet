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
message_la_LDFLAGS = -module @SHARED_LIB_FLAG@
message_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = get.cpp jquery.cpp message.cpp put.cpp tntnet.xml

SUFFIXES=.ecpp .js .cpp

.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -m application/x-javascript -b -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=message|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
