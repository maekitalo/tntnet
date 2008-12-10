/*
 * Copyright (C) 2005 Tommi Maekitalo
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


#ifndef TNT_INFLATESTREAM_H
#define TNT_INFLATESTREAM_H

#include <iostream>
#include <stdexcept>
#include <zlib.h>

namespace tnt
{
  class InflateError : public std::runtime_error
  {
      int zRet;

    public:
      InflateError(int zRet_, const std::string& msg)
        : std::runtime_error(msg),
          zRet(zRet_)
          { }

      int getRet() const  { return zRet; }
  };

  class InflateStreamBuf : public std::streambuf
  {
      z_stream stream;
      char_type* obuffer;
      unsigned bufsize;
      std::streambuf* sink;

    public:
      explicit InflateStreamBuf(std::streambuf* sink_, unsigned bufsize = 8192);
      ~InflateStreamBuf();

      /// see std::streambuf
      int_type overflow(int_type c);
      /// see std::streambuf
      int_type underflow();
      /// see std::streambuf
      int sync();

      void setSink(std::streambuf* sink_)   { sink = sink_; }
      uLong getAdler() const   { return stream.adler; }
  };

  class InflateStream : public std::ostream
  {
      InflateStreamBuf streambuf;

    public:
      explicit InflateStream(std::streambuf* sink)
        : std::ostream(0),
          streambuf(sink)
        { init(&streambuf); }
      explicit InflateStream(std::ostream& sink)
        : std::ostream(0),
          streambuf(sink.rdbuf())
        { init(&streambuf); }

      void setSink(std::streambuf* sink)   { streambuf.setSink(sink); }
      void setSink(std::ostream& sink)     { streambuf.setSink(sink.rdbuf()); }
      uLong getAdler() const   { return streambuf.getAdler(); }
  };
}

#endif // TNT_INFLATESTREAM_H

