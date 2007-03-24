pkglib_LTLIBRARIES = calci18n.la

calci18n_la_SOURCES = \
	calci18n.ecpp \
	en.png \
	fi.png \
	de.png

calci18n_la_LDFLAGS = -module
calci18n_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties calci18n.txt calci18n.fi calci18n.de
CLEANFILES = calci18n.cpp tntnet.conf tntnet.properties \
	en.cpp calci18n.txt \
	fi.cpp calci18n.fi fi/calci18n.tntdata \
	de.cpp calci18n.de de/calci18n.tntdata
EXTRA_DIST = calci18n.txt.fi calci18n.txt.de

SUFFIXES=.ecpp
.ecpp.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc $<
.png.cpp:
	$(top_builddir)/sdk/tools/ecppc/ecppc -b $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	/bin/sh $(top_builddir)/tntnet-config --config=calci18n >$@

calci18n.txt: calci18n.ecpp
	ecppl calci18n.ecpp

calci18n.fi: calci18n.ecpp calci18n.txt.fi
	[ -d fi ] || mkdir fi
	rm -f calci18n.fi
	( cd fi && ecppll -o calci18n.tntdata ../calci18n.ecpp ../calci18n.txt.fi && zip ../calci18n.fi calci18n.tntdata )

calci18n.de: calci18n.ecpp calci18n.txt.de
	[ -d de ] || mkdir de
	rm -f calci18n.de
	( cd de && ecppll -o calci18n.tntdata ../calci18n.ecpp ../calci18n.txt.de && zip ../calci18n.de calci18n.tntdata )

