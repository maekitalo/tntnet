////////////////////////////////////////////////////////////////////////
// dispatcher.h
//

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
      mutable RWLock rwlock;

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
          RdLock lock;
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
        RdLock lock(rwlock);
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

