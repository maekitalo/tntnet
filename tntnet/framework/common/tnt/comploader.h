/* tnt/comploader.h
   Copyright (C) 2003 Tommi MÃ¤kitalo

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

#ifndef COMPLOADER_H
#define COMPLOADER_H

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

  /// Hält Symbole zum erzeugen von Komponenten.
  class component_library : public dl::library
  {
      typedef component* (*creator_type)(const compident&,
        const urlmapper&, comploader&);
      typedef std::map<std::string, creator_type> creatormap_type;

      creatormap_type creatormap;
      std::string libname;

    public:
      component_library()
        { }

      component_library(const std::string& path, const std::string& name,
          int flag = RTLD_LAZY)
        : dl::library((path + '/' + name + ".so").c_str(), flag),
          libname(name)
        { }

      component_library(const std::string& name, int flag = RTLD_LAZY)
        : dl::library((name + ".so").c_str(), flag),
          libname(name)
        { }

      component* create(const std::string& component_name, comploader& cl,
        const urlmapper& rootmapper);

      const std::string& getName() const  { return libname; }
  };

  /// pro-Thread Klasse zum laden von Komponenten
  class comploader
  {
    public:
      class load_library_listener
      {
        public:
          virtual void onLoadLibrary(component_library&) = 0;
          virtual void onCreateComponent(component_library&,
            const compident& ci, component&) = 0;
      };

    private:
      typedef std::map<std::string, component_library> librarymap_type;
      typedef std::map<compident, component*> componentmap_type;
      typedef std::set<load_library_listener*> listener_container_type;
      typedef std::list<std::string> search_path_type;

      // loaded libraries
      static RWLock libraryMonitor;
      static librarymap_type librarymap;

      // map soname/compname to compinstance
      RWLock componentMonitor;
      componentmap_type componentmap;
      listener_container_type listener;
      static search_path_type search_path;

    public:
      virtual ~comploader();

      virtual component& fetchComp(const compident& compident,
        const urlmapper& rootmapper);

      // lookup library; load if needed
      virtual component_library& fetchLib(const std::string& libname);

      void addLoadLibraryListener(load_library_listener* listener);
      bool removeLoadLibraryListener(load_library_listener* listener);

      void cleanup(unsigned seconds);  // delete comps older than seconds

      static void addSearchPath(const std::string& path)
        { search_path.push_back(path); }
  };
}

#endif // COMPLOADER_H

