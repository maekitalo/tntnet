bin_PROGRAMS = tntnet

AM_CPPFLAGS = -I../common

tntnet_SOURCES = \
	dispatcher.cpp \
	job.cpp \
	server.cpp \
	tntnet.cpp \
	ssl.cpp \
	listener.cpp \
	tnt/dispatcher.h \
	tnt/job.h \
	tnt/listener.h \
	tnt/server.h \
	tnt/ssl.h \
	tnt/tntnet.h

tntnet_LDFLAGS = -lssl -lboost_regex -lpthread
tntnet_LDADD = $(top_builddir)/framework/common/libtntnet.la
