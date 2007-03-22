/* tnt/componentfactory.h
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
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

#define TNT_COMPONENT(componentName) \
  extern "C" { \
    tnt::ComponentFactoryImpl<componentName> componentName ## __factory( \
        #componentName );   \
  }

#define TNT_SINGLETONCOMPONENT(componentName) \
  extern "C" { \
    tnt::ComponentFactoryImpl<componentName, \
      tnt::SingletonComponentFactory> componentName ## __factory( \
        #componentName );   \
  }

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

  template <typename componentType>
  class FactoryComponent : public componentType
  {
      tnt::ComponentFactory* factory;

    public:
      FactoryComponent(tnt::ComponentFactory* factory_,
          const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)
        : componentType(ci, um, cl),
          factory(factory_)
        { }
      void drop()
        { if (factory) factory->drop(this); }
  };

  template <typename ComponentType, typename FactoryBaseType = tnt::ComponentFactory>
  class ComponentFactoryImpl : public FactoryBaseType
  {
    public:
      ComponentFactoryImpl(const std::string& componentName)
        : FactoryBaseType(componentName)
        { }

      virtual Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl)
      {
        return new FactoryComponent<ComponentType>(this, ci, um, cl);
      }
  };

}

#endif // TNT_COMPONENTFACTORY_H
