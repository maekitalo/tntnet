/* tnt/httperror.h
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


#ifndef TNT_HTTPERROR_H
#define TNT_HTTPERROR_H

#include <stdexcept>
#include <string>
#include <tnt/httpmessage.h>

namespace tnt
{
  /// HTTP-error-class
  class HttpError : public std::exception, public HttpMessage
  {
      std::string msg;
      std::string body;

    public:
      HttpError(unsigned errcode, const std::string& msg);
      HttpError(unsigned errcode, const std::string& msg, const std::string& b);
      ~HttpError() throw() { }

      const char* what() const throw ()
      { return msg.c_str(); }

      std::string getErrcodeStr() const
      { return msg.substr(0, 3); }

      unsigned getErrcode() const
      {
        return (msg[0] - '0') * 100
             + (msg[1] - '0') * 10
             + (msg[2] - '0');
      }

      std::string getErrmsg() const;

      /// returns the body of the message.
      const std::string& getBody() const        { return body; }
  };

  /// HTTP-error 404
  class NotFoundException : public HttpError
  {
      std::string url;

    public:
      explicit NotFoundException(const std::string& url_);
      ~NotFoundException() throw() { }

      const std::string& getUrl() const  { return url; }
  };

  class NotAuthorized : public HttpError
  {
    public:
      explicit NotAuthorized(const std::string& realm);
  };

  class MovedTemporarily : public HttpError
  {
    public:
      explicit MovedTemporarily(const std::string& url);
  };
}

#endif // TNT_HTTPERROR_H
