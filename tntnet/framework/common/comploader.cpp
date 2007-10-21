/* comploader.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <tnt/comploader.h>
#include <tnt/componentfactory.h>
#include <tnt/tntconfig.h>
#include <tnt/httperror.h>
#include <cxxtools/log.h>

namespace
{
  cxxtools::Mutex mutex;
}

namespace tnt
{

////////////////////////////////////////////////////////////////////////
// ComponentLibrary
//
log_define("tntnet.comploader")

ComponentLibrary::~ComponentLibrary()
{
  for (langlibsType::iterator it = langlibs.begin();
       it != langlibs.end(); ++it)
    delete it->second;
}

Component* ComponentLibrary::create(
  const std::string& component_name, Comploader& cl,
  const Urlmapper& rootmapper)
{
  log_debug("create \"" << component_name << '"');

  ComponentFactory* factory;

  // look for factory in my map
  factoryMapType::const_iterator i = factoryMap.find(component_name);
  if (i == factoryMap.end())
    throw NotFoundException(component_name);

  factory = i->second;

  // call the creator
  Compident ci = Compident(libname, component_name);
  log_debug("call creator for \"" << ci << '"');

  return factory->create(ci, rootmapper, cl);
}

LangLib* ComponentLibrary::getLangLib(const std::string& lang)
{
  cxxtools::MutexLock lock(mutex);
  langlibsType::const_iterator it = langlibs.find(lang);
  if (it != langlibs.end())
    return it->second;

  LangLib* l = 0;
  try
  {
    std::string n = (path.empty() ? libname : (path + '/' + libname));
    l = new LangLib(n, lang);
  }
  catch (const unzipError& e)
  {
    log_warn("unzipError: " << e.what());
  }
  langlibs[lang] = l;
  return l;
}

////////////////////////////////////////////////////////////////////////
// Comploader
//
Comploader::Comploader()
{
  if (config)
  {
    Tntconfig::config_entries_type configLoad;
    config->getConfigValues("Load", configLoad);

    for (Tntconfig::config_entries_type::const_iterator it = configLoad.begin();
         it != configLoad.end(); ++it)
    {
      if (it->params.empty())
        throw std::runtime_error("missing libraryname in Load-command");
      fetchLib(it->params[0]);
    }
  }
}

Comploader::librarymap_type Comploader::librarymap;
static const Tntconfig emptyconfig;
const Tntconfig* Comploader::config = &emptyconfig;
Comploader::search_path_type Comploader::search_path;
ComponentLibrary::factoryMapType* Comploader::currentFactoryMap = 0;

Component& Comploader::fetchComp(const Compident& ci,
  const Urlmapper& rootmapper)
{
  log_debug("fetchComp \"" << ci << '"');

  cxxtools::MutexLock lock(mutex);

  // lookup Component
  componentmap_type::iterator it = componentmap.find(ci);
  if (it == componentmap.end())
  {
    ComponentLibrary& lib = fetchLib(ci.libname);
    Component* comp = lib.create(ci.compname, *this, rootmapper);

    componentmap[ci] = comp;
    return *comp;
  }
  else
    return *(it->second);
}

Component* Comploader::createComp(const Compident& ci,
  const Urlmapper& rootmapper)
{
  log_debug("createComp \"" << ci << '"');

  ComponentLibrary& lib = fetchLib(ci.libname);
  Component* comp = lib.create(ci.compname, *this, rootmapper);
  return comp;
}

const char* Comploader::getLangData(const Compident& ci,
  const std::string& lang)
{
  log_debug("getLangData(" << ci << ", \"" << lang << "\")");
  ComponentLibrary& lib = fetchLib(ci.libname);
  LangLib* langLib = lib.getLangLib(lang);
  if (langLib)
    return langLib->getData(ci.compname);
  else
    return 0;
}

namespace
{
  template <typename T>
  class ValueResetter
  {
      T& value;
      T null;

    public:
      explicit ValueResetter(T& value_, T null_ = T())
        : value(value_),
          null(null_)
          { }
      ~ValueResetter()
      { value = null; }
  };
};

ComponentLibrary& Comploader::fetchLib(const std::string& libname)
{
  log_debug("fetchLib \"" << libname << '"');

  librarymap_type::iterator it = librarymap.find(libname);
  if (it == librarymap.end())
  {
    ComponentLibrary::factoryMapType factoryMap;
    currentFactoryMap = &factoryMap;
    ValueResetter<ComponentLibrary::factoryMapType*> valueResetter(currentFactoryMap, 0);

    // load library
    log_info("load library \"" << libname << '"');
    ComponentLibrary lib;

    bool found = false;
    for (search_path_type::const_iterator p = search_path.begin();
         p != search_path.end(); ++p)
    {
      try
      {
        log_debug("load library \"" << libname << "\" from " << *p << " dir");
        lib = ComponentLibrary(*p, libname);
        found = true;
        break;
      }
      catch (const cxxtools::dl::DlopenError&)
      {
      }
    }

    if (!found)
    {
      try
      {
        log_debug("load library \"" << libname << "\" from current dir");
        lib = ComponentLibrary(".", libname);
      }
      catch (const cxxtools::dl::DlopenError& e)
      {
        log_debug("library \"" << e.getLibname() << "\" in current dir not found - search lib-path");
        lib = ComponentLibrary(libname);
      }
    }

    lib.factoryMap = factoryMap;
    log_debug("insert new library " << libname);
    it = librarymap.insert(librarymap_type::value_type(libname, lib)).first;
  }
  else
    log_debug("library " << libname << " found");

  return it->second;
}

void Comploader::configure(const Tntconfig& config_)
{
  config = &config_;

  Tntconfig::config_entries_type compPath;
  config_.getConfigValues("CompPath", compPath);
  for (Tntconfig::config_entries_type::const_iterator it = compPath.begin();
       it != compPath.end(); ++it)
  {
    if (it->params.size() > 0)
      search_path.push_back(it->params[0]);
  }
}

void Comploader::registerFactory(const std::string& component_name,
  ComponentFactory* factory)
{
  log_debug("Comploader::registerFactory(" << component_name << ", " << factory << ')');

  if (currentFactoryMap)
    currentFactoryMap->insert(ComponentLibrary::factoryMapType::value_type(component_name, factory));
  else
  {
    log_debug("register component without library-name");
    librarymap_type::iterator it = librarymap.find(std::string());
    if (it == librarymap.end())
    {
      // empty library not in map - create a new empty library-object
      it = librarymap.insert(
        librarymap_type::value_type(std::string(),
                                    ComponentLibrary())).first;
    }
    it->second.registerFactory(component_name, factory);
  }
}

}
