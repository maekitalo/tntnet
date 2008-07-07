/* httperror.cpp
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


#include <tnt/httperror.h>
#include <tnt/http.h>

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // HttpError
  //

  namespace
  {
    std::string httpErrorFormat(unsigned errcode, const std::string& msg)
    {
      char d[3];
      d[2] = '0' + errcode % 10;
      errcode /= 10;
      d[1] = '0' + errcode % 10;
      errcode /= 10;
      d[0] = '0' + errcode % 10;
      std::string ret(d, 3);
      ret += ' ';
      ret += msg;
      return ret;
    }
  }

  HttpError::HttpError(unsigned errcode, const std::string& m)
    : msg(httpErrorFormat(errcode, m)),
      body("<html><body><h1>Error</h1><p>" + m + "</p></body></html>")
  {
  }

  HttpError::HttpError(unsigned errcode, const std::string& m, const std::string& b)
    : msg(httpErrorFormat(errcode, m)),
      body(b)
  {
  }

  std::string HttpError::getErrmsg() const
  {
    std::string::size_type p = msg.find('\n', 4);
    return p == std::string::npos ? msg.substr(4) : msg.substr(4, p - 4);
  }

  NotFoundException::NotFoundException(const std::string& url_)
    : HttpError(HTTP_NOT_FOUND, "not found (" + url_ + ')'),
      url(url_)
  {
  }

  NotAuthorized::NotAuthorized(const std::string& realm)
    : HttpError(HTTP_UNAUTHORIZED, "not authorized", "<html><body><h1>not authorized</h1></body></html>")
  {
    setHeader(httpheader::wwwAuthenticate, "Basic realm=\"" + realm + '"');
  }

  MovedTemporarily::MovedTemporarily(const std::string& url)
    : HttpError(HTTP_MOVED_TEMPORARILY,
                "moved temporarily",
                "<html><body>moved to <a href=\"" + url + "\">" + url + "</a></body></html>")
  {
    setHeader(httpheader::location, url);
  }
}
