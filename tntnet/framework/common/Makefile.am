lib_LTLIBRARIES = libtntnet.la

libtntnet_la_SOURCES = \
	compident.cpp \
	comploader.cpp \
	component.cpp \
	componentfactory.cpp \
	contentdisposition.cpp \
	contenttype.cpp \
	cookie.cpp \
	deflatestream.cpp \
	dispatcher.cpp \
	ecpp.cpp \
	encoding.cpp \
	htmlescostream.cpp \
	http.cpp \
	httperror.cpp \
	httpheader.cpp \
	httpmessage.cpp \
	httpparser.cpp \
	httprequest.cpp \
	httpreply.cpp \
	inflatestream.cpp \
	ioapi.c \
	job.cpp \
	langlib.cpp \
	listener.cpp \
	messageattribute.cpp \
	messageheader.cpp \
	messageheaderparser.cpp \
	mimedb.cpp \
	multipart.cpp \
	poller.cpp \
	savepoint.cpp \
	scope.cpp \
	scopemanager.cpp \
	sessionscope.cpp \
	tntconfig.cpp \
	tntnet.cpp \
	unzip.c \
	unzipfile.cpp \
	urlescostream.cpp \
	urlmapper.cpp \
	util.cpp \
	worker.cpp \
	zdata.cpp \
	crypt.h \
	ioapi.h \
	unzip.h

nobase_include_HEADERS = \
	tnt/compident.h \
	tnt/comploader.h \
	tnt/component.h \
	tnt/componentfactory.h \
	tnt/configurator.h \
	tnt/contentdisposition.h \
	tnt/contenttype.h \
	tnt/convert.h \
	tnt/cookie.h \
	tnt/data.h \
	tnt/deflatestream.h \
	tnt/dispatcher.h \
	tnt/ecpp.h \
	tnt/encoding.h \
	tnt/htmlescostream.h \
	tnt/http.h \
	tnt/httperror.h \
	tnt/httpheader.h \
	tnt/httpmessage.h \
	tnt/httpparser.h \
	tnt/httprequest.h \
	tnt/httpreply.h \
	tnt/inflatestream.h \
	tnt/job.h \
	tnt/langlib.h \
	tnt/maptarget.h \
	tnt/messageattribute.h \
	tnt/messageheader.h \
	tnt/messageheaderparser.h \
	tnt/mimedb.h \
	tnt/multipart.h \
	tnt/object.h \
	tnt/parser.h \
	tnt/poller.h \
	tnt/query_params.h \
	tnt/savepoint.h \
	tnt/scope.h \
	tnt/scopemanager.h \
	tnt/sessionscope.h \
	tnt/socketif.h \
	tnt/stringlessignorecase.h \
	tnt/threadcontext.h \
	tnt/tntconfig.h \
	tnt/tntnet.h \
	tnt/unzipfile.h \
	tnt/urlescostream.h \
	tnt/urlmapper.h \
	tnt/worker.h \
	tnt/zdata.h

noinst_HEADERS = \
	tnt/listener.h \
	tnt/pollerimpl.h \
	tnt/ssl.h \
	tnt/tcpjob.h \
	tnt/util.h

libtntnet_la_CXXFLAGS = -DPKGLIBDIR=\"@libdir@/@PACKAGE@\" $(PTHREAD_CFLAGS)
libtntnet_la_LDFLAGS = -version-info @sonumber@ -lz -lcxxtools $(PTHREAD_LIBS) @SHARED_LIB_FLAG@

if MAKE_GNUTLS
libtntnet_la_SOURCES += \
	gcryptinit.c \
	gnutls.cpp

noinst_HEADERS += \
	tnt/gcryptinit.h \
	tnt/gnutls.h

libtntnet_la_LDFLAGS += -lgnutls -lgcrypt
endif

if MAKE_OPENSSL
libtntnet_la_SOURCES += \
	openssl.cpp

noinst_HEADERS += \
	tnt/openssl.h

libtntnet_la_LDFLAGS += -lssl -lcrypto
endif

