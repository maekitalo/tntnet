/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include <tnt/contenttype.h>
#include <tnt/util.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctype.h>
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
      throwRuntimeError(msg.str());
    }

    if (in.get() != std::ios::traits_type::eof())
    {
      std::ostringstream msg;
      msg << "error 2 parsing content-type-header at "
          << in.tellg()
          << ": "
          << ct;
      throwRuntimeError(msg.str());
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
      ::tolower);
    std::transform(subtype.begin(), subtype.end(), subtype.begin(),
      ::tolower);

    return OK;
  }

  Contenttype::return_type Contenttype::onParameter(
    const std::string& attribute, const std::string& value)
  {
    log_debug("Contenttype::onParameter " << attribute << ", " << value);
    std::string att = attribute;
    std::transform(att.begin(), att.end(), att.begin(),
      ::tolower);

    parameter.insert(parameter_type::value_type(att, value));
    if (attribute == "boundary")
      boundary = value;
    return OK;
  }
}
