/* componentfactory.cpp
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

#include "tnt/componentfactory.h"
#include "tnt/comploader.h"
#include <cxxtools/log.h>

log_define("tntnet.componentfactory")

namespace tnt
{
  const std::string factorySuffix = "__factory";

  ComponentFactory::ComponentFactory(const std::string& componentName)
    : configured(false)
  {
    log_debug("create componentfactory for " << componentName);
    Comploader::addStaticFactory(componentName, this);
  }

  void ComponentFactory::doConfigure(const tnt::Tntconfig& config)
  {
  }

  Component* ComponentFactory::create(const tnt::Compident& ci,
    const tnt::Urlmapper& um, tnt::Comploader& cl)
  {
    if (!configured)
    {
      doConfigure(cl.getConfig());
      configured = true;
    }

    return doCreate(ci, um, cl);
  }

  void ComponentFactory::drop(Component* comp)
  {
    log_debug("delete component " << comp);
    delete comp;
  }

  Component* SingletonComponentFactory::create(const tnt::Compident& ci,
    const tnt::Urlmapper& um, tnt::Comploader& cl)
  {
      cxxtools::MutexLock lock(mutex);
      if (theComponent == 0)
      {
        theComponent = ComponentFactory::create(ci, um, cl);
        refs = 1;
      }
      else
        ++refs;

      return theComponent;
  }

  void SingletonComponentFactory::drop(Component* comp)
  {
    cxxtools::MutexLock lock(mutex);
    if (--refs <= 0)
    {
      log_debug("delete component " << comp);
      delete theComponent;
      theComponent = 0;
    }
    else
      log_debug("component " << comp << " references left = " << refs);
  }
}
