/* unzipcomp.cpp
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
  class urlmapper;
  class comploader;
}

static cxxtools::Mutex mutex;
static tnt::component* theComponent = 0;
static unsigned refs = 0;

////////////////////////////////////////////////////////////////////////
// prototypes for external functions
//
extern "C"
{
  tnt::component* create_unzip(const tnt::compident& ci,
    const tnt::urlmapper& um, tnt::comploader& cl);
}

////////////////////////////////////////////////////////////////////////
// componentdeclaration
//
class unzipcomp : public tnt::component
{
    static std::string document_root;

  protected:
    virtual ~unzipcomp() { };

  public:
    virtual unsigned operator() (tnt::httpRequest& request,
      tnt::httpReply& reply, cxxtools::query_params& qparam);
    virtual bool drop();
};

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::component* create_unzip(const tnt::compident& ci,
  const tnt::urlmapper& um, tnt::comploader& cl)
{
  cxxtools::MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new unzipcomp();
    refs = 1;
  }
  else
    ++refs;

  return theComponent;
}

////////////////////////////////////////////////////////////////////////
// componentdefinition
//

unsigned unzipcomp::operator() (tnt::httpRequest& request,
  tnt::httpReply& reply, cxxtools::query_params& qparams)
{
  std::string pi = request.getPathInfo();

  if (request.getArgsCount() < 1)
    reply.throwError(HTTP_INTERNAL_SERVER_ERROR, "missing archive name");

  unzipFile f(request.getArg(0));
  unzipFileStream in(f, pi, false);

  // set Content-Type
  if (request.getArgs().size() > 1 && request.getArg(1).size() > 0)
    reply.setContentType(request.getArg(1));

  std::copy(std::istreambuf_iterator<char>(in),
            std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(reply.out()));

  return HTTP_OK;
}

bool unzipcomp::drop()
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

