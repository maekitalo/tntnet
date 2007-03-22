/* messageheader.cpp
 * Copyright (C) 2003 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <tnt/messageheader.h>
#include <tnt/messageheaderparser.h>
#include <cxxtools/log.h>

namespace tnt
{
  log_define("tntnet.messageheader")

  std::istream& operator>> (std::istream& in, Messageheader& data)
  {
    data.parse(in);
    return in;
  }

  void Messageheader::parse(std::istream& in, size_t maxHeaderSize)
  {
    Parser p(*this);
    p.parse(in);
  }

  Messageheader::return_type Messageheader::onField(const std::string& name, const std::string& value)
  {
    log_debug(name << ' ' << value);
    insert(value_type(name, value));
    return OK;
  }
}
