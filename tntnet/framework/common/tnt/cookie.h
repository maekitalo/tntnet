/* tnt/cookie.h
   Copyright (C) 2005 Tommi Maekitalo

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

#ifndef TNT_COOKIE_H
#define TNT_COOKIE_H

#include <string>
#include <map>
#include <iosfwd>

namespace tnt
{
  class cookies;
  class cookie_parser;

  class cookie
  {
      friend std::ostream& operator<< (std::ostream& out, const cookies& c);
      friend class cookie_parser;

    public:
      static const std::string MaxAge;
      static const std::string Comment;
      static const std::string Domain;
      static const std::string Path;
      static const std::string Secure;
      static const std::string Version;
      static const std::string Expires;

    private:
      typedef std::map<std::string, std::string> attrs_type;
      std::string value;
      attrs_type attrs;
      bool secure;

    public:
      cookie()
        : secure(false)
      { }

      cookie(const std::string& v)
        : value(v),
          secure(false)
      { }
      cookie(const char* v)
        : value(v),
          secure(false)
      { }

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
      std::string getComment() const { return getAttr(Comment); }
      std::string getDomain() const  { return getAttr(Domain); }
      std::string getPath() const    { return getAttr(Path); }
      std::string getVersion() const { return getAttr(Version); }
      std::string getExpires() const { return getAttr(Expires); }
      bool        isSecure() const   { return secure; }

      void setMaxAge(unsigned value);
      void setComment(const std::string& value)  { setAttr(Comment, value); }
      void setDomain(const std::string& value)   { setAttr(Domain, value); }
      void setPath(const std::string& value)     { setAttr(Path, value); }
      void setVersion(const std::string& value)  { setAttr(Version, value); }
      void setExpires(const std::string& value)  { setAttr(Expires, value); }
      void setSecure(bool f = true)              { secure = f; }
  };

  class cookies
  {
      friend std::ostream& operator<< (std::ostream& out, const cookies& c);
      friend class cookie_parser;

      typedef std::map<std::string, cookie> cookies_type;
      cookies_type data;
      static const cookie empty_cookie;

    public:
      void set(const std::string& header);
      void clear()  { data.clear(); }

      const cookie& getCookie(const std::string& name) const
      {
        cookies_type::const_iterator it = data.find(name);
        return it == data.end() ? empty_cookie : it->second;
      }

      void setCookie(const std::string& name, const cookie& value)
        { data[name] = value; }

      void clearCookie(const std::string& name);
      void clearCookie(const std::string& name, const cookie& c);

      bool hasCookie(const std::string& name) const
        { return data.find(name) != data.end(); }
      bool hasCookies() const
        { return !data.empty(); }
  };

  std::ostream& operator<< (std::ostream& out, const cookies& c);
}

#endif // TNT_COOKIE_H

