/* deflatestream.cpp
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

#include "tnt/deflatestream.h"
#include <cxxtools/log.h>
#include <sstream>

log_define("tntnet.deflatestream");

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

  DeflateStreamBuf::DeflateStreamBuf(std::streambuf* sink_, int level, unsigned bufsize_)
    : sink(sink_),
      obuffer(bufsize_)
  {
    memset(&stream, 0, sizeof(z_stream));
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = 0;
    stream.total_out = 0;
    stream.total_in = 0;
    stream.next_in = Z_NULL;
    stream.next_out = Z_NULL;
    stream.avail_in = 0;
    stream.avail_out = 0;

    //checkError(::deflateInit(&stream, level), stream);
    int strategy = Z_DEFAULT_STRATEGY;
    level = 6;
    checkError(::deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, 8, strategy), stream);

    setp(obuffer.begin(), obuffer.end());
  }

  DeflateStreamBuf::~DeflateStreamBuf()
  {
    ::deflateEnd(&stream);
  }

  DeflateStreamBuf::int_type DeflateStreamBuf::overflow(int_type c)
  {
    log_debug("DeflateStreamBuf::overflow");

    // initialize input-stream for
    stream.next_in = (Bytef*)obuffer.data();
    stream.avail_in = pptr() - pbase();

    char zbuffer[8192];
    while (stream.avail_in > 0)
    {
      // initialize zbuffer
      stream.next_out = (Bytef*)zbuffer;
      stream.avail_out = sizeof(zbuffer);

      log_debug("pre:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);
      checkError(::deflate(&stream, Z_NO_FLUSH), stream);
      log_debug("post:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);

      // copy zbuffer to sink
      std::streamsize count = sizeof(zbuffer) - stream.avail_out;
      if (count > 0)
      {
        std::streamsize n = sink->sputn(zbuffer, count);
        if (n < count)
          return traits_type::eof();
      }
    }

    // reset outbuffer
    setp(obuffer.begin(), obuffer.end());
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
    stream.next_in = (Bytef*)obuffer.data();
    stream.avail_in = pptr() - pbase();
    char zbuffer[8192];
    while (stream.avail_in > 0)
    {
      // initialize zbuffer
      stream.next_out = (Bytef*)zbuffer;
      stream.avail_out = sizeof(zbuffer);

      log_debug("pre:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);
      checkError(::deflate(&stream, Z_SYNC_FLUSH), stream);
      log_debug("post:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);

      // copy zbuffer to sink
      std::streamsize count = sizeof(zbuffer) - stream.avail_out;
      if (count > 0)
      {
        std::streamsize n = sink->sputn(zbuffer, count);
        if (n < count)
          return -1;
      }
    };

    // reset outbuffer
    setp(obuffer.begin(), obuffer.end());
    return 0;
  }

  int DeflateStreamBuf::end()
  {
    char zbuffer[8192];
    // initialize input-stream for
    stream.next_in = (Bytef*)obuffer.data();
    stream.avail_in = pptr() - pbase();
    while (true)
    {
      // initialize zbuffer
      stream.next_out = (Bytef*)zbuffer;
      stream.avail_out = sizeof(zbuffer);

      log_debug("pre:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);
      int ret = checkError(::deflate(&stream, Z_FINISH), stream);
      log_debug("post:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);

      // copy zbuffer to sink
      std::streamsize count = sizeof(zbuffer) - stream.avail_out;
      if (count > 0)
      {
        std::streamsize n = sink->sputn(zbuffer, count);
        if (n < count)
          return -1;
      }
      if (ret == Z_STREAM_END)
        break;
    };

    // reset outbuffer
    setp(obuffer.begin(), obuffer.end());
    return 0;
  }

  void DeflateStream::end()
  {
    if (streambuf.end() != 0)
      setstate(failbit);
  }
}
