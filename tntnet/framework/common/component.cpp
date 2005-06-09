/* component.cpp
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

#include "tnt/component.h"
#include "tnt/http.h"
#include "tnt/httpreply.h"
#include "tnt/httprequest.h"
#include <sstream>
#include <cxxtools/query_params.h>

namespace tnt
{
  compident::compident(const std::string& ident)
  {
    std::string::size_type pos = ident.find('@');
    if (pos == std::string::npos)
      compname = ident;
    else
    {
      compname = ident.substr(0, pos);
      libname = ident.substr(pos + 1);
    }
  }

  unsigned component::operator() (httpRequest& request,
    httpReply& reply, cxxtools::query_params& qparam)
  {
    return DECLINED;
  }

  std::string component::getAttribute(const std::string& name,
    const std::string& def) const
  {
    return def;
  }

  unsigned component::getDataCount(const httpRequest& request) const
  { return 0; }

  unsigned component::getDataLen(const httpRequest& request, unsigned n) const
  { return 0; }

  const char* component::getDataPtr(const httpRequest& request, unsigned n) const
  { return 0; }

  std::string component::scall(httpRequest& request, cxxtools::query_params& qparam)
  {
    // set up new reply-object
    std::ostringstream result;
    httpReply reply(result);

    // don't output any http-headers
    reply.setDirectModeNoFlush();

    // call the component
    (*this)(request, reply, qparam);

    return result.str();
  }

  std::string component::scall(httpRequest& request)
  {
    cxxtools::query_params qparam;
    return scall(request, qparam);
  }

  std::string component::scall(cxxtools::query_params& qparam)
  {
    httpRequest request;
    return scall(request, qparam);
  }

  std::string component::scall()
  {
    httpRequest request;
    cxxtools::query_params qparam;
    return scall(request, qparam);
  }
}
