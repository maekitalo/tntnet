AC_INIT(tntnet, 3.0, [Tommi Maekitalo <tommi@tntnet.org>])
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
AC_PROG_LIBTOOL
AC_PROG_SED
PKG_PROG_PKG_CONFIG

AC_LANG(C++)
AX_CXX_COMPILE_STDCXX_11(noext, optional)
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

AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE([
#include <cxxtools/xml/xmldeserializer.h>
void f()
{
  cxxtools::xml::XmlDeserializer d;
  d.readAttributes(true);
}])], AC_DEFINE(CXXTOOLS_XMLDESERIALISER_ATTR, [], [Defined if cxxtools xmldeserializer can read attributes]))

AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE([
#include <cxxtools/net/tcpserver.h>
int flag = cxxtools::net::TcpServer::REUSEADDR;
])], AC_DEFINE(HAVE_CXXTOOLS_REUSEADDR, [], [Defined if cxxtools supports SO_REUSEADDR flag]))

PKG_CHECK_MODULES([MINIZIP], [minizip],
  [HAVE_MINIZIP=true], [HAVE_MINIZIP=false])
AC_SUBST([MINIZIP_CFLAGS])
AC_SUBST([MINIZIP_LIBS])
AM_CONDITIONAL([HAVE_MINIZIP], [test $HAVE_MINIZIP = true])

AC_CHECK_FUNCS([epoll_create])
AC_CHECK_FUNCS([epoll_create1])
AC_CHECK_FUNCS([sendfile])
AC_CHECK_HEADERS([sys/sendfile.h])

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

    AC_CHECK_HEADER([tntdb.h], , AC_MSG_ERROR([tntdb headers not found]))
  ]
)

AC_ARG_WITH([md2man],
  AS_HELP_STRING([--with-md2man], [use go-md2man tool to generate man pages from markdown])
  [with_md2man=$withval])

AS_IF(
  [test "$with_md2man"],
  AC_CHECK_PROG(MD2MAN, [go-md2man], [go-md2man])
  AS_IF(
    [test -z "$MD2MAN"],
    AC_MSG_ERROR([go-md2man not found])
  )

)

AM_CONDITIONAL(GENERATE_MAN, test ! -z "$with_md2man")

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

AC_CONFIG_FILES([misc/tntnet-project], [chmod +x misc/tntnet-project])

AC_CONFIG_FILES([
    Makefile
    demo/Makefile
    demo/accumulator/Makefile
    demo/calc/Makefile
    demo/calcajax/Makefile
    demo/calcapp/Makefile
    demo/calcmvc/Makefile
    demo/comp/Makefile
    demo/cookie/Makefile
    demo/gd/Makefile
    demo/hello/Makefile
    demo/newsdb/Makefile
    demo/rest/Makefile
    demo/savepoint/Makefile
    demo/session/Makefile
    demo/upload/Makefile
    doc/man/Makefile
    etc/Makefile
    framework/common/Makefile
    framework/runtime/Makefile
    framework/defcomp/Makefile
    misc/Makefile
    pkgconfig/tntnet.pc
    pkgconfig/tntnet_sdk.pc
    sdk/tools/common/Makefile
    sdk/tools/ecppc/Makefile
    test/Makefile
    ])

AC_OUTPUT
