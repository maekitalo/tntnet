/* static.cpp
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

#include "static.h"
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/http.h>
#include <tnt/comploader.h>
#include <fstream>
#include <cxxtools/log.h>
#include <cxxtools/thread.h>
#include <sys/types.h>
#include <sys/stat.h>

log_define("tntnet.static");

static cxxtools::Mutex mutex;
static tnt::component* theComponent = 0;
static unsigned refs = 0;

////////////////////////////////////////////////////////////////////////
// external functions
//

tnt::component* create_static(const tnt::compident& ci,
  const tnt::urlmapper& um, tnt::comploader& cl)
{
  cxxtools::MutexLock lock(mutex);
  if (theComponent == 0)
  {
    theComponent = new tntcomp::staticcomp();
    refs = 1;

    tntcomp::staticcomp::setDocumentRoot(cl.getConfig().getValue("DocumentRoot"));
  }
  else
    ++refs;

  return theComponent;
}

namespace tntcomp
{
  //////////////////////////////////////////////////////////////////////
  // componentdefinition
  //

  std::string staticcomp::document_root;

  unsigned staticcomp::operator() (tnt::httpRequest& request,
    tnt::httpReply& reply, cxxtools::query_params& qparams)
  {
    if (!tnt::httpRequest::checkUrl(request.getPathInfo()))
      throw tnt::httpError(HTTP_BAD_REQUEST, "illegal url");

    std::string file;
    if (!document_root.empty())
      file = document_root + '/';
    file += request.getPathInfo();

    log_debug("file: " << file);

    struct stat st;
    if (stat(file.c_str(), &st) != 0)
    {
      log_warn("error in stat for file \"" << file << "\"");
      reply.throwNotFound(request.getPathInfo());
    }

    if (!S_ISREG(st.st_mode))
    {
      log_warn("no regular file \"" << file << "\"");
      reply.throwNotFound(request.getPathInfo());
    }

    std::string lastModified = tnt::httpMessage::htdate(st.st_ctime);

    {
      std::string s = request.getHeader(tnt::httpMessage::IfModifiedSince);
      if (s == lastModified)
        return HTTP_NOT_MODIFIED;
    }

    std::ifstream in(file.c_str());

    if (!in)
    {
      log_warn("file \"" << file << "\" not found");
      reply.throwNotFound(request.getPathInfo());
    }

    // set Content-Type
    if (request.getArgs().size() > 0 && request.getArg(0).size() > 0)
      reply.setContentType(request.getArg(0));

    reply.setHeader(tnt::httpMessage::Last_Modified, lastModified);

    // send data
    reply.out() << in.rdbuf();

    return HTTP_OK;
  }

  bool staticcomp::drop()
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

}
