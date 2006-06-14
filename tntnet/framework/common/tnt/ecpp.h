/* tnt/ecpp.h
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
  class Urlmapper;
  class Comploader;
  class EcppSubComponent;

  //////////////////////////////////////////////////////////////////////
  // Subcompident
  //
  struct Subcompident : public tnt::Compident
  {
      std::string subname;

      Subcompident(const tnt::Compident& ci, const std::string& sub)
        : tnt::Compident(ci),
          subname(sub)
         { }
      Subcompident(const std::string& lib, const std::string& comp, const std::string& sub)
        : tnt::Compident(lib, comp),
          subname(sub)
         { }

      explicit Subcompident(const std::string& ident);
      std::string toString() const;
  };

  //////////////////////////////////////////////////////////////////////
  // EcppComponent
  //
  class EcppComponent : public Component
  {
      friend class EcppSubComponent;

      Compident myident;
      const Urlmapper& rootmapper;
      Comploader& loader;

      typedef std::map<std::string, EcppSubComponent*> subcomps_type;
      subcomps_type subcomps;

      virtual subcomps_type& getSubcomps()               { return subcomps; }
      virtual const subcomps_type& getSubcomps() const   { return subcomps; }

      typedef std::set<std::string> libnotfound_type;
      typedef std::set<Compident> compnotfound_type;

    protected:
      virtual ~EcppComponent();

      void registerSubComp(const std::string& name, EcppSubComponent* comp);

      Component& fetchComp(const std::string& url) const;
      Component& fetchComp(const Compident& ci) const;
      Component& fetchComp(const Subcompident& ci) const;
      Component* createComp(const Compident& ci) const;
      Component* createComp(const std::string& url) const
        { return createComp(Compident(url)); }

      /// helper-methods for calling components
      template <typename compident_type,
                typename parameter1_type,
                typename parameter2_type>
        unsigned callComp(const compident_type& ci, HttpRequest& request,
            parameter1_type& p1, parameter2_type& p2)
        { return fetchComp(ci).call(request, p1, p2); }

      template <typename compident_type,
                typename parameter_type>
        unsigned callComp(const compident_type& ci, HttpRequest& request,
            parameter_type& p1)
        { return fetchComp(ci).call(request, p1); }

      template <typename compident_type>
        unsigned callComp(const compident_type& ci, HttpRequest& request)
        { return fetchComp(ci).call(request); }

      /// helper-methods for fetching contents of components
      template <typename compident_type,
                typename parameter1_type>
        std::string scallComp(const compident_type& ci, HttpRequest& request,
            parameter1_type& p1)
        { return fetchComp(ci).scall(request, p1); }

      template <typename compident_type>
        std::string scallComp(const compident_type& ci, HttpRequest& request)
        { return fetchComp(ci).scall(request); }

      const char* getData(const HttpRequest& request, const char* def) const;

    public:
      EcppComponent(const Compident& ci, const Urlmapper& um, Comploader& cl);

      const Compident& getCompident() const  { return myident; }

      EcppSubComponent& fetchSubComp(const std::string& sub) const;

      /// helper-methods for calling subcomponents
      template <typename parameter1_type,
                typename parameter2_type>
        unsigned callSubComp(const std::string& sub, HttpRequest& request,
            parameter1_type& p1,
            parameter2_type& p2) const;

      template <typename parameter1_type>
        unsigned callSubComp(const std::string& sub, HttpRequest& request,
            parameter1_type& p1) const;

      /// helper-methods for fetching contents of subcomponents
      template <typename parameter1_type>
        std::string scallSubComp(const std::string& sub, HttpRequest& request,
            parameter1_type& p1) const;
  };

  //////////////////////////////////////////////////////////////////////
  // EcppSubComponent
  //
  class EcppSubComponent : public EcppComponent
  {
      EcppComponent& main;
      std::string subcompname;

      virtual subcomps_type& getSubcomps()
        { return main.getSubcomps(); }
      virtual const subcomps_type& getSubcomps() const
        { return main.getSubcomps(); }

    public:
      EcppSubComponent(EcppComponent& p, const std::string& name)
        : EcppComponent(p.myident, p.rootmapper, p.loader),
          main(p),
          subcompname(name)
        {
          p.registerSubComp(name, this);
        }

      virtual void drop();
      Subcompident getCompident() const
        { return Subcompident(main.getCompident(), subcompname); }

      EcppComponent& getMainComponent() const { return main; }
  };

  //////////////////////////////////////////////////////////////////////
  // inline methods
  //
  inline Component& EcppComponent::fetchComp(const Subcompident& ci) const
  {
    return dynamic_cast<EcppComponent&>(
                        fetchComp( static_cast<const Compident&>(ci) )
                        )
                 .fetchSubComp(ci.subname);
  }

  template <typename parameter1_type,
            typename parameter2_type>
    unsigned EcppComponent::callSubComp(
        const std::string& sub,
        HttpRequest& request,
        parameter1_type& p1,
        parameter2_type& p2) const
    { return fetchSubComp(sub).call(request, p1, p2); }

  template <typename parameter1_type>
    unsigned EcppComponent::callSubComp(
        const std::string& sub,
        HttpRequest& request,
        parameter1_type& p1) const
    { return fetchSubComp(sub).call(request, p1); }

  /// helper-methods for fetching contents of subcomponents
  template <typename parameter1_type>
    std::string EcppComponent::scallSubComp(
        const std::string& sub,
        HttpRequest& request,
        parameter1_type& p1) const
    { return fetchSubComp(sub).scall(request, p1); }

  //////////////////////////////////////////////////////////////////////
  // scope-helper
  //
  inline std::string getPageScopePrefix(const Compident& id)
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
  typedef tnt::ObjectTemplate<varname##_type> varname##_objecttype;          \
  const std::string varname##_scopekey = key;          \
  tnt::Objectptr varname##_pointer = request.get##scope##Scope().get(varname##_scopekey);          \
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

#endif // TNT_ECPP_H

