pkglib_LTLIBRARIES = chat.la

chat_la_SOURCES = \
	chat.ecpp \
	ajax.js \
	getchat.ecpp \
	putchat.ecpp \
	sad.png \
	smile.png \
	wink.png

chat_la_LDFLAGS = -module
chat_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

EXTRA_DIST = global.ecpp
noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = chat.cpp tntnet.conf tntnet.properties

SUFFIXES=.ecpp .js .png
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
.js.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -b $<
.png.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -b $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=chat >$@
