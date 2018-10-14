ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = @PROJECT@

ecppSources = \
	actionmain.ecpp \
	json/example.ecpp \
	json/getnumbers.ecpp \
	json/dbaccess.ecpp \
	action/insertdata.ecpp \
	action/myaction.ecpp \
	webmain.ecpp

staticSources = \
    resources/app/actions.js \
    resources/app/json.js \
    resources/app/main.js \
    resources/app/dbaccess.js \
    resources/app/dbinsert.js \
    resources/css/noty/themes/nest.css \
    resources/css/noty/themes/relax.css \
    resources/css/noty/themes/mint.css \
    resources/css/noty/themes/semanticui.css \
    resources/css/noty/themes/light.css \
    resources/css/noty/themes/metroui.css \
    resources/css/noty/themes/sunset.css \
    resources/css/noty/themes/bootstrap-v4.css \
    resources/css/noty/themes/bootstrap-v3.css \
    resources/css/@PROJECT@.css \
    resources/css/statusbar.css \
    resources/css/noty.css.map \
    resources/css/noty.css \
    resources/css/dropdown.css \
    resources/html/json.html \
    resources/html/dbaccess.html \
    resources/html/dbinsert.html \
    resources/html/actions.html \
    resources/js/require.js \
    resources/js/main.js \
    resources/js/jquery-3.3.1.min.js \
    resources/js/noty.min.js \
    resources/js/tntnet.js \
    resources/js/noty.js

noinst_HEADERS = \
	action.ecpp \
	include/actionreply.h \
	include/noty.h \
	include/@PROJECT@/config.h 

@PROJECT@_SOURCES = \
	src/@PROJECT@/config.cpp \
	src/main.cpp \
	$(ecppSources)

nodist_@PROJECT@_SOURCES = \
	resources.cpp

@PROJECT@_LDADD = -lcxxtools -ltntnet -ltntdb

nobase_dist_noinst_DATA = $(staticSources) @PROJECT@.xml

CLEANFILES = $(ecppSources:.ecpp=.cpp) $(ecppSources:.ecpp=.deps) resources.cpp

#
# Rules for tntnet applications
#
ECPPFLAGS=-I$(srcdir)

resources.cpp: $(staticSources) Makefile.am
	$(AM_V_GEN)$(ECPPC) -bb -z -n resources -p -o resources.cpp $(ECPPFLAGS) $(staticSources)

SUFFIXES=.ecpp .cpp

.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -n `echo $<|$(SED) 's|^$(srcdir)/||; s|\.ecpp$$||'` $(ECPPFLAGS) -o $@ $<
.ecpp.deps:
	$(AM_V_GEN)$(ECPPC) -M $(ECPPFLAGS) $< | $(SED) '1s|\(.*\).cpp:|\1.cpp $@:|' > $@

-include $(ecppSources:.ecpp=.deps)
