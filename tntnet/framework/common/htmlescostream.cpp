/* htmlescostream.h
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


#include "tnt/htmlescostream.h"

namespace tnt
{
  std::streambuf::int_type HtmlEscStreamBuf::overflow(std::streambuf::int_type ch)
  {
    switch (ch)
    {
      case '<': return sink->sputn("&lt;", 4);
      case '>': return sink->sputn("&gt;", 4);
      case '&': return sink->sputn("&amp;", 5);
      case '"': return sink->sputn("&quot;", 6);
      case '\'': return sink->sputn("&#39;", 5);
      default: return sink->sputc(ch);
    }
  }

  std::streambuf::int_type HtmlEscStreamBuf::underflow()
  {
    return traits_type::eof();
  }

  int HtmlEscStreamBuf::sync()
  {
    return sink->pubsync();
  }

}
