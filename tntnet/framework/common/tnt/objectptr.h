/* tnt/objectptr.h
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

#ifndef TNT_OBJECTPTR_H
#define TNT_OBJECTPTR_H

#include <tnt/object.h>

namespace tnt
{
  class objectptr
  {
      object* ptr;
    public:
      objectptr(object* p)
        : ptr(p)
        {
          if (ptr)
            ptr->addRef();
        }
      ~objectptr()
        {
          if (ptr)
            ptr->release();
        }
      objectptr& operator= (const objectptr& p)
        {
          if (ptr)
            ptr->release();
          ptr = p.ptr;
          if (ptr)
            ptr->addRef();
        }

      const object* getPtr() const      { return ptr; }
      object* getPtr()                  { return ptr; }
      operator const object* () const   { return ptr; }
      const object* operator-> () const { return ptr; }
      object* operator-> ()             { return ptr; }
  };
}

#endif // TNT_OBJECTPTR_H

