/* tnt/htmlescostream.h
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


#ifndef TNT_HTMLESCOSTREAM_H
#define TNT_HTMLESCOSTREAM_H

#include <iostream>

namespace tnt
{
  class HtmlEscStreamBuf : public std::streambuf
  {
      std::streambuf* sink;

      std::streambuf::int_type overflow(std::streambuf::int_type ch);
      std::streambuf::int_type underflow();
      int sync();

    public:
      HtmlEscStreamBuf(std::streambuf* sink_)
        : sink(sink_)
        { }

      void setSink(std::streambuf* sink_)
        { sink = sink_; }
  };

  class HtmlEscOstream : public std::ostream
  {
      HtmlEscStreamBuf streambuf;

    public:
      HtmlEscOstream(std::ostream& sink)
        : std::ostream(0),
          streambuf(sink.rdbuf())
        { rdbuf(&streambuf); }
      HtmlEscOstream(std::streambuf* sink)
        : std::ostream(0),
          streambuf(sink)
        { rdbuf(&streambuf); }

      void setSink(std::ostream& sink)
        { streambuf.setSink(sink.rdbuf()); }
      void setSink(std::streambuf* sink)
        { streambuf.setSink(sink); }
  };
}

#endif // TNT_HTMLESCOSTREAM_H

