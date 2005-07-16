/* unzipcomp.cpp
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

#include "unzip.h"

#include <tnt/component.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <fstream>
#include <cxxtools/thread.h>
#include "unzip_pp.h"

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
  tnt::Component* create_unzip(const tnt::Compident& ci,
    const tnt::Urlmapper& um, tnt::Comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class Unzipcomp : public tnt::Component
{
    static std::string document_root;

  protected:
    virtual ~Unzipcomp() { };

  public:
    virtual unsigned operator() (tnt::HttpRequest& request,
      tnt::HttpReply& reply, cxxtools::QueryParams& qparam);
    virtual bool drop();
};

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::Component* create_unzip(const tnt::Compident& ci,
  const tnt::Urlmapper& um, tnt::Comploader& cl)
{
  cxxtools::MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new Unzipcomp();
    refs = 1;
  }
  else
    ++refs;

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//

unsigned Unzipcomp::operator() (tnt::HttpRequest& request,
  tnt::HttpReply& reply, cxxtools::QueryParams& qparams)
{
  std::string pi = request.getPathInfo();

  if (request.getArgsCount() < 1)
    reply.throwError(HTTP_INTERNAL_SERVER_ERROR, "missing archive name");

  try
  {
    unzipFile f(request.getArg(0));
    unzipFileStream in(f, pi, false);

    // set Content-Type
    if (request.getArgs().size() > 1 && request.getArg(1).size() > 0)
      reply.setContentType(request.getArg(1));

    std::copy(std::istreambuf_iterator<char>(in),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(reply.out()));
  }
  catch (const unzipEndOfListOfFile&)
  {
    reply.throwNotFound(pi);
  }

  return HTTP_OK;
}

bool Unzipcomp::drop()
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

