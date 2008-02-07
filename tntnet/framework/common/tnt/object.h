/* tnt/object.h
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


#ifndef TNT_OBJECT_H
#define TNT_OBJECT_H

#include <cxxtools/refcounted.h>

namespace tnt
{
  class Object : public cxxtools::RefCounted
  {
    public:
      virtual ~Object()
      { }
  };

  template <typename data_type>
  class PointerObject : public Object
  {
      data_type* ptr;

    public:
      explicit PointerObject(data_type* ptr_ = 0)
        : ptr(ptr_)
        { }
      ~PointerObject()
        { delete ptr; }
      void set(data_type* ptr_)
        { delete ptr; ptr = ptr_; }
      data_type* get()  { return ptr; }
  };
}

#endif // TNT_OBJECT_H

