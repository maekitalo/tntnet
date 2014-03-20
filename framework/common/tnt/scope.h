/*
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef TNT_SCOPE_H
#define TNT_SCOPE_H

#include <map>
#include <string>
#include <tnt/object.h>
#include <cxxtools/mutex.h>
#include <cxxtools/refcounted.h>

namespace tnt
{
  template <typename objectType>
  class NullDestroyPolicy
  {
    public:
      static void destroy(objectType* ptr) { }
  };

  class Scope : public cxxtools::AtomicRefCounted
  {
    public:
      typedef Object::pointer_type pointer_type;

    private:
      typedef std::map<std::string, pointer_type> container_type;

      container_type _data;
      mutable cxxtools::Mutex _mutex;

      void privatePut(const std::string& key, pointer_type o);

      // non-copyable
      Scope(const Scope&);
      Scope& operator=(const Scope&);

    public:
      Scope();
      virtual ~Scope();

      void lock()   { _mutex.lock(); }
      void unlock() { _mutex.unlock(); }

      bool has(const std::string& key) const
        { return _data.find(key) != _data.end(); }

      template <typename T> T*
      get(const std::string& key)
      {
        container_type::iterator it = _data.find(key);
        return it == _data.end() ? 0 : it->second->cast<T>();
      }

      /// Put new Object in scope. If key already exists,
      /// it is replaced and old Object released.
      template <typename T, template <class> class destroyPolicy>
      void put(const std::string& key, T* o)
      {
        try
        {
          tnt::PointerObject<T, destroyPolicy>* ptr = new tnt::PointerObject<T, destroyPolicy>(o);
          privatePut(key, ptr);
        }
        catch (const std::bad_alloc&)
        {
          destroyPolicy<T>::destroy(o);
          throw;
        }
      }

      template <typename T>
      void put(const std::string& key, T* o)
      { put<T, cxxtools::DeletePolicy>(key, o); }

      template <typename T>
      void put(const std::string& key, T* o, bool transferOwnership)
      {
        if (transferOwnership)
          put<T, cxxtools::DeletePolicy>(key, o);
        else
          put<T, NullDestroyPolicy>(key, o);
      }

      void erase(const std::string& key) { _data.erase(key); }
      bool empty() const                 { return _data.empty(); }
      void clear()                       { _data.clear(); }

      typedef container_type::const_iterator const_iterator;
      typedef container_type::iterator iterator;
      typedef container_type::value_type value_type;
      const_iterator begin() const   { return _data.begin(); }
      const_iterator end()  const    { return _data.end(); }
      iterator begin()               { return _data.begin(); }
      iterator end()                 { return _data.end(); }
  };
}

#endif // TNT_SCOPE_H

