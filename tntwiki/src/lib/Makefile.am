AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libtntwiki.la

libtntwiki_la_SOURCES = \
	PageManager.cpp \
	MarkdownHtml.cpp \
	MarkdownParser.cpp \
	MarkdownSpanElementParser.cpp \
	Renderer.cpp

libtntwiki_la_LDFLAGS = -lcxxtools -ltntdb -version-info @sonumber@ $(PTHREAD_LIBS)
