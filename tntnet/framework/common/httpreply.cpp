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


#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/httpheader.h>
#include <tnt/deflatestream.h>
#include <tnt/httperror.h>
#include <tnt/tntconfig.h>
#include <tnt/htmlescostream.h>
#include <tnt/urlescostream.h>
#include <tnt/encoding.h>
#include <cxxtools/log.h>
#include <cxxtools/md5stream.h>
#include <cxxtools/mutex.h>
#include <sstream>
#include <zlib.h>
#include <netinet/in.h>

namespace tnt
{
  log_define("tntnet.httpreply")

  ////////////////////////////////////////////////////////////////////////
  // HttpReply
  //
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

  //////////////////////////////////////////////////////////////////////
  // HttpReply::Impl
  //

  struct HttpReply::Impl
  {
    std::ostream* socket;
    std::ostringstream outstream;
    HtmlEscOstream safe_outstream;
    UrlEscOstream url_outstream;

    Encoding acceptEncoding;

    unsigned keepAliveCounter;

    bool sendStatusLine;
    bool headRequest;
    bool clearSession;

    Impl(std::ostream& s, bool sendStatusLine);

    struct Pool
    {
      std::vector<Impl*> pool;
      cxxtools::Mutex poolMutex;

      ~Pool()
      {
        for (unsigned n = 0; n < pool.size(); ++n)
          delete pool[n];
      }

      Impl* getInstance(std::ostream& s, bool sendStatusLine);
      void releaseInstance(Impl* inst);
      void clear();
    };

    static Pool pool;

  private:
    Impl(const Impl&);
    Impl& operator=(const Impl&);
  };

  HttpReply::Impl::Pool HttpReply::Impl::pool;

  HttpReply::Impl* HttpReply::Impl::Pool::getInstance(std::ostream& s, bool sendStatusLine)
  {
    cxxtools::MutexLock lock(poolMutex);

    if (pool.empty())
      return new Impl(s, sendStatusLine);

    Impl* impl = pool.back();
    pool.pop_back();

    impl->socket = &s;
    impl->keepAliveCounter = 0;
    impl->sendStatusLine = sendStatusLine;
    impl->headRequest = false;
    impl->clearSession = false;
    impl->acceptEncoding.clear();
    impl->safe_outstream.setSink(impl->outstream.rdbuf());

    return impl;
  }

  void HttpReply::Impl::Pool::releaseInstance(Impl* inst)
  {
    cxxtools::MutexLock lock(poolMutex);
    if (pool.size() < 64)
    {
      inst->outstream.clear();
      inst->outstream.str(std::string());
      inst->safe_outstream.clear();
      inst->url_outstream.clear();
      pool.push_back(inst);
    }
    else
    {
      delete inst;
    }
  }

  void HttpReply::Impl::Pool::clear()
  {
    cxxtools::MutexLock lock(poolMutex);
    for (unsigned n = 0; n < pool.size(); ++n)
      delete pool[n];
    pool.clear();
  }

  HttpReply::Impl::Impl(std::ostream& s, bool sendStatusLine_)
    : socket(&s),
      safe_outstream(outstream),
      url_outstream(outstream),
      keepAliveCounter(0),
      sendStatusLine(sendStatusLine_),
      headRequest(false),
      clearSession(false)
  {
  }

  //////////////////////////////////////////////////////////////////////
  // HttpReply
  //
  HttpReply::HttpReply(std::ostream& s, bool sendStatusLine_)
    : impl(Impl::pool.getInstance(s, sendStatusLine_)),
      current_outstream(&impl->outstream),
      safe_outstream(&impl->safe_outstream),
      url_outstream(&impl->url_outstream)
  {
  }

  HttpReply::~HttpReply()
  {
    Impl::pool.releaseInstance(impl);
  }

  void HttpReply::setHeadRequest(bool sw)
  {
    impl->headRequest = sw;
  }

  void HttpReply::clearSession()
  {
    impl->clearSession = true;
  }

  bool HttpReply::isClearSession() const
  {
    return impl->clearSession;
  }

  void HttpReply::resetContent()
  {
    impl->outstream.str(std::string());
  }

  void HttpReply::rollbackContent(unsigned size)
  {
    impl->outstream.str( impl->outstream.str().substr(0, size) );
    impl->outstream.seekp(size);
  }

  bool HttpReply::isDirectMode() const
  {
    return current_outstream == impl->socket;
  }

  std::string::size_type HttpReply::getContentSize() const
  {
    return impl->outstream.str().size();
  }

  std::ostream& HttpReply::getDirectStream()
  {
    return *impl->socket;
  }

  void HttpReply::setKeepAliveCounter(unsigned c)
  {
    impl->keepAliveCounter = c;
  }

  unsigned HttpReply::getKeepAliveCounter() const
  {
    return impl->keepAliveCounter;
  }

  void HttpReply::setAcceptEncoding(const Encoding& enc)
  {
    impl->acceptEncoding = enc;
  }

  bool HttpReply::tryCompress(std::string& body)
  {
    log_debug("gzip");

    std::string cbody = doCompress(body);

    std::string::size_type oldSize = body.size();
    // only send compressed data, if the data is compressed more than 1/8th
    if (oldSize - (oldSize >> 3) > cbody.size())
    {
      body = cbody;
      log_info("gzip body " << oldSize << " bytes to " << body.size() << " bytes");

      return true;
    }

    return false;
  }

  void HttpReply::postRunCleanup()
  {
    Impl::pool.clear();
  }

  void HttpReply::send(unsigned ret, const char* msg, bool ready) const
  {
    std::string body = impl->outstream.str();

    std::ostream hsocket(impl->socket->rdbuf());
    hsocket.imbue(std::locale::classic());

    // send header

    if (impl->sendStatusLine)
    {
      log_debug("HTTP/" << getMajorVersion() << '.' << getMinorVersion()
             << ' ' << ret << ' ' << msg);
      hsocket << "HTTP/" << getMajorVersion() << '.' << getMinorVersion()
              << ' ' << ret << ' ' << msg << "\r\n";
    }

    if (!hasHeader(httpheader::date))
    {
      std::string current = htdateCurrent();
      log_debug(httpheader::date << ' ' << current);
      hsocket << httpheader::date << ' ' << current << "\r\n";
    }

    if (!hasHeader(httpheader::server))
    {
      log_debug(httpheader::server << ' ' << httpheader::serverName);
      hsocket << httpheader::server << ' ' << httpheader::serverName << "\r\n";
    }

    if (ready)
    {
      if (body.size() >= TntConfig::it().minCompressSize
        && !hasHeader(httpheader::contentEncoding)
        && impl->acceptEncoding.accept("gzip")
        && tryCompress(body))
      {
        log_debug(httpheader::contentEncoding << " gzip");
        hsocket << httpheader::contentEncoding << " gzip\r\n";
      }

      if (!hasHeader(httpheader::contentLength))
      {
        log_debug(httpheader::contentLength << ' ' << body.size());
        hsocket << httpheader::contentLength << ' ' << body.size() << "\r\n";
      }

      if (!hasHeader(httpheader::contentType))
      {
        log_debug(httpheader::contentType << ' ' << TntConfig::it().defaultContentType);
        hsocket << httpheader::contentType << ' ' << TntConfig::it().defaultContentType << "\r\n";
      }

      if (!hasHeader(httpheader::connection))
      {
        if (TntConfig::it().keepAliveTimeout > 0 && getKeepAliveCounter() > 0)
        {
          log_debug(httpheader::keepAlive << " timeout=" << TntConfig::it().keepAliveTimeout << ", max=" << getKeepAliveCounter());
          log_debug(httpheader::connection << ' ' << httpheader::connectionKeepAlive);
          hsocket << httpheader::keepAlive << " timeout=" << TntConfig::it().keepAliveTimeout << ", max=" << getKeepAliveCounter() << "\r\n"
                  << httpheader::connection << ' ' << httpheader::connectionKeepAlive << "\r\n";
        }
        else
        {
          log_debug(httpheader::connection << ' ' << httpheader::connectionClose);
          hsocket << httpheader::connection << ' ' << httpheader::connectionClose << "\r\n";
        }
      }
    }

    for (header_type::const_iterator it = header.begin();
         it != header.end(); ++it)
    {
      log_debug(it->first << ' ' << it->second);
      hsocket << it->first << ' ' << it->second << "\r\n";
    }

    if (hasCookies())
    {
      log_debug(httpheader::setCookie << ' ' << httpcookies);

      for (Cookies::cookies_type::const_iterator it = httpcookies.data.begin();
        it != httpcookies.data.end(); ++it)
      {
        hsocket << httpheader::setCookie << ' ';
        it->second.write(hsocket, it->first);
        hsocket << "\r\n";
      }
    }

    hsocket << "\r\n";

    // send body

    if (impl->headRequest)
      log_debug("HEAD-request - empty body");
    else
    {
      log_debug("send " << body.size() << " bytes body");
      // We send in chunks just to leave to ostream operator from time to time.
      // There are iostream libraries, which do have a global lock, which
      // block other iostreams while writing.
      const unsigned chunkSize = 65536;
      for (unsigned n = 0; n < body.size(); n += chunkSize)
        hsocket.write(body.data() + n, std::min(static_cast<unsigned>(body.size() - n), chunkSize));
    }
  }

  void HttpReply::sendReply(unsigned ret, const char* msg)
  {
    if (!isDirectMode())
    {
      send(ret, msg, true);
      impl->socket->flush();
    }
  }

  void HttpReply::setMd5Sum()
  {
    cxxtools::Md5stream md5;
    md5 << impl->outstream.str().size();
    setHeader(httpheader::contentMD5, md5.getHexDigest());
  }

  unsigned HttpReply::redirect(const std::string& newLocation)
  {
    setHeader(httpheader::location, newLocation);

    impl->outstream.str(std::string());
    impl->outstream << "<html><body>moved to <a href=\"" << newLocation << "\">" << newLocation << "</a></body></html>";

    throw HttpReturn(HTTP_MOVED_TEMPORARILY, "moved temporarily");

    return HTTP_MOVED_TEMPORARILY;
  }

  unsigned HttpReply::notAuthorized(const std::string& realm)
  {
    setHeader(httpheader::wwwAuthenticate, "Basic realm=\"" + realm + '"');

    impl->outstream.str(std::string());
    impl->outstream << "<html><body><h1>not authorized</h1></body></html>";

    throw HttpReturn(HTTP_UNAUTHORIZED, "not authorized");

    return HTTP_UNAUTHORIZED;
  }

  void HttpReply::setContentLengthHeader(size_t size)
  {
    std::ostringstream s;
    s.imbue(std::locale::classic());
    s << size;
    setHeader(httpheader::contentLength, s.str());
  }

  void HttpReply::setKeepAliveHeader()
  {
    log_debug("setKeepAliveHeader()");
    removeHeader(httpheader::connection);
    removeHeader(httpheader::keepAlive);
    if (TntConfig::it().keepAliveTimeout > 0 && getKeepAliveCounter() > 0)
    {
      std::ostringstream s;
      s.imbue(std::locale::classic());
      s << "timeout=" << TntConfig::it().keepAliveTimeout << ", max=" << getKeepAliveCounter();
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
      current_outstream = impl->socket;
      impl->safe_outstream.setSink(*impl->socket);
    }
  }

  void HttpReply::setDirectModeNoFlush()
  {
    current_outstream = impl->socket;
    impl->safe_outstream.setSink(*impl->socket);
  }

  void HttpReply::setCookie(const std::string& name, const Cookie& value)
  {
    log_debug("setCookie(\"" << name << "\",\"" << value.getValue() << "\")");
    tnt::Cookie cookie(value);
    if (!cookie.hasPath())
      cookie.setPath("/");
    httpcookies.setCookie(name, cookie);
  }

  void HttpReply::clearCookie(const std::string& name)
  {
    log_debug("clearCookie(\"" << name << "\")");
    tnt::Cookie cookie;
    cookie.setPath("/");
    httpcookies.clearCookie(name, cookie);
  }

  bool HttpReply::keepAlive() const
  {
    if (isDirectMode())
    {
      header_type::const_iterator it = header.find(httpheader::connection);
      return it != header.end() &&
        tnt::StringCompareIgnoreCase<const char*>(
            it->second,
            httpheader::connectionKeepAlive) == 0;
    }
    else
    {
      return TntConfig::it().keepAliveTimeout > 0 && getKeepAliveCounter() > 0;
    }
  }
}
