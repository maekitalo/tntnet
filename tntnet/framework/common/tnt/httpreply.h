/* tnt/httpreply.h
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

#ifndef TNT_HTTPREPLY_H
#define TNT_HTTPREPLY_H

#include <tnt/httpmessage.h>
#include <tnt/htmlescostream.h>
#include <sstream>

namespace tnt
{
  class savepoint;

  /// HTTP-Reply-message
  class httpReply : public httpMessage
  {
      friend class savepoint;

      std::string contentType;
      std::ostream& socket;
      std::ostringstream outstream;
      std::ostream* current_outstream;
      HtmlEscOstream save_outstream;

      unsigned keepAliveCounter;
      static unsigned keepAliveTimeout;

      void send(unsigned ret);

    public:
      explicit httpReply(std::ostream& s);

      void setContentType(const std::string& t)    { contentType = t; }
      const std::string& getContentType() const    { return contentType; }

      virtual void throwError(unsigned errorCode, const std::string& errorMessage) const;
      virtual void throwError(const std::string& errorMessage) const;
      virtual void throwNotFound(const std::string& errorMessage) const;
      unsigned redirect(const std::string& newLocation);

      void sendReply(unsigned ret);

      /// returns outputstream
      std::ostream& out()   { return *current_outstream; }
      /// returns save outputstream (unsave html-chars are escaped)
      std::ostream& sout()  { return save_outstream; }

      void setContentLengthHeader(size_t size);
      void setKeepAliveHeader(unsigned timeout = 15);

      virtual void setDirectMode();
      virtual void setDirectModeNoFlush();
      virtual bool isDirectMode() const
        { return current_outstream == &socket; }
      std::string::size_type getContentSize() const
        { return outstream.str().size(); }

      void setMd5Sum();

      void setCookie(const std::string& name, const cookie& value);
      void setCookie(const std::string& name, const std::string& value, unsigned seconds)
        { setCookie(name, cookie(value, seconds)); }
      void setCookies(const cookies& c)
        { httpcookies = c; }
      void clearCookie(const std::string& name)
        { httpcookies.clearCookie(name); }
      void clearCookie(const std::string& name, const cookie& c)
        { httpcookies.clearCookie(name, c); }
      bool hasCookies() const
        { return httpcookies.hasCookies(); }
      const cookies& getCookies() const
        { return httpcookies; }

      void setKeepAliveCounter(unsigned c)  { keepAliveCounter = c; }
      unsigned getKeepAliveCounter() const  { return keepAliveCounter; }
      static void setKeepAliveTimeout(unsigned ms)  { keepAliveTimeout = ms; }
      static unsigned getKeepAliveTimeout()         { return keepAliveTimeout; }

      bool keepAlive() const;
  };

  /// HTTP-error-class
  class httpError : public std::exception
  {
      std::string msg;

    public:
      httpError(const std::string& m)
        : msg(m)
        { }

      httpError(unsigned errcode, const std::string& msg);
      ~httpError() throw ()
        { }

      const char* what() const throw ()
      { return msg.c_str(); }
  };

  /// HTTP-error 404
  class notFoundException : public httpError
  {
    public:
      notFoundException(const std::string& url)
        : httpError(404, "Not Found (" + url + ')')
        { }
  };
}

#endif // TNT_HTTPREPLY_H
