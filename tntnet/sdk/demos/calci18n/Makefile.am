pkglib_LTLIBRARIES = calci18n.la

nodist_calci18n_la_SOURCES = \
	calci18n.cpp \
	en.cpp \
	fi.cpp \
	de.cpp

EXTRA_DIST = \
	calci18n.ecpp \
	calci18n.txt.fi \
	calci18n.txt.de \
	en.png \
	fi.png \
	de.png

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
calci18n_la_LDFLAGS = -module $(PTHREAD_LIBS) @SHARED_LIB_FLAG@
calci18n_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.conf tntnet.properties calci18n.txt calci18n.fi calci18n.de
CLEANFILES = calci18n.cpp tntnet.conf tntnet.properties \
	en.cpp calci18n.txt \
	fi.cpp calci18n.fi fi/calci18n.tntdata \
	de.cpp calci18n.de de/calci18n.tntdata

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc
ECPPL=$(top_builddir)/sdk/tools/ecppl/ecppl
ECPPLL=$(top_builddir)/sdk/tools/ecppll/ecppll
SUFFIXES=.ecpp .cpp .png

.ecpp.cpp:
	$(ECPPC) $<
.png.cpp:
	$(ECPPC) -b $<

tntnet.properties:
	@CXXTOOLS_CONFIG@ --properties tntnet >$@
tntnet.conf:
	( /bin/sh $(top_builddir)/tntnet-config --config=calci18n; echo CompPath .libs ) >$@

calci18n.txt: calci18n.ecpp
	$(ECPPL) -o calci18n.txt calci18n.ecpp

calci18n.fi: calci18n.ecpp calci18n.txt.fi
	[ -d fi ] || mkdir fi
	rm -f calci18n.fi
	( $(ECPPLL) -o fi/calci18n.tntdata calci18n.ecpp calci18n.txt.fi && cd fi && zip ../calci18n.fi calci18n.tntdata )

calci18n.de: calci18n.ecpp calci18n.txt.de
	[ -d de ] || mkdir de
	rm -f calci18n.de
	( $(ECPPLL) -o de/calci18n.tntdata calci18n.ecpp calci18n.txt.de && cd de && zip ../calci18n.de calci18n.tntdata )
