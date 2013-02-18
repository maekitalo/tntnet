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

  class Mapping
  {
    private:
      std::string vhost;
      std::string url;
      std::string method;
      int ssl;

      cxxtools::Regex r_vhost;
      cxxtools::Regex r_url;
      cxxtools::Regex r_method;

      Maptarget target;

    public:
      typedef Maptarget::args_type args_type;

      Mapping() { }

      Mapping(const std::string& vhost_, const std::string& url_,
          const std::string& method_, int ssl_, const Maptarget& target_);

      const std::string& getVHost() const   { return vhost; }
      const std::string& getUrl() const     { return url; }
      const std::string& getMethod() const  { return method; }
      int getSsl() const                    { return ssl; }

      const Maptarget& getTarget() const    { return target; }

      Mapping& setPathInfo(const std::string& p)
        { target.setPathInfo(p); return *this; }

      Mapping& setArgs(const args_type& a)
        { target.setArgs(a); return *this; }

      Mapping& setVHost(const std::string& vhost_)
        { vhost = vhost_; r_vhost = cxxtools::Regex(vhost_); return *this; }

      Mapping& setUrl(const std::string& url_)
        { url = url_; r_url = cxxtools::Regex(url_); return *this; }

      Mapping& setMethod(const std::string& method_)
        { method = method_; r_method = cxxtools::Regex(method_); return *this; }

      Mapping& setSsl(bool sw)
        { ssl = (sw ? SSL_YES : SSL_NO); return *this; }

      Mapping& unsetSsl()
        { ssl = SSL_ALL; return *this; }

      bool match(const HttpRequest& request, cxxtools::RegexSMatch& smatch) const;

  };

  // Dispatcher - one per host
  class Dispatcher : public Urlmapper
  {
      typedef std::vector<Mapping> urlmap_type;
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
        Maptarget ci;
        urlmap_type::size_type pos;

        UrlMapCacheValue() { }
        UrlMapCacheValue(const Maptarget& ci_, urlmap_type::size_type pos_)
          : ci(ci_),
            pos(pos_)
          { }
      };

      typedef std::map<UrlMapCacheKey, UrlMapCacheValue> urlMapCacheType;
      mutable cxxtools::ReadWriteMutex urlMapCacheMutex;
      mutable urlMapCacheType urlMapCache;

      Maptarget mapCompNext(const HttpRequest& request, urlmap_type::size_type& pos) const;

    public:
      virtual ~Dispatcher()  { }

      Mapping& addUrlMapEntry(const std::string& vhost, const std::string& url,
        const std::string& method, int ssl, const Maptarget& ci);

      Mapping& addUrlMapEntry(const std::string& vhost, const std::string& url,
        const Maptarget& ci)
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

          Maptarget getNext();
      };
  };

}

#endif // TNT_DISPATCHER_H

