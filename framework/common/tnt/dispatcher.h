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
      std::string _vhost;
      std::string _url;
      std::string _method;
      int _ssl;

      cxxtools::Regex _r_vhost;
      cxxtools::Regex _r_url;
      cxxtools::Regex _r_method;

      Maptarget _target;

    public:
      typedef Maptarget::args_type args_type;

      Mapping() { }

      Mapping(const std::string& vhost, const std::string& url, const std::string& method, int ssl, const Maptarget& target);

      const std::string& getVHost() const  { return _vhost; }
      const std::string& getUrl() const    { return _url; }
      const std::string& getMethod() const { return _method; }
      int getSsl() const                   { return _ssl; }

      const Maptarget& getTarget() const   { return _target; }

      Mapping& setPathInfo(const std::string& p)
      {
        _target.setPathInfo(p);
        return *this;
      }

      Mapping& setArgs(const args_type& a)
      {
        _target.setArgs(a);
        return *this;
      }

      Mapping& setArg(const std::string& name, const std::string& value)
      {
        _target.setArg(name, value);
        return *this;
      }

      Mapping& setVHost(const std::string& vhost)
      {
        _vhost = vhost;
        _r_vhost = cxxtools::Regex(vhost);
        return *this;
      }

      Mapping& setUrl(const std::string& url)
      {
        _url = url;
        _r_url = cxxtools::Regex(url);
        return *this;
      }

      Mapping& setMethod(const std::string& method)
      {
        _method = method;
        _r_method = cxxtools::Regex(method);
        return *this;
      }

      Mapping& setSsl(bool sw)
      {
        _ssl = (sw ? SSL_YES : SSL_NO);
        return *this;
      }

      Mapping& unsetSsl()
      {
        _ssl = SSL_ALL;
        return *this;
      }

      bool match(const HttpRequest& request, cxxtools::RegexSMatch& smatch) const;
  };

  /// @cond internal
  class Dispatcher : public Urlmapper // one per host
  {
    friend class PosType;

    private:
      typedef std::vector<Mapping> urlmap_type;

      urlmap_type _urlmap; // map url to soname/compname
      mutable cxxtools::ReadWriteMutex _mutex;

      class UrlMapCacheKey
      {
        private:
          std::string _vhost;
          std::string _url;
          std::string _method;
          bool _ssl;
          urlmap_type::size_type _pos;

        public:
          UrlMapCacheKey() { }
          UrlMapCacheKey(const HttpRequest& request, urlmap_type::size_type pos);

          bool operator< (const UrlMapCacheKey& other) const;

          const std::string& getHost() const    { return _vhost; }
          const std::string& getUrl() const     { return _url; }
          const std::string& getMethod() const  { return _method; }
          bool getSsl() const                   { return _ssl; }
          urlmap_type::size_type getPos() const { return _pos; }
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

      mutable cxxtools::ReadWriteMutex _urlMapCacheMutex;
      mutable urlMapCacheType _urlMapCache;

      Maptarget mapCompNext(const HttpRequest& request, urlmap_type::size_type& pos) const;

    public:
      virtual ~Dispatcher()  { }

      Mapping& addUrlMapEntry(const std::string& vhost, const std::string& url, const std::string& method, int ssl, const Maptarget& ci);

      Mapping& addUrlMapEntry(const std::string& vhost, const std::string& url, const Maptarget& ci)
        { return addUrlMapEntry(vhost, url, std::string(), SSL_ALL, ci); }

      class PosType
      {
        private:
          const Dispatcher& _dis;
          cxxtools::ReadLock _lock;
          urlmap_type::size_type _pos;
          const HttpRequest& _request;
          bool _first;

        public:
          PosType(const Dispatcher& d, const HttpRequest& r)
            : _dis(d),
              _lock(d._mutex),
              _pos(0),
              _request(r),
              _first(true)
            { }

          Maptarget getNext();
      };
  };
  /// @endcond internal
}

#endif // TNT_DISPATCHER_H

