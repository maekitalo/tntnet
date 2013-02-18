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


#include "tnt/dispatcher.h"
#include <tnt/httperror.h>
#include <tnt/httprequest.h>
#include <tnt/tntconfig.h>
#include <functional>
#include <iterator>
#include <algorithm>
#include <cxxtools/log.h>

log_define("tntnet.dispatcher")

namespace tnt
{

namespace
{
  std::ostream& operator<< (std::ostream& out, const Mapping& r)
  {
    out << r.getVHost() << ':'
        << r.getUrl();

    if (r.getSsl() != SSL_ALL || !r.getMethod().empty())
      out << ':' << r.getMethod();

    if (r.getSsl() == SSL_NO)
      out << ":NOSSL";
    else if (r.getSsl() == SSL_YES)
      out << ":SSL";

    return out;
  }
}

Mapping::Mapping(const std::string& vhost_, const std::string& url_,
    const std::string& method_, int ssl_, const Maptarget& target_)
  : vhost(vhost_),
    url(url_),
    method(method_),
    ssl(ssl_),
    r_vhost(vhost_),
    r_url(url_),
    r_method(method_),
    target(target_)
{ }

bool Mapping::match(const HttpRequest& request, cxxtools::RegexSMatch& smatch) const
{
  return (vhost.empty() || r_vhost.match(request.getHost()))
      && (url.empty() || r_url.match(request.getUrl(), smatch))
      && (method.empty() || r_method.match(request.getMethod()))
      && (ssl == SSL_ALL
          || (ssl == SSL_YES && request.isSsl())
          || (ssl == SSL_NO && !request.isSsl()));
}

Dispatcher::UrlMapCacheKey::UrlMapCacheKey(const HttpRequest& request, urlmap_type::size_type pos_)
  : vhost(request.getHost()),
    url(request.getUrl()),
    method(request.getMethod()),
    ssl(request.isSsl()),
    pos(pos_)
{
}

bool Dispatcher::UrlMapCacheKey::operator< (const UrlMapCacheKey& other) const
{
  int c;

  c = vhost.compare(other.vhost);
  if (c != 0)
    return c < 0;

  c = url.compare(other.url);
  if (c != 0)
    return c < 0;

  c = method.compare(other.method);
  if (c != 0)
    return c < 0;

  if (ssl != other.ssl)
    return ssl < other.ssl;

  return pos < other.pos;
}

Mapping& Dispatcher::addUrlMapEntry(const std::string& vhost,
  const std::string& url, const std::string& method, int ssl, const Maptarget& ci)
{
  cxxtools::WriteLock lock(mutex);

  log_debug("map vhost <" << vhost << "> url <" << url << "> method <" << method << "> ssl <" << ssl << "> to <" << ci << '>');
  urlmap.push_back(Mapping(vhost, url, method, ssl, ci));
  return urlmap.back();
}

namespace {
  class regmatch_formatter : public std::unary_function<const std::string&, std::string>
  {
    public:
      cxxtools::RegexSMatch what;
      std::string operator() (const std::string& s) const
      { return what.format(s); }
  };
}

Maptarget Dispatcher::mapCompNext(const HttpRequest& request,
  Dispatcher::urlmap_type::size_type& pos) const
{
  std::string vhost = request.getHost();
  std::string compUrl = request.getUrl();

  if (pos < urlmap.size())
  {
    // check cache
    cxxtools::ReadLock lock(urlMapCacheMutex, false);

    urlMapCacheType::key_type cacheKey(request, pos);
    log_debug("host=\"" << cacheKey.getHost() << "\" url=\"" << cacheKey.getUrl() << "\" method=\"" << cacheKey.getMethod() << "\" ssl=" << cacheKey.getSsl() << " pos=" << cacheKey.getPos());

    if (TntConfig::it().maxUrlMapCache > 0)
    {
      lock.lock();
      urlMapCacheType::const_iterator um = urlMapCache.find(cacheKey);
      if (um != urlMapCache.end())
      {
        pos = um->second.pos;
        log_debug("match <" << urlmap[pos] << "> => " << um->second.ci << " (cached)");
        return um->second.ci;
      }

      log_debug("entry not found in cache");
    }
    else
      log_debug("cache disabled");

    // no cache hit
    regmatch_formatter formatter;

    for (; pos < urlmap.size(); ++pos)
    {
      if (urlmap[pos].match(request, formatter.what))
      {
        const Maptarget& src = urlmap[pos].getTarget();

        Maptarget ci;
        ci.libname = formatter(src.libname);
        ci.compname = formatter(src.compname);

        if (src.hasPathInfo())
          ci.setPathInfo(formatter(src.getPathInfo()));

        for (Maptarget::args_type::const_iterator it = src.getArgs().begin(); it != src.getArgs().end(); ++it)
          ci.args[it->first] = formatter(it->second);

        if (TntConfig::it().maxUrlMapCache > 0)
        {
          lock.unlock();
          cxxtools::WriteLock wlock(urlMapCacheMutex);

          // clear cache after maxUrlMapCache distinct requests
          if (urlMapCache.size() > TntConfig::it().maxUrlMapCache)
          {
            log_warn("clear url-map-cache");
            urlMapCache.clear();
          }

          urlMapCache.insert(urlMapCacheType::value_type(cacheKey, UrlMapCacheValue(ci, pos)));
        }

        log_debug("match <" << urlmap[pos] << "> => " << ci);
        return ci;
      }
      else
      {
        log_debug("no match <" << urlmap[pos] << '>');
      }
    }
  }

  throw NotFoundException(compUrl, vhost);
}

Maptarget Dispatcher::PosType::getNext()
{
  if (first)
    first = false;
  else
    ++pos;

  return dis.mapCompNext(request, pos);
}

}
