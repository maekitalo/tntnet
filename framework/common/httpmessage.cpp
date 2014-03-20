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


#include <tnt/httpmessage.h>
#include <cxxtools/log.h>
#include <cxxtools/mutex.h>
#include <list>
#include <sstream>
#include <stdio.h>

namespace tnt
{
  ////////////////////////////////////////////////////////////////////////
  // HttpMessage
  //
  void HttpMessage::clear()
  {
    header.clear();
    _majorVersion = 1;
    _minorVersion = 0;
  }

  const char* HttpMessage::getHeader(const char* key, const char* def) const
  {
    header_type::const_iterator i = header.find(key);
    return i == header.end() ? def : i->second;
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
    gmtime_r(&t, &tm);
    return htdate(&tm);
  }

  std::string HttpMessage::htdate(struct ::tm* tm)
  {
    static const char* wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char* monthn[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char buffer[80];

    sprintf(buffer, "%s, %02d %s %d %02d:%02d:%02d GMT",
      wday[tm->tm_wday], tm->tm_mday, monthn[tm->tm_mon], tm->tm_year + 1900,
      tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buffer;
  }

  std::string HttpMessage::htdateCurrent()
  {
    static struct ::tm lastTm;
    static time_t lastDay = 0;
    static time_t lastTime = 0;
    static std::string lastHtdate;
    static cxxtools::Mutex mutex;

    /*
     * we cache the last split tm-struct here, because it is pretty expensive
     * to calculate the date with gmtime_r.
     */

    time_t t;
    time(&t);

    cxxtools::MutexLock lock(mutex);

    if (lastTime != t)
    {
      time_t day = t / (24*60*60);
      if (day != lastDay)
      {
        // Day differs, we calculate new date.
        gmtime_r(&t, &lastTm);
        lastDay = day;
      }

      lastTm.tm_sec = t % 60;
      t /= 60;
      lastTm.tm_min = t % 60;
      t /= 60;
      lastTm.tm_hour = t % 24;
      lastHtdate = htdate(&lastTm);
      lastTime = t;
    }

    // we do a copy of lastHtdate first to make sure we have the lock
    std::string ret = lastHtdate;
    return ret;
  }

  namespace
  {
    class Pstr
    {
      private:
        const char* _start;
        const char* _end;

      public:
        typedef size_t size_type;

        Pstr(const char* s, const char* e)
          : _start(s), _end(e)
          { }

        void setStart(const char* s)
          { _start = s; }

        void setEnd(const char* e)
          { _end = e; }

        size_type size() const { return _end - _start; }
        bool empty() const     { return _start == _end; }

        bool operator== (const Pstr& s) const
        {
          return size() == s.size()
            && std::equal(_start, _end, s._start);
        }

        bool operator== (const char* str) const
        {
          size_type i;
          for (i = 0; i < size() && str[i] != '\0'; ++i)
            if (_start[i] != str[i])
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

