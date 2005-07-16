/* messageheader.cpp
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
