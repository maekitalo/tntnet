/* tnt/ctype.h
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#ifndef TNT_FASTCTYPE_H
#define TNT_FASTCTYPE_H

// Defines std::is...-functions here inline without locales.
// They are often much faster than the standard-functions.

namespace tnt
{
  namespace
  {
    inline bool myisdigit(char ch)
    {
      return ch >= '0' && ch <= '9';
    }

    inline bool myisspace(char ch)
    {
      return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
    }

    inline bool myislower(char ch)
    {
      return ch >= 'a' && ch <= 'z';
    }

    inline bool myisupper(char ch)
    {
      return ch >= 'A' && ch <= 'Z';
    }

    inline bool myisalpha(char ch)
    {
      return myislower(ch)
          || myisupper(ch);
    }

    inline bool myisalnum(char ch)
    {
      return myisalpha(ch)
          || myisdigit(ch);
    }

    inline char mytoupper(char ch)
    {
      return myislower(ch) ? ch - 'a' + 'A' : ch;
    }

    inline char mytolower(char ch)
    {
      return myisupper(ch) ? ch - 'A' + 'a' : ch;
    }
  }
}

#endif // TNT_FASTCTYPE_H
