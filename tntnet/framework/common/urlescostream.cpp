/* urlescostream.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "tnt/urlescostream.h"
#include <sstream>

namespace tnt
{
  std::streambuf::int_type UrlEscStreamBuf::overflow(std::streambuf::int_type ch)
  {
    static char hex[] = "0123456789ABCDEF";
    if (ch > 32 && ch < 127 && ch != '%' && ch != '+' && ch != '=' && ch != '&')
      sink->sputc(ch);
    else if (ch == ' ')
      sink->sputc('+');
    else
    {
      sink->sputc('%');
      char hi = (ch >> 4) & 0x0f;
      char lo = ch & 0x0f;
      sink->sputc(hex[hi]);
      sink->sputc(hex[lo]);
    }
  }

  std::streambuf::int_type UrlEscStreamBuf::underflow()
  {
    return traits_type::eof();
  }

  int UrlEscStreamBuf::sync()
  {
    return sink->pubsync();
  }

  std::string urlEscape(const std::string& str)
  {
    std::ostringstream s;
    UrlEscOstream e(s);
    e << str;
    return s.str();
  }

}
