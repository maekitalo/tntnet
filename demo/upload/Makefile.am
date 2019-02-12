bin_PROGRAMS = upload

upload_SOURCES = \
	main.cpp \
	upload.ecpp

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
upload_LDFLAGS = -lcxxtools
upload_LDADD = $(top_builddir)/framework/common/libtntnet.la -lcxxtools

noinst_DATA = tntnet.xml
dist_noinst_DATA = README
CLEANFILES = upload.cpp

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .cpp

.ecpp.cpp: $(ECPPC)
	$(AM_V_GEN)$(ECPPC) -o $@ $<
