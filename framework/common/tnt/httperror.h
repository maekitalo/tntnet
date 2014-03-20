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


#ifndef TNT_HTTPERROR_H
#define TNT_HTTPERROR_H

#include <stdexcept>
#include <string>
#include <tnt/httpmessage.h>

namespace tnt
{
  class HttpReturn
  {
    private:
      unsigned _returncode;
      const char* _msg;

    public:
      explicit HttpReturn(unsigned errcode);
      HttpReturn(unsigned errcode, const char* msg);

      unsigned getReturnCode() const { return _returncode; }

      const char* getMessage() const { return _msg; }

      static const char* httpMessage(unsigned httpstatus);
  };

  /// HTTP-error-class
  class HttpError : public std::exception, public HttpMessage
  {
    private:
      std::string _msg;
      std::string _body;

    public:
      explicit HttpError(unsigned errcode);
      HttpError(unsigned errcode, const std::string& msg);
      HttpError(unsigned errcode, const std::string& msg, const std::string& b);
      ~HttpError() throw() { }

      const char* what() const throw ()
        { return _msg.c_str(); }

      std::string getErrcodeStr() const
        { return _msg.substr(0, 3); }

      unsigned getErrcode() const
      {
        return (_msg[0] - '0') * 100
             + (_msg[1] - '0') * 10
             + (_msg[2] - '0');
      }

      std::string getErrmsg() const;

      /// returns the body of the message.
      const std::string& getBody() const { return _body; }
  };

  /// HTTP-error 404
  class NotFoundException : public HttpError
  {
    private:
      std::string _url;
      std::string _vhost;

    public:
      explicit NotFoundException(const std::string& url, const std::string& vhost = std::string());
      ~NotFoundException() throw() { }

      const std::string& getUrl() const   { return _url; }
      const std::string& getVHost() const { return _vhost; }
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

