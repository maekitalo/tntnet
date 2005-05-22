////////////////////////////////////////////////////////////////////////
// tnt/sessionscope.h
//

#ifndef TNT_SESSIONSCOPE_H
#define TNT_SESSIONSCOPE_H

#include <tnt/scope.h>
#include <time.h>

namespace tnt
{
  class sessionscope : public scope
  {
      time_t atime;

    public:
      sessionscope()           { touch(); }
      void touch()             { time(&atime); }
      time_t getAtime() const  { return atime; }
  };
}

#endif // TNT_SESSIONSCOPE_H

