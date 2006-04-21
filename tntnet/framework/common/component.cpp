/* component.cpp
 * Copyright (C) 2003-2006 Tommi Maekitalo
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

}
