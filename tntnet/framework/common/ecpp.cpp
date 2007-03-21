/* ecpp.cpp
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

#include <tnt/ecpp.h>
#include <tnt/urlmapper.h>
#include <tnt/comploader.h>
#include <tnt/httperror.h>
#include <tnt/httprequest.h>
#include <cxxtools/log.h>

log_define("tntnet.ecpp")

namespace tnt
{
  //////////////////////////////////////////////////////////////////////
  // Subcompident
  //
  inline std::ostream& operator<< (std::ostream& out, const Subcompident& comp)
  { return out << comp.toString(); }

  Subcompident::Subcompident(const std::string& ident)
    : tnt::Compident(ident)
  {
    std::string::size_type pos = compname.find('.');
    if (pos != std::string::npos)
    {
      subname = compname.substr(pos + 1);
      compname = compname.substr(0, pos);
    }
  }

  std::string Subcompident::toString() const
  {
    std::string ret = tnt::Compident::toString();
    if (!subname.empty())
    {
      ret += '.';
      ret += subname;
    }
    return ret;
  }

  //////////////////////////////////////////////////////////////////////
  // EcppComponent
  //
  cxxtools::RWLock equivMonitor;

  EcppComponent::EcppComponent(const Compident& ci, const Urlmapper& um,
    Comploader& cl)
    : myident(ci),
      rootmapper(um),
      loader(cl)
  { }

  EcppComponent::~EcppComponent()
  { }

  void EcppComponent::registerSubComp(const std::string& name, EcppSubComponent* comp)
  {
    log_debug(getCompident() << ": registerSubComp " << name);

    subcomps_type::const_iterator it = getSubcomps().find(name);
    if (it != getSubcomps().end())
      log_error("duplicate subcomp " << name);
    else
      getSubcomps().insert(subcomps_type::value_type(name, comp));
  }

  Component& EcppComponent::fetchComp(const std::string& url) const
  {
    log_debug("fetchComp(\"" << url << "\")");

    Subcompident ci(url);
    if (ci.libname.empty())
      ci.libname = myident.libname;
    if (ci.compname.empty())
      ci.compname = myident.compname;

    Component* comp;

    // fetch component by name
    comp = &loader.fetchComp(ci, rootmapper);

    // if there is a subcomponent, fetch it
    if (!ci.subname.empty())
    {
      try
      {
        EcppComponent& e = dynamic_cast<EcppComponent&>(*comp);
        comp = &e.fetchSubComp(ci.subname);
      }
      catch (const NotFoundException&)
      {
        throw;
      }
      catch (const std::exception&)
      {
        throw NotFoundException(ci.toString());
      }
    }

    return *comp;
  }

  Component& EcppComponent::fetchComp(const Compident& ci) const
  {
    if (ci.libname.empty())
    {
      Compident cii(ci);
      cii.libname = myident.libname;
      return loader.fetchComp(cii, rootmapper);
    }
    else
      return loader.fetchComp(ci, rootmapper);
  }

  Component* EcppComponent::createComp(const Compident& ci) const
  {
    log_debug("createComp(" << ci << ")");
    if (ci.libname.empty())
    {
      Compident cii(ci);
      cii.libname = myident.libname;
      return loader.createComp(cii, rootmapper);
    }
    else
      return loader.createComp(ci, rootmapper);
  }

  EcppSubComponent& EcppComponent::fetchSubComp(const std::string& sub) const
  {
    log_debug(getCompident() << ": fetchSubComp(\"" << sub << "\")");

    subcomps_type::const_iterator it = getSubcomps().find(sub);
    if (it == getSubcomps().end())
      throw NotFoundException(Subcompident(getCompident(), sub).toString());
    return *it->second;
  }

  const char* EcppComponent::getData(const HttpRequest& request, const cxxtools::QueryParams& qparam,
    const char* def) const
  {
    static const std::string LANG = "LANG";

    std::string lang;
    if (qparam.has(LANG))
      lang = qparam.param(LANG);
    else if (request.getQueryParams().has(LANG))
      lang = request.getQueryParams().param(LANG);
    else
      lang = request.getLang();

    if (!lang.empty())
    {
      const char* data = loader.getLangData(myident, lang);
      if (data)
        return data;
    }

    return def;
  }

  ///////////////////////////////////////////////////////////////////////
  // ecppSubComponent
  //
  void EcppSubComponent::drop()
  { }

}
