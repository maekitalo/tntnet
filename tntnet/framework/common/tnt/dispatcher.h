/* tnt/dispatcher.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_DISPATCHER_H
#define TNT_DISPATCHER_H

#include <cxxtools/mutex.h>
#include <tnt/urlmapper.h>
#include <tnt/maptarget.h>
#include <vector>
#include <map>
#include <tnt/regex.h>

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
          Regex regex;

        public:
          VHostRegex(const std::string& vhost_, const Regex& regex_)
            : vhost(vhost_),
              regex(regex_)
              { }

          bool match(const std::string& vhost_, const std::string& str_,
            RegexSMatch& smatch, int eflags = 0) const
          {
            return (vhost.empty() || Regex(vhost).match(vhost_))
                && regex.match(str_, smatch);
          }
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

      typedef std::map<UrlMapCacheKey, CompidentType> urlMapCacheType;
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

