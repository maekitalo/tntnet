bin_PROGRAMS = tntnet

AM_CPPFLAGS = -I../common

tntnet_SOURCES = \
	dispatcher.cpp \
	job.cpp \
	listener.cpp \
	poller.cpp \
	server.cpp \
	tntnet.cpp \
	tnt/dispatcher.h \
	tnt/job.h \
	tnt/listener.h \
	tnt/poller.h \
	tnt/server.h \
	tnt/tntnet.h

tntnet_LDFLAGS = -lboost_regex -lpthread $(cxxtools_ldflags)
tntnet_LDADD = $(top_builddir)/framework/common/libtntnet.la

if MAKE_OPENSSL
tntnet_SOURCES += \
	ssl.cpp \
	tnt/ssl.h
tntnet_LDFLAGS += -lssl
endif
