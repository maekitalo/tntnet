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
  const std::string httpMessage::Content_Type = "Content-Type:";
  const std::string httpMessage::Content_Length = "Content-Length:";
  const std::string httpMessage::Connection = "Connection:";
  const std::string httpMessage::Connection_close = "close";
  const std::string httpMessage::Connection_Keep_Alive = "Keep-Alive";
  const std::string httpMessage::Last_Modified = "Last-Modified:";
  const std::string httpMessage::Server = "Server:";
  const std::string httpMessage::ServerName = "tntnet 1.1";
  const std::string httpMessage::Location = "Location:";
  const std::string httpMessage::AcceptLanguage = "Accept-Language:";
  const std::string httpMessage::Date = "Date:";
  const std::string httpMessage::KeepAlive = "Keep-Alive:";
  const std::string httpMessage::KeepAliveParam = "timeout=15, max=10";
  const std::string httpMessage::IfModifiedSince = "If-Modified-Since:";
  const std::string httpMessage::Host = "Host:";
  const std::string httpMessage::CacheControl = "Cache-Control:";
  const std::string httpMessage::Content_MD5 = "Content-MD5:";
  const std::string httpMessage::SetCookie = "Set-Cookie:";
  const std::string httpMessage::Cookie = "Cookie:";

  size_t httpMessage::maxRequestSize = 0;

  ////////////////////////////////////////////////////////////////////////
  // httpMessage
  //
  log_define("tntnet.http");

  void httpMessage::clear()
  {
    method.clear();
    url.clear();
    query_string.clear();
    header.clear();
    body.clear();
    major_version = 0;
    minor_version = 0;
    content_size = 0;
  }

  std::string httpMessage::getHeader(const std::string& key, const std::string& def) const
  {
    header_type::const_iterator i = header.find(key);
    return i == header.end() ? def : i->second;
  }

  void httpMessage::setHeader(const std::string& key, const std::string& value)
  {
    log_debug("httpMessage::setHeader(\"" << key << "\", \"" << value << "\")");
    header.insert(header_type::value_type(key, value));
  }

  void httpMessage::setContentLengthHeader(size_t size)
  {
    std::ostringstream s;
    s << size;
    setHeader(Content_Length, s.str());
  }

  std::string httpMessage::dumpHeader() const
  {
    std::ostringstream h;
    dumpHeader(h);
    return h.str();
  }

  void httpMessage::dumpHeader(std::ostream& out) const
  {
    for (header_type::const_iterator it = header.begin();
         it != header.end(); ++it)
      out << it->first << ' ' << it->second << '\n';
  }

  bool httpMessage::keepAlive() const
  {
    header_type::const_iterator it = header.find(Connection);

    if (getMajorVersion() == 1
     && getMinorVersion() == 1)
    {
      // keep-Alive if value not "close"
      return it == header.end() || it->second != Connection_close;
    }
    else
    {
      // keep-Alive if explicitely requested
      return it != header.end() && it->second == Connection_Keep_Alive;
    }
  }

  std::string httpMessage::htdate(time_t t)
  {
    struct tm tm;
    localtime_r(&t, &tm);
    return htdate(&tm);
  }

  std::string httpMessage::htdate(struct tm* tm)
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
    class pstr
    {
        const char* start;
        const char* end;

      public:
        typedef size_t size_type;

        pstr(const char* s, const char* e)
          : start(s), end(e)
          { }

        size_type size() const  { return end - start; }

        bool operator== (const pstr& s) const
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

  bool httpMessage::checkUrl(const std::string& url)
  {
    typedef std::list<pstr> tokens_type;

    // teile in Komponenten auf
    enum state_type {
      state_start,
      state_token,
      state_end
    };

    state_type state = state_start;

    tokens_type tokens;
    const char* p = url.data();
    const char* e = p + url.size();
    const char* s = p;
    for (; state != state_end && p != e; ++p)
    {
      switch (state)
      {
        case state_start:
          if (*p != '/')
          {
            s = p;
            state = state_token;
          }
          break;

        case state_token:
          if (*p == '/')
          {
            tokens.push_back(pstr(s, p));
            state = state_start;
          }
          else if (p == e)
          {
            tokens.push_back(pstr(s, p));
            state = state_end;
          }
          break;

        case state_end:
          break;
      }
    }

    // entferne jedes .. inklusive der vorhergehenden Komponente
    tokens_type::iterator i = tokens.begin();
    while (i != tokens.end())
    {
      if (*i == "..")
      {
        if (i == tokens.begin())
          return false;
        --i;
        i = tokens.erase(i);  // lösche 2 Komponenten
        i = tokens.erase(i);
      }
      else
      {
        ++i;
      }
    }

    return true;
  }

}
