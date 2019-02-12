bin_PROGRAMS = hello

hello_SOURCES = \
	hello.ecpp \
	main.cpp \
	tntnet.png

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
hello_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
dist_noinst_DATA = README
CLEANFILES = hello.cpp tntnet.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .png .cpp

.ecpp.cpp: $(ECPPC)
	$(AM_V_GEN)$(ECPPC) -o $@ $<
.png.cpp:
	$(AM_V_GEN)$(ECPPC) -b -m image/png -o $@ $<
