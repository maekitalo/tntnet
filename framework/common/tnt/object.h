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


#ifndef TNT_OBJECT_H
#define TNT_OBJECT_H

#include <cxxtools/refcounted.h>
#include <cxxtools/smartptr.h>

namespace tnt
{
  class Object : public cxxtools::RefCounted
  {
    public:
      virtual ~Object() { }

      typedef cxxtools::SmartPtr<Object> pointer_type;

      template <typename data_type>
      data_type* cast();
  };

  template <typename data_type, template <class> class destroyPolicy = cxxtools::DeletePolicy>
  class PointerObject : public Object, public destroyPolicy<data_type>
  {
    private:
      data_type* _ptr;

    public:
      explicit PointerObject(data_type* ptr = 0)
        : _ptr(ptr)
        { }
      ~PointerObject()
        { destroyPolicy<data_type>::destroy(_ptr); }
      void set(data_type* ptr)
        { destroyPolicy<data_type>::destroy(_ptr); _ptr = ptr; }
      data_type* get() { return _ptr; }
  };

  template <typename data_type>
  Object::pointer_type createPointerObject(const data_type& d)
  {
    // assign the PointerObject to a smart pointer prior calling the ctor of
    // the created object to prevent memory leaky in case the ctor throws an
    // exception
    PointerObject<data_type>* _ptr = new PointerObject<data_type>();
    Object::pointer_type ret = _ptr;
    _ptr->set(new data_type(d));
    return ret;
  }

  template <typename data_type>
  data_type* Object::cast()
    { return static_cast<PointerObject<data_type>*>(this)->get(); }
}

#endif // TNT_OBJECT_H

