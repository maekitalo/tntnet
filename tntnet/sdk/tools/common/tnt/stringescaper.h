/* tnt/stringescaper.h
 * Copyright (C) 2003 Tommi Maekitalo
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef TNT_STRINGESCAPER_H
#define TNT_STRINGESCAPER_H

#include <functional>
#include <string.h>
#include <string>
#include <locale>

namespace tnt
{
  class stringescaper : public std::unary_function<const char*, char>
  {
      bool escQuote;
      mutable char data[5];

    public:
      stringescaper(bool escQuote_ = true)
        : escQuote(escQuote_)
      { }

      const char* operator() (char ch) const
      {
        if (ch == '\n')
          strcpy(data, "\\n");
        else if (ch == '\t')
          strcpy(data, "\\t");
        else if (ch == '?')
          strcpy(data, "\\?");
        else if (escQuote && ch == '"')
          strcpy(data, "\\\"");
        else if (std::isprint(ch, std::locale()) && ch != '\\')
        {
          data[0] = ch;
          data[1] = '\0';
        }
        else
        {
          data[0] = '\\';
          data[1] = (char)(((unsigned char)ch >> 6) + '0');
          data[2] = (char)((((unsigned char)ch >> 3) & 0x7) + '0');
          data[3] = (char)(((unsigned char)ch & 0x7) + '0');
          data[4] = '\0';
        }

        return data;
      }

      static std::string escape(const std::string& str,
          const stringescaper& se = stringescaper(false));
      static std::string mk_stringconst(const std::string& str,
          unsigned maxcols = 0, const stringescaper& se = stringescaper(true));
  };
}

#endif // TNT_STRINGESCAPER_H

