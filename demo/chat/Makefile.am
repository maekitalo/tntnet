pkglib_LTLIBRARIES = chat.la

chat_la_SOURCES = \
	chat.ecpp \
	ajax.js \
	getchat.ecpp \
	putchat.ecpp \
	sad.png \
	smile.png \
	wink.png

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
chat_la_LDFLAGS = -module @SHARED_LIB_FLAG@
chat_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

dist_noinst_DATA = tntnet.xml global.ecpp
CLEANFILES = chat.cpp ajax.cpp getchat.cpp putchat.cpp \
    sad.cpp smile.cpp wink.cpp \
    tntnet.xml

getchat.cpp: getchat.ecpp \
  global.ecpp

putchat.cpp: putchat.ecpp \
  global.ecpp

SUFFIXES=.ecpp .js .png .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ -I $(srcdir) $<
.js.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -b -m application/x-javascript -o $@ $<
.png.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -b -m image/png -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=chat|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
