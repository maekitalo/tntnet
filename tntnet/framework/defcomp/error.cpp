/* error.cpp
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>

namespace tnt
{
  class Urlmapper;
  class Comploader;

  ////////////////////////////////////////////////////////////////////////
  // componentdeclaration
  //
  class Error : public tnt::Component
  {
      friend class ErrorFactory;

    public:
      Error()
      { }

      virtual unsigned operator() (tnt::HttpRequest& request,
        tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
      virtual void drop();
  };

  ////////////////////////////////////////////////////////////////////////
  // factory
  //
  class ErrorFactory : public tnt::SingletonComponentFactory
  {
    public:
      ErrorFactory(const std::string& componentName)
        : tnt::SingletonComponentFactory(componentName)
        { }
      virtual tnt::Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
  };

  tnt::Component* ErrorFactory::doCreate(const tnt::Compident&,
    const tnt::Urlmapper&, tnt::Comploader&)
  {
    return new Error();
  }

  TNT_COMPONENTFACTORY(error, ErrorFactory)

  ////////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  unsigned Error::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, cxxtools::QueryParams&)
  {
    std::string msg;

    const tnt::HttpRequest::args_type& args = request.getArgs();

    tnt::HttpRequest::args_type::const_iterator i = args.begin();
    if (i == args.end())
      reply.throwError("400 internal error");

    msg = *i++;
    for ( ; i != args.end(); ++i)
    {
      msg += ' ';
      msg += *i;
    }

    reply.throwError(msg);

    return DECLINED;
  }

  void Error::drop()
  {
    factory.drop(this);
  }

}
