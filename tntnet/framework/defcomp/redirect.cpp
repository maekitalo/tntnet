/* redirect.cpp
   Copyright (C) 2003 Tommi Mäkitalo

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

#include <tnt/component.h>
#include <tnt/http.h>
#include <fstream>
#include <cxxtools/thread.h>

namespace tnt
{
  class urlmapper;
  class comploader;
}

static Mutex mutex;
static tnt::component* theComponent = 0;
static unsigned refs = 0;

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::component* create_redirect(const tnt::compident& ci, const tnt::urlmapper& um,
    tnt::comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class redirectcomp : public tnt::component
{
  protected:
    virtual ~redirectcomp() { };

  public:
    virtual unsigned operator() (tnt::httpRequest& request,
      tnt::httpReply& reply, query_params& qparam);
    virtual bool drop();
};

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::component* create_redirect(const tnt::compident& ci, const tnt::urlmapper& um,
  tnt::comploader& cl)
{
  MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new redirectcomp();
    refs = 1;
  }

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//
unsigned redirectcomp::operator() (tnt::httpRequest& request,
  tnt::httpReply& reply, query_params&)
{
  return reply.redirect(request.getPathInfo());
}

bool redirectcomp::drop()
{
  MutexLock lock(mutex);
  if (--refs == 0)
  {
     delete this;
     theComponent = 0;
     return true;
  }
  else
    return false;
}

