/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
      QueryParams(const QueryParams& src)
        : cxxtools::QueryParams(src),
          paramScope(src.paramScope)
        { if (paramScope) paramScope->addRef(); }
      QueryParams& operator= (const QueryParams& src)
      {
        cxxtools::QueryParams::operator=(src);
        if (paramScope && paramScope != src.paramScope)
        {
          if (paramScope->release() == 0)
            delete paramScope;
          paramScope = src.paramScope;
          paramScope->addRef();
        }
        return *this;
      }
      ~QueryParams()
      {
        if (paramScope && paramScope->release() == 0)
          delete paramScope;
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
