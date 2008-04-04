/* httpreply.cpp
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


#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/httpheader.h>
#include <tnt/deflatestream.h>
#include <tnt/httperror.h>
#include <cxxtools/log.h>
#include <cxxtools/md5stream.h>
#include <cxxtools/dynbuffer.h>
#include <zlib.h>
#include <netinet/in.h>

namespace tnt
{
  log_define("tntnet.httpreply")

  ////////////////////////////////////////////////////////////////////////
  // HttpReply
  //
  unsigned HttpReply::keepAliveTimeout = 15000;
  unsigned HttpReply::minCompressSize = 1024;
  std::string HttpReply::defaultContentType = "text/html; charset=iso-8859-1";

  namespace
  {
    std::string doCompress(const std::string& body)
    {
      std::string ret;

      std::ostringstream b;
      char f[] = "\x1f\x8b\x08\x00"
           "\x00\x00\x00\x00"
           "\x04\x03";
      b.write(f, sizeof(f) - 1);

      DeflateStream deflator(b);
      deflator.write(body.data(), body.size());
      deflator.end();

      uLong crc = crc32(0, reinterpret_cast<const Bytef*>(body.data()), body.size());
      uint32_t u = crc;
      b.put(static_cast<char>(u & 0xFF));
      b.put(static_cast<char>((u >>= 8) & 0xFF));
      b.put(static_cast<char>((u >>= 8) & 0xFF));
      b.put(static_cast<char>((u >>= 8) & 0xFF));

      u = body.size();
      b.put(static_cast<char>(u & 0xFF));
      b.put(static_cast<char>((u >>= 8) & 0xFF));
      b.put(static_cast<char>((u >>= 8) & 0xFF));
      b.put(static_cast<char>((u >>= 8) & 0xFF));

      return b.str();
    }
  }

  void HttpReply::tryCompress(std::string& body)
  {
    if (body.size() >= minCompressSize && !hasHeader(httpheader::contentEncoding))
    {
      if (acceptEncoding.accept("gzip"))
      {
        log_debug("gzip");

        std::string cbody = doCompress(body);

        std::string::size_type oldSize = body.size();
        // only send compressed data, if the data is compressed more than 1/8th
        if (oldSize - (oldSize >> 3) > cbody.size())
        {
          body = cbody;
          log_info("gzip body " << oldSize << " bytes to " << body.size() << " bytes");

          setHeader(httpheader::contentEncoding, "gzip");
        }
      }
    }
  }

  void HttpReply::send(unsigned ret, const char* msg, bool ready)
  {
    std::string body = outstream.str();

    // complete headers

    if (!hasHeader(httpheader::date))
      setHeader(httpheader::date, htdateCurrent());

    if (!hasHeader(httpheader::server))
      setHeader(httpheader::server, httpheader::serverName);

    if (ready)
    {
      tryCompress(body);

      if (!hasHeader(httpheader::connection))
        setKeepAliveHeader();

      if (!hasHeader(httpheader::contentLength))
        setContentLengthHeader(body.size());
    }

    // send header

    if (sendStatusLine)
    {
      log_debug("HTTP/" << getMajorVersion() << '.' << getMinorVersion()
             << ' ' << ret << ' ' << msg);
      socket << "HTTP/" << getMajorVersion() << '.' << getMinorVersion()
             << ' ' << ret << ' ' << msg << "\r\n";
    }

    for (header_type::const_iterator it = header.begin();
         it != header.end(); ++it)
    {
      log_debug(it->first << ' ' << it->second);
      socket << it->first << ' ' << it->second << "\r\n";
    }

    if (hasCookies())
    {
      log_debug(httpheader::setCookie << ' ' << httpcookies);
      socket << httpheader::setCookie << ' ' << httpcookies << "\r\n";
    }

    socket << "\r\n";

    // send body

    if (headRequest)
      log_debug("HEAD-request - empty body");
    else
    {
      log_debug("send " << body.size() << " bytes body");
      // We send in chunks just to leave to ostream operator from time to time.
      // There are iostream libraries, which do have a global lock, which
      // block other iostreams while writing.
      const unsigned chunkSize = 65536;
      for (unsigned n = 0; n < body.size(); n += chunkSize)
        socket.write(body.data() + n, std::min(static_cast<unsigned>(body.size() - n), chunkSize));
    }
  }

  HttpReply::HttpReply(std::ostream& s, bool sendStatusLine_)
    : socket(s),
      current_outstream(&outstream),
      safe_outstream(outstream),
      url_outstream(outstream),
      keepAliveCounter(0),
      sendStatusLine(sendStatusLine_),
      headRequest(false)
  {
    setHeader(httpheader::contentType, defaultContentType);
  }

  void HttpReply::sendReply(unsigned ret, const char* msg)
  {
    if (!isDirectMode())
    {
      send(ret, msg, true);
      socket.flush();
    }
  }

  void HttpReply::setMd5Sum()
  {
    cxxtools::Md5stream md5;
    md5 << outstream.str().size();
    setHeader(httpheader::contentMD5, md5.getHexDigest());
  }

  unsigned HttpReply::redirect(const std::string& newLocation)
  {
    throw MovedTemporarily(newLocation);
    return HTTP_MOVED_TEMPORARILY;
  }

  unsigned HttpReply::notAuthorized(const std::string& realm)
  {
    throw NotAuthorized(realm);
    return HTTP_UNAUTHORIZED;
  }

  void HttpReply::setContentLengthHeader(size_t size)
  {
    std::ostringstream s;
    s << size;
    setHeader(httpheader::contentLength, s.str());
  }

  void HttpReply::setKeepAliveHeader()
  {
    log_debug("setKeepAliveHeader()");
    removeHeader(httpheader::connection);
    removeHeader(httpheader::keepAlive);
    if (keepAliveTimeout > 0 && getKeepAliveCounter() > 0)
    {
      std::ostringstream s;
      s << "timeout=" << keepAliveTimeout << ", max=" << getKeepAliveCounter();
      setHeader(httpheader::keepAlive, s.str());

      setHeader(httpheader::connection, httpheader::connectionKeepAlive);
    }
    else
      setHeader(httpheader::connection, httpheader::connectionClose);
  }

  void HttpReply::setDirectMode(unsigned ret, const char* msg)
  {
    if (!isDirectMode())
    {
      send(ret, msg, false);
      current_outstream = &socket;
      safe_outstream.setSink(socket);
    }
  }

  void HttpReply::setDirectModeNoFlush()
  {
    current_outstream = &socket;
    safe_outstream.setSink(socket);
  }

  void HttpReply::setCookie(const std::string& name, const Cookie& value)
  {
    log_debug("setCookie(\"" << name << "\",\"" << value.getValue() << "\")");
    tnt::Cookie cookie(value);
    cookie.setPath("/");
    httpcookies.setCookie(name, cookie);
  }

  bool HttpReply::keepAlive() const
  {
    if (getKeepAliveCounter() <= 0
        || getKeepAliveTimeout() <= 0)
      return false;

    header_type::const_iterator it = header.find(httpheader::connection);
    return it != header.end() && it->second == httpheader::connectionKeepAlive;
  }
}
