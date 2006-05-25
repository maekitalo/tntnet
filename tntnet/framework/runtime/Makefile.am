bin_PROGRAMS = tntnet

tntnet_SOURCES = \
	dispatcher.cpp \
	job.cpp \
	listener.cpp \
	poller.cpp \
	regex.cpp \
	worker.cpp \
	tntnet.cpp \
	tnt/dispatcher.h \
	tnt/job.h \
	tnt/listener.h \
	tnt/poller.h \
	tnt/regex.h \
	tnt/worker.h \
	tnt/tntnet.h

tntnet_LDFLAGS = -lpthread
tntnet_LDADD = $(top_builddir)/framework/common/libtntnet.la

if MAKE_OPENSSL
tntnet_SOURCES += \
	ssl.cpp \
	tnt/ssl.h
tntnet_LDFLAGS += -lssl -lcrypto
endif

tntnet_CXXFLAGS=-DTNTNET_CONF=\"@sysconfdir@/tntnet/tntnet.conf\" -DTNTNET_PID=\"@sharedstatedir@/tntnet.pid\"
