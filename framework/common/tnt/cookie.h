/*
 * Copyright (C) 2005 Tommi Maekitalo
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
    friend class HttpReply;

    public:
      static const std::string maxAge;
      static const std::string comment;
      static const std::string domain;
      static const std::string path;
      static const std::string secure;
      static const std::string version;
      static const std::string expires;

    private:
      typedef std::map<std::string, std::string, StringLessIgnoreCase<std::string> > attrs_type;

      std::string _value;
      attrs_type _attrs;
      bool _secureFlag;

      void write(std::ostream& out, const std::string& name) const;

    public:
      Cookie()
        : _secureFlag(false)
        { }

      Cookie(const std::string& v, unsigned maxAge = 0)
        : _value(v),
          _secureFlag(false)
      {
        if (maxAge)
          setMaxAge(maxAge);
      }
      Cookie(const char* v, unsigned maxAge = 0)
        : _value(v),
          _secureFlag(false)
      {
        if (maxAge)
          setMaxAge(maxAge);
      }

      const std::string& getValue() const { return _value; }

      std::string getAttr(const std::string& name) const
      {
        attrs_type::const_iterator it = _attrs.find(name);
        return it == _attrs.end() ? std::string() : it->second;
      }

      operator const std::string& () const { return _value; }

      void setAttr(const std::string& name, const std::string& value)
        { _attrs[name] = value; }
      bool hasAttr(const std::string& name) const
        { return _attrs.find(name) != _attrs.end(); }

      unsigned    getMaxAge() const;
      std::string getComment() const            { return getAttr(comment); }
      std::string getDomain() const             { return getAttr(domain); }
      std::string getPath() const               { return getAttr(path); }
      std::string getVersion() const            { return getAttr(version); }
      std::string getExpires() const            { return getAttr(expires); }
      bool        isSecure() const              { return _secureFlag; }

      void setMaxAge(unsigned seconds);
      void setComment(const std::string& value) { setAttr(comment, value); }
      void setDomain(const std::string& value)  { setAttr(domain, value); }
      void setPath(const std::string& value)    { setAttr(path, value); }
      void setVersion(const std::string& value) { setAttr(version, value); }
      void setExpires(const std::string& value) { setAttr(expires, value); }
      void setSecure(bool f = true)             { _secureFlag = f; }

      bool hasMaxAge() const                    { return hasAttr(maxAge); }
      bool hasComment() const                   { return hasAttr(comment); }
      bool hasDomain() const                    { return hasAttr(domain); }
      bool hasPath() const                      { return hasAttr(path); }
      bool hasVersion() const                   { return hasAttr(version); }
      bool hasExpires() const                   { return hasAttr(expires); }

  };

  class Cookies
  {
    friend std::ostream& operator<< (std::ostream& out, const Cookies& c);
    friend class HttpReply;

    private:
      typedef std::map<std::string, Cookie, StringLessIgnoreCase<std::string> > cookies_type;
      cookies_type _data;

      static const Cookie _emptyCookie;

    public:
      void set(const std::string& header);
      void clear() { _data.clear(); }

      const Cookie& getCookie(const std::string& name) const
      {
        cookies_type::const_iterator it = _data.find(name);
        return it == _data.end() ? _emptyCookie : it->second;
      }

      void setCookie(const std::string& name, const Cookie& value)
        { _data[name] = value; }

      void clearCookie(const std::string& name);
      void clearCookie(const std::string& name, const Cookie& c);

      bool hasCookie(const std::string& name) const
        { return _data.find(name) != _data.end(); }
      bool hasCookies() const
        { return !_data.empty(); }
  };

  std::ostream& operator<< (std::ostream& out, const Cookies& c);
}

#endif // TNT_COOKIE_H

