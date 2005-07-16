/* tnt/pointer.h
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

#ifndef TNT_POINTER_H
#define TNT_POINTER_H

#include <cxxtools/thread.h>

namespace tnt
{
  /// smart pointer for objects with own reference-counting through
  /// addRef() and release()-methods.
  template <typename data_type>
  class Pointer
  {
      data_type* ptr;

    public:
      Pointer(data_type* p = 0)
        : ptr(p)
      {
        if (ptr)
          ptr->addRef();
      }

      Pointer(const Pointer& p)
        : ptr(p.ptr)
      {
        if (ptr)
          ptr->addRef();
      }

      ~Pointer()
      {
        if (ptr)
          ptr->release();
      }

      Pointer& operator= (const Pointer& s)
      {
        if (ptr)
          ptr->release();
        ptr = s.ptr;
        if (ptr)
          ptr->addRef();
        return *this;
      }

      const data_type* getPtr() const      { return ptr; }
      data_type* getPtr()                  { return ptr; }
      operator const data_type* () const   { return ptr; }
      const data_type* operator-> () const { return ptr; }
      data_type* operator-> ()             { return ptr; }
  };
}

#endif // TNT_POINTER_H

