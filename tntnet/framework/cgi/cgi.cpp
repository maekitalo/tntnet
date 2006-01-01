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
#include <tnt/comploader.h>
#include <tnt/tntconfig.h>
#include <tnt/httpreply.h>
#include <tnt/convert.h>
#include <cxxtools/dynbuffer.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>

log_define("cgi");

namespace tnt
{
  void Cgi::getMethod()
  {
    const char* requestMethod = getenv("REQUEST_METHOD");
    if (requestMethod)
      request.setMethod(requestMethod);
  }

  void Cgi::getContentType()
  {
    const char* contentType = getenv("CONTENT_TYPE");
    if (contentType)
      request.setHeader(httpheader::contentType, contentType);
  }

  void Cgi::getQueryString()
  {
    const char* queryString = getenv("QUERY_STRING");
    if (queryString)
    {
      request.setQueryString(queryString);
    }
  }

  void Cgi::getPathInfo()
  {
    const char* pathInfo = getenv("PATH_INFO");
    if (pathInfo)
      request.setPathInfo(pathInfo);
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
  }

  int Cgi::run()
  {
    getMethod();
    getContentType();
    getQueryString();
    getPathInfo();
    readBody();

    request.doPostParse();

    Tntconfig config;
    // TODO load configuration:
    //  config.load(configfile);

    Comploader::configure(config);
    Comploader comploader;

    Compident compident(std::string(), componentName);
    log_debug("fetch component " << compident);
    Component& comp = comploader.fetchComp(compident, Urlmapper());

    log_debug("call component");
    HttpReply reply(std::cout, false);
    unsigned ret = comp(request, reply, request.getQueryParams());

    log_debug("send reply");
    reply.sendReply(ret);
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
