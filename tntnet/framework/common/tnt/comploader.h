/* tnt/comploader.h
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

#ifndef TNT_COMPLOADER_H
#define TNT_COMPLOADER_H

#include <cxxtools/dlloader.h>
#include <cxxtools/thread.h>
#include <tnt/urlmapper.h>
#include <tnt/langlib.h>
#include <map>
#include <set>
#include <list>
#include <string>
#include <utility>

namespace tnt
{
  class Comploader;
  class ComponentFactory;
  class Tntconfig;

  class ComponentLibrary : public cxxtools::dl::Library
  {
      typedef std::map<std::string, ComponentFactory*> factoryMapType;

      factoryMapType factoryMap;
      std::string libname;
      std::string path;
      typedef std::map<std::string, LangLib*> langlibsType;
      langlibsType langlibs;

    public:
      ComponentLibrary()
        { }

      ComponentLibrary(const std::string& path_, const std::string& name)
        : cxxtools::dl::Library((path_ + '/' + name).c_str()),
          libname(name),
          path(path_)
        { }

      ComponentLibrary(const std::string& name)
        : cxxtools::dl::Library(name.empty() ? 0 : name.c_str()),
          libname(name)
        { }

      ~ComponentLibrary();

      Component* create(const std::string& component_name, Comploader& cl,
        const Urlmapper& rootmapper);
      LangLib* getLangLib(const std::string& lang);

      const std::string& getName() const  { return libname; }
      void addStaticFactory(const std::string& component_name,
        ComponentFactory* factory)
        { factoryMap.insert(factoryMapType::value_type(component_name, factory)); }
  };

  class Comploader
  {
    private:
      typedef std::map<std::string, ComponentLibrary> librarymap_type;
      typedef std::map<Compident, Component*> componentmap_type;
      typedef std::list<std::string> search_path_type;

      // loaded libraries
      static cxxtools::RWLock libraryMonitor;
      static librarymap_type librarymap;

      // map soname/compname to compinstance
      cxxtools::RWLock componentMonitor;
      componentmap_type componentmap;
      static const Tntconfig* config;
      static search_path_type search_path;
      static bool staticFactoryAddEnabled;

    public:
      Comploader();
      virtual ~Comploader();

      virtual Component& fetchComp(const Compident& compident,
        const Urlmapper& rootmapper);
      virtual Component* createComp(const Compident& compident,
        const Urlmapper& rootmapper);
      const char* getLangData(const Compident& compident, const std::string& lang);

      // lookup library; load if needed
      virtual ComponentLibrary& fetchLib(const std::string& libname);

      void cleanup(unsigned seconds);  // delete comps older than seconds

      static const Tntconfig& getConfig()        { return *config; }
      static void configure(const Tntconfig& config);
      static void addStaticFactory(const std::string& component_name, ComponentFactory* factory);
  };
}

#endif // TNT_COMPLOADER_H

