ACLOCAL_AMFLAGS = -I m4 --install
AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = @PROJECT@

ecppSources = \
	@PROJECT@.ecpp

@PROJECT@_SOURCES = \
	main.cpp \
	$(ecppSources)

@PROJECT@_LDFLAGS = -lcxxtools -ltntnet

CLEANFILES = $(ecppSources:.ecpp=.cpp) $(ecppSources:.ecpp=.deps) resources.cpp

#
# Rules for tntnet applications
#

SUFFIXES=.ecpp .ico .cpp

ECPPFLAGS=-I$(top_srcdir)

.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -n `echo $<|$(SED) 's|^$(srcdir)/||; s|\.ecpp$$||'` $(ECPPFLAGS) -o $@ $<
.ecpp.deps:
	$(AM_V_GEN)$(ECPPC) -M $(ECPPFLAGS) $< | $(SED) '1s/\(.*\).cpp:/\1.cpp \1.deps:/' > $@

-include $(ecppSources:.ecpp=.deps)
