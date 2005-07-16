/* httpreply.cpp
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

#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/httpheader.h>
#include <cxxtools/log.h>
#include <cxxtools/md5stream.h>

namespace tnt
{
  log_define("tntnet.http")

  ////////////////////////////////////////////////////////////////////////
  // HttpReply
  //
  unsigned HttpReply::keepAliveTimeout = 15000;

  void HttpReply::send(unsigned ret)
  {
    log_debug("HTTP/" << getMajorVersion() << '.' << getMinorVersion()
           << ' ' << ret << " OK");
    socket << "HTTP/" << getMajorVersion() << '.' << getMinorVersion()
           << ' ' << ret << " OK" << "\r\n";

    if (!hasHeader(httpheader::date))
      setHeader(httpheader::date, htdate(time(0)));

    if (!hasHeader(httpheader::server))
      setHeader(httpheader::server, httpheader::serverName);

    if (keepAliveTimeout > 0 && keepAliveCounter > 0)
    {
      if (!hasHeader(httpheader::connection))
        setKeepAliveHeader(getKeepAliveTimeout() + 999 / 1000);

      if (!hasHeader(httpheader::contentLength))
        setContentLengthHeader(outstream.str().size());
    }
    else
    {
      if (!hasHeader(httpheader::connection))
        setKeepAliveHeader(0);
    }

    if (!hasHeader(httpheader::contentType))
      setHeader(httpheader::contentType, contentType);

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

    if (getMethod() == "HEAD")
      log_debug("HEAD-request - empty body");
    else
    {
      log_debug("send " << outstream.str().size()
             << " bytes body, method=" << getMethod());
      socket << outstream.str();
    }
  }

  HttpReply::HttpReply(std::ostream& s)
    : contentType("text/html"),
      socket(s),
      current_outstream(&outstream),
      save_outstream(outstream),
      keepAliveCounter(0)
  { }

  void HttpReply::sendReply(unsigned ret)
  {
    if (!isDirectMode())
    {
      send(ret);
      socket.flush();
    }
  }

  void HttpReply::setMd5Sum()
  {
    cxxtools::Md5stream md5;
    md5 << outstream.str().size();
    setHeader(httpheader::contentMD5, md5.getHexDigest());
  }

  void HttpReply::throwError(unsigned errorCode, const std::string& errorMessage) const
  {
    throw HttpError(errorCode, errorMessage);
  }

  void HttpReply::throwError(const std::string& errorMessage) const
  {
    throw HttpError(errorMessage);
  }

  void HttpReply::throwNotFound(const std::string& errorMessage) const
  {
    throw NotFoundException(errorMessage);
  }

  unsigned HttpReply::redirect(const std::string& newLocation)
  {
    setHeader(httpheader::location, newLocation);
    return HTTP_MOVED_TEMPORARILY;
  }

  void HttpReply::setContentLengthHeader(size_t size)
  {
    std::ostringstream s;
    s << size;
    setHeader(httpheader::contentLength, s.str());
  }

  void HttpReply::setKeepAliveHeader(unsigned timeout)
  {
    log_debug("setKeepAliveHeader(" << timeout << ')');
    removeHeader(httpheader::connection);
    removeHeader(httpheader::keepAlive);
    if (timeout > 0 && getKeepAliveCounter() > 0)
    {
      std::ostringstream s;
      s << "timeout=" << timeout << ", max=" << getKeepAliveCounter();
      setHeader(httpheader::keepAlive, s.str());

      setHeader(httpheader::connection, httpheader::connectionKeepAlive);
    }
    else
      setHeader(httpheader::connection, httpheader::connectionClose);
  }

  void HttpReply::setDirectMode()
  {
    if (!isDirectMode())
    {
      send(HTTP_OK);
      current_outstream = &socket;
      save_outstream.setSink(socket);
    }
  }

  void HttpReply::setDirectModeNoFlush()
  {
    current_outstream = &socket;
    save_outstream.setSink(socket);
  }

  void HttpReply::setCookie(const std::string& name, const Cookie& value)
  {
    log_debug("setCookie(\"" << name << "\",\"" << value.getValue() << "\")");
    httpcookies.setCookie(name, value);
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
