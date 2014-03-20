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

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/httperror.h>
#include <sstream>

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
      Error() { }

      virtual unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
  };

  ////////////////////////////////////////////////////////////////////////
  // factory
  //
  static ComponentFactoryImpl<Error> errorFactory("error");

  ////////////////////////////////////////////////////////////////////////
  // componentdefinition
  //
  unsigned Error::operator() (tnt::HttpRequest& request,
    tnt::HttpReply& reply, tnt::QueryParams&)
  {
    std::istringstream s(request.getArg("code"));
    unsigned errorcode;
    s >> errorcode;
    if (!s || errorcode < 300 || errorcode >= 1000)
      throw tnt::HttpError(HTTP_INTERNAL_SERVER_ERROR, "configuration error");

    std::string msg = request.getArg("message");
    if (msg.empty())
      msg = HttpReturn::httpMessage(errorcode);

    throw tnt::HttpError(errorcode, msg);

    return DECLINED;
  }
}

