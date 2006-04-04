/* cgi.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of tntnet.

Tntnet is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntnet is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntnet; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <tnt/cgi.h>
#include <tnt/urlmapper.h>
#include <tnt/httpreply.h>
#include <tnt/convert.h>
#include <cxxtools/dynbuffer.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <config.h>

// fastcgi is not yet implemented!!!
#ifdef WITH_FASTCGI
# include "fastcgi.h"
# include <errno.h>
#endif

#ifndef CONFIG_DIR
# define CONFIG_DIR "/etc/tntnet/"
#endif

#ifndef TNTNET_CONF
# define TNTNET_CONF CONFIG_DIR "tntnet.conf"
#endif

log_define("tntnet.cgi")

namespace tnt
{
  void Cgi::getHeader(const char* env, const std::string& headername)
  {
    const char* value = getenv(env);
    if (value)
      request.setHeader(headername, value);
  }

  void Cgi::getMethod()
  {
    const char* requestMethod = getenv("REQUEST_METHOD");
    if (requestMethod)
      request.setMethod(requestMethod);
  }

  void Cgi::getQueryString()
  {
    const char* queryString = getenv("QUERY_STRING");
    if (queryString)
      request.setQueryString(queryString);
  }

  void Cgi::getPathInfo()
  {
    const char* pathInfo = getenv("PATH_INFO");
    if (pathInfo)
      request.setPathInfo(pathInfo);
  }

  void Cgi::getRemoteAddr()
  {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));

    addr.sin_family = AF_INET;

    const char* remotePort = getenv("REMOTE_PORT");
    if (remotePort)
      addr.sin_port = stringTo<u_int16_t>(remotePort);

    const char* remoteAddr = getenv("REMOTE_ADDR");
    if (remoteAddr)
      inet_pton(AF_INET, remoteAddr, &addr);

    request.setPeerAddr(addr);
  }

  void Cgi::readBody()
  {
    const char* contentLength = getenv("CONTENT_LENGTH");
    unsigned length = contentLength ? stringTo<unsigned>(contentLength) : 0;
    if (length > 0)
    {
      cxxtools::DynBuffer buffer(length);
      std::cin.get(buffer.data(), length);
      request.setBody(std::string(buffer.data(), std::cin.gcount()));
    }
  }

  Cgi::Cgi(int argc, char* argv[])
  {
    cxxtools::Arg<const char*> componentNameArg(argc, argv, 'n', argv[0]);
    componentName = componentNameArg.getValue();
    std::string::size_type pos = componentName.find_last_of('/');
    if (pos != std::string::npos)
      componentName.erase(0, pos + 1);

    log_debug("componentName=" << componentName);

    // load configuration:
    cxxtools::Arg<const char*> conf(argc, argv, 'c', TNTNET_CONF);

    if (conf.isSet())
      config.load(conf);
    else
    {
      const char* tntnetConf = ::getenv("TNTNET_CONF");
      if (tntnetConf)
        config.load(tntnetConf);
      else
      {
        try
        {
          config.load(TNTNET_CONF);
        }
        catch (const std::exception& e)
        {
          log_warn(e.what());
        }
      }
    }
  }

  void Cgi::execute()
  {
    Compident compident = Compident(std::string(), componentName);
    log_debug("fetch component " << compident);
    Component& comp = comploader.fetchComp(compident, Urlmapper());

    scopeManager.preCall(request, compident.libname);

    log_debug("call component");
    HttpReply reply(std::cout, false);
    unsigned ret = comp(request, reply, request.getQueryParams());

    // Don't call postCall. postCall just sets a session-cookie,
    // which don't make sense in cgi, because the program stops after every
    // request.
    // scopeManager.postCall(request, reply, compident.libname);

    log_debug("send reply");
    reply.sendReply(ret);
  }

  bool Cgi::isFastCgi() const
  {
#ifdef WITH_FASTCGI
    struct sockaddr sock;
    socklen_t len = sizeof(sock);
    return getpeername(FCGI_LISTENSOCK_FILENO, &sock, &len) == -1
        && errno == ENOTCONN;
#else
    return false;
#endif
  }

  int Cgi::run()
  {
#ifdef WITH_FASTCGI
    return isCgi() ? runCgi() : runFCgi();
#else
    return runCgi();
#endif
  }

  int Cgi::runCgi()
  {
    getMethod();
    getHeader("CONTENT_TYPE", httpheader::contentType);
    getQueryString();
    getPathInfo();
    getHeader("HTTP_CONNECTION", httpheader::connection);
    getHeader("HTTP_USER_AGENT", httpheader::userAgent);
    getHeader("HTTP_ACCEPT", httpheader::accept);
    getHeader("HTTP_ACCEPT_ENCODING", httpheader::acceptEncoding);
    getHeader("HTTP_ACCEPT_CHARSET", httpheader::acceptCharset);
    getHeader("HTTP_ACCEPT_LANGUAGE", httpheader::acceptLanguage);
    getHeader("HTTP_HOST", httpheader::host);
    getRemoteAddr();

    readBody();

    request.doPostParse();

    Comploader::configure(config);

    execute();
    return 0;
  }

  int Cgi::runFCgi()
  {
    return 0;
  }
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();
    tnt::Cgi app(argc, argv);
    return app.run();
  }
  catch (const std::exception& e)
  {
    log_fatal(e.what());
    return -1;
  }
}
