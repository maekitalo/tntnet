/* tnt/scope.h
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


#ifndef TNT_SCOPE_H
#define TNT_SCOPE_H

#include <map>
#include <string>
#include <tnt/object.h>
#include <cxxtools/mutex.h>
#include <cxxtools/smartptr.h>

namespace tnt
{
  class Scope
  {
    public:
      typedef Object::pointer_type pointer_type;

    private:
      typedef std::map<std::string, pointer_type> container_type;

      container_type data;
      mutable cxxtools::Mutex mutex;
      cxxtools::Mutex refmutex;
      unsigned refs;

      // non-copyable
      Scope(const Scope&);
      Scope& operator=(const Scope&);

    public:
      Scope();
      virtual ~Scope();

      void lock()   { mutex.lock(); }
      void unlock() { mutex.unlock(); }

      void addRef();
      void release();

      bool has(const std::string& key) const
        { return data.find(key) != data.end(); }

      pointer_type get(const std::string& key);

      /// Put new Object in scope. If key already exists,
      /// it is replaced and old Object released.
      void put(const std::string& key, pointer_type o);

      void erase(const std::string& key)  { data.erase(key); }
      bool empty() const  { return data.empty(); }
      void clear()        { data.clear(); }

      typedef container_type::const_iterator const_iterator;
      typedef container_type::iterator iterator;
      typedef container_type::value_type value_type;
      const_iterator begin() const   { return data.begin(); }
      const_iterator end()  const    { return data.end(); }
      iterator begin()               { return data.begin(); }
      iterator end()                 { return data.end(); }
  };
}

#endif // TNT_SCOPE_H

