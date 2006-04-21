/* scope.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
