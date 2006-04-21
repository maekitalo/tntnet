/* tnt/scope.h
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

#ifndef TNT_SCOPE_H
#define TNT_SCOPE_H

#include <map>
#include <string>
#include <tnt/object.h>
#include <tnt/pointer.h>
#include <cxxtools/thread.h>

namespace tnt
{
  class Scope
  {
      typedef std::map<std::string, Pointer<Object> > container_type;
      container_type data;
      mutable cxxtools::Mutex mutex;
      unsigned refs;

      // non-copyable
      Scope(const Scope&);
      Scope& operator=(const Scope&);

      friend class HttpRequest;

    public:
      Scope();
      ~Scope();

      void lock()   { mutex.lock(); }
      void unlock() { mutex.unlock(); }

      void addRef();
      void release();

      bool has(const std::string& key) const
        { return data.find(key) != data.end(); }

      Object* get(const std::string& key);

      /// Put new Object in scope. If key already exists,
      /// it is replaced and old Object released.
      void replace(const std::string& key, Object* o);

      /// Put new Object in scope. If key already exists,
      /// o is released. Returns Object identified by key.
      Object* putNew(const std::string& key, Object* o);

      void erase(const std::string& key);
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

