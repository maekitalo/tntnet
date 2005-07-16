/* tnt/ecppc/scopevar.h
   Copyright (C) 2005 Tommi Maekitalo

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

        void get(std::ostream& o) const;
    };
  }
}

#endif // TNT_ECPPC_SCOPEVAR_H

