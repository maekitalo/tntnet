/* stringescaper.cpp
 * Copyright (C) 2006 Tommi Maekitalo
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

#include "tnt/stringescaper.h"
#include <sstream>
#include <iterator>
#include <algorithm>

namespace tnt
{
  std::string stringescaper::escape(const std::string& str,
      const stringescaper& se)
  {
    std::ostringstream c_str;
    std::transform(str.begin(), str.end(),
      std::ostream_iterator<const char*>(c_str),
      se);
    return c_str.str();
  }

  std::string stringescaper::mk_stringconst(const std::string& str,
      unsigned maxcols, const stringescaper& se)
  {
    std::ostringstream c_str;
    unsigned col = 1;
    c_str << '"';
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
      const char* charp = se(*it);
      unsigned len = strlen(charp);
      col += len;

      if (maxcols > 0 && col == maxcols)
      {
        c_str << "\"\n\"";
        col = 1 + len;
      }

      c_str << charp;
    }

    return c_str.str();
  }
}
