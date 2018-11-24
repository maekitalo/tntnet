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
#include <tnt/chunkedostream.h>
#include <tnt/cstream.h>
#include <cxxtools/log.h>
#include <cxxtools/md5stream.h>
#include <cxxtools/mutex.h>
#include <sstream>
#include <stdio.h>
#include <zlib.h>
#include <netinet/in.h>

namespace tnt
{
  log_define("tntnet.httpreply")

  namespace
  {
    class Compressor
    {
        ocstream _zbody;
        DeflateStream _deflator;
        uLong _crc;
        unsigned _size;

      public:
        Compressor()
          : _deflator(_zbody),
            _crc(0),
            _size(0)
        {
        }

        void compress(const char* d, unsigned s)
        {
          _deflator.write(d, s);
          _size += s;
          _crc = crc32(_crc, reinterpret_cast<const Bytef*>(d), s);
        }

        void finalize()
        {
          _deflator.end();
        }

        std::string::size_type uncompressedSize()
        { return _size; }

        std::string::size_type zsize()
        { return _zbody.size(); }

        std::string str() const
        { return _zbody.str(); }

        void output(std::ostream& out)
        { _zbody.output(out); }

        void clear()
        {
          _zbody.makeEmpty();
          _deflator.reinitialize();
          _crc = 0;
          _size = 0;
        }
    };
  }

  ////////////////////////////////////////////////////////////////////////
  // HttpReply
  //

  //////////////////////////////////////////////////////////////////////
  // HttpReply::Impl
  //
  struct HttpReply::Impl
  {
    std::ostream* socket;
    ocstream outstream;
    HtmlEscOstream safeOutstream;
    UrlEscOstream urlOutstream;
    ChunkedOStream chunkedOutstream;
    Compressor compressor;

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
    impl->safeOutstream.setSink(impl->outstream.rdbuf());
    impl->chunkedOutstream.setSink(impl->outstream.rdbuf());

    return impl;
  }

  void HttpReply::Impl::Pool::releaseInstance(Impl* inst)
  {
    cxxtools::MutexLock lock(poolMutex);
    if (pool.size() < 64)
    {
      inst->outstream.clear();
      inst->outstream.makeEmpty();
      inst->safeOutstream.clear();
      inst->urlOutstream.clear();
      inst->chunkedOutstream.clear();
      inst->compressor.clear();
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
      safeOutstream(outstream),
      urlOutstream(outstream),
      chunkedOutstream(s),
      keepAliveCounter(0),
      sendStatusLine(sendStatusLine_),
      headRequest(false),
      clearSession(false)
    { }

  HttpReply::HttpReply(std::ostream& s, bool sendStatusLine)
    : _impl(Impl::pool.getInstance(s, sendStatusLine)),
      _currentOutstream(&_impl->outstream),
      _safeOutstream(&_impl->safeOutstream),
      _urlOutstream(&_impl->urlOutstream)
    { }

  HttpReply::~HttpReply()
    { Impl::pool.releaseInstance(_impl); }

  void HttpReply::setHeadRequest(bool sw)
    { _impl->headRequest = sw; }

  void HttpReply::clearSession()
    { _impl->clearSession = true; }

  bool HttpReply::isClearSession() const
    { return _impl->clearSession; }

  void HttpReply::resetContent()
    { _impl->outstream.makeEmpty(); }

  void HttpReply::rollbackContent(unsigned size)
  {
    _impl->outstream.rollback(size);
  }

  bool HttpReply::isDirectMode() const
    { return _currentOutstream == _impl->socket; }

  std::string::size_type HttpReply::getContentSize() const
    { return _impl->outstream.size(); }

  unsigned HttpReply::chunkedBytesWritten() const
    { return _impl->chunkedOutstream.bytesWritten(); }

  std::ostream& HttpReply::getDirectStream()
    { return *_impl->socket; }

  void HttpReply::setKeepAliveCounter(unsigned c)
    { _impl->keepAliveCounter = c; }

  unsigned HttpReply::getKeepAliveCounter() const
    { return _impl->keepAliveCounter; }

  void HttpReply::setAcceptEncoding(const Encoding& enc)
    { _impl->acceptEncoding = enc; }

  bool HttpReply::tryCompress(std::string& body)
  {
    Compressor compressor;
    compressor.compress(body.data(), body.size());
    compressor.finalize();

    std::string::size_type oldSize = body.size();
    // only send compressed data, if the data is compressed more than 1/8th
    if (oldSize - (oldSize >> 3) > compressor.zsize())
    {
      body = compressor.str();
      log_info("gzip body " << oldSize << " bytes to " << compressor.zsize() << " bytes");

      return true;
    }

    return false;
  }

  void HttpReply::postRunCleanup()
    { Impl::pool.clear(); }

  void HttpReply::sendHttpStatus(std::ostream& hsocket, unsigned ret, const char* msg) const
  {
    if (_impl->sendStatusLine)
    {
      if (msg == 0)
        msg = HttpReturn::httpMessage(ret);

      log_debug("HTTP/" << getMajorVersion() << '.' << getMinorVersion()
             << ' ' << ret << ' ' << msg);
      hsocket << "HTTP/" << getMajorVersion() << '.' << getMinorVersion()
              << ' ' << ret << ' ' << msg << "\r\n";
    }
  }

  void HttpReply::sendHttpHeaders(std::ostream& hsocket) const
  {
    if (!hasHeader(httpheader::date))
    {
      char current[50];
      htdateCurrent(current);
      log_debug(httpheader::date << ' ' << current);
      hsocket << httpheader::date << ' ' << current << "\r\n";
    }

    if (!TntConfig::it().server.empty()
      && !hasHeader(httpheader::server))
    {
      log_debug(httpheader::server << ' ' << TntConfig::it().server);
      hsocket << httpheader::server << ' ' << TntConfig::it().server << "\r\n";
    }

    for (header_type::const_iterator it = header.begin(); it != header.end(); ++it)
    {
      log_debug(it->first << ' ' << it->second);
      hsocket << it->first << ' ' << it->second << "\r\n";
    }

    if (hasCookies())
    {
      log_debug(httpheader::setCookie << ' ' << httpcookies);

      for (Cookies::cookies_type::const_iterator it = httpcookies._data.begin();
        it != httpcookies._data.end(); ++it)
      {
        hsocket << httpheader::setCookie << ' ';
        it->second.write(hsocket, it->first);
        hsocket << "\r\n";
      }
    }

  }

  void HttpReply::send(unsigned ret, const char* msg, bool ready) const
  {
    std::ostream hsocket(_impl->socket->rdbuf());

    // send header
    sendHttpStatus(hsocket, ret, msg);
    sendHttpHeaders(hsocket);

    bool compressed = false;

    if (ready)
    {
      if (_currentOutstream == &_impl->chunkedOutstream)
      {
        log_debug(httpheader::transferEncoding << " chunked");
        hsocket << httpheader::transferEncoding << " chunked\r\n";
      }
      else
      {
        ocstream& body = _impl->outstream;

        if (body.size() >= TntConfig::it().minCompressSize
            && !hasHeader(httpheader::contentEncoding)
            && _impl->acceptEncoding.accept("gzip")
            && !hasHeader(httpheader::contentLength))
        {
          for (unsigned n = 0; n < body.chunkcount(); ++n)
            _impl->compressor.compress(body.chunk(n), body.chunksize(n));
          _impl->compressor.finalize();

          compressed = true;
          log_debug(httpheader::contentEncoding << " gzip");
          log_debug(httpheader::contentLength << ' ' << _impl->compressor.zsize());

          hsocket << httpheader::contentLength << ' ' << _impl->compressor.zsize() << "\r\n"
                  << httpheader::contentEncoding << " gzip\r\n";
          log_info("gzip body " << body.size() << " bytes to " << _impl->compressor.zsize() << " bytes");
        }
        else
        {
          if (!hasHeader(httpheader::contentLength))
          {
            log_debug(httpheader::contentLength << ' ' << body.size());
            hsocket << httpheader::contentLength << ' ' << body.size() << "\r\n";
          }
        }
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

    hsocket << "\r\n";

    // send body
    if (_impl->headRequest)
      log_debug("HEAD-request - empty body");
    else
    {
      ocstream& body = _impl->outstream;
      if (_currentOutstream == &_impl->chunkedOutstream)
      {
        body.output(*_currentOutstream);
      }
      else
      {
        if (compressed)
        {
          log_debug("send " << _impl->compressor.zsize() << " bytes body (compressed)");
          _impl->compressor.output(hsocket);
        }
        else
        {
          log_debug("send " << body.size() << " bytes body");
          body.output(hsocket);
        }
      }
    }
  }

  bool HttpReply::sendReply(unsigned ret, const char* msg)
  {
    log_debug("sendReply");
    if (_currentOutstream == &_impl->chunkedOutstream)
    {
      log_debug("finish chunked encoding");
      _impl->chunkedOutstream.finish();
      *_impl->socket << "\r\n";
    }
    else if (!isDirectMode())
    {
      log_debug("send data");
      send(ret, msg, true);
    }

    _impl->socket->flush();
    return !_impl->socket->fail();
  }

  void HttpReply::setMd5Sum()
  {
    cxxtools::Md5stream md5;
    const tnt::ocstream& c = _impl->outstream;
    for (unsigned n = 0; n < c.chunkcount(); ++n)
      md5.write(c.chunk(n), c.chunksize(n));
    setHeader(httpheader::contentMD5, md5.getHexDigest());
  }

  unsigned HttpReply::redirect(const std::string& newLocation, Redirect type)
  {
    setHeader(httpheader::location, newLocation);

    _impl->outstream.makeEmpty();
    _impl->outstream << "<html><body>moved to <a href=\"" << newLocation << "\">" << newLocation << "</a></body></html>";

    unsigned httpCode = static_cast<unsigned>(type);
    throw HttpReturn(httpCode);

    return httpCode;
  }

  unsigned HttpReply::notAuthorized(const std::string& realm)
  {
    setHeader(httpheader::wwwAuthenticate, "Basic realm=\"" + realm + '"');

    _impl->outstream.makeEmpty();
    _impl->outstream << "<html><body><h1>not authorized</h1></body></html>";

    throw HttpReturn(HTTP_UNAUTHORIZED);

    return HTTP_UNAUTHORIZED;
  }

  void HttpReply::setContentLengthHeader(size_t size)
  {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lu", static_cast<unsigned long>(size));
    header.setHeader(httpheader::contentLength, buffer, true);
  }

  void HttpReply::setKeepAliveHeader()
  {
    log_debug("setKeepAliveHeader()");
    removeHeader(httpheader::connection);
    removeHeader(httpheader::keepAlive);
    if (TntConfig::it().keepAliveTimeout > 0 && getKeepAliveCounter() > 0)
    {
      char buffer[64];
      snprintf(buffer, sizeof(buffer),
        "timeout=%lu, max=%u",
        static_cast<unsigned long>(TntConfig::it().keepAliveTimeout.totalSeconds()),
        static_cast<unsigned>(getKeepAliveCounter()));
      header.setHeader(httpheader::keepAlive, buffer, true);

      header.setHeader(httpheader::connection, httpheader::connectionKeepAlive, true);
    }
    else
      header.setHeader(httpheader::connection, httpheader::connectionClose, true);
  }

  void HttpReply::setMaxAgeHeader(unsigned seconds)
  {
    if (seconds > 0)
    {
      char buffer[64];
      snprintf(buffer, sizeof(buffer), "max-age=%u", seconds);
      header.setHeader(httpheader::cacheControl, buffer, true);
      header.setHeader(httpheader::age, "0", true);
    }
    else
    {
      header.setHeader(httpheader::cacheControl, "no-cache", true);
      header.setHeader(httpheader::pragma, "no-cache", true);
      header.setHeader(httpheader::expires, "-1", true);
    }
  }

  void HttpReply::setDirectMode(unsigned ret, const char* msg)
  {
    if (!isDirectMode())
    {
      log_debug("enable direct mode");
      send(ret, msg, false);
      setDirectModeNoFlush();
    }
  }

  void HttpReply::setDirectModeNoFlush()
  {
    _currentOutstream = _impl->socket;
    _impl->safeOutstream.setSink(*_impl->socket);
    _impl->urlOutstream.setSink(*_impl->socket);
  }

  void HttpReply::setChunkedEncoding(unsigned ret, const char* msg)
  {
    log_debug("set chunked encoding");
    _currentOutstream = &_impl->chunkedOutstream;
    _impl->chunkedOutstream.setSink(*_impl->socket);
    _impl->safeOutstream.setSink(_impl->chunkedOutstream.rdbuf());
    _impl->urlOutstream.setSink(_impl->chunkedOutstream.rdbuf());
    send(ret, msg, true);
  }

  bool HttpReply::isChunkedEncoding() const
  {
    return _currentOutstream == &_impl->chunkedOutstream;
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
