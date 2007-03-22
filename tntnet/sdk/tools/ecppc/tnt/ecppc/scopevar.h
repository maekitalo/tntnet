/* tnt/ecppc/scopevar.h
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef TNT_ECPPC_SCOPEVAR_H
#define TNT_ECPPC_SCOPEVAR_H

#include <string>
#include <iosfwd>
#include <tnt/ecpp/scopetypes.h>

namespace tnt
{
  namespace ecppc
  {
    using ecpp::scope_container_type;
    using ecpp::scope_type;

    class Scopevar
    {
        scope_container_type scope_container;
        scope_type scope;
        std::string type;
        std::string var;
        std::string init;

      public:
        Scopevar()  { }
        Scopevar(scope_container_type scope_container_, scope_type scope_,
                 const std::string& type_,
                 const std::string& var_, const std::string& init_)
          : scope_container(scope_container_),
            scope(scope_),
            type(type_),
            var(var_),
            init(init_)
            { }

        scope_container_type getScopeContainer() const
          { return scope_container; }
        scope_type getScope() const
          { return scope; }

        void get(std::ostream& o) const;
    };
  }
}

#endif // TNT_ECPPC_SCOPEVAR_H

