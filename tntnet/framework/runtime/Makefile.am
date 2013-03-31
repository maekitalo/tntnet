AM_CPPFLAGS = -I$(top_srcdir)/framework/common

bin_PROGRAMS = tntnet

tntnet_SOURCES = \
	main.cpp \
	process.cpp

nobase_include_HEADERS = \
	tnt/process.h

tntnet_LDADD = $(top_builddir)/framework/common/libtntnet.la
tntnet_LDFLAGS = -lcxxtools
tntnet_CXXFLAGS=-DTNTNET_CONF=\"@sysconfdir@/tntnet/tntnet.xml\" -DTNTNET_PID=\"@localstatedir@/tntnet.pid\"

