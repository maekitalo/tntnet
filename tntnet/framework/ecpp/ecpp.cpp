/* ecpp.cpp
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

#include <tnt/ecpp.h>
#include <tnt/urlmapper.h>
#include <tnt/comploader.h>
#include <tnt/httperror.h>
#include <tnt/httprequest.h>
#include <cxxtools/log.h>

log_define("tntnet.ecpp");

namespace tnt
{
  //////////////////////////////////////////////////////////////////////
  // subcompident
  //
  inline std::ostream& operator<< (std::ostream& out, const subcompident& comp)
  { return out << comp.toString(); }

  subcompident::subcompident(const std::string& ident)
    : tnt::compident(ident)
  {
    std::string::size_type pos = compname.find('.');
    if (pos != std::string::npos)
    {
      subname = compname.substr(pos + 1);
      compname = compname.substr(0, pos);
    }
  }

  std::string subcompident::toString() const
  {
    std::string ret = tnt::compident::toString();
    if (!subname.empty())
    {
      ret += '.';
      ret += subname;
    }
    return ret;
  }

  //////////////////////////////////////////////////////////////////////
  // ecppComponent
  //
  cxxtools::RWLock equivMonitor;
  ecppComponent::libnotfound_type ecppComponent::libnotfound;
  ecppComponent::compnotfound_type ecppComponent::compnotfound;

  ecppComponent::ecppComponent(const compident& ci, const urlmapper& um,
    comploader& cl)
    : myident(ci),
      rootmapper(um),
      loader(cl)
  { }

  ecppComponent::~ecppComponent()
  { }

  void ecppComponent::registerSubComp(const std::string& name, ecppSubComponent* comp)
  {
    log_debug(getCompident() << ": registerSubComp " << name);

    subcomps_type::const_iterator it = getSubcomps().find(name);
    if (it != getSubcomps().end())
      log_error("duplicate subcomp " << name);
    else
      getSubcomps().insert(subcomps_type::value_type(name, comp));
  }

  component& ecppComponent::fetchComp(const std::string& url) const
  {
    log_debug("fetchComp(\"" << url << "\")");

    subcompident ci(url);
    if (ci.libname.empty())
      ci.libname = myident.libname;
    if (ci.compname.empty())
      ci.compname = myident.compname;

    component* comp;
    try
    {
      // suche Komponente nach Namen
      comp = &loader.fetchComp(ci, rootmapper);

      // wenn eine Subkomponente übergeben wurde, suche nach dieser
      if (!ci.subname.empty())
      {
        try
        {
          ecppComponent& e = dynamic_cast<ecppComponent&>(*comp);
          comp = &e.fetchSubComp(ci.subname);
        }
        catch (const std::bad_cast&)
        {
          throw notFoundException(ci.toString());
        }
      }
    }
    catch (const notFoundException&)
    {
      // wenn nicht gefunden, dann nach Url
      compident ci = rootmapper.mapComp(url);
      comp = &loader.fetchComp(ci, rootmapper);
    }

    // liefere Komponente
    return *comp;
  }

  component& ecppComponent::fetchComp(const compident& ci) const
  {
    if (ci.libname.empty())
    {
      compident cii(ci);
      cii.libname = myident.libname;
      return loader.fetchComp(cii, rootmapper);
    }
    else
      return loader.fetchComp(ci, rootmapper);
  }

  ecppSubComponent& ecppComponent::fetchSubComp(const std::string& sub) const
  {
    log_debug(getCompident() << ": fetchSubComp(\"" << sub << "\")");

    subcomps_type::const_iterator it = getSubcomps().find(sub);
    if (it == getSubcomps().end())
      throw notFoundException(subcompident(getCompident(), sub).toString());
    return *it->second;
  }

  const component* ecppComponent::getDataComponent(const httpRequest& request) const
  {
    std::string LANG = request.getLang();

    if (!LANG.empty())
    {
      compident ci = compident(getCompident().libname + '.' + LANG,
                                    getCompident().compname);

      // check compidentmap
      {
        cxxtools::RdLock lock(equivMonitor);

        libnotfound_type::const_iterator lit = libnotfound.find(ci.libname);
        if (lit != libnotfound.end())
          return this;

        compnotfound_type::const_iterator cit = compnotfound.find(ci);
        if (cit != compnotfound.end())
          return this;
      }

      try
      {
        return &fetchComp(ci);
      }
      catch (const cxxtools::dl::symbol_not_found&)
      {
        // symbol with lang not found - remember this
        rememberCompNotFound(ci);
      }
      catch (const cxxtools::dl::dlopen_error&)
      {
        // library with lang not found - remember this
        rememberLibNotFound(ci.libname);
      }
    }

    return this;
  }

  void ecppComponent::rememberLibNotFound(const std::string& lib)
  {
    log_debug("remember lib not found " << lib);
    cxxtools::WrLock wrlock(equivMonitor);
    libnotfound.insert(lib);
  }

  void ecppComponent::rememberCompNotFound(const compident& ci)
  {
    log_debug("remember comp not found " << ci);
    cxxtools::WrLock wrlock(equivMonitor);
    compnotfound.insert(ci);
  }

  std::string ecppComponent::scallComp(const std::string& url,
    httpRequest& request)
  {
    cxxtools::query_params qparam;
    return fetchComp(url)(request, qparam);
  }

  std::string ecppComponent::scallComp(const compident& ci,
    httpRequest& request)
  {
    cxxtools::query_params qparam;
    return fetchComp(ci)(request, qparam);
  }

  std::string ecppComponent::scallComp(const subcompident& ci,
    httpRequest& request)
  {
    cxxtools::query_params qparam;
    return dynamic_cast<ecppComponent&>(fetchComp(ci))
               .fetchSubComp(ci.subname)(request, qparam);
  }

  std::string ecppComponent::scallComp(const std::string& url)
  {
    httpRequest request;
    cxxtools::query_params qparam;
    return fetchComp(url)(request, qparam);
  }

  std::string ecppComponent::scallComp(const compident& ci)
  {
    httpRequest request;
    cxxtools::query_params qparam;
    return fetchComp(ci)(request, qparam);
  }

  std::string ecppComponent::scallComp(const subcompident& ci)
  {
    httpRequest request;
    cxxtools::query_params qparam;
    return dynamic_cast<ecppComponent&>(fetchComp(ci))
               .fetchSubComp(ci.subname)(request, qparam);
  }


  ///////////////////////////////////////////////////////////////////////
  // ecppSubComponent
  //
  bool ecppSubComponent::drop()
  {
    return false;
  }

}
