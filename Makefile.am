ACLOCAL_AMFLAGS = -I m4

if MAKE_SERVER
SERVERDIRS = \
    framework/runtime \
    framework/defcomp \
    etc
endif

if MAKE_SDK
SDKDIRS = \
    sdk/tools/common \
    sdk/tools/ecppc \
    misc
endif

if MAKE_DEMOS
DEMODIRS = \
    demo
endif

if MAKE_UNITTEST
TESTDIRS = \
    test
endif

SUBDIRS = \
    framework/common \
    doc/man \
    $(SERVERDIRS) \
    $(SDKDIRS) \
    $(DEMODIRS) \
    $(TESTDIRS)

EXTRA_DIST = \
    COPYING \
    demo/newsdb/Makefile \
    demo/newsdb/README \
    demo/newsdb/global.ecpp \
    demo/newsdb/index.ecpp \
    demo/newsdb/news.ecpp \
    demo/newsdb/newscss.css \
    demo/newsdb/newsdata.sh \
    demo/newsdb/newsdb.sql \
    doc/jquery-howto.markdown \
    doc/quick-start-guide.markdown \
    doc/static-howto.markdown \
    doc/upload-howto.markdown \
    m4/acx_pthread.m4 \
    m4/ax_check_compile_flag.m4 \
    m4/ax_compiler_vendor.m4 \
    m4/pkg.m4 \
    misc/ecpp.vim \
    misc/tntnet-logo.svg \
    $(pkgdata_SCRIPTS)

pkgconfigdir = $(libdir)/pkgconfig/

pkgconfig_DATA = \
    pkgconfig/tntnet.pc \
    pkgconfig/tntnet_sdk.pc
