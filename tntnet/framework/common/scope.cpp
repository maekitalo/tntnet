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

  static unsigned scopes_total = 0;

  Scope::Scope()
    : refs(1)
  {
    ++scopes_total;
    log_debug("new Scope " << this << " total=" << scopes_total);
  }

  Scope::~Scope()
  {
    --scopes_total;
    log_debug("Scope " << this << " deleted; " << scopes_total << " left");
  }

  void Scope::addRef()
  {
    ++refs;
    log_debug("Scope::addRef(); this=" << this << " refs=" << refs);
  }

  void Scope::release()
  {
    log_debug("Scope::release(); this=" << this << " refs=" << refs);
    if (--refs == 0)
    {
      log_debug("delete Scope " << this);
      delete this;
    }
  }

  Object* Scope::get(const std::string& key)
  {
    container_type::iterator it = data.find(key);

    log_debug("Scope::get(\"" << key << "\") Scope=" << this
           << " => " << (it == data.end() ? 0 : it->second));

    return it == data.end() ? 0 : it->second.getPtr();
  }

  void Scope::replace(const std::string& key, Object* o)
  {
    log_debug("Scope::replace(\"" << key << ", " << o << "\") Scope=" << this);

    o->addRef();
    container_type::iterator it = data.find(key);
    if (it == data.end())
      data.insert(container_type::value_type(key, o));
    else
    {
      it->second->release();
      it->second = o;
    }
  }

  Object* Scope::putNew(const std::string& key, Object* o)
  {
    log_debug("Scope::putNew(\"" << key << "\", " << o << ") Scope=" << this);

    container_type::iterator it = data.find(key);
    if (it == data.end())
    {
      data.insert(container_type::value_type(key, o));
      return o;
    }
    else
    {
      o->addRef();
      o->release();
      return it->second.getPtr();
    }
  }

  void Scope::erase(const std::string& key)
  {
    container_type::iterator it = data.find(key);
    if (it != data.end())
    {
      it->second->release();
      data.erase(it);
    }
  }
}
