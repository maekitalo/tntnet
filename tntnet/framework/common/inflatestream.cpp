/* inflatestream.cpp
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


#include "tnt/inflatestream.h"
#include <cxxtools/log.h>
#include <cxxtools/dynbuffer.h>
#include <sstream>

log_define("tntnet.inflatestream")

namespace tnt
{
  namespace
  {
    void checkError(int ret, z_stream& stream)
    {
      if (ret != Z_OK && ret != Z_STREAM_END)
      {
        log_error("InflateError " << ret << ": \"" << (stream.msg ? stream.msg : "") << '"');
        std::ostringstream msg;
        msg << "inflate-error " << ret;
        if (stream.msg)
          msg << ": " << stream.msg;
        throw InflateError(ret, msg.str());
      }
    }
  }

  InflateStreamBuf::InflateStreamBuf(std::streambuf* sink_, unsigned bufsize_)
    : obuffer(new char_type[bufsize_]),
      bufsize(bufsize_),
      sink(sink_)
  {
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = 0;
    stream.total_out = 0;
    stream.total_in = 0;

    checkError(::inflateInit(&stream), stream);
    setp(obuffer, obuffer + bufsize);
  }

  InflateStreamBuf::~InflateStreamBuf()
  {
    ::inflateEnd(&stream);
    delete obuffer;
  }

  InflateStreamBuf::int_type InflateStreamBuf::overflow(int_type c)
  {
    log_debug("InflateStreamBuf::overflow");

    // initialize input-stream for
    stream.next_in = (Bytef*)obuffer;
    stream.avail_in = pptr() - pbase();

    do
    {
      // initialize zbuffer
      cxxtools::Dynbuffer<Bytef> zbuffer(bufsize);
      stream.next_out = zbuffer.data();
      stream.avail_out = bufsize;

      log_debug("pre:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);
      checkError(::inflate(&stream, Z_SYNC_FLUSH), stream);
      log_debug("post:avail_out=" << stream.avail_out << " avail_in=" << stream.avail_in);

      // copy zbuffer to sink
      std::streamsize count = bufsize - stream.avail_out;
      std::streamsize n = sink->sputn(reinterpret_cast<char*>(zbuffer.data()), count);
      if (n < count)
        return traits_type::eof();
    } while (stream.avail_in > 0);

    // reset outbuffer
    setp(obuffer, obuffer + bufsize);
    if (c != traits_type::eof())
      sputc(traits_type::to_char_type(c));

    return 0;
  }

  InflateStreamBuf::int_type InflateStreamBuf::underflow()
  {
    return traits_type::eof();
  }

  int InflateStreamBuf::sync()
  {
    if (overflow(traits_type::eof()) == traits_type::eof())
      return -1;
    return 0;
  }
}
