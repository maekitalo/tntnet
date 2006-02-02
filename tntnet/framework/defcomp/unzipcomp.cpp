/* unzipcomp.cpp
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

#include "unzip.h"

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/unzipfile.h>

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class Unzip : public tnt::Component
  {
      friend class UnzipFactory;

    public:
      virtual unsigned operator() (tnt::HttpRequest& request,
        tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
      virtual void drop();
  };

  ////////////////////////////////////////////////////////////////////////
  // factory
  //
  class UnzipFactory : public tnt::SingletonComponentFactory
  {
    public:
      UnzipFactory(const std::string& componentName)
        : tnt::SingletonComponentFactory(componentName)
        { }
      virtual tnt::Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
  };

  tnt::Component* UnzipFactory::doCreate(const tnt::Compident&,
    const tnt::Urlmapper&, tnt::Comploader&)
  {
    return new Unzip();
  }

  TNT_COMPONENTFACTORY(Unzip, UnzipFactory)

  ////////////////////////////////////////////////////////////////////////
  // componentdefinition
  //

  unsigned Unzip::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, cxxtools::QueryParams& qparams)
  {
    std::string pi = request.getPathInfo();

    if (request.getArgsCount() < 1)
      reply.throwError(HTTP_INTERNAL_SERVER_ERROR, "missing archive name");

    try
    {
      unzipFile f(request.getArg(0));
      unzipFileStream in(f, pi, false);

      // set Content-Type
      if (request.getArgs().size() > 1 && request.getArg(1).size() > 0)
        reply.setContentType(request.getArg(1));

      reply.out() << in.rdbuf();
    }
    catch (const unzipEndOfListOfFile&)
    {
      reply.throwNotFound(pi);
    }

    return HTTP_OK;
  }

  void Unzip::drop()
  {
    factory.drop(this);
  }

}
