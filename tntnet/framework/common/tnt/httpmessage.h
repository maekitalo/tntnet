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
      typedef Messageheader header_type;

    private:
      unsigned short majorVersion;
      unsigned short minorVersion;

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

      /// returns true, if the message has the specified header.
      bool hasHeader(const char* key) const        { return header.hasHeader(key); }
      bool hasHeader(const std::string& key) const { return header.hasHeader(key); }
      /// returns the content of the specified header or the passed default
      /// when not set.
      const char* getHeader(const char* key, const char* def = "") const;

      /// returns the http-major-version-number.
      unsigned short getMajorVersion() const
        { return majorVersion; }
      /// returns the http-minor-version-number.
      unsigned short getMinorVersion() const
        { return minorVersion; }
      /// sets the http-version-number
      void setVersion(unsigned short major, unsigned short minor)
        { majorVersion = major; minorVersion = minor; }

      /// Returns a constant Iterator, which points to the first header.
      /// The value of the iterator is a std::pair<std::string, std::string>.
      header_type::const_iterator header_begin() const
        { return header.begin(); }
      /// Returns a constant Iterator, which points past the last header.
      header_type::const_iterator header_end() const
        { return header.end(); }

      /// Adds the specified header to the message.
      void setHeader(const std::string& key, const std::string& value, bool replace = true)
        { header.setHeader(key, value, replace); }
      /// Removes the header with the specified name from the message.
      void removeHeader(const std::string& key)
        { header.removeHeader(key); }

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
  };
}

#endif // TNT_HTTPMESSAGE_H
