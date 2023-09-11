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


#include <tnt/comploader.h>
#include <tnt/componentfactory.h>
#include <tnt/httperror.h>
#include <tnt/tntconfig.h>
#include <cxxtools/log.h>
#include <cxxtools/dlloader.h>
#include <stdlib.h>

log_define("tntnet.comploader")

namespace
{
    std::mutex mutex;
}

namespace tnt
{
////////////////////////////////////////////////////////////////////////
// ComponentLibrary
//

ComponentLibrary::~ComponentLibrary()
{
    if (_handle)
        ::dlclose(_handle);
}

void* ComponentLibrary::dlopen(const std::string& name, bool local)
{
    log_debug("dlopen <" << name << ">, " << local);

    int flags = local ? RTLD_NOW|RTLD_LOCAL: RTLD_NOW|RTLD_GLOBAL;
    std::string n = name;
    if (!n.empty() && n[0] == '!')
    {
        flags = RTLD_NOW|RTLD_GLOBAL;
        n.erase(0, 1);
        log_debug("dlopen => <" << n << '>');
    }

    void* ret = ::dlopen((n + ".so").c_str(), flags);
    if (ret != 0)
    {
        log_info("library \"" << n << ".so\"");
        return ret;
    }

    ret = ::dlopen((n + ".a").c_str(), flags);
    if (ret != 0)
    {
        log_info("library \"" << n << ".a\"");
        return ret;
    }

    ret = ::dlopen((n + ".dll").c_str(), flags);
    if (ret != 0)
    {
        log_info("library \"" << n << ".dll\"");
        return ret;
    }

    ret = ::dlopen(n.c_str(), flags);
    if (ret == 0)
        log_warn("failed to load library \"" << n << '"');
    else
        log_info("library \"" << n << "\"");

    return ret;
}

Component* ComponentLibrary::create(const std::string& component_name,
                                    Comploader& cl, const Urlmapper& rootmapper)
{
    log_debug("create \"" << component_name << '"');

    ComponentFactory* factory;

    // look for factory in my map
    factoryMapType::const_iterator i = _factoryMap.find(component_name);
    if (i == _factoryMap.end())
        throw NotFoundException(component_name);

    factory = i->second;

    // call the creator
    Compident ci = Compident(_libname, component_name);
    log_debug("call creator for \"" << ci << '"');

    return factory->create(ci, rootmapper, cl);
}

////////////////////////////////////////////////////////////////////////
// Comploader
//
Comploader::librarymap_type& Comploader::getLibrarymap()
{
    static librarymap_type librarymap;
    return librarymap;
}

ComponentLibrary::factoryMapType* Comploader::currentFactoryMap = 0;

Component& Comploader::fetchComp(const Compident& ci, const Urlmapper& rootmapper)
{
    log_debug("fetchComp \"" << ci << '"');

    std::lock_guard<std::mutex> lock(mutex);

    // lookup Component
    componentmap_type::iterator it = componentmap.find(ci);
    if (it == componentmap.end())
    {
        it = componentmap.find(ci);
        if (it == componentmap.end())
        {
            ComponentLibrary& lib = fetchLib(ci.libname);
            Component* comp = lib.create(ci.compname, *this, rootmapper);

            componentmap[ci] = comp;
            return *comp;
        }
    }

    return *(it->second);
}

Component* Comploader::createComp(const Compident& ci, const Urlmapper& rootmapper)
{
    log_debug("createComp \"" << ci << '"');

    ComponentLibrary& lib = fetchLib(ci.libname);
    Component* comp = lib.create(ci.compname, *this, rootmapper);
    return comp;
}

namespace
{
    template <typename T>
    class ValueResetter
    {
        T& _value;
        T _null;

    public:
        explicit ValueResetter(T& value, T null = T())
          : _value(value),
            _null(null)
          { }

        ~ValueResetter()
          { _value = _null; }
    };
}

ComponentLibrary& Comploader::fetchLib(const std::string& libname)
{
    log_debug("fetchLib \"" << libname << '"');

    std::string n = libname;
    bool local = false;
    if (!n.empty() && n[0] == '!')
    {
        local = true;
        n.erase(0, 1);
    }

    librarymap_type& librarymap = getLibrarymap();
    librarymap_type::iterator it = librarymap.find(n);
    if (it == librarymap.end())
    {
        ComponentLibrary::factoryMapType factoryMap;
        currentFactoryMap = &factoryMap;
        ValueResetter<ComponentLibrary::factoryMapType*> valueResetter(currentFactoryMap, 0);

        // load library
        log_info("load library \"" << n << '"');
        std::unique_ptr<ComponentLibrary> lib;

        for (TntConfig::CompPathType::const_iterator p = TntConfig::it().compPath.begin();
             (!lib || !lib->loaded()) && p != TntConfig::it().compPath.end(); ++p)
        {
            log_debug("load library \"" << n << "\" from " << *p << " dir");
            lib.reset(new ComponentLibrary(*p, n, local));
        }

        if (!lib || !lib->loaded())
        {
            log_debug("load library \"" << n << "\" from current dir");
            lib.reset(new ComponentLibrary(".", n, local));
        }

#ifdef PKGLIBDIR
        if (!lib || !lib->loaded())
        {
            log_debug("load library \"" << n << "\" from package lib dir <" << PKGLIBDIR << '>');
            lib.reset(new ComponentLibrary(PKGLIBDIR, n, local));
        }
#endif

        if (!lib || !lib->loaded())
        {
            log_debug("library \"" << n << "\" in current dir not found - search lib-path");
            lib.reset(new ComponentLibrary(n, local));
        }

        if (!lib || !lib->loaded())
            throw LibraryNotFound(n);

        lib->_factoryMap = factoryMap;
        log_debug("insert new library " << n);
        it = librarymap.emplace(n, std::move(lib)).first;
    }
    else
        log_debug("library " << n << " found");

    return *it->second;
}

void Comploader::registerFactory(const std::string& component_name, ComponentFactory* factory)
{
    log_debug("Comploader::registerFactory(" << component_name << ", " << factory << ')');

    if (currentFactoryMap)
        currentFactoryMap->insert(ComponentLibrary::factoryMapType::value_type(component_name, factory));
    else
    {
        librarymap_type& librarymap = getLibrarymap();
        log_debug("register component without library-name");
        librarymap_type::iterator it = librarymap.find(std::string());
        if (it == librarymap.end())
        {
            // empty library not in map - create a new empty library-object
            it = librarymap.emplace(std::string(), std::unique_ptr<ComponentLibrary>(new ComponentLibrary())).first;
        }
        it->second->registerFactory(component_name, factory);
    }
}

}
