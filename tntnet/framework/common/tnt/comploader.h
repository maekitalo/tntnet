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
#include "urlmapper.h"
#include <map>
#include <set>
#include <list>
#include <string>
#include <utility>
#include <cxxtools/thread.h>

namespace tnt
{
  class comploader;
  class tntconfig;

  /// Hält Symbole zum erzeugen von Komponenten.
  class component_library : public cxxtools::dl::library
  {
      typedef component* (*creator_type)(const compident&,
        const urlmapper&, comploader&);
      typedef std::map<std::string, creator_type> creatormap_type;

      creatormap_type creatormap;
      std::string libname;

    public:
      component_library()
        { }

      component_library(const std::string& path, const std::string& name)
        : cxxtools::dl::library((path + '/' + name).c_str()),
          libname(name)
        { }

      component_library(const std::string& name)
        : cxxtools::dl::library(name.c_str()),
          libname(name)
        { }

      component* create(const std::string& component_name, comploader& cl,
        const urlmapper& rootmapper);

      const std::string& getName() const  { return libname; }
  };

  /// pro-Thread Klasse zum laden von Komponenten
  class comploader
  {
    private:
      typedef std::map<std::string, component_library> librarymap_type;
      typedef std::map<compident, component*> componentmap_type;
      typedef std::list<std::string> search_path_type;

      // loaded libraries
      static cxxtools::RWLock libraryMonitor;
      static librarymap_type librarymap;

      // map soname/compname to compinstance
      cxxtools::RWLock componentMonitor;
      componentmap_type componentmap;
      const tntconfig& config;
      static search_path_type search_path;

    public:
      comploader(const tntconfig& config_)
        : config(config_)
        { }
      virtual ~comploader();

      virtual component& fetchComp(const compident& compident,
        const urlmapper& rootmapper);

      // lookup library; load if needed
      virtual component_library& fetchLib(const std::string& libname);

      void cleanup(unsigned seconds);  // delete comps older than seconds

      const tntconfig& getConfig() const  { return config; }

      static void addSearchPath(const std::string& path)
        { search_path.push_back(path); }
  };
}

#endif // TNT_COMPLOADER_H

