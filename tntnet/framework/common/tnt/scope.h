/* tnt/scope.h
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

#ifndef TNT_SCOPE_H
#define TNT_SCOPE_H

#include <map>
#include <string>
#include <tnt/object.h>
#include <cxxtools/thread.h>

namespace tnt
{
  class scope
  {
      typedef std::map<std::string, object*> container_type;
      container_type data;
      mutable cxxtools::Mutex mutex;
      unsigned refs;

      // non-copyable
      scope(const object&);
      scope& operator=(const object&);

    public:
      scope() : refs(1) {}
      ~scope();

      unsigned addRef()  { return ++refs; }
      unsigned release();

      bool has(const std::string& key) const
        { return data.find(key) != data.end(); }

      object* get(const std::string& key) const;

      /// Put new object in scope. If key already exists,
      /// it is replaced and old object released.
      void replace(const std::string& key, object* o);

      /// Put new object in scope. If key already exists,
      /// o is released. Returns object identified by key.
      object* putNew(const std::string& key, object* o);

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

