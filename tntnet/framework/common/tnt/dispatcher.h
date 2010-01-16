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


#ifndef TNT_DISPATCHER_H
#define TNT_DISPATCHER_H

#include <cxxtools/mutex.h>
#include <tnt/urlmapper.h>
#include <tnt/maptarget.h>
#include <vector>
#include <map>
#include <cxxtools/regex.h>

namespace tnt
{
  // Dispatcher - one per host
  class Dispatcher : public Urlmapper
  {
    public:
      typedef Maptarget CompidentType;

    private:
      class VHostRegex
      {
          std::string vhost;
          std::string regexstr;
          cxxtools::Regex regex;

        public:
          VHostRegex(const std::string& vhost_, const std::string& regex_)
            : vhost(vhost_),
              regexstr(regex_),
              regex(regex_)
              { }

          bool match(const std::string& vhost_, const std::string& str_,
            cxxtools::RegexSMatch& smatch, int eflags = 0) const
          {
            return (vhost.empty() || cxxtools::Regex(vhost).match(vhost_))
                && regex.match(str_, smatch);
          }

          const std::string& getRegex() const  { return regexstr; }
      };

      typedef std::vector<std::pair<VHostRegex, CompidentType> > urlmap_type;
      urlmap_type urlmap;   // map url to soname/compname
      mutable cxxtools::ReadWriteMutex mutex;

      class UrlMapCacheKey
      {
          std::string vhost;
          std::string url;
          urlmap_type::const_iterator pos;

        public:
          UrlMapCacheKey() { }
          UrlMapCacheKey(const std::string& vhost_, const std::string& url_,
              urlmap_type::const_iterator pos_)
            : vhost(vhost_),
              url(url_),
              pos(pos_)
              { }

          bool operator< (const UrlMapCacheKey& other) const
          {
            int c = url.compare(other.url);
            if (c != 0)
              return c < 0;
            c = vhost.compare(other.vhost);
            if (c != 0)
              return c < 0;
            return pos < other.pos;
          }
      };

      struct UrlMapCacheValue
      {
        CompidentType ci;
        urlmap_type::const_iterator pos;

        UrlMapCacheValue() { }
        UrlMapCacheValue(CompidentType ci_, urlmap_type::const_iterator pos_)
          : ci(ci_),
            pos(pos_)
          { }
      };

      typedef std::map<UrlMapCacheKey, UrlMapCacheValue> urlMapCacheType;
      mutable cxxtools::ReadWriteMutex urlMapCacheMutex;
      mutable urlMapCacheType urlMapCache;
      static urlMapCacheType::size_type maxUrlMapCache;

      // don't make this public - it's not threadsafe:
      CompidentType mapCompNext(const std::string& vhost,
        const std::string& compUrl, urlmap_type::const_iterator& pos) const;

    public:
      virtual ~Dispatcher()  { }

      CompidentType& addUrlMapEntry(const std::string& vhost, const std::string& url,
        const CompidentType& ci);

      Compident mapComp(const std::string& vhost, const std::string& compUrl) const;

      static urlMapCacheType::size_type getMaxUrlMapCache()
        { return maxUrlMapCache; }
      static void setMaxUrlMapCache(urlMapCacheType::size_type s)
        { maxUrlMapCache = s; }

      friend class PosType;

      class PosType
      {
          const Dispatcher& dis;
          cxxtools::ReadLock lock;
          urlmap_type::const_iterator pos;
          std::string vhost;
          std::string url;
          bool first;

        public:
          PosType(const Dispatcher& d, const std::string& v,
            const std::string& u)
            : dis(d),
              lock(dis.mutex),
              pos(dis.urlmap.begin()),
              vhost(v),
              url(u),
              first(true)
          { }

          CompidentType getNext();
      };
  };

}

#endif // TNT_DISPATCHER_H

