/* scope.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include <tnt/scope.h>
#include <cxxtools/thread.h>
#include <cxxtools/log.h>

namespace tnt
{
  log_define("tntnet.scope")

  Scope::Scope()
    : refs(1)
  {
    log_debug("new Scope " << this);
  }

  Scope::~Scope()
  {
    log_debug("Scope " << this << " deleted");
  }

  void Scope::addRef()
  {
    cxxtools::MutexLock lock(refmutex);
    log_debug("Scope::addRef(); this=" << this << " refs=" << refs);
    ++refs;
  }

  void Scope::release()
  {
    refmutex.lock();
    log_debug("Scope::release(); this=" << this << " refs=" << refs);
    if (--refs == 0)
    {
      refmutex.unlock();
      log_debug("delete Scope " << this);
      delete this;
    }
    else
      refmutex.unlock();
  }

  Scope::pointer_type Scope::get(const std::string& key)
  {
    container_type::iterator it = data.find(key);
    Scope::pointer_type ret = (it == data.end() ? Scope::pointer_type(0) : it->second);
    log_debug("Scope::get(\"" << key << "\") => " << ret.getPointer());
    return ret;
  }

  void Scope::put(const std::string& key, Scope::pointer_type o)
  {
    log_debug("Scope::put(\"" << key << "\", " << o.getPointer() << "\") Scope=" << this);
    data.insert(container_type::value_type(key, o));
  }

}
