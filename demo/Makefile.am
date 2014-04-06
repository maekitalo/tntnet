SUBDIRS = \
    calc \
    calcajax \
    calcapp \
    calcmvc \
    calci18n \
    chat \
    comp \
    config \
    controls \
    cookie \
    gd \
    hello \
    mvc \
    newsdb \
    savepoint \
    session \
    sprintf \
    strings \
    rajax-jquery \
    upload

pkglib_LTLIBRARIES = alldemos.la

nodist_alldemos_la_SOURCES = \
	alldemos.cpp

EXTRA_DIST = \
	alldemos.ecpp \
	tntnet.xml \
	demos.txt

AM_CPPFLAGS = -I$(top_srcdir)/framework/common
alldemos_la_LDFLAGS = -module @SHARED_LIB_FLAG@
alldemos_la_LIBADD = $(top_builddir)/framework/common/libtntnet.la

CLEANFILES = alldemos.cpp

SUFFIXES=.ecpp .cpp
.ecpp.cpp:
	$(AM_V_GEN)$(top_builddir)/sdk/tools/ecppc/ecppc -o $@ $<
