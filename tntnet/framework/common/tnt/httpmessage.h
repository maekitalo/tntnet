/* tnt/httpmessage.h
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

#ifndef TNT_HTTPMESSAGE_H
#define TNT_HTTPMESSAGE_H

#include <tnt/messageheader.h>
#include <tnt/cookie.h>

namespace tnt
{
  /// baseclass for HTTP-messages
  class httpMessage
  {
    public:
      class parser;
      friend class parser;

      typedef messageheader header_type;

      static const std::string Content_Type;
      static const std::string Content_Length;
      static const std::string Connection;
      static const std::string Connection_close;
      static const std::string Connection_Keep_Alive;
      static const std::string Last_Modified;
      static const std::string Server;
      static const std::string ServerName;
      static const std::string Location;
      static const std::string AcceptLanguage;
      static const std::string AcceptEncoding;
      static const std::string ContentEncoding;
      static const std::string Date;
      static const std::string KeepAlive;
      static const std::string IfModifiedSince;
      static const std::string Host;
      static const std::string CacheControl;
      static const std::string Content_MD5;
      static const std::string SetCookie;
      static const std::string Cookie;
      static const std::string Pragma;
      static const std::string Expires;

    private:
      std::string method;
      std::string url;
      std::string query_string;
      std::string body;
      unsigned short major_version;
      unsigned short minor_version;

      size_t content_size;

      static size_t maxRequestSize;

    protected:
      header_type header;
      cookies httpcookies;

    public:
      httpMessage()
        : major_version(0),
          minor_version(0)
        { }
      virtual ~httpMessage()
      { }

      void clear();

      const std::string& getMethod() const      { return method; }
      void setMethod(const std::string& m)      { method = m; }
      /// return url with get-parameter
      std::string getQuery() const
        { return query_string.empty() ? url : url + '?' + query_string; }
      const std::string& getUrl() const         { return url; }
      const std::string& getQueryString() const { return query_string; }
      bool hasHeader(const std::string& key)    { return header.find(key) != header.end(); }
      std::string getHeader(const std::string& key,
        const std::string& def = std::string()) const;
      const std::string& getBody() const        { return body; }

      unsigned short getMajorVersion() const
        { return major_version; }
      unsigned short getMinorVersion() const
        { return minor_version; }
      void setVersion(unsigned short major, unsigned short minor)
        { major_version = major; minor_version = minor; }

      size_t getContentSize() const
        { return content_size; }
      std::string getVirtualHost() const
        { return getHeader(Host); }

      header_type::const_iterator header_begin() const
        { return header.begin(); }
      header_type::const_iterator header_end() const
        { return header.end(); }

      void setHeader(const std::string& key, const std::string& value);
      void removeHeader(const std::string& key)
        { header.erase(key); }

      std::string dumpHeader() const;
      void dumpHeader(std::ostream& out) const;

      static std::string htdate(time_t t);
      static std::string htdate(struct tm* tm);

      static bool checkUrl(const std::string& url);

      static void setMaxRequestSize(size_t s)    { maxRequestSize = s; }
  };
}

#endif // TNT_HTTPMESSAGE_H
