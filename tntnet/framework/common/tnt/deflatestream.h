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

#ifndef TNT_DEFLATESTREAM_H
#define TNT_DEFLATESTREAM_H

#include <iostream>
#include <stdexcept>
#include <zlib.h>

namespace tnt
{
  class DeflateError : public std::runtime_error
  {
      int zRet;

    public:
      DeflateError(int zRet_, const std::string& msg)
        : std::runtime_error(msg),
          zRet(zRet_)
          { }

      int getRet() const  { return zRet; }
  };

  class DeflateStreamBuf : public std::streambuf
  {
      z_stream stream;
      char_type* obuffer;
      unsigned bufsize;
      std::streambuf* sink;

    public:
      explicit DeflateStreamBuf(std::streambuf* sink_, int level = Z_DEFAULT_COMPRESSION, unsigned bufsize = 512);
      ~DeflateStreamBuf();

      /// see std::streambuf
      int_type overflow(int_type c);
      /// see std::streambuf
      int_type underflow();
      /// see std::streambuf
      int sync();

      void setSink(std::streambuf* sink_)   { sink = sink_; }
      uLong getAdler() const                { return stream.adler; }
  };

  class DeflateStream : public std::ostream
  {
      DeflateStreamBuf streambuf;

    public:
      explicit DeflateStream(std::streambuf* sink, int level = Z_DEFAULT_COMPRESSION)
        : std::ostream(&streambuf),
          streambuf(sink, level)
          { }
      explicit DeflateStream(std::ostream& sink, int level = Z_DEFAULT_COMPRESSION)
        : std::ostream(&streambuf),
          streambuf(sink.rdbuf(), level)
          { }

      void setSink(std::streambuf* sink)   { streambuf.setSink(sink); }
      void setSink(std::ostream& sink)     { streambuf.setSink(sink.rdbuf()); }
      uLong getAdler() const   { return streambuf.getAdler(); }
  };
}

#endif // TNT_DEFLATESTREAM_H

