/* tnt/convert.h
   Copyright (C) 2004 Tommi Maekitalo

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

#ifndef TNT_CONVERT_H
#define TNT_CONVERT_H

#include <sstream>
#include <stdexcept>

namespace tnt
{
  template <typename T>
  inline std::string to_string(const T& value)
  {
    std::ostringstream s;
    s << value;
    return s.str();
  }

  template <>
  inline std::string to_string(const std::string& value)
  { return value; }

  inline std::string to_string(const char* value)
  { return std::string(value); }

  template <typename T>
  inline T string_to(const std::string& value)
  {
    T ret;
    std::istringstream s(value);
    s >> ret;
    if (!s)
      throw std::runtime_error("cannot cast " + value);
    return ret;
  }

  template <>
  inline std::string string_to<std::string>(const std::string& value)
  { return value; }
}

#endif // TNT_CONVERT_H

