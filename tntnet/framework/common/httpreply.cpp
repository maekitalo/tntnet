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
#include <cxxtools/log.h>
#include <cxxtools/md5stream.h>

namespace tnt
{
  log_define("tntnet.http");

  ////////////////////////////////////////////////////////////////////////
  // httpReply
  //
  unsigned httpReply::keepAliveTimeout = 15000;

  void httpReply::send(unsigned ret)
  {
    log_debug("HTTP/" << getMajorVersion() << '.' << getMinorVersion()
           << ' ' << ret << " OK");
    socket << "HTTP/" << getMajorVersion() << '.' << getMinorVersion()
           << ' ' << ret << " OK" << "\r\n";

    if (!hasHeader(Date))
      setHeader(Date, htdate(time(0)));

    if (!hasHeader(Server))
      setHeader(Server, ServerName);

    if (keepAliveTimeout > 0 && keepAliveCounter > 0)
    {
      if (!hasHeader(Connection))
        setKeepAliveHeader(getKeepAliveTimeout() + 999 / 1000);

      if (!hasHeader(Content_Length))
        setContentLengthHeader(outstream.str().size());
    }
    else
    {
      if (!hasHeader(Connection))
        setKeepAliveHeader(0);
    }

    if (!hasHeader(Content_Type))
      setHeader(Content_Type, contentType);

    for (header_type::const_iterator it = header.begin();
         it != header.end(); ++it)
    {
      log_debug(it->first << ' ' << it->second);
      socket << it->first << ' ' << it->second << "\r\n";
    }

    if (hasCookies())
    {
      log_debug(SetCookie << ' ' << httpcookies);
      socket << SetCookie << ' ' << httpcookies << "\r\n";
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

  httpReply::httpReply(std::ostream& s)
    : contentType("text/html"),
      socket(s),
      current_outstream(&outstream),
      keepAliveCounter(0)
  { }

  void httpReply::sendReply(unsigned ret)
  {
    if (!isDirectMode())
    {
      send(ret);
      socket.flush();
    }
  }

  void httpReply::setMd5Sum()
  {
    cxxtools::md5stream md5;
    md5 << outstream.str().size();
    setHeader(Content_MD5, md5.getHexDigest());
  }

  void httpReply::throwError(unsigned errorCode, const std::string& errorMessage) const
  {
    throw httpError(errorCode, errorMessage);
  }

  void httpReply::throwError(const std::string& errorMessage) const
  {
    throw httpError(errorMessage);
  }

  void httpReply::throwNotFound(const std::string& errorMessage) const
  {
    throw notFoundException(errorMessage);
  }

  unsigned httpReply::redirect(const std::string& newLocation)
  {
    setHeader(Location, newLocation);
    return HTTP_MOVED_TEMPORARILY;
  }

  void httpReply::setContentLengthHeader(size_t size)
  {
    std::ostringstream s;
    s << size;
    setHeader(Content_Length, s.str());
  }

  void httpReply::setKeepAliveHeader(unsigned timeout)
  {
    log_debug("setKeepAliveHeader(" << timeout << ')');
    removeHeader(Connection);
    removeHeader(KeepAlive);
    if (timeout > 0 && getKeepAliveCounter() > 0)
    {
      std::ostringstream s;
      s << "timeout=" << timeout << ", max=" << getKeepAliveCounter();
      setHeader(KeepAlive, s.str());

      setHeader(Connection, Connection_Keep_Alive);
    }
    else
      setHeader(Connection, Connection_close);
  }

  void httpReply::setDirectMode()
  {
    if (!isDirectMode())
    {
      send(HTTP_OK);
      current_outstream = &socket;
    }
  }

  void httpReply::setDirectModeNoFlush()
  {
    current_outstream = &socket;
  }

  void httpReply::setCookie(const std::string& name, const cookie& value)
  {
    log_debug("setCookie(\"" << name << "\",\"" << value.getValue() << "\")");
    httpcookies.setCookie(name, value);
  }

  bool httpReply::keepAlive() const
  {
    if (getKeepAliveCounter() <= 0
        || getKeepAliveTimeout() <= 0)
      return false;

    header_type::const_iterator it = header.find(Connection);
    return it != header.end() && it->second == Connection_Keep_Alive;
  }
}
