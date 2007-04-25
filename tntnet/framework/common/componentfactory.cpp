/* componentfactory.cpp
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


#include "tnt/componentfactory.h"
#include "tnt/comploader.h"
#include <cxxtools/log.h>

log_define("tntnet.componentfactory")

namespace tnt
{
  const std::string factorySuffix = "__factory";

  ComponentFactory::ComponentFactory(const std::string& componentName)
    : theComponent(0)
  {
    log_debug("create componentfactory for " << componentName);
    Comploader::registerFactory(componentName, this);
  }

  ComponentFactory::~ComponentFactory()
  {
    delete theComponent;
  }

  void ComponentFactory::doConfigure(const tnt::Tntconfig& config)
  {
  }

  Component* ComponentFactory::create(const tnt::Compident& ci,
    const tnt::Urlmapper& um, tnt::Comploader& cl)
  {
    if (theComponent == 0)
    {
      doConfigure(cl.getConfig());
      theComponent = doCreate(ci, um, cl);
    }

    return theComponent;
  }
}
