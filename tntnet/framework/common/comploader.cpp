/* comploader.cpp
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

#include <tnt/comploader.h>
#include <tnt/tntconfig.h>
#include <cxxtools/log.h>

namespace tnt
{

////////////////////////////////////////////////////////////////////////
// ComponentLibrary
//
log_define("tntnet.comploader")

Component* ComponentLibrary::create(
  const std::string& component_name, Comploader& cl,
  const Urlmapper& rootmapper)
{
  log_debug("create \"" << component_name << '"');

  creator_type creator;

  // look for creator in my map
  creatormap_type::const_iterator i = creatormap.find(component_name);
  if (i == creatormap.end())
  {
    // creatorsymbol not known - load it
    log_debug("lookup symbol \"create_" << component_name << '"');

    creator = (creator_type)sym(("create_" + component_name).c_str()).getSym();
    creatormap.insert(creatormap_type::value_type(component_name, creator));
  }
  else
    creator = i->second;

  // call the creator-function
  Compident ci = Compident(libname, component_name);
  log_info("create \"" << ci << '"');

  return creator(ci, rootmapper, cl);
}

////////////////////////////////////////////////////////////////////////
// Comploader
//
Comploader::Comploader(const Tntconfig& config_)
  : config(config_)
{
  Tntconfig::config_entries_type configLoad;
  config.getConfigValues("Load", configLoad);

  for (Tntconfig::config_entries_type::const_iterator it = configLoad.begin();
       it != configLoad.end(); ++it)
  {
    if (it->params.empty())
      throw std::runtime_error("missing libraryname in Load-command");
    fetchLib(it->params[0]);
  }
}

Comploader::~Comploader()
{
  for (componentmap_type::iterator i = componentmap.begin();
       i != componentmap.end(); ++i)
    i->second->drop();
}

cxxtools::RWLock Comploader::libraryMonitor;
Comploader::librarymap_type Comploader::librarymap;
Comploader::search_path_type Comploader::search_path;

Component& Comploader::fetchComp(const Compident& ci,
  const Urlmapper& rootmapper)
{
  log_debug("fetchComp \"" << ci << '"');

  cxxtools::RdLock lock(componentMonitor);

  // lookup Component
  componentmap_type::iterator it = componentmap.find(ci);
  if (it == componentmap.end())
  {
    // Component not known - lookup shared lib, fetch creator and create a new
    // Component
    lock.unlock();

    cxxtools::WrLock wrlock(componentMonitor);

    // doublecheck after getting writelock
    it = componentmap.find(ci);
    if (it == componentmap.end())
    {
      ComponentLibrary& lib = fetchLib(ci.libname);
      Component* comp = lib.create(ci.compname, *this, rootmapper);

      componentmap[ci] = comp;
      comp->touch();
      return *comp;
    }
    else
    {
      it->second->touch();
      return *(it->second);
    }
  }
  else
  {
    it->second->touch();
    return *(it->second);
  }
}

ComponentLibrary& Comploader::fetchLib(const std::string& libname)
{
  log_debug("fetchLib " << libname);

  cxxtools::RdLock lock(libraryMonitor);
  librarymap_type::iterator i = librarymap.find(libname);
  if (i == librarymap.end())
  {
    lock.unlock();

    cxxtools::WrLock wrlock(libraryMonitor);

    // doublecheck after writelock
    librarymap_type::iterator i = librarymap.find(libname);
    if (i == librarymap.end())
    {
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

      i = librarymap.insert(librarymap_type::value_type(libname, lib)).first;
    }

    return i->second;
  }
  else
    return i->second;
}

void Comploader::cleanup(unsigned seconds)
{
  time_t t = time(0) - seconds;

  cxxtools::RdLock rdlock(componentMonitor);
  for (componentmap_type::iterator it = componentmap.begin();
       it != componentmap.end(); ++it)
  {
    if (it->second->getLastAccesstime() < t)
    {
      // we have something to clean
      rdlock.unlock();

      cxxtools::WrLock wrlock(componentMonitor);
      while (it != componentmap.end())
      {
        if (it->second->getLastAccesstime() < t)
        {
          componentmap_type::iterator it2 = it;
          ++it;
          log_debug("drop " << it2->first);
          if (it2->second->drop())
            log_debug("deleted " << it2->first);
          componentmap.erase(it2);
        }
      else
        ++it;
      }
      break;
    }
  }
}

}
