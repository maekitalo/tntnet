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
#include <functional>
#include <iterator>
#include <algorithm>
#include <cxxtools/log.h>

log_define("tntnet.dispatcher")

namespace tnt
{

Dispatcher::CompidentType& Dispatcher::addUrlMapEntry(const std::string& vhost,
  const std::string& url, const CompidentType& ci)
{
  cxxtools::WriteLock lock(mutex);

  log_debug("map vhost <" << vhost << "> url <" << url << "> to <" << ci << '>');
  urlmap.push_back(urlmap_type::value_type(VHostRegex(vhost, url), ci));
  return urlmap.back().second;
}

Compident Dispatcher::mapComp(const std::string& vhost,
  const std::string& compUrl) const
{
  urlmap_type::const_iterator pos = urlmap.begin();
  return mapCompNext(vhost, compUrl, pos);
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

Dispatcher::urlMapCacheType::size_type Dispatcher::maxUrlMapCache = 8192;

Dispatcher::CompidentType Dispatcher::mapCompNext(const std::string& vhost,
  const std::string& compUrl, Dispatcher::urlmap_type::const_iterator& pos) const
{
  // check cache
  cxxtools::ReadLock lock(urlMapCacheMutex, false);
  urlMapCacheType::key_type cacheKey;

  if (maxUrlMapCache > 0)
  {
    lock.lock();
    cacheKey = urlMapCacheType::key_type(vhost, compUrl, pos);
    urlMapCacheType::const_iterator um = urlMapCache.find(cacheKey);
    if (um != urlMapCache.end())
    {
      log_debug("vhost <" << vhost << "> url <" << compUrl << "> match regex <" << um->second.pos->first.getRegex() << "> (cached) => " << um->second.ci);
      pos = um->second.pos;
      return um->second.ci;
    }
  }

  // no cache hit
  regmatch_formatter formatter;

  for (; pos != urlmap.end(); ++pos)
  {
    if (pos->first.match(vhost, compUrl, formatter.what))
    {
      const CompidentType& src = pos->second;

      CompidentType ci;
      ci.libname = formatter(src.libname);
      ci.compname = formatter(src.compname);

      if (src.hasPathInfo())
        ci.setPathInfo(formatter(src.getPathInfo()));
      std::transform(src.getArgs().begin(), src.getArgs().end(),
        std::back_inserter(ci.getArgsRef()), formatter);

      if (maxUrlMapCache > 0)
      {
        // clear cache after maxUrlMapCache distinct requests
        if (urlMapCache.size() >= maxUrlMapCache)
        {
          log_warn("clear url-map-cache");
          urlMapCache.clear();
        }

        lock.unlock();
        cxxtools::WriteLock wlock(urlMapCacheMutex);
        urlMapCache.insert(urlMapCacheType::value_type(cacheKey, UrlMapCacheValue(ci, pos)));
      }

      log_debug("vhost <" << vhost << "> url <" << compUrl << "> match regex <" << pos->first.getRegex() << "> => " << ci);
      return ci;
    }
    else
    {
      log_debug("vhost <" << vhost << "> url <" << compUrl << "> does not match regex <" << pos->first.getRegex() << '>');
    }
  }

  throw NotFoundException(compUrl);
}

Dispatcher::CompidentType Dispatcher::PosType::getNext()
{
  if (first)
    first = false;
  else
    ++pos;

  return dis.mapCompNext(vhost, url, pos);
}

}
