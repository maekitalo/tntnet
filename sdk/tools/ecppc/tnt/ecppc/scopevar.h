/*
 * Copyright (C) 2005 Tommi Maekitalo
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
        scope_container_type _scopeContainer;
        scope_type _scope;
        std::string _type;
        std::string _var;
        std::string _init;
        unsigned _myline;
        std::string _myfile;

      public:
        Scopevar() { }
        Scopevar(scope_container_type scopeContainer, scope_type scope,
                 const std::string& type, const std::string& var,
                 const std::string& init, unsigned myline, const std::string& myfile)
          : _scopeContainer(scopeContainer),
            _scope(scope),
            _type(type),
            _var(var),
            _init(init),
            _myline(myline),
            _myfile(myfile)
            { }

        scope_container_type getScopeContainer() const { return _scopeContainer; }
        scope_type getScope() const                    { return _scope; }

        void get(std::ostream& o, bool linenumbersEnabled) const;
    };
  }
}

#endif // TNT_ECPPC_SCOPEVAR_H

