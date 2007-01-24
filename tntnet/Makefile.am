bin_SCRIPTS = tntnet-config

SUBDIRS = \
    framework/common \
    framework/runtime \
    framework/defcomp \
    framework/cgi \
    doc/man \
    etc

if MAKE_SDK
SUBDIRS += \
    sdk/tools/common \
    sdk/tools/ecppc \
    sdk/tools/ecppl \
    sdk/tools/ecppll
endif

if MAKE_DEMOS
SUBDIRS += \
    sdk/demos \
    sdk/demos/calc \
    sdk/demos/calcajax \
    sdk/demos/calcmvc \
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
    sdk/demos/upload
endif

EXTRA_DIST = ecpp.vim \
    doc/tntnet.pdf \
    doc/tntnet.odt \
    doc/tntnet-configuration.pdf \
    doc/tntnet-configuration.odt \
    doc/logo-tntnet.cdr \
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
    sdk/demos/newsdb/tntnet.properties

