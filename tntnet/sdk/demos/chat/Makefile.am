pkglib_LTLIBRARIES = chat.la

nodist_chat_la_SOURCES = \
	chat.cpp \
	ajax.cpp \
	getchat.cpp \
	putchat.cpp \
	sad.cpp \
	smile.cpp \
	wink.cpp

EXTRA_DIST = \
	chat.ecpp \
	ajax.js \
	getchat.ecpp \
	global.ecpp \
	putchat.ecpp \
	sad.png \
	smile.png \
	wink.png

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
chat_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
chat_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties
CLEANFILES = chat.cpp ajax.cpp getchat.cpp putchat.cpp \
    sad.cpp smile.cpp wink.cpp \
    tntnet.conf tntnet.properties

SUFFIXES=.ecpp .js .png .cpp
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
