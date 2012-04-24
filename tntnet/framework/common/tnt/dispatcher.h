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
#include <tnt/tntconfig.h>
#include <vector>
#include <map>
#include <cxxtools/regex.h>

namespace tnt
{
  class HttpRequest;

  // Dispatcher - one per host
  class Dispatcher : public Urlmapper
  {
    public:
      typedef Maptarget CompidentType;

      static const int SSL_ALL = TntConfig::SSL_ALL;
      static const int SSL_NO  = TntConfig::SSL_NO;
      static const int SSL_YES = TntConfig::SSL_YES;

      class VHostRegex
      {
          std::string vhost;
          std::string url;
          std::string method;
          int ssl;

          cxxtools::Regex r_vhost;
          cxxtools::Regex r_url;
          cxxtools::Regex r_method;

        public:
          VHostRegex(const std::string& vhost_, const std::string& url_,
              const std::string& method_, int ssl_);

          bool match(const HttpRequest& request, cxxtools::RegexSMatch& smatch) const;

          const std::string& getVHost() const   { return vhost; }
          const std::string& getUrl() const     { return url; }
          const std::string& getMethod() const  { return method; }
          int getSsl() const                    { return ssl; }
      };

    private:
      typedef std::vector<std::pair<VHostRegex, CompidentType> > urlmap_type;
      urlmap_type urlmap;   // map url to soname/compname
      mutable cxxtools::ReadWriteMutex mutex;

      class UrlMapCacheKey
      {
          std::string vhost;
          std::string url;
          std::string method;
          bool ssl;
          urlmap_type::size_type pos;

        public:
          UrlMapCacheKey() { }
          UrlMapCacheKey(const HttpRequest& request, urlmap_type::size_type pos_);

          bool operator< (const UrlMapCacheKey& other) const;

          const std::string& getHost() const  { return vhost; }
          const std::string& getUrl() const  { return url; }
          const std::string& getMethod() const  { return method; }
          bool getSsl() const  { return ssl; }
          urlmap_type::size_type getPos() const  { return pos; }
      };

      struct UrlMapCacheValue
      {
        CompidentType ci;
        urlmap_type::size_type pos;

        UrlMapCacheValue() { }
        UrlMapCacheValue(CompidentType ci_, urlmap_type::size_type pos_)
          : ci(ci_),
            pos(pos_)
          { }
      };

      typedef std::map<UrlMapCacheKey, UrlMapCacheValue> urlMapCacheType;
      mutable cxxtools::ReadWriteMutex urlMapCacheMutex;
      mutable urlMapCacheType urlMapCache;

      // don't make this public - it's not threadsafe:
      CompidentType mapCompNext(const HttpRequest& request, urlmap_type::size_type& pos) const;

    public:
      virtual ~Dispatcher()  { }

      CompidentType& addUrlMapEntry(const std::string& vhost, const std::string& url,
        const std::string& method, int ssl, const CompidentType& ci);

      CompidentType& addUrlMapEntry(const std::string& vhost, const std::string& url,
        const CompidentType& ci)
      { return addUrlMapEntry(vhost, url, std::string(), SSL_ALL, ci); }

      friend class PosType;

      class PosType
      {
          const Dispatcher& dis;
          cxxtools::ReadLock lock;
          urlmap_type::size_type pos;
          const HttpRequest& request;
          bool first;

        public:
          PosType(const Dispatcher& d, const HttpRequest& r)
            : dis(d),
              lock(dis.mutex),
              pos(0),
              request(r),
              first(true)
          { }

          CompidentType getNext();
      };
  };

}

#endif // TNT_DISPATCHER_H

