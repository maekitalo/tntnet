AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = libtntwiki.la

libtntwiki_la_SOURCES = \
	MarkdownHtml.cpp \
	MarkdownParser.cpp \
	MarkdownSpanElementParser.cpp \
	PageManager.cpp \
	Renderer.cpp \
	UserManager.cpp

libtntwiki_la_LDFLAGS = -lcxxtools -ltntdb -version-info @sonumber@ $(PTHREAD_LIBS)
