/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#ifndef TNT_STRINGLESSIGNORECASE_H
#define TNT_STRINGLESSIGNORECASE_H

#include <string>
#include <functional>
#include <cctype>

namespace tnt
{
  template <typename stringType>
  int StringCompareIgnoreCase(const stringType& s1, const stringType& s2)
  {
    typename stringType::const_iterator it1 = s1.begin();
    typename stringType::const_iterator it2 = s2.begin();
    while (it1 != s1.end() && it2 != s2.end())
    {
        if (*it1 != *it2)
        {
            char c1 = std::toupper(*it1);
            char c2 = std::toupper(*it2);
            if (c1 < c2)
                return -1;
            else if (c2 < c1)
                return 1;
        }
        ++it1;
        ++it2;
    }

    return it1 == s1.end() ? (it2 != s2.end()) ? -1 : 0
                           : 1;
  }

  template <>
  int StringCompareIgnoreCase<const char*>(const char* const& s1, const char* const& s2);

  inline int StringCompareIgnoreCase(const std::string& s1, const std::string& s2)
    { return StringCompareIgnoreCase<std::string>(s1, s2); }

  template <typename stringType = std::string>
  class StringLessIgnoreCase : public std::binary_function<stringType, stringType, bool>
  {
    public:
      bool operator()(const stringType& s1, const stringType& s2) const
        { return StringCompareIgnoreCase(s1, s2) < 0; }
  };

}

#endif // TNT_STRINGLESSIGNORECASE_H

