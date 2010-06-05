bin_PROGRAMS = seweb-app

seweb_app_SOURCES = \
	main.cpp \
	veranstaltung.cpp

noinst_HEADERS = \
    seweb/service/veranstaltung.h

seweb_app_LDADD = $(top_builddir)/lib/libseweb.la
seweb_app_LDFLAGS = -lcxxtools -lcxxtools-http -lcxxtools-xml -lcxxtools-xmlrpc -ltntdb
AM_CPPFLAGS = -I$(top_srcdir)/lib
