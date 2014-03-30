/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "tnt/deflatestream.h"
#include <cxxtools/log.h>
#include <sstream>
#include <string.h>

log_define("tntnet.deflatestream")

namespace tnt
{
  namespace
  {
    int checkError(int ret, z_stream& stream)
    {
      if (ret != Z_OK && ret != Z_STREAM_END)
      {
        log_error("DeflateError " << ret << ": \"" << (stream.msg ? stream.msg : "") << '"');
        std::ostringstream msg;
        msg << "deflate-error " << ret;
        if (stream.msg)
          msg << ": " << stream.msg;
        throw DeflateError(ret, msg.str());
      }
      return ret;
    }
  }

  DeflateStreamBuf::DeflateStreamBuf(std::streambuf* sink, int level, unsigned bufsize)
    : _obuffer(bufsize),
      _sink(sink)
  {
    memset(&_stream, 0, sizeof(z_stream));
    _stream.zalloc = Z_NULL;
    _stream.zfree = Z_NULL;
    _stream.opaque = 0;
    _stream.total_out = 0;
    _stream.total_in = 0;
    _stream.next_in = Z_NULL;
    _stream.next_out = Z_NULL;
    _stream.avail_in = 0;
    _stream.avail_out = 0;

    int strategy = Z_DEFAULT_STRATEGY;
    level = 6;
    checkError(::deflateInit2(&_stream, level, Z_DEFLATED, -MAX_WBITS, 8, strategy), _stream);

    setp(&_obuffer[0], &_obuffer[0] + _obuffer.size());
  }

  DeflateStreamBuf::~DeflateStreamBuf()
  {
    ::deflateEnd(&_stream);
  }

  DeflateStreamBuf::int_type DeflateStreamBuf::overflow(int_type c)
  {
    // initialize input-stream
    _stream.next_in = reinterpret_cast<Bytef*>(&_obuffer[0]);
    _stream.avail_in = pptr() - &_obuffer[0];

    // initialize zbuffer for deflated data
    char zbuffer[8192];
    _stream.next_out = reinterpret_cast<Bytef*>(zbuffer);
    _stream.avail_out = sizeof(zbuffer);

    // deflate
    checkError(::deflate(&_stream, Z_NO_FLUSH), _stream);

    // copy zbuffer to sink / consume deflated data
    std::streamsize count = sizeof(zbuffer) - _stream.avail_out;
    if (count > 0)
    {
      std::streamsize n = _sink->sputn(zbuffer, count);
      if (n < count)
        return traits_type::eof();
    }

    // move remaining characters to start of obuffer
    if (_stream.avail_in > 0)
      memmove(&_obuffer[0], _stream.next_in, _stream.avail_in);

    // reset outbuffer
    setp(&_obuffer[0] + _stream.avail_in, &_obuffer[0] + _obuffer.size());
    if (c != traits_type::eof())
      sputc(traits_type::to_char_type(c));

    return 0;
  }

  DeflateStreamBuf::int_type DeflateStreamBuf::underflow()
  {
    return traits_type::eof();
  }

  int DeflateStreamBuf::sync()
  {
    // initialize input-stream for
    _stream.next_in = reinterpret_cast<Bytef*>(&_obuffer[0]);
    _stream.avail_in = pptr() - pbase();
    char zbuffer[8192];
    while (_stream.avail_in > 0)
    {
      // initialize zbuffer
      _stream.next_out = (Bytef*)zbuffer;
      _stream.avail_out = sizeof(zbuffer);

      checkError(::deflate(&_stream, Z_SYNC_FLUSH), _stream);

      // copy zbuffer to sink
      std::streamsize count = sizeof(zbuffer) - _stream.avail_out;
      if (count > 0)
      {
        std::streamsize n = _sink->sputn(zbuffer, count);
        if (n < count)
          return -1;
      }
    };

    // reset outbuffer
    setp(&_obuffer[0], &_obuffer[0] + _obuffer.size());
    return 0;
  }

  int DeflateStreamBuf::end()
  {
    char zbuffer[8192];
    // initialize input-stream for
    _stream.next_in = reinterpret_cast<Bytef*>(&_obuffer[0]);
    _stream.avail_in = pptr() - pbase();
    while (true)
    {
      // initialize zbuffer
      _stream.next_out = (Bytef*)zbuffer;
      _stream.avail_out = sizeof(zbuffer);

      int ret = checkError(::deflate(&_stream, Z_FINISH), _stream);

      // copy zbuffer to sink
      std::streamsize count = sizeof(zbuffer) - _stream.avail_out;
      if (count > 0)
      {
        std::streamsize n = _sink->sputn(zbuffer, count);
        if (n < count)
          return -1;
      }
      if (ret == Z_STREAM_END)
        break;
    };

    // reset outbuffer
    setp(&_obuffer[0], &_obuffer[0] + _obuffer.size());
    return 0;
  }

  void DeflateStream::end()
  {
    if (_streambuf.end() != 0)
      setstate(failbit);
  }
}
