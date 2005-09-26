/* tnt/componentfactory.h
   Copyright (C) 2005 Tommi Maekitalo

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

#ifndef TNT_COMPONENTFACTORY_H
#define TNT_COMPONENTFACTORY_H

#include <tnt/component.h>
#include <tnt/comploader.h>
#include <cxxtools/thread.h>

#define TNT_COMPONENTFACTORY(componentName, factoryType, factoryName) \
  extern "C" { factoryType componentName ## _factory; } \
  static factoryType& factoryName = componentName ## _factory;

namespace tnt
{
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
      ComponentFactory()
        : configured(false)
        { }

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
      SingletonComponentFactory()
        : theComponent(0),
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
      virtual Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl)
      {
        return new ComponentType(ci, um, cl);
      }
  };

}

#endif // TNT_COMPONENT_H
