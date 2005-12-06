/* 
   Copyright (C) 2005 Tommi Maekitalo

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
      char_type obuffer[512];
      std::streambuf* sink;

    public:
      explicit InflateStreamBuf(std::streambuf* sink_);
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
        : std::ostream(&streambuf),
          streambuf(sink)
          { }
      explicit InflateStream(std::ostream& sink)
        : std::ostream(&streambuf),
          streambuf(sink.rdbuf())
          { }

      void setSink(std::streambuf* sink)   { streambuf.setSink(sink); }
      void setSink(std::ostream& sink)     { streambuf.setSink(sink.rdbuf()); }
      uLong getAdler() const   { return streambuf.getAdler(); }
  };
}

#endif // TNT_INFLATESTREAM_H

