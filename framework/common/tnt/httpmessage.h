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
  /// Base class for HTTP messages
  class HttpMessage
  {
    public:
      typedef Messageheader header_type;

    private:
      unsigned short _majorVersion;
      unsigned short _minorVersion;

    protected:
      header_type header;
      Cookies httpcookies;

    public:
      HttpMessage()
        : _majorVersion(1),
          _minorVersion(0)
        { }
      virtual ~HttpMessage()
        { }

      /// Remove all request-specific content
      virtual void clear();

      /// @{
      /// Check whether the message has the specified header
      bool hasHeader(const char* key) const        { return header.hasHeader(key); }
      bool hasHeader(const std::string& key) const { return header.hasHeader(key); }
      /// @}

      /// Get the content of the specified header if it is set,
      /// the passed default otherwise
      const char* getHeader(const char* key, const char* def = "") const;

      /// Get the major http version number
      unsigned short getMajorVersion() const
        { return _majorVersion; }

      /// Get the minor http version number
      unsigned short getMinorVersion() const
        { return _minorVersion; }

      /// Set the http version number
      void setVersion(unsigned short majorVersion, unsigned short minorVersion)
        { _majorVersion = majorVersion; _minorVersion = minorVersion; }

      /** Get a constant iterator which points to the first header

          The value type of the iterator is std::pair<std::string, std::string>
       */
      header_type::const_iterator header_begin() const
        { return header.begin(); }

      /// Get a constant iterator which points past the last header
      header_type::const_iterator header_end() const
        { return header.end(); }

      /// Add the specified header to the message
      void setHeader(const std::string& key, const std::string& value, bool replace = true)
        { header.setHeader(key, value, replace); }

      /// Remove the specified header from the message
      void removeHeader(const std::string& key)
        { header.removeHeader(key); }

      /// Get all headers in one string
      std::string dumpHeader() const;

      /// Print all headers to the specified output stream
      void dumpHeader(std::ostream& out) const;

      /// @{
      /// Get a date string, formatted as needed in http
      static std::string htdate(time_t t);
      static std::string htdate(struct ::tm* tm);
      /// @}

      /// Get a string for the current time, formatted as needed in http
      static std::string htdateCurrent();

      // TODO: Documentation revision: Is this meant to check for absolute URLs?
      /** Check for double-dot-url

          @return false if the url used as a filename would escape from the basedir
       */
      static bool checkUrl(const std::string& url);
  };
}

#endif // TNT_HTTPMESSAGE_H

