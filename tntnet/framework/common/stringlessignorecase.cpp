/* stringlessignorecase.cpp
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

#include <tnt/stringlessignorecase.h>
#include <cctype>

namespace tnt
{
  bool StringLessIgnoreCase::operator()(const std::string& s1, const std::string& s2) const
  {
    std::string::const_iterator it1 = s1.begin();
    std::string::const_iterator it2 = s2.begin();
    while (it1 != s1.end() && it2 != s2.end())
    {
      char c1 = std::toupper(*it1);
      char c2 = std::toupper(*it2);
      if (c1 < c2)
        return true;
      else if (c2 < c1)
        return false;
      ++it1;
      ++it2;
    }
    return it1 == s1.end() ? (it2 != s2.end()) : (it2 == s2.end());
  }
}
