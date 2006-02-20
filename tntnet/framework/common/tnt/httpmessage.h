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
#include <tnt/httpheader.h>
#include <tnt/cookie.h>
#include <time.h>

namespace tnt
{
  /// Baseclass for HTTP-messages.
  class HttpMessage
  {
    public:
      class Parser;
      friend class Parser;

      typedef Messageheader header_type;

    private:
      std::string method;
      std::string url;
      std::string queryString;
      std::string body;
      unsigned short majorVersion;
      unsigned short minorVersion;

      size_t contentSize;

      static size_t maxRequestSize;

    protected:
      header_type header;
      Cookies httpcookies;

    public:
      HttpMessage()
        : majorVersion(1),
          minorVersion(0)
        { }
      virtual ~HttpMessage()
      { }

      /// Removes all request-specific content.
      virtual void clear();

      /// Returns the http-method (normally GET or POST) of a request.
      const std::string& getMethod() const      { return method; }
      /// sets the http-method of this request.
      void setMethod(const std::string& m)      { method = m; }
      /// returns url with get-parameters.
      std::string getQuery() const
        { return queryString.empty() ? url : url + '?' + queryString; }
      /// returns the request-url without parameters.
      const std::string& getUrl() const         { return url; }
      /// returns get-parameters as string.
      const std::string& getQueryString() const { return queryString; }
      /// sets query-string
      void setQueryString(const std::string& queryString_)
        { queryString = queryString_; }
      /// returns true, if the message has the specified header.
      bool hasHeader(const std::string& key)    { return header.find(key) != header.end(); }
      /// returns the content of the specified header or the passed default
      /// when not set.
      std::string getHeader(const std::string& key,
        const std::string& def = std::string()) const;
      /// returns the body of the message.
      const std::string& getBody() const        { return body; }
      /// sets the body of the message.
      void setBody(const std::string& body_)    { body = body_; }

      /// returns the http-major-version-number.
      unsigned short getMajorVersion() const
        { return majorVersion; }
      /// returns the http-minor-version-number.
      unsigned short getMinorVersion() const
        { return minorVersion; }
      /// sets the http-version-number
      void setVersion(unsigned short major, unsigned short minor)
        { majorVersion = major; minorVersion = minor; }

      /// returns the value of the content-size-header as read from the client.
      size_t getContentSize() const
        { return contentSize; }
      /// returns the virtual-host-header of this request.
      std::string getVirtualHost() const
        { return getHeader(httpheader::host); }

      /// Returns a constant Iterator, which points to the first header.
      /// The value of the iterator is a std::pair<std::string, std::string>.
      header_type::const_iterator header_begin() const
        { return header.begin(); }
      /// Returns a constant Iterator, which points past the last header.
      header_type::const_iterator header_end() const
        { return header.end(); }

      /// Adds the specified header to the message.
      void setHeader(const std::string& key, const std::string& value);
      /// Removes the header with the specified name from the message.
      void removeHeader(const std::string& key)
        { header.erase(key); }

      /// Returns all headers as a string.
      std::string dumpHeader() const;
      /// Prints all headers to the specified output-stream.
      void dumpHeader(std::ostream& out) const;

      /// Returns a properly formatted date-string, as needed in http.
      static std::string htdate(time_t t);
      /// Returns a properly formatted date-string, as needed in http.
      static std::string htdate(struct ::tm* tm);
      /// Returns a properly formatted current time-string, as needed in http.
      static std::string htdateCurrent();

      /// Checks for double-dot-url. Returns false, if the url used as
      /// a filename would escape from the basedir.
      static bool checkUrl(const std::string& url);

      /// Sets a limit for a maximum request size.
      static void setMaxRequestSize(size_t s)    { maxRequestSize = s; }
  };
}

#endif // TNT_HTTPMESSAGE_H
