/* fstatic.cpp
   Copyright (C) 2003 Tommi Maekitalo

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

#include "fstatic.h"
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/httpheader.h>
#include <tnt/comploader.h>
#include <fstream>
#include <cxxtools/log.h>
#include <sys/types.h>
#include <sys/stat.h>

log_define("tntnet.fstatic")

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // factory
  //
  class FstaticFactory : public tnt::SingletonComponentFactory
  {
    public:
      FstaticFactory(const std::string& componentName)
        : tnt::SingletonComponentFactory(componentName)
        { }
      virtual tnt::Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
  };

  tnt::Component* FstaticFactory::doCreate(const tnt::Compident&,
    const tnt::Urlmapper&, tnt::Comploader&)
  {
    return new Fstatic();
  }

  TNT_COMPONENTFACTORY(Fstatic, FstaticFactory)

  //////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  unsigned Fstatic::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, cxxtools::QueryParams& qparams)
  {
    if (!tnt::HttpRequest::checkUrl(request.getPathInfo()))
      throw tnt::HttpError(HTTP_BAD_REQUEST, "illegal url");

    std::string file;
    if (!getDocumentRoot().empty())
      file = getDocumentRoot() + '/';
    file += request.getPathInfo();

    log_debug("file: " << file);

    struct stat st;
    if (stat(file.c_str(), &st) != 0)
    {
      log_warn("error in stat for file \"" << file << "\"");
      reply.throwNotFound(request.getPathInfo());
    }

    if (!S_ISREG(st.st_mode))
    {
      log_warn("no regular file \"" << file << "\"");
      reply.throwNotFound(request.getPathInfo());
    }

    std::ifstream in(file.c_str());

    if (!in)
    {
      log_warn("file \"" << file << "\" not found");
      reply.throwNotFound(request.getPathInfo());
    }

    // set Content-Type
    if (request.getArgs().size() > 0)
      reply.setContentType(request.getArg(0));

    // set Content-Length
    reply.setContentLengthHeader(st.st_size);

    // set Keep-Alive
    if (request.keepAlive())
      reply.setHeader(tnt::httpheader::connection,
                      tnt::httpheader::connectionKeepAlive);

    // send datea
    reply.setDirectMode();
    reply.out() << in.rdbuf();

    return HTTP_OK;
  }

  void Fstatic::drop()
  {
    factory.drop(this);
  }

}
