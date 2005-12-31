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

#include <tnt/urlmapper.h>
#include <tnt/comploader.h>
#include <tnt/tntconfig.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/componentfactory.h>
#include <tnt/convert.h>
#include <cxxtools/dynbuffer.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>

log_define("cgi");

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<const char*> componentNameArg(argc, argv, 'n', argv[0]);
    std::string componentName = componentNameArg.getValue();
    std::string::size_type pos = componentName.find_last_of('/');
    if (pos != std::string::npos)
      componentName.erase(0, pos + 1);

    log_debug("componentName=" << componentName);

    tnt::Urlmapper urlmapper;

    tnt::Tntconfig config;
    // TODO load configuration:
    //  config.load(configfile);

    // TODO configure comploader:
    //  tnt::Complader::addSeachPath(path);

    tnt::Comploader comploader(config);
    tnt::Compident compident(std::string(), componentName);

    log_debug("fetch component " << compident);
    tnt::Component& comp = comploader.fetchComp(compident, urlmapper);

    const char* pathInfoSz = getenv("PATH_INFO");
    std::string pathInfo;
    if (pathInfoSz)
      pathInfo = pathInfoSz;

    cxxtools::QueryParams qparam;

    const char* queryString = getenv("QUERY_STRING");
    if (queryString)
      qparam.parse_url(queryString);
    const char* contentLength = getenv("CONTENT_LENGTH");
    //const char* contentType = getenv("CONTENT_TYPE");
    unsigned length = contentLength ? tnt::stringTo<unsigned>(contentLength) : 0;
    if (length > 0)
    {
      cxxtools::DynBuffer buffer(length);
      std::cin.get(buffer.data(), length);
      qparam.parse_url(std::string(buffer.data(), std::cin.gcount()));
    }

    tnt::HttpRequest request;
    request.setPathInfo(pathInfo);
    tnt::HttpReply reply(std::cout, false);
    
    log_debug("call component");
    unsigned ret = comp(request, reply, qparam);

    log_debug("send reply");
    reply.sendReply(ret);
  }
  catch (const std::exception& e)
  {
    log_fatal(e.what());
    return -1;
  }
}
