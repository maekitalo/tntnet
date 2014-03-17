/*
 * Copyright (C) 2014 Tommi Maekitalo
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

#ifndef TNT_CHUNKEDOSTREAM_H
#define TNT_CHUNKEDOSTREAM_H

#include <iostream>

namespace tnt
{
  class ChunkedWriter : public std::streambuf
  {
    private:
      std::streambuf* _obuf;
      char* _buffer;
      unsigned _bufsize;

    public:
      explicit ChunkedWriter(std::streambuf* obuf, unsigned bufsize = 8192)
        : _obuf(obuf),
          _buffer(0),
          _bufsize(bufsize)
        { }

      ~ChunkedWriter()
        { delete _buffer; }

      virtual int sync();
      virtual int_type overflow(int_type ch);
      virtual int_type underflow();

      void finish();

      void setSink(std::streambuf* obuf)
        { _obuf = obuf; }
  };

  class ChunkedOStream : public std::ostream
  {
    private:
      ChunkedWriter _streambuf;

    public:
      explicit ChunkedOStream(std::ostream& sink)
        : _streambuf(sink.rdbuf())
        { std::ostream::init(&_streambuf); }

      explicit ChunkedOStream(std::streambuf* obuf)
        : _streambuf(obuf)
        { std::ostream::init(&_streambuf); }

      void finish()
        { _streambuf.finish(); }

      void setSink(std::ostream& sink)
        { _streambuf.setSink(sink.rdbuf()); }

      void setSink(std::streambuf* sink)
        { _streambuf.setSink(sink); }
  };
}

#endif // TNT_CHUNKEDOSTREAM_H

