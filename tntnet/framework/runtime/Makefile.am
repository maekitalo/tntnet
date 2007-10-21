bin_PROGRAMS = tntnet

tntnet_SOURCES = \
	main.cpp \
	process.cpp \
	tnt/process.h

tntnet_LDFLAGS = $(PTHREAD_LIBS)
tntnet_LDADD = $(top_builddir)/framework/common/libtntnet.la
tntnet_CXXFLAGS=-DTNTNET_CONF=\"@sysconfdir@/tntnet/tntnet.conf\" -DTNTNET_PID=\"@localstatedir@/tntnet.pid\"
AM_CXXFLAGS=$(PTHREAD_CFLAGS)

