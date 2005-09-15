/* componentfactory.cpp
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

#include "tnt/componentfactory.h"

namespace tnt
{
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
    delete comp;
  }

  Component* SingletonComponentFactory::create(const tnt::Compident& ci,
    const tnt::Urlmapper& um, tnt::Comploader& cl)
  {
      cxxtools::MutexLock lock(mutex);
      if (theComponent == 0)
      {
        theComponent = doCreate(ci, um, cl);
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
      delete theComponent;
      theComponent = 0;
    }
  }
}
