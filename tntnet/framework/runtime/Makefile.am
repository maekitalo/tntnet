bin_PROGRAMS = tntnet

AM_CPPFLAGS = -I../common

tntnet_SOURCES = \
	dispatcher.cpp \
	job.cpp \
	listener.cpp \
	poller.cpp \
	worker.cpp \
	tntnet.cpp \
	tnt/dispatcher.h \
	tnt/job.h \
	tnt/listener.h \
	tnt/poller.h \
	tnt/worker.h \
	tnt/tntnet.h

tntnet_LDFLAGS = -lboost_regex -lpthread $(cxxtools_ldflags)
tntnet_LDADD = $(top_builddir)/framework/common/libtntnet.la

if MAKE_OPENSSL
tntnet_SOURCES += \
	ssl.cpp \
	tnt/ssl.h
tntnet_LDFLAGS += -lssl
endif
