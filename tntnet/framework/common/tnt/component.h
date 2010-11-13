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
#include <time.h>

namespace tnt
{

class HttpRequest;
class HttpReply;
class QueryParams;

class Component
{
  public:
    virtual ~Component() { }

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

}

#endif // TNT_COMPONENT_H
