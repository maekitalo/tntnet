/* tnt/query_params.h
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_QUERY_PARAMS_H
#define TNT_QUERY_PARAMS_H

#include <cxxtools/query_params.h>
#include <tnt/scope.h>

namespace tnt
{
  class QueryParams : public cxxtools::QueryParams
  {
      Scope* paramScope;

    public:
      QueryParams()
        : paramScope(0)
        { }
      QueryParams(QueryParams& parent, bool use_parent_values)
        : cxxtools::QueryParams(parent, use_parent_values),
          paramScope(0)
        { }
      QueryParams(const QueryParams& src)
        : cxxtools::QueryParams(src),
          paramScope(src.paramScope)
        { if (paramScope) paramScope->addRef(); }
      QueryParams& operator= (const QueryParams& src)
      {
        cxxtools::QueryParams::operator=(src);
        if (paramScope && paramScope != src.paramScope)
        {
          paramScope->release();
          paramScope = src.paramScope;
          paramScope->addRef();
        }
        return *this;
      }
      ~QueryParams()
      {
        if (paramScope)
          paramScope->release();
      }

      Scope& getScope()
      {
        if (!paramScope)
          paramScope = new Scope();
        return *paramScope;
      }
  };
}

#endif // TNT_QUERY_PARAMS_H
