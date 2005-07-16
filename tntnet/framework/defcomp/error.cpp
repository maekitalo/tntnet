/* error.cpp
   Copyright (C) 2003 Tommi Maekitalo

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
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <fstream>
#include <cxxtools/thread.h>

namespace tnt
{
  class Urlmapper;
  class Comploader;
}

static cxxtools::Mutex mutex;
static tnt::Component* theComponent = 0;
static unsigned refs = 0;

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::Component* create_error(const tnt::Compident& ci, const tnt::Urlmapper& um,
    tnt::Comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class Errorcomp : public tnt::Component
{
  protected:
    virtual ~Errorcomp() { };

  public:
    virtual unsigned operator() (tnt::HttpRequest& request,
      tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
    virtual bool drop();
};

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::Component* create_error(const tnt::Compident& ci, const tnt::Urlmapper& um,
  tnt::Comploader& cl)
{
  cxxtools::MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new Errorcomp();
    refs = 1;
  }

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//
unsigned Errorcomp::operator() (tnt::HttpRequest& request,
  tnt::HttpReply& reply, cxxtools::QueryParams&)
{
  std::string msg;

  const tnt::HttpRequest::args_type& args = request.getArgs();

  tnt::HttpRequest::args_type::const_iterator i = args.begin();
  if (i == args.end())
    reply.throwError("400 internal error");

  msg = *i++;
  for ( ; i != args.end(); ++i)
  {
    msg += ' ';
    msg += *i;
  }

  reply.throwError(msg);

  return DECLINED;
}

bool Errorcomp::drop()
{
  cxxtools::MutexLock lock(mutex);
  if (--refs == 0)
  {
     delete this;
     theComponent = 0;
     return true;
  }
  else
    return false;
}

