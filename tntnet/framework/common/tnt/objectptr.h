/* tnt/objectptr.h
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


#ifndef TNT_OBJECTPTR_H
#define TNT_OBJECTPTR_H

#include <tnt/object.h>

namespace tnt
{
  class Objectptr
  {
      Object* ptr;

    public:
      Objectptr(Object* p)
        : ptr(p)
        {
          if (ptr)
            ptr->addRef();
        }
      ~Objectptr()
        {
          if (ptr)
            ptr->release();
        }
      Objectptr& operator= (const Objectptr& p)
        {
          if (ptr)
            ptr->release();
          ptr = p.ptr;
          if (ptr)
            ptr->addRef();
          return *this;
        }

      const Object* getPtr() const      { return ptr; }
      Object* getPtr()                  { return ptr; }
      operator const Object* () const   { return ptr; }
      operator Object* ()               { return ptr; }
      const Object* operator-> () const { return ptr; }
      Object* operator-> ()             { return ptr; }
  };
}

#endif // TNT_OBJECTPTR_H

