/* static.cpp
 * Copyright (C) 2003,2007 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "static.h"
#include "mimehandler.h"
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httperror.h>
#include <tnt/http.h>
#include <tnt/httpheader.h>
#include <tnt/httperror.h>
#include <tnt/comploader.h>
#include <fstream>
#include <cxxtools/log.h>
#include <sys/types.h>
#include <sys/stat.h>

log_define("tntnet.static")

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // factory
  //
  class StaticFactory : public tnt::SingletonComponentFactory
  {
    public:
      StaticFactory(const std::string& componentName)
        : tnt::SingletonComponentFactory(componentName)
        { }
      virtual tnt::Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
      virtual void doConfigure(const tnt::Tntconfig& config);
  };

  tnt::Component* StaticFactory::doCreate(const tnt::Compident&,
    const tnt::Urlmapper&, tnt::Comploader&)
  {
    return new Static();
  }

  void StaticFactory::doConfigure(const tnt::Tntconfig& config)
  {
    if (Static::handler == 0)
      Static::handler = new MimeHandler(config);

    Static::documentRoot = config.getValue(Static::configDocumentRoot);
  }

  TNT_COMPONENTFACTORY(static, StaticFactory)

  //////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  const std::string& Static::configDocumentRoot = "DocumentRoot";
  std::string Static::documentRoot;
  MimeHandler* Static::handler;

  unsigned Static::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, cxxtools::QueryParams& qparams)
  {
    if (!tnt::HttpRequest::checkUrl(request.getPathInfo()))
      throw tnt::HttpError(HTTP_BAD_REQUEST, "illegal url");

    std::string file;
    if (!documentRoot.empty())
      file = documentRoot + '/';
    file += request.getPathInfo();

    log_debug("file: " << file);

    struct stat st;
    if (stat(file.c_str(), &st) != 0)
    {
      log_warn("error in stat for file \"" << file << "\"");
      return DECLINED;
    }

    if (!S_ISREG(st.st_mode))
    {
      log_warn("no regular file \"" << file << "\"");
      return DECLINED;
    }

    std::string lastModified = tnt::HttpMessage::htdate(st.st_ctime);

    {
      std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);
      if (s == lastModified)
        return HTTP_NOT_MODIFIED;
    }

    std::ifstream in(file.c_str());

    if (!in)
    {
      log_warn("file \"" << file << "\" not found");
      return DECLINED;
    }

    // set Content-Type
    if (request.getArgs().size() > 0 && request.getArg(0).size() > 0)
      reply.setContentType(request.getArg(0));
    else if (handler)
      reply.setContentType(handler->getMimeType(request.getPathInfo()));

    reply.setHeader(tnt::httpheader::lastModified, lastModified);

    // set Content-Length
    reply.setContentLengthHeader(st.st_size);

    // set Keep-Alive
    if (request.keepAlive())
      reply.setHeader(tnt::httpheader::connection,
                      tnt::httpheader::connectionKeepAlive);

    // send data
    log_info("send static file \"" << file << "\" size " << st.st_size << " bytes");

    if (isTop())
      reply.setDirectMode();

    reply.out() << in.rdbuf() << std::flush;

    return HTTP_OK;
  }

  void Static::drop()
  {
    factory.drop(this);
  }

}
