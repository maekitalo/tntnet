bin_PROGRAMS = seweb-web

ecppSources = \
    index.ecpp \
    htmlmain.ecpp \
    menu.ecpp \
    veranstaltung.ecpp \
    veranstaltungJson.ecpp \
    veranstaltungXml.ecpp \
    veranstaltungen.ecpp

seweb_web_SOURCES = \
    $(ecppSources) \
	main.cpp

EXTRA_DIST = \
    global.ecpp

seweb_web_LDADD = $(top_builddir)/lib/libseweb.la $(top_builddir)/jquery/libtnt-jquery-ui-1.8.2.la
seweb_web_LDFLAGS = -ltntnet -lcxxtools-xmlrpc
AM_CPPFLAGS = -I$(top_srcdir)/lib

CLEANFILES = $(ecppSources:.ecpp=.cpp)

SUFFIXES=.ecpp .cpp

.ecpp.cpp:
	$(ECPPC) -I $(top_srcdir)/web -o $@ $<
