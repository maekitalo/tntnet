/* tnt/httpreply.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef TNT_HTTPREPLY_H
#define TNT_HTTPREPLY_H

#include <tnt/httpmessage.h>
#include <tnt/htmlescostream.h>
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

      std::string contentType;
      std::ostream& socket;
      std::ostringstream outstream;
      std::ostream* current_outstream;
      HtmlEscOstream save_outstream;

      Encoding acceptEncoding;

      unsigned keepAliveCounter;
      static unsigned keepAliveTimeout;
      static unsigned minCompressSize;

      bool sendStatusLine;

      void tryCompress(std::string& body);
      void send(unsigned ret, const char* msg);

    public:
      explicit HttpReply(std::ostream& s, bool sendStatusLine = true);

      void setContentType(const std::string& t)    { contentType = t; }
      const std::string& getContentType() const    { return contentType; }

      virtual void throwError(unsigned errorCode, const std::string& errorMessage) const;
      virtual void throwError(const std::string& errorMessage) const;
      virtual void throwNotFound(const std::string& errorMessage) const;
      unsigned redirect(const std::string& newLocation);

      void sendReply(unsigned ret, const char* msg = "OK");
      void sendReply(unsigned ret, const std::string& msg)
        { sendReply(ret, msg.c_str()); }

      /// returns outputstream
      std::ostream& out()   { return *current_outstream; }
      /// returns save outputstream (unsave html-chars are escaped)
      std::ostream& sout()  { return save_outstream; }

      void setContentLengthHeader(size_t size);
      void setKeepAliveHeader(unsigned timeout = 15);

      virtual void setDirectMode(unsigned ret = HTTP_OK, const char* msg = "OK");
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

      void setKeepAliveCounter(unsigned c)          { keepAliveCounter = c; }
      unsigned getKeepAliveCounter() const          { return keepAliveCounter; }

      static void setKeepAliveTimeout(unsigned ms)  { keepAliveTimeout = ms; }
      static unsigned getKeepAliveTimeout()         { return keepAliveTimeout; }

      static void setMinCompressSize(unsigned s)    { minCompressSize = s; }
      static unsigned getMinCompressSize()          { return minCompressSize; }

      static void setCompressCacheSize(unsigned s);
      static unsigned getCompressCacheSize();

      void setAcceptEncoding(const Encoding& enc)    { acceptEncoding = enc; }
      void setAcceptEncoding(const std::string& enc) { acceptEncoding.parse(enc); }

      bool keepAlive() const;
  };
}

#endif // TNT_HTTPREPLY_H
