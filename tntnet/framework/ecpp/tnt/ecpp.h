/* tnt/ecpp.h
   Copyright (C) 2003 Tommi Mäkitalo

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

#ifndef ECPP_H
#define ECPP_H

#include <tnt/component.h>
#include <map>
#include <set>

namespace tnt
{
  class urlmapper;
  class comploader;
  class ecppSubComponent;

  //////////////////////////////////////////////////////////////////////
  // subcompident
  //
  struct subcompident : public tnt::compident
  {
      std::string subname;

      subcompident(const tnt::compident& ci, const std::string& s)
        : tnt::compident(ci),
          subname(s)
         { }

      explicit subcompident(const std::string& ident);
      std::string toString() const;
  };

  //////////////////////////////////////////////////////////////////////
  // ecppComponent
  //
  class ecppComponent : public component
  {
      friend class ecppSubComponent;

      compident myident;
      const urlmapper& rootmapper;
      comploader& loader;

      typedef std::map<std::string, ecppSubComponent*> subcomps_type;
      subcomps_type subcomps;

      virtual subcomps_type& getSubcomps()               { return subcomps; }
      virtual const subcomps_type& getSubcomps() const   { return subcomps; }

      typedef std::set<std::string> libnotfound_type;
      typedef std::set<compident> compnotfound_type;

      static libnotfound_type libnotfound;
      static compnotfound_type compnotfound;

      static void rememberLibNotFound(const std::string& lib);
      static void rememberCompNotFound(const compident& ci);

    protected:
      virtual ~ecppComponent();
      void registerSubComp(const std::string& name, ecppSubComponent* comp);

    public:
      ecppComponent(const compident& ci, const urlmapper& um, comploader& cl);

      component& fetchComp(const std::string& url) const;
      component& fetchComp(const compident& ci) const;

      ecppSubComponent& fetchSubComp(const std::string& sub) const;

      unsigned callComp(const std::string& url, httpRequest& request,
        httpReply& reply, query_params& qparam);

      const compident& getCompident() const  { return myident; }

      const component* getDataComponent(const httpRequest& request) const;
  };

  //////////////////////////////////////////////////////////////////////
  // ecppSubComponent
  //
  class ecppSubComponent : public ecppComponent
  {
      ecppComponent& main;
      std::string subcompname;

      virtual subcomps_type& getSubcomps()
        { return main.getSubcomps(); }
      virtual const subcomps_type& getSubcomps() const
        { return main.getSubcomps(); }

    public:
      ecppSubComponent(ecppComponent& p, const std::string& name)
        : ecppComponent(p.myident, p.rootmapper, p.loader),
          main(p),
          subcompname(name)
        {
          p.registerSubComp(name, this);
        }

      virtual bool drop();
      subcompident getCompident() const
        { return subcompident(main.getCompident(), subcompname); }

      ecppComponent& getMainComponent() const { return main; }
  };

  inline unsigned ecppComponent::callComp(const std::string& url, httpRequest& request,
    httpReply& reply, query_params& qparam)
  {
    return fetchComp(url)(request, reply, qparam);
  }

}

namespace ecpp_component
{
  using tnt::component;
  using tnt::compident;
  using tnt::urlmapper;
  using tnt::comploader;
  using tnt::ecppComponent;
  using tnt::ecppSubComponent;
  using tnt::httpRequest;
  using tnt::httpReply;
}

#endif // ECPP_H

