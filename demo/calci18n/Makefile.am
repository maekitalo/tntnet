pkglib_LTLIBRARIES = calci18n.la

calci18n_la_SOURCES = \
	calci18n.ecpp \
	en.cpp \
	fi.cpp \
	de.cpp

EXTRA_DIST = \
	calci18n.txt.fi \
	calci18n.txt.de \
	en.png \
	fi.png \
	de.png

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
calci18n_la_LDFLAGS = -module @SHARED_LIB_FLAG@
calci18n_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

noinst_DATA = tntnet.xml calci18n.txt calci18n.fi calci18n.de
CLEANFILES = calci18n.cpp tntnet.xml \
	en.cpp calci18n.txt \
	fi.cpp calci18n.fi fi/calci18n.tntdata \
	de.cpp calci18n.de de/calci18n.tntdata

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc
ECPPL=$(top_builddir)/sdk/tools/ecppl/ecppl
ECPPLL=$(top_builddir)/sdk/tools/ecppll/ecppll
SUFFIXES=.ecpp .cpp .png

.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -o $@ $<
.png.cpp:
	$(AM_V_GEN)$(ECPPC) -b -o $@ $<

tntnet.xml:
	$(AM_V_GEN)$(SHELL) $(top_builddir)/misc/tntnet-config --config=calci18n|$(AWK) '/<\/tntnet>/{print "  <compPath>\n    <entry>.libs</entry>\n  </compPath>"} {print}' >$@

calci18n.txt: $(srcdir)/calci18n.ecpp
	$(AM_V_GEN)$(ECPPL) -o calci18n.txt calci18n.ecpp

calci18n.fi: $(srcdir)/calci18n.ecpp $(srcdir)/calci18n.txt.fi
	$(AM_V_GEN)( [ -d fi ] || mkdir fi; \
		rm -f calci18n.fi; \
		$(ECPPLL) -o fi/calci18n.tntdata $(srcdir)/calci18n.ecpp $(srcdir)/calci18n.txt.fi && \
		cd fi && \
		zip ../calci18n.fi calci18n.tntdata )

calci18n.de: $(srcdir)/calci18n.ecpp $(srcdir)/calci18n.txt.de
	$(AM_V_GEN)( [ -d de ] || mkdir de; \
		rm -f calci18n.de; \
		$(ECPPLL) -o de/calci18n.tntdata $(srcdir)/calci18n.ecpp $(srcdir)/calci18n.txt.de && \
		cd de && \
		zip ../calci18n.de calci18n.tntdata )
