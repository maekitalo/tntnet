/* httpmessage.cpp
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

#include <tnt/httpmessage.h>
#include <cxxtools/log.h>
#include <list>
#include <sstream>

namespace tnt
{
  size_t HttpMessage::maxRequestSize = 0;

  ////////////////////////////////////////////////////////////////////////
  // HttpMessage
  //
  log_define("tntnet.httpmessage")

  void HttpMessage::clear()
  {
    method.clear();
    url.clear();
    queryString.clear();
    header.clear();
    body.clear();
    majorVersion = 0;
    minorVersion = 0;
    contentSize = 0;
  }

  std::string HttpMessage::getHeader(const std::string& key, const std::string& def) const
  {
    header_type::const_iterator i = header.find(key);
    return i == header.end() ? def : i->second;
  }

  void HttpMessage::setHeader(const std::string& key, const std::string& value)
  {
    log_debug("HttpMessage::setHeader(\"" << key << "\", \"" << value << "\")");
    std::string k = key;
    if (k.size() > 0 && k.at(k.size() - 1) != ':')
      k += ':';
    header.insert(header_type::value_type(k, value));
  }

  std::string HttpMessage::dumpHeader() const
  {
    std::ostringstream h;
    dumpHeader(h);
    return h.str();
  }

  void HttpMessage::dumpHeader(std::ostream& out) const
  {
    for (header_type::const_iterator it = header.begin();
         it != header.end(); ++it)
      out << it->first << ' ' << it->second << '\n';
  }

  std::string HttpMessage::htdate(time_t t)
  {
    struct ::tm tm;
    localtime_r(&t, &tm);
    return htdate(&tm);
  }

  std::string HttpMessage::htdate(struct ::tm* tm)
  {
    const char* wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* monthn[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char buffer[80];

    mktime(tm);
    sprintf(buffer, "%s, %02d %s %d %02d:%02d:%02d GMT",
      wday[tm->tm_wday], tm->tm_mday, monthn[tm->tm_mon], tm->tm_year + 1900,
      tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buffer;
  }

  namespace
  {
    class Pstr
    {
        const char* start;
        const char* end;

      public:
        typedef size_t size_type;

        Pstr(const char* s, const char* e)
          : start(s), end(e)
          { }

        void setStart(const char* s)
        {
          start = s;
        }

        void setEnd(const char* e)
        {
          end = e;
        }

        size_type size() const  { return end - start; }
        bool empty() const      { return start == end; }

        bool operator== (const Pstr& s) const
        {
          return size() == s.size()
            && std::equal(start, end, s.start);
        }

        bool operator== (const char* str) const
        {
          size_type i;
          for (i = 0; i < size() && str[i] != '\0'; ++i)
            if (start[i] != str[i])
              return false;
          return i == size() && str[i] == '\0';
        }
    };
  }

  bool HttpMessage::checkUrl(const std::string& url)
  {
    unsigned level = 0;
    const char* p = url.data();
    const char* e = p + url.size();
    Pstr str(p, p);
    for (; p != e; ++p)
    {
      if (*p == '/')
      {
        str.setEnd(p);
        if (!str.empty())
        {
          if (str == ".")
            ;
          else if (str == "..")
          {
            if (level == 0)
              return false;
            --level;
          }
          else
            ++level;
        }
        str.setStart(p + 1);
      }
    }

    if (level == 0 && (str.setEnd(p), str == ".."))
      return false;

    return true;
  }

}
