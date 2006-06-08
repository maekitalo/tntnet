/* tnt/componentfactory.h
 * Copyright (C) 2005 Tommi Maekitalo
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
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef TNT_COMPONENTFACTORY_H
#define TNT_COMPONENTFACTORY_H

#include <tnt/component.h>
#include <tnt/comploader.h>
#include <cxxtools/thread.h>

#define TNT_COMPONENTFACTORY(componentName, factoryType) \
  extern "C" { \
    factoryType componentName ## __factory( #componentName );   \
  } \
  static factoryType& factory = componentName ## __factory;

namespace tnt
{
  extern const std::string factorySuffix;

  class ComponentFactory
  {
      // noncopyable
      ComponentFactory(const ComponentFactory&);
      ComponentFactory& operator= (const ComponentFactory&);

      bool configured;

    protected:
      virtual Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl) = 0;
      virtual void doConfigure(const tnt::Tntconfig& config);

    public:
      ComponentFactory(const std::string& componentName_);

      virtual Component* create(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
      virtual void drop(Component* comp);
  };

  class SingletonComponentFactory : public ComponentFactory
  {
      cxxtools::Mutex mutex;
      Component* theComponent;
      unsigned refs;

    protected:
      virtual Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl) = 0;

    public:
      SingletonComponentFactory(const std::string& componentName)
        : ComponentFactory(componentName),
          theComponent(0),
          refs(0)
      { }

      virtual Component* create(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
      virtual void drop(Component* comp);
  };

  template <typename FactoryBaseType, typename ComponentType>
  class ComponentFactoryImpl : public FactoryBaseType
  {
    public:
      ComponentFactoryImpl(const std::string& componentName)
        : ComponentFactory(componentName)
        { }

      virtual Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl)
      {
        return new ComponentType(ci, um, cl);
      }
  };

}

#endif // TNT_COMPONENTFACTORY_H
