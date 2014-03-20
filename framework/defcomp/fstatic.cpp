/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "fstatic.h"
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
      virtual tnt::Component* doCreate(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);
  };

  tnt::Component* FstaticFactory::doCreate(const tnt::Compident&, const tnt::Urlmapper&, tnt::Comploader&)
    { return new Fstatic(); }

  TNT_COMPONENTFACTORY(fstatic, FstaticFactory)

  //////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  unsigned Fstatic::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparams)
  {
    if (!tnt::HttpRequest::checkUrl(request.getPathInfo())
      || request.getPathInfo().find('\0') != std::string::npos)
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

    // send data
    log_info("send static file \"" << file << "\" size " << st.st_size << " bytes");
    reply.setDirectMode();
    reply.out() << in.rdbuf();

    return HTTP_OK;
  }

  void Fstatic::drop()
    { factory.drop(this); }
}

