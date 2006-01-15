/* component.cpp
   Copyright (C) 2003-2006 Tommi Maekitalo

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

#include "tnt/component.h"
#include "tnt/http.h"
#include "tnt/httpreply.h"
#include "tnt/httprequest.h"
#include <sstream>

namespace tnt
{
  unsigned Component::operator() (HttpRequest& request,
    HttpReply& reply, cxxtools::QueryParams& qparam)
  {
    return DECLINED;
  }

  unsigned Component::endTag(HttpRequest& request,
    HttpReply& reply, cxxtools::QueryParams& qparam)
  {
    return DECLINED;
  }

  std::string Component::getAttribute(const std::string& name,
    const std::string& def) const
  {
    return def;
  }

  unsigned Component::call(HttpRequest& request, HttpReply& reply)
  {
    cxxtools::QueryParams qparam;
    return call(request, reply, qparam);
  }

  unsigned Component::call(HttpReply& reply, cxxtools::QueryParams& qparam)
  {
    HttpRequest request;
    return call(request, reply, qparam);
  }

  unsigned Component::call(HttpReply& reply)
  {
    HttpRequest request;
    cxxtools::QueryParams qparam;
    return call(request, reply, qparam);
  }

  std::string Component::scall(HttpRequest& request, cxxtools::QueryParams& qparam)
  {
    // set up new reply-object
    std::ostringstream result;
    HttpReply reply(result);

    // don't output any http-headers
    reply.setDirectModeNoFlush();

    // call the component
    (*this)(request, reply, qparam);

    return result.str();
  }

  std::string Component::scall(HttpRequest& request)
  {
    cxxtools::QueryParams qparam;
    return scall(request, qparam);
  }

  std::string Component::scall(cxxtools::QueryParams& qparam)
  {
    HttpRequest request;
    return scall(request, qparam);
  }

  std::string Component::scall()
  {
    HttpRequest request;
    cxxtools::QueryParams qparam;
    return scall(request, qparam);
  }

}
