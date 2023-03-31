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

#include <tnt/urlmapper.h>
#include <memory>
#include <map>
#include <list>
#include <string>
#include <utility>
#include <dlfcn.h>

/// @cond internal

namespace tnt
{
class Comploader;
class ComponentFactory;

class LibraryNotFound : public std::exception
{
    std::string _libname;
    std::string _msg;

public:
    explicit LibraryNotFound(const std::string& libname)
      : _libname(libname),
        _msg("Library not found: " + libname)
      { }
    ~LibraryNotFound() throw() { }

    const char* what() const throw()
      { return _msg.c_str(); }

    const std::string& getLibname() const
      { return _libname; }
};

class ComponentLibrary
{
    friend class Comploader;

    typedef void* HandleType;

    typedef std::map<std::string, ComponentFactory*> factoryMapType;

    factoryMapType _factoryMap;
    std::string _libname;
    std::string _path;
    HandleType _handle;

    static void* dlopen(const std::string& name, bool local);

    ComponentLibrary(const ComponentLibrary&) = delete;
    ComponentLibrary& operator=(const ComponentLibrary&) = delete;

public:
    ComponentLibrary()
      : _handle(nullptr)
        { }

    ComponentLibrary(const std::string& path, const std::string& name, bool local)
      : _libname(name),
        _path(path),
        _handle(dlopen(path + '/' + name, local))
        { }

    ComponentLibrary(const std::string& name, bool local)
      : _libname(name),
        _handle(dlopen(name, local))
        { }

    ~ComponentLibrary();

    Component* create(const std::string& compname, Comploader& cl, const Urlmapper& rootmapper);

    const std::string& getName() const { return _libname; }

    void registerFactory(const std::string& compname, ComponentFactory* factory)
      { _factoryMap.emplace(compname, factory); }
};

class Comploader
{
    typedef std::map<std::string, std::unique_ptr<ComponentLibrary>> librarymap_type;
    typedef std::map<Compident, Component*> componentmap_type;

    // loaded libraries
    static librarymap_type& getLibrarymap();

    // map soname/compname to compinstance
    componentmap_type componentmap;
    static ComponentLibrary::factoryMapType* currentFactoryMap;

public:
    Component& fetchComp(const Compident& compident, const Urlmapper& rootmapper = Urlmapper());
    Component* createComp(const Compident& compident, const Urlmapper& rootmapper);
    const char* getLangData(const Compident& compident, const std::string& lang);

    // lookup library; load if needed
    ComponentLibrary& fetchLib(const std::string& libname);

    static void registerFactory(const std::string& compname, ComponentFactory* factory);
};
}

#endif // TNT_COMPLOADER_H

