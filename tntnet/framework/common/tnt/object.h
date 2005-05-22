////////////////////////////////////////////////////////////////////////
// tnt/object.h
//

#ifndef TNT_OBJECT_H
#define TNT_OBJECT_H

#include <cxxtools/thread.h>

namespace tnt
{
  class object
  {
      unsigned refs;

      // non-copyable
      object(const object&);
      object& operator=(const object&);

    protected:
      virtual ~object();

    public:
      object()
        : refs(1)
        { }

      virtual unsigned addRef();
      virtual unsigned release();
  };

}

#endif // TNT_OBJECT_H

