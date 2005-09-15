/* tnt/comploader.h
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

#ifndef TNT_COMPLOADER_H
#define TNT_COMPLOADER_H

#include <cxxtools/dlloader.h>
#include <tnt/urlmapper.h>
#include <map>
#include <set>
#include <list>
#include <string>
#include <utility>
#include <cxxtools/thread.h>

namespace tnt
{
  class Comploader;
  class ComponentFactory;
  class Tntconfig;

  /// Hält Symbole zum erzeugen von Komponenten.
  class ComponentLibrary : public cxxtools::dl::Library
  {
      typedef std::map<std::string, ComponentFactory*> factoryMapType;

      factoryMapType factoryMap;
      std::string libname;

    public:
      ComponentLibrary()
        { }

      ComponentLibrary(const std::string& path, const std::string& name)
        : cxxtools::dl::Library((path + '/' + name).c_str()),
          libname(name)
        { }

      ComponentLibrary(const std::string& name)
        : cxxtools::dl::Library(name.c_str()),
          libname(name)
        { }

      Component* create(const std::string& component_name, Comploader& cl,
        const Urlmapper& rootmapper);

      const std::string& getName() const  { return libname; }
  };

  /// pro-Thread Klasse zum laden von Komponenten
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
      const Tntconfig& config;
      static search_path_type search_path;

    public:
      Comploader(const Tntconfig& config_);
      virtual ~Comploader();

      virtual Component& fetchComp(const Compident& compident,
        const Urlmapper& rootmapper);

      // lookup library; load if needed
      virtual ComponentLibrary& fetchLib(const std::string& libname);

      void cleanup(unsigned seconds);  // delete comps older than seconds

      const Tntconfig& getConfig() const  { return config; }

      static void addSearchPath(const std::string& path)
        { search_path.push_back(path); }
  };
}

#endif // TNT_COMPLOADER_H

