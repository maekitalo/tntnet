/* tnt/dispatcher.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifndef TNT_DISPATCHER_H
#define TNT_DISPATCHER_H

#include <cxxtools/thread.h>
#include <tnt/urlmapper.h>
#include <vector>
#include <map>
#include "tnt/regex.h"

namespace tnt
{
  // Dispatcher - one per host
  class Dispatcher : public Urlmapper
  {
    public:
      class CompidentType : public Compident
      {
        public:
          typedef std::vector<std::string> args_type;

        private:
          std::string pathinfo;
          args_type args;
          bool pathinfo_set;

        public:
          CompidentType()
            : pathinfo_set(false)
            { }

          explicit CompidentType(const std::string& ident)
            : Compident(ident),
              pathinfo_set(false)
            { }

          bool hasPathInfo() const
            { return pathinfo_set; }
          void setPathInfo(const std::string& p)
            { pathinfo = p; pathinfo_set = true; }
          void setArgs(const args_type& a)
            { args = a; }
          const std::string& getPathInfo() const
            { return pathinfo; }
          const args_type& getArgs() const
            { return args; }
          args_type& getArgsRef()
            { return args; }
      };

    private:
      typedef std::vector<std::pair<regex, CompidentType> > urlmap_type;
      urlmap_type urlmap;   // map url to soname/compname
      mutable cxxtools::RWLock rwlock;

      typedef std::map<std::pair<std::string, urlmap_type::const_iterator>,
                       CompidentType> urlMapCacheType;
      mutable urlMapCacheType urlMapCache;
      static urlMapCacheType::size_type maxUrlMapCache;

      // don't make this public - it's not threadsafe:
      CompidentType mapCompNext(const std::string& compUrl,
        urlmap_type::const_iterator& pos) const;

    public:
      virtual ~Dispatcher()  { }

      void addUrlMapEntry(const std::string& url, const CompidentType& ci);

      Compident mapComp(const std::string& compUrl) const;

      static urlMapCacheType::size_type getMaxUrlMapCache()
        { return maxUrlMapCache; }
      static void setMaxUrlMapCache(urlMapCacheType::size_type s)
        { maxUrlMapCache = s; }

      friend class PosType;

      class PosType
      {
          const Dispatcher& dis;
          cxxtools::RdLock lock;
          urlmap_type::const_iterator pos;
          std::string url;
          bool first;

        public:
          PosType(const Dispatcher& d, const std::string& u)
            : dis(d),
              lock(dis.rwlock),
              pos(dis.urlmap.begin()),
              url(u),
              first(true)
          { }

          CompidentType getNext();
      };
  };

}

#endif // TNT_DISPATCHER_H

