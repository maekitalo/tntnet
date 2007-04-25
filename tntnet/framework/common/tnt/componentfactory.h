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

#include <string>

namespace tnt
{
  extern const std::string factorySuffix;

  class Component;
  class Compident;
  class Comploader;
  class Urlmapper;
  class Tntconfig;

  class ComponentFactory
  {
      // noncopyable
      ComponentFactory(const ComponentFactory&);
      ComponentFactory& operator= (const ComponentFactory&);

      Component* theComponent;

    protected:
      virtual Component* doCreate(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl) = 0;
      virtual void doConfigure(const tnt::Tntconfig& config);

    public:
      ComponentFactory(const std::string& componentName_);
      ~ComponentFactory();

      virtual Component* create(const tnt::Compident& ci,
        const tnt::Urlmapper& um, tnt::Comploader& cl);
  };

  template <typename ComponentType>
  class ComponentFactoryImpl : public ComponentFactory
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
