lib_LTLIBRARIES = libtntnet.la

libtntnet_la_SOURCES = \
	chunkedostream.cpp \
	cmd.cpp \
	compident.cpp \
	comploader.cpp \
	component.cpp \
	componentfactory.cpp \
	contentdisposition.cpp \
	contenttype.cpp \
	cookie.cpp \
	cstream.cpp \
	deflatestream.cpp \
	dispatcher.cpp \
	ecpp.cpp \
	encoding.cpp \
	htmlescostream.cpp \
	httperror.cpp \
	httpheader.cpp \
	httpmessage.cpp \
	httpparser.cpp \
	httprequest.cpp \
	httpreply.cpp \
	job.cpp \
	listener.cpp \
	mbcomponent.cpp \
	messageattribute.cpp \
	messageheader.cpp \
	messageheaderparser.cpp \
	mimedb.cpp \
	multipart.cpp \
	poller.cpp \
	pollerimpl.cpp \
	savepoint.cpp \
	scope.cpp \
	scopemanager.cpp \
	stringlessignorecase.cpp \
	tcpjob.cpp \
	tntconfig.cpp \
	tntnet.cpp \
	tntnetimpl.cpp \
	unzipfile.cpp \
	urlescostream.cpp \
	urlmapper.cpp \
	util.cpp \
	worker.cpp \
	zdata.cpp \
	crypt.h

nobase_include_HEADERS = \
	tnt/applicationunlocker.h \
	tnt/chunkedostream.h \
	tnt/cmd.h \
	tnt/compident.h \
	tnt/comploader.h \
	tnt/component.h \
	tnt/componentfactory.h \
	tnt/configurator.h \
	tnt/contentdisposition.h \
	tnt/contenttype.h \
	tnt/cookie.h \
	tnt/data.h \
	tnt/deflatestream.h \
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
	tnt/mapping.h \
	tnt/maptarget.h \
	tnt/mbcomponent.h \
	tnt/messageattribute.h \
	tnt/messageheader.h \
	tnt/messageheaderparser.h \
	tnt/mimedb.h \
	tnt/multipart.h \
	tnt/object.h \
	tnt/parser.h \
	tnt/query_params.h \
	tnt/savepoint.h \
	tnt/scope.h \
	tnt/scopemanager.h \
	tnt/sessionscope.h \
	tnt/sessionunlocker.h \
	tnt/socketif.h \
	tnt/stringlessignorecase.h \
	tnt/threadcontext.h \
	tnt/tntconfig.h \
	tnt/tntnet.h \
	tnt/unzipfile.h \
	tnt/urlescostream.h \
	tnt/urlmapper.h \
	tnt/zdata.h

noinst_HEADERS = \
	tnt/cstream.h \
	tnt/dispatcher.h \
	tnt/job.h \
	tnt/listener.h \
	tnt/poller.h \
	tnt/pollerimpl.h \
	tnt/tcpjob.h \
	tnt/util.h \
	tnt/worker.h \
	tntnetimpl.h \
	ioapi.h \
	unzip.h

libtntnet_la_CXXFLAGS = -DPKGLIBDIR=\"@libdir@/@PACKAGE@\"
libtntnet_la_LDFLAGS = -version-info @sonumber@ @SHARED_LIB_FLAG@
libtntnet_la_LIBADD = -lz -lcxxtools

if HAVE_MINIZIP
libtntnet_la_CXXFLAGS += $(MINIZIP_CFLAGS)
libtntnet_la_LDFLAGS += $(MINIZIP_LIBS)
else
libtntnet_la_SOURCES += \
	ioapi.c \
	unzip.c
endif
