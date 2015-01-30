ACLOCAL_AMFLAGS = -I m4 --install
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = @PROJECT@

noinst_HEADERS = \
	configuration.h \
	model/index.ecpp

ecppSources = \
	controller/index.ecpp \
	json/example.ecpp \
	view/index.ecpp \
	webmain.ecpp

staticSources = \
	resources/@PROJECT@.css

@CPROJECT@_SOURCES = \
	main.cpp \
	configuration.cpp \
	$(ecppSources)

nodist_@CPROJECT@_SOURCES = \
	resources.cpp

@CPROJECT@_LDADD = -lcxxtools -ltntnet

nobase_dist_noinst_DATA = $(staticSources)

CLEANFILES = $(ecppSources:.ecpp=.cpp) $(ecppSources:.ecpp=.deps) resources.cpp

EXTRA_DIST = $(staticSources)

#
# Rules for tntnet applications
#

ECPPFLAGS=-I$(srcdir)

# resources
resources.cpp: $(staticSources) Makefile.am
	$(AM_V_GEN)$(ECPPC) -bb -z -n resources -p -o resources.cpp $(ECPPFLAGS) $(staticSources)

SUFFIXES=.ecpp .cpp

.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -n `echo $<|$(SED) 's|^$(srcdir)/||; s|\.ecpp$$||'` $(ECPPFLAGS) -o $@ $<
.ecpp.deps:
	$(AM_V_GEN)$(ECPPC) -M $(ECPPFLAGS) $< | $(SED) '1s/\(.*\).cpp:/\1.cpp \1.deps:/' > $@

-include $(ecppSources:.ecpp=.deps)
