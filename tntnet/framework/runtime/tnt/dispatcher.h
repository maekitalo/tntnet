/* tnt/dispatcher.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <boost/regex.hpp>
#include <cxxtools/thread.h>
#include <tnt/urlmapper.h>

namespace tnt
{

  // dispatcher - one per host
  class dispatcher : public urlmapper
  {
    public:
      class compident_type : public compident
      {
        public:
          typedef std::vector<std::string> args_type;

        private:
          std::string pathinfo;
          args_type args;
          bool pathinfo_set;

        public:
          compident_type()
            : pathinfo_set(false)
            { }

          explicit compident_type(const std::string& ident)
            : compident(ident),
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
      typedef std::vector<std::pair<boost::regex, compident_type> > urlmap_type;
      urlmap_type urlmap;   // map url to soname/compname
      mutable cxxtools::RWLock rwlock;

      typedef std::pair<compident_type, urlmap_type::iterator> nextcomp_pair_type;

      // don't make this public - it's not threadsafe:
      compident_type mapCompNext(const std::string& compUrl,
        urlmap_type::const_iterator& pos) const;

    public:
      virtual ~dispatcher()  { }

      void addUrlMapEntry(const std::string& url, const compident_type& ci);

      compident mapComp(const std::string& compUrl) const;

      friend class pos_type;

      class pos_type
      {
          const dispatcher& dis;
          cxxtools::RdLock lock;
          urlmap_type::const_iterator pos;
          std::string url;
          bool first;

        public:
          pos_type(const dispatcher& d, const std::string& u)
            : dis(d),
              lock(dis.rwlock),
              pos(dis.urlmap.begin()),
              url(u),
              first(true)
          { }

          compident_type getNext();
      };

      template <class OutputIterator>
        void mapCompAll(const std::string& compUrl, OutputIterator it)
      {
        cxxtools::RdLock lock(rwlock);
        urlmap_type::const_iterator pos = urlmap.begin();
        compident_type ci;
        while (mapCompNext(compUrl, ci, pos))
        {
          *it++ = ci;
          ++pos;
        }
      }
  };

}

#endif // DISPATCHER_H

