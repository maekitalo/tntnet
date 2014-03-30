AC_INIT(tntnet, 2.3, [Tommi Maekitalo <tommi@tntnet.org>])
AC_CANONICAL_TARGET
LT_INIT([disable-static])

AM_INIT_AUTOMAKE

abi_current=13
abi_revision=0
abi_age=0
sonumber=${abi_current}:${abi_revision}:${abi_age}
AC_SUBST(sonumber)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([framework/common/config.h])
AC_CONFIG_SRCDIR([framework/common/tntnet.cpp])

AC_SEARCH_LIBS(gethostbyname, nsl socket resolv)
AC_SEARCH_LIBS(dlopen, dl, , AC_MSG_ERROR([dlopen not found]))

AC_PROG_CXX

AC_LANG(C++)
ACX_PTHREAD

LIBS="$PTHREAD_LIBS $LIBS"
CC="$PTHREAD_CC"
CXX="$PTHREAD_CXX"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CXXFLAGS="$CXXFLAGS $PTHREAD_CXXFLAGS"

AX_COMPILER_VENDOR
AS_IF([test "$ax_cv_cxx_compiler_vendor" = "ibm"],
      [CPPFLAGS="$CPPFLAGS -qrtti -qlanglvl=newexcp -D__NOLOCK_ON_INPUT -D__NOLOCK_ON_OUTPUT"],
      AX_CHECK_COMPILE_FLAG([-Wno-long-long], [CPPFLAGS="$CPPFLAGS -Wno-long-long"])
      AX_CHECK_COMPILE_FLAG([-Wall], [CPPFLAGS="$CPPFLAGS -Wall"])
      AX_CHECK_COMPILE_FLAG([-pedantic], [CPPFLAGS="$CPPFLAGS -pedantic"]))

AC_CHECK_HEADER([zlib.h], , AC_MSG_ERROR([zlib not found]))
AC_CHECK_HEADER([cxxtools/net/tcpsocket.h], , AC_MSG_ERROR([cxxtools headers not found]))
AC_CHECK_FUNCS([fopen64], ,[AM_CFLAGS=-DUSE_FILE32API])
AC_SUBST(AM_CFLAGS)

AC_ARG_WITH([epoll],
  AS_HELP_STRING([--with-epoll=yes|no|probe], [use epoll]),
  [epoll_option=$withval],
  [epoll_option=probe])

AS_CASE([$epoll_option],
  [yes],   AC_DEFINE(WITH_EPOLL, [], [Define if epoll is supported]),
  [no],    [],
  [probe], AC_TRY_RUN([
            #include <sys/epoll.h>
            int main() { return epoll_create(10) <= 0; }
            ],
            [AC_DEFINE(WITH_EPOLL, [], [Define if epoll is supported])]),
  AC_MSG_ERROR([unknown epoll-value $epoll_option])
)

AC_ARG_WITH([sendfile],
  AS_HELP_STRING([--with-sendfile=yes|no|probe], [use sendfile]),
  [sendfile_option=$withval],
  [sendfile_option=probe])

AS_CASE([$sendfile_option],
  [yes],    AC_DEFINE(WITH_SENDFILE, [], [Define if sendfile is supported]),
  [no],     [],
  [probe],  AC_CHECK_FUNCS([sendfile]),
  AC_MSG_ERROR([unknown sendfile-value $sendfile_option])
)

AC_CHECK_HEADERS([sys/sendfile.h])

#
# SSL
#
AC_ARG_WITH([ssl],
  AS_HELP_STRING([--with-ssl=gnutls|openssl|no], [build with SSL support (default is openssl)]),
  [ssl_option=$withval],
  [ssl_option=openssl])

AS_CASE([$ssl_option],
    [gnutls],
        [PKG_CHECK_MODULES([GNUTLS], [gnutls >= 2.11], [
            AC_MSG_RESULT([building with GnuTLS support])
            AC_DEFINE(WITH_GNUTLS, [], [Define to build with gnutls])
            with_gnutls=1
          ],
          [AC_MSG_ERROR(gnutls >= 2.11 not not found)])
        ],

    [openssl|yes],
        [AC_CHECK_HEADER([openssl/ssl.h], [
            AC_MSG_RESULT([building with OpenSSL support])
            AC_DEFINE(WITH_OPENSSL, [], [Define to build with openssl])
            with_openssl=1
          ],
          [AC_MSG_ERROR(openssl-headers not not found)])
        ],

    [no],
        [AC_MSG_RESULT([building without ssl support])
        ],

    [AC_MSG_ERROR([unknown ssl-value $ssl_option])]
)

AM_CONDITIONAL(MAKE_GNUTLS, test x$with_gnutls = x1)
AM_CONDITIONAL(MAKE_OPENSSL, test x$with_openssl = x1)

#
# optional components
#
AC_ARG_ENABLE([server],
  AS_HELP_STRING([--disable-server], [disable building tntnet server]),
  [enable_server=$enableval],
  [enable_server=yes])

AM_CONDITIONAL(MAKE_SERVER, test x$enable_server = xyes)

AC_ARG_ENABLE([sdk],
  AS_HELP_STRING([--disable-sdk], [disable building sdk]),
  [enable_sdk=$enableval],
  [enable_sdk=yes])

AM_CONDITIONAL(MAKE_SDK, test x$enable_sdk = xyes)

AC_ARG_ENABLE([demos],
  AS_HELP_STRING([--enable-demos], [enable building demos]),
  [enable_demos=$enableval],
  [enable_demos=no])

AM_CONDITIONAL(MAKE_DEMOS, test "$enable_demos" = "yes" -o "$enable_demos" = "install")
AM_CONDITIONAL(INSTALL_DEMOS, test "$enable_demos" = "install")

AS_IF([test "$enable_demos" = "yes"],
  [
    AS_IF([test "$enable_sdk" != "yes"],
        [AC_MSG_ERROR([demos cannot be built without building sdk])])

  AC_CHECK_PROG(ZIP, zip, 1)
  AS_IF([! test "$ZIP"],
    AC_MSG_ERROR([zip program not found (needed for calci18n demo)]))
  ]

  AC_CHECK_HEADER([gd.h], , AC_MSG_ERROR([gd headers not found]))
  AC_CHECK_HEADER([tntdb.h], , AC_MSG_ERROR([tntdb headers not found]))
)

#
# others
#
AC_CHECK_FUNCS([setenv])

case "${host_cpu}-${host_os}" in
*-aix*)
    SHARED_LIB_FLAG=-qmkshrobj
    ;;
*-darwin*)
    SHARED_LIB_FLAG=-dynamiclib
    ;;
*)
    SHARED_LIB_FLAG=-shared
    ;;
esac
AC_SUBST(SHARED_LIB_FLAG)

AC_ARG_ENABLE([unittest],
  AS_HELP_STRING([--disable-unittest], [disable unittest]),
  [enable_unittest=$enableval],
  [enable_unittest=enable_unittest])

AM_CONDITIONAL(MAKE_UNITTEST, test "$enable_unittest" = "enable_unittest")

AC_ARG_ENABLE([locale],
  AS_HELP_STRING([--disable-locale], [disable support for locales]),
  [enable_locale=$enableval],
  [enable_locale=enable_locale])

if test "$enable_locale" != "no"
then
  AC_DEFINE(ENABLE_LOCALE, [], [Defined if locale is used])
fi

AC_CONFIG_FILES([misc/tntnet-config], [chmod +x misc/tntnet-config])

AC_CONFIG_FILES([
    Makefile
    demo/Makefile
    demo/calc/Makefile
    demo/calcajax/Makefile
    demo/calcapp/Makefile
    demo/calci18n/Makefile
    demo/calcmvc/Makefile
    demo/chat/Makefile
    demo/comp/Makefile
    demo/config/Makefile
    demo/controls/Makefile
    demo/cookie/Makefile
    demo/gd/Makefile
    demo/hello/Makefile
    demo/mvc/Makefile
    demo/newsdb/Makefile
    demo/rajax-jquery/Makefile
    demo/savepoint/Makefile
    demo/session/Makefile
    demo/sprintf/Makefile
    demo/strings/Makefile
    demo/upload/Makefile
    doc/man/Makefile
    etc/Makefile
    framework/common/Makefile
    framework/runtime/Makefile
    framework/defcomp/Makefile
    misc/Makefile
    sdk/tools/common/Makefile
    sdk/tools/ecppc/Makefile
    sdk/tools/ecppl/Makefile
    sdk/tools/ecppll/Makefile
    test/Makefile
    ])

AC_OUTPUT
