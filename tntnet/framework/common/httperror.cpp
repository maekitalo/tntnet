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

  HttpError::HttpError(const std::string& m)
    : msg(m)
  {
  }

  HttpError::HttpError(unsigned errcode, const std::string& msg)
    : msg(httpErrorFormat(errcode, msg))
  {
  }

  std::string HttpError::getErrmsg() const
  {
    std::string::size_type p = msg.find('\n', 4);
    return p == std::string::npos ? msg.substr(4) : msg.substr(4, p - 4);
  }

  NotFoundException::NotFoundException(const std::string& url_)
    : HttpError("404 Not Found (" + url_ + ')'),
      url(url_)
  {
  }
}
