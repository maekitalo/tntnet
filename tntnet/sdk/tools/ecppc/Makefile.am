bin_PROGRAMS = ecppc

ecppc_SOURCES = \
	body.cpp \
	component.cpp \
	cssfilter.cpp \
	dependencygenerator.cpp \
	ecppc.cpp \
	generator.cpp \
	htmlfilter.cpp \
	jsfilter.cpp \
	scopevar.cpp \
	subcomponent.cpp \
	variable.cpp \
	tnt/ecppc/body.h \
	tnt/ecppc/component.h \
	tnt/ecppc/cssfilter.h \
	tnt/ecppc/dependencygenerator.h \
	tnt/ecppc/generator.h \
	tnt/ecppc/htmlfilter.h \
	tnt/ecppc/jsfilter.h \
	tnt/ecppc/scopevar.h \
	tnt/ecppc/subcomponent.h \
	tnt/ecppc/variable.h

ecppc_LDADD = $(top_builddir)/framework/common/libtntnet.la $(top_builddir)/sdk/tools/common/libtntnet_sdk.la $(top_builddir)/framework/ecpp/libecpp.la

AM_CPPFLAGS = -I$(top_builddir)/sdk/tools/common -I$(top_builddir)/framework/ecpp @CXXTOOLS_CXXFLAGS@
