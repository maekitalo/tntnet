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


#ifndef TNT_HTTPREPLY_H
#define TNT_HTTPREPLY_H

#include <tnt/httpmessage.h>
#include <tnt/htmlescostream.h>
#include <tnt/urlescostream.h>
#include <tnt/encoding.h>
#include <tnt/http.h>
#include <sstream>

namespace tnt
{
  class Savepoint;

  /// HTTP-Reply-message
  class HttpReply : public HttpMessage
  {
      friend class Savepoint;

      std::ostream& socket;
      std::ostringstream outstream;
      std::ostream* current_outstream;
      HtmlEscOstream safe_outstream;
      UrlEscOstream url_outstream;

      Encoding acceptEncoding;

      unsigned keepAliveCounter;
      static unsigned keepAliveTimeout;
      static unsigned minCompressSize;
      static std::string defaultContentType;

      bool sendStatusLine;
      bool headRequest;

      void send(unsigned ret, const char* msg, bool ready) const;

    public:
      explicit HttpReply(std::ostream& s, bool sendStatusLine = true);

      static bool tryCompress(std::string& body);

      void setContentType(const char* t)            { setHeader(httpheader::contentType, t); }
      void setContentType(const std::string& t)     { setHeader(httpheader::contentType, t); }
      const char* getContentType() const            { return getHeader(httpheader::contentType); }

      void setHeadRequest(bool sw = true)           { headRequest = sw; }

      /// Throws an exception, which results in a redirect.
      unsigned redirect(const std::string& newLocation);
      /// Throws an exception, which results in a login dialog in the browser.
      unsigned notAuthorized(const std::string& realm);
      /// alias for notAuthorized
      unsigned notAuthorised(const std::string& realm) { return notAuthorized(realm); }

      void sendReply(unsigned ret, const char* msg = "OK");
      void sendReply(unsigned ret, const std::string& msg)
        { sendReply(ret, msg.c_str()); }

      /// returns outputstream
      std::ostream& out()   { return *current_outstream; }
      /// returns safe outputstream (unsafe html-chars are escaped)
      std::ostream& sout()  { return safe_outstream; }
      /// returns outputstream, which url encodes output
      std::ostream& uout()  { return url_outstream; }
      void resetContent()   { outstream.str(std::string()); }

      void setContentLengthHeader(size_t size);
      void setKeepAliveHeader();

      virtual void setDirectMode(unsigned ret = HTTP_OK, const char* msg = "OK");
      virtual void setDirectModeNoFlush();
      virtual bool isDirectMode() const
        { return current_outstream == &socket; }
      std::string::size_type getContentSize() const
        { return outstream.str().size(); }
      std::ostream& getDirectStream()   { return socket; }

      void setMd5Sum();

      void setCookie(const std::string& name, const Cookie& value);
      void setCookie(const std::string& name, const std::string& value, unsigned seconds)
        { setCookie(name, Cookie(value, seconds)); }
      void setCookies(const Cookies& c)
        { httpcookies = c; }
      void clearCookie(const std::string& name);
      void clearCookie(const std::string& name, const Cookie& c)
        { httpcookies.clearCookie(name, c); }
      bool hasCookies() const
        { return httpcookies.hasCookies(); }
      const Cookies& getCookies() const
        { return httpcookies; }

      void setKeepAliveCounter(unsigned c)          { keepAliveCounter = c; }
      unsigned getKeepAliveCounter() const          { return keepAliveCounter; }

      static void setKeepAliveTimeout(unsigned ms)  { keepAliveTimeout = ms; }
      static unsigned getKeepAliveTimeout()         { return keepAliveTimeout; }

      static void setMinCompressSize(unsigned s)    { minCompressSize = s; }
      static unsigned getMinCompressSize()          { return minCompressSize; }

      static void setDefaultContentType(const std::string& ct) { defaultContentType = ct; }
      static const std::string& getDefaultContentType()        { return defaultContentType; }

      void setAcceptEncoding(const Encoding& enc)    { acceptEncoding = enc; }

      bool keepAlive() const;

      void setLocale(const std::locale& loc)    { out().imbue(loc); sout().imbue(loc); }
  };
}

#endif // TNT_HTTPREPLY_H
