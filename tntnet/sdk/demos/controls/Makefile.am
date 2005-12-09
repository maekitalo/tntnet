pkglib_LTLIBRARIES = controls.la

controls_la_SOURCES = \
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
	plus.png

controls_la_LDFLAGS = -module
controls_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

data_DATA = tntnet.conf tntnet.properties
CLEANFILES = tntnet.properties \
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

SUFFIXES=.ecpp .png .css
.ecpp.cpp:
	$(ECPPC) $<
.png.cpp:
	$(ECPPC) -b $<
.css.cpp:
	$(ECPPC) -b $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
