noinst_PROGRAMS = chat

ecppSources = \
	chat.ecpp \
	messages.ecpp \
	put.ecpp

staticSources = \
	resources/jquery.js \
	resources/chat.css \
	resources/chat.js

chat_SOURCES = \
	main.cpp \
	session.cpp \
	$(ecppSources)

nodist_chat_SOURCES = \
	resources.cpp

nobase_include_HEADERS = \
	session.h

chat_LDADD = $(top_builddir)/framework/common/libtntnet.la
chat_LDFLAGS = -lcxxtools

nobase_dist_noinst_DATA = $(staticSources)

CLEANFILES = $(ecppSources:.ecpp=.cpp) $(ecppSources:.ecpp=.deps) resources.cpp

#
# Rules for tntnet applications
#
ECPPC=$(top_builddir)/sdk/tools/ecppc/ecppc
ECPPFLAGS=-I$(srcdir)

resources.cpp: $(staticSources) Makefile.am
	$(AM_V_GEN)$(ECPPC) -bb -z -n resources -p -o resources.cpp $(ECPPFLAGS) $(staticSources)

SUFFIXES=.ecpp .cpp

.ecpp.cpp:
	$(AM_V_GEN)$(ECPPC) -n `echo $<|$(SED) 's|^$(srcdir)/||; s|\.ecpp$$||'` $(ECPPFLAGS) -o $@ $<
.ecpp.deps:
	$(AM_V_GEN)$(ECPPC) -M $(ECPPFLAGS) $< | $(SED) '1s/\(.*\).cpp:/\1.cpp \1.deps:/' > $@

-include $(ecppSources:.ecpp=.deps)

