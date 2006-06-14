/* contenttype.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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

#include <tnt/contenttype.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctype.h>
#include <functional>
#include <algorithm>
#include <cxxtools/log.h>

log_define("tntnet.contenttype")

namespace tnt
{
  Contenttype::Contenttype(const std::string& ct)
  {
    log_debug("Contenttype <= " << ct);

    std::istringstream in(ct);

    in >> *this;
    if (!in)
    {
      std::ostringstream msg;
      msg << "error 1 parsing content-type-header at "
          << in.tellg()
          << ": "
          << ct;
      throw std::runtime_error(msg.str());
    }

    if (in.get() != std::ios::traits_type::eof())
    {
      std::ostringstream msg;
      msg << "error 2 parsing content-type-header at "
          << in.tellg()
          << ": "
          << ct;
      throw std::runtime_error(msg.str());
    }
  }

  Contenttype::return_type Contenttype::onType(
    const std::string& t, const std::string& s)
  {
    log_debug("Contenttype::onType " << t << ", " << s);
    if (s.empty())
      return FAIL;

    type = t;
    subtype = s;

    std::transform(type.begin(), type.end(), type.begin(),
      std::ptr_fun(tolower));
    std::transform(subtype.begin(), subtype.end(), subtype.begin(),
      std::ptr_fun(tolower));

    return OK;
  }

  Contenttype::return_type Contenttype::onParameter(
    const std::string& attribute, const std::string& value)
  {
    log_debug("Contenttype::onParameter " << attribute << ", " << value);
    std::string att = attribute;
    std::transform(att.begin(), att.end(), att.begin(),
      std::ptr_fun(tolower));

    parameter.insert(parameter_type::value_type(att, value));
    if (attribute == "boundary")
      boundary = value;
    return OK;
  }
}
