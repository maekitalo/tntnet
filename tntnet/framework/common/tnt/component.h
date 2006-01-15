/* tnt/component.h
   Copyright (C) 2003-2006 Tommi Maekitalo

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

#ifndef TNT_COMPONENT_H
#define TNT_COMPONENT_H

#include <tnt/compident.h>
#include "tnt/config.h"

#include <time.h>

#ifdef HAVE_LIMITS
#include <limits>
#endif

namespace cxxtools
{
  class QueryParams;
}

namespace tnt
{

class HttpRequest;
class HttpReply;

class Component
{
    time_t atime;

  public:
    Component()     { time(&atime); }
    virtual ~Component() { }

    void touch(time_t plus = 0)      { time(&atime); atime += plus; }
    time_t getLastAccesstime() const { return atime; }
#ifdef HAVE_LIMITS
    void lock()                      { atime = std::numeric_limits<time_t>::max(); }
#else
    void lock()                      { touch(3600); }
#endif
    void unlock()                    { touch(); }

    virtual unsigned operator() (HttpRequest& request,
      HttpReply& reply, cxxtools::QueryParams& qparam);
    virtual unsigned endTag (HttpRequest& request,
      HttpReply& reply, cxxtools::QueryParams& qparam);
    virtual void drop() = 0;

    virtual std::string getAttribute(const std::string& name,
      const std::string& def = std::string()) const;

    /// explicitly call operator() - sometimes more readable
    unsigned call(HttpRequest& request, HttpReply& reply, cxxtools::QueryParams& qparam)
      { return operator() (request, reply, qparam); }
    /// call component without parameters
    unsigned call(HttpRequest& request, HttpReply& reply);
    /// call component dummy request
    unsigned call(HttpReply& reply, cxxtools::QueryParams& qparam);
    /// call component without parameters and dummy request
    unsigned call(HttpReply& reply);

    /// return output as a string rather than outputting to stream
    std::string scall(HttpRequest& request, cxxtools::QueryParams& qparam);
    /// return output as a string rather than outputting to stream without query-parameters
    std::string scall(HttpRequest& request);
    /// return output as a string rather than outputting to stream with empty request
    std::string scall(cxxtools::QueryParams& qparam);
    /// return output as a string rather than outputting to stream with empty request and without query-parameters
    std::string scall();
};

}

#endif // TNT_COMPONENT_H
