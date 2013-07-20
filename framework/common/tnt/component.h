/*
 * Copyright (C) 2003-2006 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */



#ifndef TNT_COMPONENT_H
#define TNT_COMPONENT_H

#include <tnt/compident.h>
#include <tnt/sessionscope.h>

namespace tnt
{

class HttpRequest;
class HttpReply;
class QueryParams;
class TntConfig;

class Component
{
  public:
    virtual ~Component() { }

    virtual void configure(const tnt::TntConfig& config);

    virtual unsigned topCall(HttpRequest& request,
      HttpReply& reply, tnt::QueryParams& qparam);
    virtual unsigned operator() (HttpRequest& request,
      HttpReply& reply, tnt::QueryParams& qparam);
    virtual unsigned endTag (HttpRequest& request,
      HttpReply& reply, tnt::QueryParams& qparam);

    virtual std::string getAttribute(const std::string& name,
      const std::string& def = std::string()) const;

    /// explicitly call operator() - sometimes more readable
    unsigned call(HttpRequest& request, HttpReply& reply, tnt::QueryParams& qparam)
      { return operator() (request, reply, qparam); }
    /// call component without parameters
    unsigned call(HttpRequest& request, HttpReply& reply);

    /// return output as a string rather than outputting to stream
    std::string scall(HttpRequest& request, tnt::QueryParams& qparam);
    /// return output as a string rather than outputting to stream without query-parameters
    std::string scall(HttpRequest& request);
};

#define TNT_VAR(scope, type, varname, key, construct)                          \
  type* varname##_pointer;                                                     \
  {                                                                            \
    const std::string varname##_scopekey = key;                                \
    tnt::Scope& _scope = scope;                                                \
    varname##_pointer = _scope.get< type >(varname##_scopekey);                \
    if ( ! varname##_pointer )                                                 \
      _scope.put< type >(varname##_scopekey,                                   \
          varname##_pointer = new type construct);                             \
  }                                                                            \
  type& varname = *varname##_pointer;

#define TNT_SESSION_COMPONENT_VAR(type, varname, construct) \
  TNT_VAR(request.getSessionScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_SESSION_PAGE_VAR(type, varname, construct) \
  TNT_VAR(request.getSessionScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_SESSION_SHARED_VAR(type, varname, construct) \
  TNT_VAR(request.getSessionScope(), type, varname, #type #varname, construct)

#define TNT_SESSION_GLOBAL_VAR(type, varname, construct) \
  TNT_VAR(request.getSessionScope(), type, varname, #type #varname, construct)

#define TNT_SECURE_SESSION_COMPONENT_VAR(type, varname, construct) \
  TNT_VAR(request.getSecureSessionScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_SECURE_SESSION_PAGE_VAR(type, varname, construct) \
  TNT_VAR(request.getSecureSessionScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_SECURE_SESSION_SHARED_VAR(type, varname, construct) \
  TNT_VAR(request.getSecureSessionScope(), type, varname, #type #varname, construct)

#define TNT_SECURE_SESSION_GLOBAL_VAR(type, varname, construct) \
  TNT_VAR(request.getSecureSessionScope(), type, varname, #type #varname, construct)

#define TNT_APPLICATION_COMPONENT_VAR(type, varname, construct) \
  TNT_VAR(request.getApplicationScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_APPLICATION_PAGE_VAR(type, varname, construct) \
  TNT_VAR(request.getApplicationScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_APPLICATION_SHARED_VAR(type, varname, construct) \
  TNT_VAR(request.getApplicationScope(), type, varname, #type #varname, construct)

#define TNT_APPLICATION_GLOBAL_VAR(type, varname, construct) \
  TNT_VAR(request.getApplicationScope(), type, varname, #type #varname, construct)

#define TNT_THREAD_COMPONENT_VAR(type, varname, construct) \
  TNT_VAR(request.getThreadScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_THREAD_PAGE_VAR(type, varname, construct) \
  TNT_VAR(request.getThreadScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_THREAD_SHARED_VAR(type, varname, construct) \
  TNT_VAR(request.getThreadScope(), type, varname, #type #varname, construct)

#define TNT_THREAD_GLOBAL_VAR(type, varname, construct) \
  TNT_VAR(request.getThreadScope(), type, varname, #type #varname, construct)

#define TNT_REQUEST_COMPONENT_VAR(type, varname, construct) \
  TNT_VAR(request.getRequestScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_REQUEST_PAGE_VAR(type, varname, construct) \
  TNT_VAR(request.getRequestScope(), type, varname, getCompident().toString() + ":" #type #varname, construct)

#define TNT_REQUEST_SHARED_VAR(type, varname, construct) \
  TNT_VAR(request.getRequestScope(), type, varname, #type #varname, construct)

#define TNT_REQUEST_GLOBAL_VAR(type, varname, construct) \
  TNT_VAR(request.getRequestScope(), type, varname, #type #varname, construct)

#define TNT_PARAM(type, varname, construct) \
  TNT_VAR(qparam.getScope(), type, varname, #varname, construct)

}

#endif // TNT_COMPONENT_H
