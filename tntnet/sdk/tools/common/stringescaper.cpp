/* stringescaper.cpp
   Copyright (C) 2006 Tommi Maekitalo

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

#include "tnt/stringescaper.h"
#include <sstream>
#include <iterator>

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
