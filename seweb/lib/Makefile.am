lib_LTLIBRARIES = libseweb.la

libseweb_la_SOURCES = \
	configuration.cpp \
	veranstaltung.cpp \
	wettkampf.cpp

noinst_HEADERS = \
	seweb/configuration.h \
	seweb/veranstaltung.h \
	seweb/wettkampf.h

libseweb_la_CXXFLAGS = -DSYSCONFDIR=\"@sysconfdir@\"
libseweb_la_LDFLAGS = -version-info @sonumber@ -lcxxtools -lcxxtools-xml $(PTHREAD_LIBS)
