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


#ifndef TNT_STRINGESCAPER_H
#define TNT_STRINGESCAPER_H

#include <functional>
#include <string.h>
#include <string>
#include <cctype>

namespace tnt
{
  class stringescaper : public std::unary_function<const char*, char>
  {
      bool _escQuote;
      mutable char _data[5];

    public:
      explicit stringescaper(bool escQuote = true)
        : _escQuote(escQuote)
        { }

      const char* operator() (char ch) const
      {
        if (ch == '\n')
          strcpy(_data, "\\n");
        else if (ch == '\t')
          strcpy(_data, "\\t");
        else if (ch == '?')
          strcpy(_data, "\\?");
        else if (_escQuote && ch == '"')
          strcpy(_data, "\\\"");
        else if (std::isprint(ch) && ch != '\\')
        {
          _data[0] = ch;
          _data[1] = '\0';
        }
        else
        {
          _data[0] = '\\';
          _data[1] = (char)(((unsigned char)ch >> 6) + '0');
          _data[2] = (char)((((unsigned char)ch >> 3) & 0x7) + '0');
          _data[3] = (char)(((unsigned char)ch & 0x7) + '0');
          _data[4] = '\0';
        }

        return _data;
      }

      static std::string escape(const std::string& str, const stringescaper& = stringescaper(false));
      static std::string mk_stringconst(const std::string& str, unsigned maxcols = 0,
                                        const stringescaper& = stringescaper(true));
  };
}

#endif // TNT_STRINGESCAPER_H

