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
#include <tnt/http.h>
#include <iosfwd>

namespace tnt
{
  class Savepoint;
  class Encoding;

  /// HTTP reply message
  class HttpReply : public HttpMessage
  {
    private:
      struct Impl;
      Impl* _impl;
      std::ostream* _current_outstream;
      std::ostream* _safe_outstream;
      std::ostream* _url_outstream;

      void send(unsigned ret, const char* msg, bool ready) const;

    public:
      explicit HttpReply(std::ostream& s, bool sendStatusLine = true);
      ~HttpReply();

      static bool tryCompress(std::string& body);
      static void postRunCleanup();

      void setContentType(const char* t)        { setHeader(httpheader::contentType, t); }
      void setContentType(const std::string& t) { setHeader(httpheader::contentType, t); }
      const char* getContentType() const        { return getHeader(httpheader::contentType); }

      void setHeadRequest(bool sw = true);

      /// Configure the session to be cleared after the current request
      void clearSession();

      /// Check if the session is configured to be cleared after the current request
      bool isClearSession() const;

      enum Redirect { permanently = HTTP_MOVED_PERMANENTLY, temporarily = HTTP_MOVED_TEMPORARILY };
      /// @{
      /** Redirect the user to another URL

          This function never returns as it throws an exception to actually send the redirect header.
       */
      unsigned redirect(const std::string& newLocation, Redirect type = temporarily);
      unsigned redirectTemporary(const std::string& newLocation)
        { return redirect(newLocation, temporarily); }
      unsigned redirectPermantently(const std::string& newLocation)
        { return redirect(newLocation, permanently); }
      /// @}

      /** Request authorization from the client

          This will result in a dialog being shown in the user's web browser.
       */
      unsigned notAuthorized(const std::string& realm);

      // TODO: Why does this exist?
      /// alias for notAuthorized
      unsigned notAuthorised(const std::string& realm) { return notAuthorized(realm); }

      void sendReply(unsigned ret, const char* msg = "OK");
      void sendReply(unsigned ret, const std::string& msg)
        { sendReply(ret, msg.c_str()); }

      /// Get output stream
      std::ostream& out()    { return *_current_outstream; }

      /// Get safe output stream (unsafe html characters written into this stream are escaped)
      std::ostream& sout()   { return *_safe_outstream; }

      /// Get url output stream (everything written into this stream is url-encoded)
      std::ostream& uout()   { return *_url_outstream; }

      void resetContent();
      void rollbackContent(unsigned size);

      void setContentLengthHeader(size_t size);
      void setKeepAliveHeader();
      void setMaxAgeHeader(unsigned seconds);

      virtual void setDirectMode(unsigned ret = HTTP_OK, const char* msg = "OK");
      virtual void setDirectModeNoFlush();
      virtual bool isDirectMode() const;
      std::string::size_type getContentSize() const;
      std::ostream& getDirectStream();

      /** Enable chunked encoding for the current request

          When enabling chunked encoding, the content is sent immediately in
          chunks instead of collecting content into a string before sending.
          After enabling chunked encoding, it must be ensured that no exceptions
          are thrown. Also, headers are sent immediately after enabling chunked
          encoding and hence setting headers must happen before calling this method.

          Furthermore, session cookies cannot be sent with chunked encoding enabled,
          hence when a new session is created, chunked encoding must not be used.
          Sessions are created automatically when a session variable is used and
          no session cookie was received.
      */
      void setChunkedEncoding(unsigned ret = HTTP_OK, const char* msg = "OK");

      /// Check whether chunked encoding is enabled
      bool isChunkedEncoding() const;

      // TODO: Documentation revision
      /** Sets the content-md5 header.

          The current content is used to calculate the md5 header. Hence no
          output must be created after calling that method. Normally there is
          no good reason to call that method at all.
      */
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

      void setKeepAliveCounter(unsigned c);
      unsigned getKeepAliveCounter() const;

      void setAcceptEncoding(const Encoding& enc);

      bool keepAlive() const;

      void setLocale(const std::locale& loc)
        { out().imbue(loc); sout().imbue(loc); }
  };
}

#endif // TNT_HTTPREPLY_H

