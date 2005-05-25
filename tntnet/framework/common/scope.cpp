/* scope.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#include <tnt/scope.h>
#include <cxxtools/thread.h>
#include <cxxtools/log.h>

namespace tnt
{
  log_define("tntnet.scope");

  scope::~scope()
  {
    for (container_type::iterator it = data.begin(); it != data.end(); ++it)
      it->second->release();
  }

  unsigned scope::release()
  {
    if (--refs == 0)
    {
      delete this;
      return 0;
    }
    else
      return refs;
  }

  object* scope::get(const std::string& key) const
  {
    log_debug("scope::get(\"" << key << "\") scope=" << this);

    cxxtools::MutexLock lock(mutex);
    container_type::const_iterator it = data.find(key);

    log_debug(" => " << (it == data.end() ? 0 : it->second));

    return it == data.end() ? 0 : it->second;
  }

  void scope::replace(const std::string& key, object* o)
  {
    log_debug("scope::replace(\"" << key << ", " << o << "\") scope=" << this);

    o->addRef();
    cxxtools::MutexLock lock(mutex);
    container_type::iterator it = data.find(key);
    if (it == data.end())
      data.insert(container_type::value_type(key, o));
    else
    {
      it->second->release();
      it->second = o;
    }
  }

  object* scope::putNew(const std::string& key, object* o)
  {
    log_debug("scope::putNew(\"" << key << "\", " << o << ") scope=" << this);

    cxxtools::MutexLock lock(mutex);
    container_type::iterator it = data.find(key);
    if (it == data.end())
    {
      data.insert(container_type::value_type(key, o));
      return o;
    }
    else
    {
      o->release();
      return it->second;
    }
  }

  void scope::erase(const std::string& key)
  {
    cxxtools::MutexLock lock(mutex);
    container_type::iterator it = data.find(key);
    if (it != data.end())
    {
      it->second->release();
      data.erase(it);
    }
  }
}
