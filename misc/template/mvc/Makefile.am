ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

models = \
	model/index.ecpp

views = \
	view/index.ecpp \
	view/mypage.ecpp

controllers = \
	controller/index.ecpp

actions = \
	action/myaction.ecpp

bin_PROGRAMS = @PROJECT@

noinst_HEADERS = \
	actionreply.h \
	action.ecpp \
	configuration.h \
	noty.h \
	session.ecpp \
	$(models)

ecppSources = \
	actionmain.ecpp \
	json/example.ecpp \
	webmain.ecpp \
	$(views) \
	$(controllers) \
	$(actions)

notySources =  \
	resources/js/noty/packaged/jquery.noty.packaged.min.js \
	resources/js/noty/packaged/jquery.noty.packaged.js \
	resources/js/noty/promise.js \
	resources/js/noty/jquery.noty.js \
	resources/js/noty/themes/default.js \
	resources/js/noty/themes/relax.js \
	resources/js/noty/themes/bootstrap.js \
	resources/js/noty/layouts/topRight.js \
	resources/js/noty/layouts/topLeft.js \
	resources/js/noty/layouts/bottomCenter.js \
	resources/js/noty/layouts/top.js \
	resources/js/noty/layouts/bottomRight.js \
	resources/js/noty/layouts/bottomLeft.js \
	resources/js/noty/layouts/center.js \
	resources/js/noty/layouts/inline.js \
	resources/js/noty/layouts/bottom.js \
	resources/js/noty/layouts/topCenter.js \
	resources/js/noty/layouts/centerRight.js \
	resources/js/noty/layouts/centerLeft.js

staticSources = \
	$(notySources) \
	resources/js/jquery-3.3.1.min.js \
	resources/js/tntnet.js \
	resources/mypage.js \
	resources/@PROJECT@.css

@PROJECT@_SOURCES = \
	main.cpp \
	configuration.cpp \
	$(ecppSources)

nodist_@PROJECT@_SOURCES = \
	resources.cpp

@PROJECT@_LDADD = -lcxxtools -ltntnet

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
	$(AM_V_GEN)$(ECPPC) -M $(ECPPFLAGS) $< | $(SED) '1s|\(.*\).cpp:|\1.cpp $@:|' > $@

-include $(ecppSources:.ecpp=.deps)
