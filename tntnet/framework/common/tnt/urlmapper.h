////////////////////////////////////////////////////////////////////////
// urlmapper.h
//

#ifndef URLMAPPER_H
#define URLMAPPER_H

#include "component.h"

namespace tnt
{
  // map compUrl to compident
  class urlmapper
  {
    public:
      virtual ~urlmapper()  { }
      virtual compident mapComp(const std::string& compUrl) const = 0;
  };
}

#endif // URLMAPPER_H

