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
    class Subcomponent : public Component
    {
        typedef ecpp::Parser::cppargs_type cppargs_type;
        cppargs_type cppargs;
        const Component* outerclass;

      public:
        Subcomponent()
          : Component(std::string()),
            outerclass(0)
          { }

        Subcomponent(const std::string& classname_,
            const Component& outerclass_, const cppargs_type& cppargs_)
          : Component(outerclass_, classname_),
            cppargs(cppargs_),
            outerclass(&outerclass_)
          { }

        void getHeader(std::ostream& o) const;
        void getDefinition(std::ostream& o, bool debug, bool externData) const;
    };
  }
}

#endif // TNT_ECPPC_COMPONENT_H

