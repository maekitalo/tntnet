/* tnt/ecppc/subcomponent.h
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

#ifndef TNT_ECPPC_SUBCOMPONENT_H
#define TNT_ECPPC_SUBCOMPONENT_H

#include <tnt/ecppc/component.h>

namespace tnt
{
  namespace ecppc
  {
    class subcomponent : public component
    {
        typedef ecpp::parser::cppargs_type cppargs_type;
        cppargs_type cppargs;
        const component* outerclass;

      public:
        subcomponent()
          : component(std::string()),
            outerclass(0)
          { }

        subcomponent(const std::string& classname_,
            const component& outerclass_, const cppargs_type& cppargs_)
          : component(outerclass_, classname_),
            outerclass(&outerclass_),
            cppargs(cppargs_)
          { }

        void getHeader(std::ostream& o) const;
        void getDefinition(std::ostream& o, bool debug, bool externData) const;
    };
  }
}

#endif // TNT_ECPPC_COMPONENT_H

