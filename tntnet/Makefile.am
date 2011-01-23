ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS = tntnet-config

tntnet-config: tntnet-config.in

if MAKE_SERVER
SERVERDIRS = \
    framework/runtime \
    framework/defcomp \
    etc
endif

if MAKE_CGI
CGIDIRS = \
    framework/cgi
endif

if MAKE_SDK
SDKDIRS = \
    sdk/tools/common \
    sdk/tools/ecppc \
    sdk/tools/ecppl \
    sdk/tools/ecppll
endif

if MAKE_DEMOS
DEMOSDIRS = \
    sdk/demos \
    sdk/demos/backgroundjob \
    sdk/demos/calc \
    sdk/demos/calcajax \
    sdk/demos/calcapp \
    sdk/demos/calcmvc \
    sdk/demos/calci18n \
    sdk/demos/chat \
    sdk/demos/comp \
    sdk/demos/config \
    sdk/demos/controls \
    sdk/demos/cookie \
    sdk/demos/hello \
    sdk/demos/savepoint \
    sdk/demos/session \
    sdk/demos/sprintf \
    sdk/demos/strings \
    sdk/demos/rajax-jquery \
    sdk/demos/upload
endif

if MAKE_UNITTEST
  UNITTESTDIR = utest
endif

SUBDIRS = \
    framework/common \
    doc/man \
    $(UNITTESTDIR) \
    $(SERVERDIRS) \
    $(CGIDIRS) \
    $(SDKDIRS) \
    $(DEMOSDIRS)

EXTRA_DIST = ecpp.vim \
    doc/tntnet.pdf \
    doc/tntnet.odt \
    doc/tntnet-configuration.pdf \
    doc/tntnet-configuration.odt \
    doc/logo-tntnet.cdr \
    doc/tntnet.svg \
    doc/tntnet-dynamite.svg \
    doc/quick-start-guide.odt \
    doc/quick-start-guide.pdf \
    COPYING \
    sdk/demos/newsdb/Makefile \
    sdk/demos/newsdb/README \
    sdk/demos/newsdb/global.ecpp \
    sdk/demos/newsdb/index.ecpp \
    sdk/demos/newsdb/news.ecpp \
    sdk/demos/newsdb/newscss.css \
    sdk/demos/newsdb/newsdata.sh \
    sdk/demos/newsdb/newsdb.sql \
    sdk/demos/newsdb/tntnet.conf \
    sdk/demos/newsdb/tntnet.properties \
    m4/acx_pthread.m4
