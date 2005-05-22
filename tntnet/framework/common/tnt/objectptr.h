////////////////////////////////////////////////////////////////////////
// tnt/objectptr.h
//

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

