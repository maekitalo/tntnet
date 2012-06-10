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


#ifndef TNT_COMPLOADER_H
#define TNT_COMPLOADER_H

#include <cxxtools/smartptr.h>
#include <tnt/urlmapper.h>
#include <tnt/langlib.h>
#include <map>
#include <list>
#include <string>
#include <utility>
#include <dlfcn.h>

namespace tnt
{
  class Comploader;
  class ComponentFactory;

  /// @internal
  class LibraryNotFound
  {
      std::string libname;
    public:
      explicit LibraryNotFound(const std::string& libname_)
        : libname(libname_)
        { }
      const std::string& getLibname() const  { return libname; }
  };

  template <typename objectType>
  class Dlcloser
  {
    protected:
      void destroy(objectType* ptr)
      {
        ::dlclose(*ptr);
        delete ptr;
      }
  };

  class ComponentLibrary
  {
      friend class Comploader;

      typedef void* HandleType;
      typedef cxxtools::SmartPtr<HandleType, cxxtools::ExternalRefCounted, Dlcloser> HandlePointer;
      HandlePointer handlePtr;

      typedef std::map<std::string, ComponentFactory*> factoryMapType;
      factoryMapType factoryMap;

      std::string libname;
      std::string path;
      typedef std::map<std::string, LangLib::PtrType> langlibsType;
      langlibsType langlibs;

      void* dlopen(const std::string& name, bool local);
      void init(const std::string& name, bool local);

    public:
      ComponentLibrary()
        { }

      ComponentLibrary(const std::string& path_, const std::string& name, bool local)
        : libname(name),
          path(path_)
        { init(path_ + '/' + name, local); }

      ComponentLibrary(const std::string& name, bool local)
        : libname(name)
        { init(name, local); }

      operator const void* () const  { return handlePtr.getPointer(); }

      Component* create(const std::string& component_name, Comploader& cl,
        const Urlmapper& rootmapper);
      LangLib::PtrType getLangLib(const std::string& lang);

      const std::string& getName() const  { return libname; }
      void registerFactory(const std::string& component_name, ComponentFactory* factory)
        { factoryMap.insert(factoryMapType::value_type(component_name, factory)); }
  };

  class Comploader
  {
      typedef std::map<std::string, ComponentLibrary> librarymap_type;
      typedef std::map<Compident, Component*> componentmap_type;

      // loaded libraries
      static librarymap_type& getLibrarymap();

      // map soname/compname to compinstance
      componentmap_type componentmap;
      static ComponentLibrary::factoryMapType* currentFactoryMap;

    public:
      Component& fetchComp(const Compident& compident,
        const Urlmapper& rootmapper = Urlmapper());
      Component* createComp(const Compident& compident,
        const Urlmapper& rootmapper);
      const char* getLangData(const Compident& compident, const std::string& lang);

      // lookup library; load if needed
      ComponentLibrary& fetchLib(const std::string& libname);

      static void registerFactory(const std::string& component_name, ComponentFactory* factory);
  };
}

#endif // TNT_COMPLOADER_H

