/* tnt/ecpp.h
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

#ifndef TNT_ECPP_H
#define TNT_ECPP_H

#include <tnt/component.h>
#include <tnt/scope.h>
#include <tnt/sessionscope.h>
#include <tnt/httprequest.h>
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

      subcompident(const tnt::compident& ci, const std::string& sub)
        : tnt::compident(ci),
          subname(sub)
         { }
      subcompident(const std::string& lib, const std::string& comp, const std::string& sub)
        : tnt::compident(lib, comp),
          subname(sub)
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
        httpReply& reply, cxxtools::query_params& qparam);
      unsigned callComp(const compident& ci, httpRequest& request,
        httpReply& reply, cxxtools::query_params& qparam);
      unsigned callComp(const subcompident& ci, httpRequest& request,
        httpReply& reply, cxxtools::query_params& qparam);

      std::string scallComp(const std::string& url, httpRequest& request,
        cxxtools::query_params& qparam);
      std::string scallComp(const compident& ci, httpRequest& request,
        cxxtools::query_params& qparam);
      std::string scallComp(const subcompident& ci, httpRequest& request,
        cxxtools::query_params& qparam);

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

  inline unsigned ecppComponent::callComp(const std::string& url,
    httpRequest& request, httpReply& reply,
    cxxtools::query_params& qparam)
  {
    return fetchComp(url)(request, reply, qparam);
  }

  inline unsigned ecppComponent::callComp(const compident& ci,
    httpRequest& request, httpReply& reply,
    cxxtools::query_params& qparam)
  {
    return fetchComp(ci)(request, reply, qparam);
  }

  inline unsigned ecppComponent::callComp(const subcompident& ci,
    httpRequest& request, httpReply& reply,
    cxxtools::query_params& qparam)
  {
    return dynamic_cast<ecppComponent&>(fetchComp(ci))
               .fetchSubComp(ci.subname)(request, reply, qparam);
  }

  inline std::string ecppComponent::scallComp(const std::string& url,
    httpRequest& request, cxxtools::query_params& qparam)
  {
    return fetchComp(url)(request, qparam);
  }

  inline std::string ecppComponent::scallComp(const compident& ci,
    httpRequest& request, cxxtools::query_params& qparam)
  {
    return fetchComp(ci)(request, qparam);
  }

  inline std::string ecppComponent::scallComp(const subcompident& ci,
    httpRequest& request, cxxtools::query_params& qparam)
  {
    return dynamic_cast<ecppComponent&>(fetchComp(ci))
               .fetchSubComp(ci.subname)(request, qparam);
  }

  inline std::string getPageScopePrefix(const compident& id)
  {
    return id.toString();
  }

  template <typename compident_type>
  std::string getComponentScopePrefix(const compident_type& id)
  {
    return id.toString();
  }
}

#define TNT_VAR(scope, type, varname, key, construct) \
  typedef type varname##_type;          \
  typedef tnt::objectTemplate<varname##_type> varname##_objecttype;          \
  const std::string varname##_scopekey = key;          \
  tnt::objectptr varname##_pointer = request.get##scope##Scope().get(varname##_scopekey);          \
  if (varname##_pointer == 0)          \
    varname##_pointer = request.get##scope##Scope().putNew(varname##_scopekey, new varname##_objecttype construct);          \
  type& varname = varname##_objecttype::getRef(varname##_pointer);

#define TNT_SESSION_COMPONENT_VAR(type, varname, key, construct) \
  TNT_VAR(Session, type, varname, getComponentScopePrefix(getCompident()) + ":" key, construct)

#define TNT_SESSION_PAGE_VAR(type, varname, key, construct) \
  TNT_VAR(Session, type, varname, getPageScopePrefix(getCompident()) + ":" key, construct)

#define TNT_SESSION_GLOBAL_VAR(type, varname, key, construct) \
  TNT_VAR(Session, type, varname, key, construct)

#define TNT_APPLICATION_COMPONENT_VAR(type, varname, key, construct) \
  TNT_VAR(Application, type, varname, getComponentScopePrefix(getCompident()) + ":" key, construct)

#define TNT_APPLICATION_PAGE_VAR(type, varname, key, construct) \
  TNT_VAR(Application, type, varname, getPageScopePrefix(getCompident()) + ":" key, construct)

#define TNT_APPLICATION_GLOBAL_VAR(type, varname, key, construct) \
  TNT_VAR(Application, type, varname, key, construct)

#define TNT_REQUEST_COMPONENT_VAR(type, varname, key, construct) \
  TNT_VAR(Request, type, varname, getComponentScopePrefix(getCompident()) + ":" key, construct)

#define TNT_REQUEST_PAGE_VAR(type, varname, key, construct) \
  TNT_VAR(Request, type, varname, getPageScopePrefix(getCompident()) + ":" key, construct)

#define TNT_REQUEST_GLOBAL_VAR(type, varname, key, construct) \
  TNT_VAR(Request, type, varname, key, construct)

namespace ecpp_component
{
  using tnt::component;
  using tnt::compident;
  using tnt::subcompident;
  using tnt::urlmapper;
  using tnt::comploader;
  using tnt::ecppComponent;
  using tnt::ecppSubComponent;
  using tnt::httpRequest;
  using tnt::httpReply;
}

#endif // TNT_ECPP_H

