bin_PROGRAMS = ecppc

ecppc_SOURCES = \
	body.cpp \
	closecomponent.cpp \
	component.cpp \
	dependencygenerator.cpp \
	ecppc.cpp \
	generator.cpp \
	scopevar.cpp \
	subcomponent.cpp \
	variable.cpp \
	tnt/ecppc/body.h \
	tnt/ecppc/closecomponent.h \
	tnt/ecppc/component.h \
	tnt/ecppc/dependencygenerator.h \
	tnt/ecppc/ecppc.h \
	tnt/ecppc/generator.h \
	tnt/ecppc/scopevar.h \
	tnt/ecppc/subcomponent.h \
	tnt/ecppc/variable.h

ecppc_LDADD = $(top_builddir)/framework/common/libtntnet.la $(top_builddir)/sdk/tools/common/libtntnet_sdk.la -lcxxtools

AM_CPPFLAGS = -I$(top_srcdir)/framework/common -I$(top_srcdir)/sdk/tools/common
