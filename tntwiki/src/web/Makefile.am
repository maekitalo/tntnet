AUTOMAKE_OPTIONS = subdir-objects

#
# Settings for this module
#
AM_CPPFLAGS = -I$(top_srcdir)/include
AM_LDFLAGS = -lcxxtools -ltntnet

#
# Create program
#
#bin_PROGRAMS = \
#	tntwiki

# Create lib
pkglib_LTLIBRARIES = tntwiki.la

cppSources =

ecppSources = \
	wiki.ecpp

staticSources =

ecppIncludes =

static.cpp: $(staticSources) Makefile.am
	$(ECPPC) -bb -z -n static -p -o static.cpp -I$(srcdir) $(staticSources)

tntwiki_la_SOURCES = \
	$(ecppSources) \
	$(cppSources)

#nodist_tntwiki_la_SOURCES = \
#	static.cpp

tntwiki_la_LDFLAGS = -module
tntwiki_la_LIBADD = \
	$(top_builddir)/src/lib/libtntwiki.la

$(top_builddir)/CIXWeb: $(thtmlSources:.thtml=.deps)

CLEANFILES = $(ecppSources:.ecpp=.cpp) $(ecppSources:.ecpp=.deps) static.cpp
EXTRA_DIST = $(ecppIncludes)

#
# Rules for tntnet applications
#
SUFFIXES=.ecpp .cpp

.ecpp.cpp:
	$(ECPPC) -o $@ -n $* -I $(srcdir) -I $(srcdir)/include $<

.ecpp.deps:
	$(ECPPC) -M -n $* -I $(srcdir) -I $(srcdir)/include $< | $(SED) '1s/\(.*\).cpp:/\1.cpp \1.deps:/' > $@

-include $(thtmlSources:.thtml=.deps)
-include $(ecppSources:.ecpp=.deps)

noinst_DATA = tntnet.xml

# vim:noet
