/* tnt/ecppc/scopevar.h
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

