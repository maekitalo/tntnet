////////////////////////////////////////////////////////////////////////
// comploader.cpp
//

#include <tnt/comploader.h>
#include <tnt/log.h>

namespace tnt
{

////////////////////////////////////////////////////////////////////////
// component_library
//
component* component_library::create(
  const std::string& component_name, comploader& cl,
  const urlmapper& rootmapper)
{
  creator_type creator;

  // look for creator in my map
  creatormap_type::const_iterator i = creatormap.find(component_name);
  if (i == creatormap.end())
  {
    // creatorsymbol not known - load it
    log_info("lookup symbol create_" << component_name);

    creator = (creator_type)sym(("create_" + component_name).c_str()).getSym();
    creatormap.insert(creatormap_type::value_type(component_name, creator));
  }
  else
    creator = i->second;

  // call the creator-function
  compident ci = compident(libname, component_name);
  log_info("create " << ci);
  return creator(ci, rootmapper, cl);
}

////////////////////////////////////////////////////////////////////////
// comploader
//
comploader::~comploader()
{
  for (componentmap_type::iterator i = componentmap.begin();
       i != componentmap.end(); ++i)
    i->second->drop();
}

RWLock comploader::libraryMonitor;
comploader::librarymap_type comploader::librarymap;

component& comploader::fetchComp(const compident& ci,
  const urlmapper& rootmapper)
{
  RdLock lock(componentMonitor);

  // lookup component
  componentmap_type::iterator it = componentmap.find(ci);
  if (it == componentmap.end())
  {
    // component not known - lookup shared lib, fetch creator and create a new
    // component
    lock.Unlock();

    WrLock wrlock(componentMonitor);

    // doublecheck after getting writelock
    it = componentmap.find(ci);
    if (it == componentmap.end())
    {
      component_library& lib = fetchLib(ci.libname);
      component* comp = lib.create(ci.compname, *this, rootmapper);

      // notify listener
      for (listener_container_type::iterator l = listener.begin();
           l != listener.end(); ++l)
        (*l)->onCreateComponent(lib, ci, *comp);

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

component_library& comploader::fetchLib(const std::string& libname)
{
  RdLock lock(libraryMonitor);
  librarymap_type::iterator i = librarymap.find(libname);
  if (i == librarymap.end())
  {
    lock.Unlock();

    WrLock wrlock(libraryMonitor);

    // doublecheck after writelock
    librarymap_type::iterator i = librarymap.find(libname);
    if (i == librarymap.end())
    {
      // load library
      log_info("load library " << libname);
      component_library lib;
      try
      {
        log_debug("load library " << libname << " from current dir");
        lib = component_library(".", libname);
      }
      catch (const dl::dlopen_error&)
      {
        log_debug("library in current dir not found - sarch path");
        lib = component_library(libname);
      }

      // notify listener
      for (listener_container_type::iterator l = listener.begin();
           l != listener.end(); ++l)
        (*l)->onLoadLibrary(lib);

      i = librarymap.insert(librarymap_type::value_type(libname, lib)).first;
    }

    return i->second;
  }
  else
    return i->second;
}

void comploader::addLoadLibraryListener(load_library_listener* l)
{
  listener.insert(l);
}

bool comploader::removeLoadLibraryListener(load_library_listener* l)
{
  return listener.erase(l) > 0;
}

void comploader::cleanup(unsigned seconds)
{
  time_t t = time(0) - seconds;

  RdLock rdlock(componentMonitor);
  for (componentmap_type::iterator it = componentmap.begin();
       it != componentmap.end(); ++it)
  {
    if (it->second->getLastAccesstime() < t)
    {
      // da ist was aufzuräumen
      rdlock.Unlock();

      WrLock wrlock(componentMonitor);
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
