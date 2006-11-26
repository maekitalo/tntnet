bin_PROGRAMS = tntnet

tntnet_SOURCES = \
	dispatcher.cpp \
	job.cpp \
	listener.cpp \
	poller.cpp \
	regex.cpp \
	syserror.cpp \
	worker.cpp \
	tntnet.cpp \
	tnt/dispatcher.h \
	tnt/job.h \
	tnt/listener.h \
	tnt/poller.h \
	tnt/regex.h \
	tnt/ssl.h \
	tnt/syserror.h \
	tnt/worker.h \
	tnt/tntnet.h

tntnet_LDFLAGS = -lpthread -lcxxtools
tntnet_LDADD = $(top_builddir)/framework/common/libtntnet.la
tntnet_CXXFLAGS=-DTNTNET_CONF=\"@sysconfdir@/tntnet/tntnet.conf\" -DTNTNET_PID=\"@localstatedir@/tntnet.pid\"

if MAKE_GNUTLS
tntnet_SOURCES += \
	gcryptinit.c \
	tnt/gcryptinit.h \
	gnutls.cpp \
	tnt/gnutls.h
tntnet_LDFLAGS += `libgnutls-config --libs`
tntnet_CXXFLAGS += `libgnutls-config --cflags`
endif

if MAKE_OPENSSL
tntnet_SOURCES += \
	openssl.cpp \
	tnt/openssl.h
tntnet_LDFLAGS += -lssl -lcrypto
endif

