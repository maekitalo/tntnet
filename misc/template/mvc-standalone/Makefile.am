ACLOCAL_AMFLAGS = -I m4 --install
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = @PROJECT@

noinst_HEADERS = \
	configuration.h

ecppSources = \
	index.ecpp \
	webmain.ecpp

staticSources = \
	resources/@PROJECT@.css

@PROJECT@_SOURCES = \
	main.cpp \
	configuration.cpp \
	$(ecppSources)

nodist_@PROJECT@_SOURCES = \
	resources.cpp

@PROJECT@_LDFLAGS = -lcxxtools -ltntnet

CLEANFILES = $(ecppSources:.ecpp=.cpp) $(ecppSources:.ecpp=.deps) resources.cpp

EXTRA_DIST = $(staticSources)

#
# Rules for tntnet applications
#

ECPPFLAGS=-I$(top_srcdir)

# resources
resources.cpp: $(staticSources) Makefile.am
	$(AM_V_GEN)$(ECPPC) -bb -z -n resources -p -o resources.cpp $(ECPPFLAGS) $(staticSources)

SUFFIXES=.ecpp .cpp

.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -n `echo $<|$(SED) 's|^$(srcdir)/||; s|\.ecpp$$||'` $(ECPPFLAGS) -o $@ $<
.ecpp.deps:
	$(AM_V_GEN)$(ECPPC) -M $(ECPPFLAGS) $< | $(SED) '1s/\(.*\).cpp:/\1.cpp \1.deps:/' > $@

-include $(ecppSources:.ecpp=.deps)
