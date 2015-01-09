AM_CPPFLAGS = \
	-I$(top_srcdir)/framework/common \
	-I$(top_srcdir)/sdk/tools/common

noinst_PROGRAMS = tntnet-test

ecppSources = \
	arg.ecpp \
	output.ecpp \
	scope.ecpp

tntnet_test_SOURCES = \
	$(ecppSources) \
	componenttest.cpp \
	cstreamtest.cpp \
	ecpptest.cpp \
	messageheadertest.cpp \
	qparamtest.cpp \
	strutest.cpp \
	testmain.cpp

tntnet_test_LDADD = \
	$(top_builddir)/framework/common/libtntnet.la \
	$(top_builddir)/sdk/tools/common/libtntnet_sdk.la \
	-lcxxtools \
	-lcxxtools-unit

CLEANFILES = $(ecppSources:.ecpp=.cpp)

ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc

SUFFIXES=.ecpp .png .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -o $@ $<
.png.cpp:
	$(AM_V_GEN)$(ECPPC) -b -m image/png -o $@ $<
