/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <tnt/cgi.h>
#include <tnt/urlmapper.h>
#include <tnt/httpreply.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <config.h>
#include <stdlib.h>
#include <sstream>

// fastcgi is not yet implemented!!!
#ifdef WITH_FASTCGI
# include "fastcgi.h"
# include <errno.h>
#endif

#ifndef TNTNET_CONF
# define TNTNET_CONF "/etc/tntnet.conf"
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

  std::string Cgi::getPeerIp() const
  {
    const char* addr = getenv("REMOTE_ADDR");
    return addr ? addr : std::string();
  }

  std::string Cgi::getServerIp() const
  {
    const char* addr = getenv("SERVER_ADDR");
    return addr ? addr : std::string();
  }

  bool Cgi::isSsl() const
  {
    return getenv("HTTPS");
  }

  void Cgi::touch()
  { }

  Scope& Cgi::getScope()
  {
    return threadScope;
  }

  void Cgi::readBody()
  {
    const char* contentLength = getenv("CONTENT_LENGTH");
    unsigned length = 0;
    if (contentLength)
    {
      std::istringstream in(contentLength);
      in >> length;
    }

    if (length > 0)
    {
      std::vector<char> buffer(length);
      std::cin.get(&buffer[0], length);
      request.setBody(std::string(&buffer[0], std::cin.gcount()));
    }
  }

  Cgi::Cgi(int argc, char* argv[])
    : request(application, this)
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

    request.setThreadContext(this);
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
