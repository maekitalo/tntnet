////////////////////////////////////////////////////////////////////////
// ecpp.cpp
//

#include <tnt/ecpp.h>
#include <tnt/urlmapper.h>
#include <tnt/comploader.h>
#include <tnt/http.h>
#include <tnt/log.h>

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

  ///////////////////////////////////////////////////////////////////////
  // ecppSubComponent
  //
  bool ecppSubComponent::drop()
  {
    return false;
  }

}
