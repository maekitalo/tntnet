/* dispatcher.cpp
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

#include "tnt/dispatcher.h"
#include <tnt/httperror.h>
#include <functional>
#include <iterator>
#include <algorithm>
#include <cxxtools/log.h>

log_define("tntnet.dispatcher")

namespace tnt
{

void Dispatcher::addUrlMapEntry(const std::string& url, const CompidentType& ci)
{
  cxxtools::WrLock lock(rwlock);

  urlmap.push_back(urlmap_type::value_type(regex(url), ci));
}

Compident Dispatcher::mapComp(const std::string& compUrl) const
{
  urlmap_type::const_iterator pos = urlmap.begin();
  return mapCompNext(compUrl, pos);
}

namespace {
  class regmatch_formatter : public std::unary_function<const std::string&, std::string>
  {
    public:
      regex_smatch what;
      std::string operator() (const std::string& s) const
      { return what.format(s); }
  };
}

Dispatcher::urlMapCacheType::size_type Dispatcher::maxUrlMapCache = 8192;

Dispatcher::CompidentType Dispatcher::mapCompNext(const std::string& compUrl,
  Dispatcher::urlmap_type::const_iterator& pos) const
{
  // check cache
  urlMapCacheType::key_type cacheKey = urlMapCacheType::key_type(compUrl, pos);
  urlMapCacheType::const_iterator um = urlMapCache.find(cacheKey);
  if (um != urlMapCache.end())
    return um->second;

  // no cache hit
  regmatch_formatter formatter;

  for (; pos != urlmap.end(); ++pos)
  {
    if (pos->first.match(compUrl, formatter.what))
    {
      const CompidentType& src = pos->second;

      CompidentType ci;
      ci.libname = formatter(src.libname);
      ci.compname = formatter(src.compname);
      if (src.hasPathInfo())
        ci.setPathInfo(formatter(src.getPathInfo()));
      std::transform(src.getArgs().begin(), src.getArgs().end(),
        std::back_inserter(ci.getArgsRef()), formatter);

      // clear cache after maxUrlMapCache distict requests
      if (urlMapCache.size() >= maxUrlMapCache)
      {
        log_warn("clear url-map-cache");
        urlMapCache.clear();
      }

      urlMapCache.insert(urlMapCacheType::value_type(cacheKey, ci));

      return ci;
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

  return dis.mapCompNext(url, pos);
}

}
