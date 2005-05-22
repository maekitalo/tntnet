////////////////////////////////////////////////////////////////////////
// tnt/object.cpp
//

#include <tnt/object.h>

namespace tnt
{
  object::~object()
  { }

  unsigned object::addRef()
  {
    return ++refs;
  }

  unsigned object::release()
  {
    if (--refs == 0)
    {
      delete this;
      return 0;
    }
    else
      return refs;
  }
}
