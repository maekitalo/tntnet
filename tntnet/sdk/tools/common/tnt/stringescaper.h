/* tnt/stringescaper.h
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

#ifndef STRINGESCAPER_H
#define STRINGESCAPER_H

#include <functional>

class stringescaper : public std::unary_function<const char*, char>
{
    bool escQuote;
    char data[5];

  public:
    stringescaper(bool escQuote_ = true)
      : escQuote(escQuote_)
    { }

    const char* operator() (char ch)
    {
      if (ch == '\n')
        strcpy(data, "\\n");
      else if (ch == '\t')
        strcpy(data, "\\t");
      else if (ch == '?')
        strcpy(data, "\\?");
      else if (escQuote && ch == '"')
        strcpy(data, "\\\"");
      else if (std::isprint(ch) && ch != '\\')
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
};

#endif // STRINGESCAPER_H

