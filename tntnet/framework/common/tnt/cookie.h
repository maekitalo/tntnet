/* tnt/cookie.h
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_COOKIE_H
#define TNT_COOKIE_H

#include <string>
#include <map>
#include <iosfwd>
#include <tnt/stringlessignorecase.h>

namespace tnt
{
  class Cookies;
  class CookieParser;

  class Cookie
  {
      friend std::ostream& operator<< (std::ostream& out, const Cookies& c);
      friend class CookieParser;

    public:
      static const std::string maxAge;
      static const std::string comment;
      static const std::string domain;
      static const std::string path;
      static const std::string secure;
      static const std::string version;
      static const std::string expires;

    private:
      typedef std::map<std::string, std::string, StringLessIgnoreCase> attrs_type;
      std::string value;
      attrs_type attrs;
      bool secureFlag;

    public:
      Cookie()
        : secureFlag(false)
      { }

      Cookie(const std::string& v, unsigned maxAge = 0)
        : value(v),
          secureFlag(false)
      {
        if (maxAge)
          setMaxAge(maxAge);
      }
      Cookie(const char* v, unsigned maxAge = 0)
        : value(v),
          secureFlag(false)
      {
        if (maxAge)
          setMaxAge(maxAge);
      }

      const std::string& getValue() const  { return value; }

      std::string getAttr(const std::string& name) const
      {
        attrs_type::const_iterator it = attrs.find(name);
        return it == attrs.end() ? std::string() : it->second;
      }

      operator const std::string& () const { return value; }

      void setAttr(const std::string& name, const std::string& value)
        { attrs[name] = value; }

      unsigned    getMaxAge() const;
      std::string getComment() const { return getAttr(comment); }
      std::string getDomain() const  { return getAttr(domain); }
      std::string getPath() const    { return getAttr(path); }
      std::string getVersion() const { return getAttr(version); }
      std::string getExpires() const { return getAttr(expires); }
      bool        isSecure() const   { return secureFlag; }

      void setMaxAge(unsigned seconds);
      void setComment(const std::string& value)  { setAttr(comment, value); }
      void setDomain(const std::string& value)   { setAttr(domain, value); }
      void setPath(const std::string& value)     { setAttr(path, value); }
      void setVersion(const std::string& value)  { setAttr(version, value); }
      void setExpires(const std::string& value)  { setAttr(expires, value); }
      void setSecure(bool f = true)              { secureFlag = f; }
  };

  class Cookies
  {
      friend std::ostream& operator<< (std::ostream& out, const Cookies& c);
      friend class cookie_parser;

      typedef std::map<std::string, Cookie, StringLessIgnoreCase> cookies_type;
      cookies_type data;

      static const Cookie emptyCookie;

    public:
      void set(const std::string& header);
      void clear()  { data.clear(); }

      const Cookie& getCookie(const std::string& name) const
      {
        cookies_type::const_iterator it = data.find(name);
        return it == data.end() ? emptyCookie : it->second;
      }

      void setCookie(const std::string& name, const Cookie& value)
        { data[name] = value; }

      void clearCookie(const std::string& name);
      void clearCookie(const std::string& name, const Cookie& c);

      bool hasCookie(const std::string& name) const
        { return data.find(name) != data.end(); }
      bool hasCookies() const
        { return !data.empty(); }
  };

  std::ostream& operator<< (std::ostream& out, const Cookies& c);
}

#endif // TNT_COOKIE_H

