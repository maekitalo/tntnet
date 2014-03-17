/*
 * Copyright (C) 2003-2006 Tommi Maekitalo
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


#include "tnt/component.h"
#include "tnt/http.h"
#include "tnt/httpreply.h"
#include "tnt/httprequest.h"
#include <sstream>

namespace tnt
{
  void Component::configure(const tnt::TntConfig& config)
    { }

  unsigned Component::topCall(HttpRequest& request, HttpReply& reply, tnt::QueryParams& qparam)
    { return operator() (request, reply, qparam); }

  unsigned Component::operator() (HttpRequest& request, HttpReply& reply, tnt::QueryParams& qparam)
    { return DECLINED; }

  unsigned Component::endTag(HttpRequest& request, HttpReply& reply, tnt::QueryParams& qparam)
    { return DECLINED; }

  std::string Component::getAttribute(const std::string& name, const std::string& def) const
    { return def; }

  unsigned Component::call(HttpRequest& request, HttpReply& reply)
  {
    tnt::QueryParams qparam;
    return call(request, reply, qparam);
  }

  std::string Component::scall(HttpRequest& request, tnt::QueryParams& qparam)
  {
    // set up new reply object
    std::ostringstream result;
    HttpReply reply(result);

    // don't output any http headers
    reply.setDirectModeNoFlush();

    // call the component
    (*this)(request, reply, qparam);

    return result.str();
  }

  std::string Component::scall(HttpRequest& request)
  {
    tnt::QueryParams qparam;
    return scall(request, qparam);
  }
}

