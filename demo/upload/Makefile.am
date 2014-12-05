pkglib_LTLIBRARIES = upload.la

upload_la_SOURCES = \
	upload.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
upload_la_LDFLAGS = -module @SHARED_LIB_FLAG@
upload_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = upload.cpp tntnet.xml

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=upload|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
