/* cookie.cpp
 * Copyright (C) 2005 Tommi Maekitalo
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

#include <tnt/cookie.h>
#include <tnt/httperror.h>
#include <tnt/fastctype.h>
#include <cxxtools/log.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace tnt
{
  log_define("tntnet.cookie")

  const Cookie Cookies::emptyCookie;

  const std::string Cookie::maxAge  = "max-age";
  const std::string Cookie::comment = "comment";
  const std::string Cookie::domain  = "domain";
  const std::string Cookie::path    = "path";
  const std::string Cookie::secure  = "secure";
  const std::string Cookie::version = "version";
  const std::string Cookie::expires = "expires";

  unsigned Cookie::getMaxAge() const
  {
    std::string a = getAttr(maxAge);
    if (!a.empty())
    {
      std::istringstream s(a);
      unsigned ret;
      s >> ret;
      if (s)
        return ret;
    }
    return 0;
  }

  void Cookie::setMaxAge(unsigned value)
  {
    std::ostringstream s;
    s << value;
    setAttr(maxAge, s.str());
  }

  void Cookies::clearCookie(const std::string& name)
  {
    cookies_type::iterator it = data.find(name);
    if (it != data.end())
      it->second.setAttr(Cookie::maxAge, "0");
    else
    {
      Cookie c;
      c.setAttr(Cookie::maxAge, "0");
      setCookie(name, c);
    }
  }

  void Cookies::clearCookie(const std::string& name, const Cookie& c)
  {
    Cookie cc(c);
    cc.setAttr(Cookie::maxAge, "0");
    setCookie(name, cc);
  }

  class CookieParser
  {
      // Cookie: $Version="1"; Customer="WILE_E_COYOTE"; $Path="/acme"
      Cookie::attrs_type common_attrs;
      Cookie::attrs_type* current_attrs;
      Cookie current_cookie;
      bool attr;
      std::string current_cookie_name;

      std::string name;
      std::string value;

      Cookies& mycookies;

      void store_cookie();
      void process_nv();

    public:
      CookieParser(Cookies& c)
        : current_attrs(&common_attrs),
          mycookies(c)
        { }

      void parse(const std::string& header);
  };

  void Cookies::set(const std::string& header)
  {
    CookieParser parser(*this);
    parser.parse(header);
  }

  void CookieParser::store_cookie()
  {
    if (!mycookies.hasCookie(current_cookie_name))
      mycookies.setCookie(current_cookie_name, current_cookie);
    current_cookie.value.clear();
  }

  void CookieParser::process_nv()
  {
    if (attr)
    {
      if (name == Cookie::secure)
      {
        log_debug("attribute: secure");
        current_cookie.secureFlag = true;
      }
      else
      {
        log_debug("attribute: " << name << '=' << value);
        current_attrs->insert(
          Cookie::attrs_type::value_type(name, value));
      }
    }
    else
    {
      if (!current_cookie_name.empty())
        store_cookie();

      log_debug("Cookie: " << name << '=' << value);

      current_cookie_name = name;
      current_cookie.value = value;
      current_cookie.secureFlag = false;
      name.clear();
      current_attrs = &current_cookie.attrs;
      current_cookie.attrs = common_attrs;
    }
  }

  void CookieParser::parse(const std::string& header)
  {
    // Cookie: $Version="1"; Customer="WILE_E_COYOTE"; $Path="/acme"

    enum state_type
    {
      state_0,
      state_name,
      state_eq,
      state_value0,
      state_value,
      state_valuee,
      state_qvalue,
      state_qvaluee
    };

    state_type state = state_0;

    for (std::string::const_iterator it = header.begin();
         it != header.end(); ++it)
    {
      char ch = *it;
      switch(state)
      {
        case state_0:
          if (ch == '$')
          {
            attr = true;
            name.clear();
            state = state_name;
          }
          else if (!myisspace(ch))
          {
            attr = false;
            name = mytolower(ch);
            state = state_name;
          }
          break;

        case state_name:
          if (myisspace(ch))
            state = (name == Cookie::secure ? state_valuee : state_eq);
          else if (ch == '=')
            state = (name == Cookie::secure ? state_valuee : state_value0);
          else if ((ch == ',' || ch == ';') && name == Cookie::secure)
            state = state_valuee;
          else
            name += mytolower(ch);
          break;

        case state_eq:
          if (ch == '=')
            state = state_value0;
          else if (!myisspace(ch))
            throw HttpError("400 invalid cookie: " + header);
          break;

        case state_value0:
          if (ch == '"')
          {
            value.clear();
            state = state_qvalue;
          }
          else if (!myisspace(ch))
          {
            value = ch;
            state = state_value;
          }
          break;

        case state_value:
          if (ch == ';' || ch == ',')
          {
            process_nv();
            state = state_0;
          }
          else if (myisspace(ch))
            state = state_valuee;
          else
            value += ch;
          break;

        case state_valuee:
          if (ch == ';' || ch == ',')
          {
            process_nv();
            state = state_0;
          }
          else if (myisspace(ch))
            state = state_valuee;
          else
            throw HttpError("400 invalid cookie: " + header);
          break;

        case state_qvalue:
          if (ch == '"')
            state = state_qvaluee;
          else
            value += ch;
          break;

        case state_qvaluee:
          if (ch == ';' || ch == ',')
          {
            process_nv();
            state = state_0;
          }
          else if (!myisspace(ch))
            throw HttpError("400 invalid cookie: " + header);
          break;
      }
    }

    if (state == state_qvaluee || state == state_value)
      process_nv();
    else if (state != state_0)
      throw HttpError("400 invalid cookie: " + header);

    if (!current_cookie.value.empty())
      store_cookie();
  }

  std::ostream& operator<< (std::ostream& out, const Cookies& c)
  {
    // Set-Cookie: Customer="WILE_E_COYOTE"; Version="1"; Path="/acme"

    bool first = true;
    for (Cookies::cookies_type::const_iterator it = c.data.begin();
         it != c.data.end(); ++it)
    {
      if (first)
        first = false;
      else
        out << ", ";

      const Cookie& cookie = it->second;

      // print name (Customer="WILE_E_COYOTE")
      out << it->first << "=\"" << cookie.getValue() << '"';

      // print secure-attribute
      if (cookie.secureFlag)
        out << "; " << Cookie::secure;

      // print attributes
      for (Cookie::attrs_type::const_iterator a = cookie.attrs.begin();
           a != cookie.attrs.end(); ++a)
        out << "; " << a->first << "=\"" << a->second << '"';
    }

    return out;
  }
}
