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
#include <tnt/encoding.h>
#include <sstream>

namespace tnt
{
  class Savepoint;

  /// HTTP-Reply-message
  class HttpReply : public HttpMessage
  {
      friend class Savepoint;

      std::string contentType;
      std::ostream& socket;
      std::ostringstream outstream;
      std::ostream* current_outstream;
      HtmlEscOstream save_outstream;

      Encoding acceptEncoding;

      unsigned keepAliveCounter;
      static unsigned keepAliveTimeout;

      bool sendStatusLine;

      void tryCompress(std::string& body);
      void send(unsigned ret);

    public:
      explicit HttpReply(std::ostream& s, bool sendStatusLine = true);

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

      void setCookie(const std::string& name, const Cookie& value);
      void setCookie(const std::string& name, const std::string& value, unsigned seconds)
        { setCookie(name, Cookie(value, seconds)); }
      void setCookies(const Cookies& c)
        { httpcookies = c; }
      void clearCookie(const std::string& name)
        { httpcookies.clearCookie(name); }
      void clearCookie(const std::string& name, const Cookie& c)
        { httpcookies.clearCookie(name, c); }
      bool hasCookies() const
        { return httpcookies.hasCookies(); }
      const Cookies& getCookies() const
        { return httpcookies; }

      void setKeepAliveCounter(unsigned c)  { keepAliveCounter = c; }
      unsigned getKeepAliveCounter() const  { return keepAliveCounter; }
      static void setKeepAliveTimeout(unsigned ms)  { keepAliveTimeout = ms; }
      static unsigned getKeepAliveTimeout()         { return keepAliveTimeout; }

      void setAcceptEncoding(const Encoding& enc)    { acceptEncoding = enc; }
      void setAcceptEncoding(const std::string& enc) { acceptEncoding.parse(enc); }

      bool keepAlive() const;
  };
}

#endif // TNT_HTTPREPLY_H
