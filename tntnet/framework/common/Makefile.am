lib_LTLIBRARIES = libtntnet.la

libtntnet_la_SOURCES = \
	comploader.cpp \
	component.cpp \
	contentdisposition.cpp \
	contenttype.cpp \
	http.cpp \
	log.cpp \
	messageattribute.cpp \
	messageheader.cpp \
	multipart.cpp \
	sodata.cpp \
	tntconfig.cpp \
	zdata.cpp

nobase_include_HEADERS = \
	tnt/comploader.h \
	tnt/component.h \
	tnt/contentdisposition.h \
	tnt/contenttype.h \
	tnt/data.h \
	tnt/http.h \
	tnt/log.h \
	tnt/logfwd.h \
	tnt/loginit.h \
	tnt/messageattribute.h \
	tnt/messageheader.h \
	tnt/multipart.h \
	tnt/sodata.h \
	tnt/tntconfig.h \
	tnt/urlmapper.h \
	tnt/zdata.h

nobase_nodist_include_HEADERS = \
	tnt/config.h

libtntnet_la_LDFLAGS = -version-info @sonumber@ -lcxxtools -lz

DISTCLEANFILES = tnt/config.h
