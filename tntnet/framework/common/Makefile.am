lib_LTLIBRARIES = libtntnet.la

libtntnet_la_SOURCES = \
	comploader.cpp \
	component.cpp \
	contentdisposition.cpp \
	contenttype.cpp \
	cookie.cpp \
	ecpp.cpp \
	htmlescostream.cpp \
	http.cpp \
	httperror.cpp \
	httpheader.cpp \
	httpmessage.cpp \
	httpparser.cpp \
	httprequest.cpp \
	httpreply.cpp \
	messageattribute.cpp \
	messageheader.cpp \
	messageheaderparser.cpp \
	mimedb.cpp \
	multipart.cpp \
	object.cpp \
	savepoint.cpp \
	scope.cpp \
	sessionscope.cpp \
	sodata.cpp \
	tntconfig.cpp \
	zdata.cpp

nobase_include_HEADERS = \
	tnt/comploader.h \
	tnt/component.h \
	tnt/contentdisposition.h \
	tnt/contenttype.h \
	tnt/convert.h \
	tnt/cookie.h \
	tnt/data.h \
	tnt/ecpp.h \
	tnt/htmlescostream.h \
	tnt/http.h \
	tnt/httperror.h \
	tnt/httpheader.h \
	tnt/httpmessage.h \
	tnt/httpparser.h \
	tnt/httprequest.h \
	tnt/httpreply.h \
	tnt/messageattribute.h \
	tnt/messageheader.h \
	tnt/messageheaderparser.h \
	tnt/mimedb.h \
	tnt/multipart.h \
	tnt/object.h \
	tnt/objectptr.h \
	tnt/objecttemplate.h \
	tnt/parser.h \
	tnt/pointer.h \
	tnt/savepoint.h \
	tnt/scope.h \
	tnt/sessionscope.h \
	tnt/sodata.h \
	tnt/tntconfig.h \
	tnt/urlmapper.h \
	tnt/zdata.h

libtntnet_la_LDFLAGS = -version-info @sonumber@ -lz @CXXTOOLS_LOG_LDFLAGS@
AM_CPPFLAGS = @CXXTOOLS_CXXFLAGS@
