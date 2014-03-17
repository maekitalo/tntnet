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
  EcppComponent::EcppComponent(const Compident& ci, const Urlmapper& um, Comploader& cl)
    : _myident(ci),
      _rootmapper(um),
      _loader(cl)
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
      ci.libname = _myident.libname;
    if (ci.compname.empty())
      ci.compname = _myident.compname;

    Component* comp;

    // fetch component by name
    comp = &_loader.fetchComp(ci, _rootmapper);

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
        throw NotFoundException(url);
      }
    }

    return *comp;
  }

  Component& EcppComponent::fetchComp(const Compident& ci) const
  {
    if (ci.libname.empty())
    {
      Compident cii(ci);
      cii.libname = _myident.libname;
      return _loader.fetchComp(cii, _rootmapper);
    }
    else
      return _loader.fetchComp(ci, _rootmapper);
  }

  Component* EcppComponent::createComp(const Compident& ci) const
  {
    log_debug("createComp(" << ci << ")");
    if (ci.libname.empty())
    {
      Compident cii(ci);
      cii.libname = _myident.libname;
      return _loader.createComp(cii, _rootmapper);
    }
    else
      return _loader.createComp(ci, _rootmapper);
  }

  EcppSubComponent& EcppComponent::fetchSubComp(const std::string& sub) const
  {
    log_debug(getCompident() << ": fetchSubComp(\"" << sub << "\")");

    subcomps_type::const_iterator it = getSubcomps().find(sub);
    if (it == getSubcomps().end())
      throw NotFoundException(Subcompident(getCompident(), sub).toString());
    return *it->second;
  }

  const char* EcppComponent::getData(const HttpRequest& request, const char* def) const
  {
    std::string lang = request.getLang();
    if (!lang.empty())
    {
      const char* data = _loader.getLangData(_myident, lang);
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
