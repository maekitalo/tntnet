/* tnt/httperror.h
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

#ifndef TNT_HTTPERROR_H
#define TNT_HTTPERROR_H

#include <stdexcept>
#include <string>

namespace tnt
{
  /// HTTP-error-class
  class HttpError : public std::exception
  {
      std::string msg;

    public:
      HttpError(const std::string& m)
        : msg(m)
        { }
      ~HttpError() throw() { }

      HttpError(unsigned errcode, const std::string& msg);

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
  };

  /// HTTP-error 404
  class NotFoundException : public HttpError
  {
      std::string url;
    public:
      NotFoundException(const std::string& url_)
        : HttpError("404 Not Found (" + url_ + ')'),
          url(url_)
        { }
      ~NotFoundException() throw() { }

      const std::string& getUrl() const  { return url; }
  };
}

#endif // TNT_HTTPERROR_H
