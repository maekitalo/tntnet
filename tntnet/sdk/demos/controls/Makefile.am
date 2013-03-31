pkglib_LTLIBRARIES = controls.la

nodist_controls_la_SOURCES = \
	controls.cpp \
	index.cpp \
	treemenu.cpp \
	page1.cpp \
	page2.cpp \
	page3.cpp \
	page4.cpp \
	styles.cpp \
	submitlink.cpp \
	tooltip.cpp \
	tooltipArea.cpp \
	treeview.cpp \
	minus.cpp \
	plus.cpp

EXTRA_DIST = \
	controls.ecpp \
	index.ecpp \
	treemenu.ecpp \
	page1.ecpp \
	page2.ecpp \
	page3.ecpp \
	page4.ecpp \
	styles.css \
	submitlink.ecpp \
	tooltip.ecpp \
	tooltipArea.ecpp \
	treeview.ecpp \
	minus.png \
	plus.png \
	tntnet.xml

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
controls_la_LDFLAGS = -module @SHARED_LIB_FLAG@
controls_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml
CLEANFILES = \
	controls.cpp \
	index.cpp \
	treemenu.cpp \
	page1.cpp \
	page2.cpp \
	page3.cpp \
	page4.cpp \
	styles.cpp \
	submitlink.cpp \
	tooltip.cpp \
	tooltipArea.cpp \
	treeview.cpp \
	minus.cpp \
	plus.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .png .css .cpp
.ecpp.cpp:
	$(ECPPC) -o $@ $<
.png.cpp:
	$(ECPPC) -b -m image/png -o $@ $<
.css.cpp:
	$(ECPPC) -b -m text/css -o $@ $<

tntnet.xml:
	$(SHELL) $(top_builddir)/tntnet-config --config=controls|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@
