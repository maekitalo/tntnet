////////////////////////////////////////////////////////////////////////
// ecpp.cpp
//

#include <tnt/ecpp.h>
#include <tnt/urlmapper.h>
#include <tnt/comploader.h>
#include <tnt/http.h>

namespace tnt
{

ecppComponent::ecppComponent(const compident& ci, const urlmapper& um,
  comploader& cl)
  : myident(ci),
    rootmapper(um),
    loader(cl)
{ }

ecppComponent::~ecppComponent()
{ }

component& ecppComponent::fetchComp(const std::string& url) const
{
  compident ci(url);
  if (ci.libname.empty())
	ci.libname = myident.libname;

  component* comp;
  try
  {
    // suche Komponente nach Namen
	comp = &loader.fetchComp(ci, rootmapper);
  }
  catch (const notFoundException&)
  {
    // wenn nicht gefunden, dann nach Url
    ci = rootmapper.mapComp(url);
	comp = &loader.fetchComp(ci, rootmapper);
  }

  // rufe Komponente auf
  return *comp;
}

component& ecppComponent::fetchComp(const compident& ci) const
{
  return loader.fetchComp(ci, rootmapper);
}

}
