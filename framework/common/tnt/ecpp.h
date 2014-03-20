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


#ifndef TNT_ECPP_H
#define TNT_ECPP_H

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/scope.h>
#include <tnt/sessionscope.h>
#include <tnt/httprequest.h>
#include <tnt/http.h>
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

    private:
      typedef std::map<std::string, EcppSubComponent*> subcomps_type;
      typedef std::set<Compident> compnotfound_type;

      Compident _myident;
      const Urlmapper& _rootmapper;
      Comploader& _loader;
      subcomps_type _subcomps;

      virtual subcomps_type& getSubcomps()             { return _subcomps; }
      virtual const subcomps_type& getSubcomps() const { return _subcomps; }

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

      const Compident& getCompident() const { return _myident; }

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
      EcppComponent& _main;
      std::string _subcompname;

      virtual subcomps_type& getSubcomps()
        { return _main.getSubcomps(); }
      virtual const subcomps_type& getSubcomps() const
        { return _main.getSubcomps(); }

    public:
      EcppSubComponent(EcppComponent& p, const std::string& name)
        : EcppComponent(p._myident, p._rootmapper, p._loader),
          _main(p),
          _subcompname(name)
        {
          p.registerSubComp(name, this);
        }

      virtual void drop();
      Subcompident getCompident() const
        { return Subcompident(_main.getCompident(), _subcompname); }

      EcppComponent& getMainComponent() const { return _main; }
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

  template <typename ComponentType>
  class EcppComponentFactoryImpl : public ComponentFactory
  {
    public:
      explicit EcppComponentFactoryImpl(const std::string& componentName)
        : ComponentFactory(componentName)
        { }

      virtual Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl)
      {
        return new ComponentType(ci, um, cl);
      }
  };

}

#endif // TNT_ECPP_H

