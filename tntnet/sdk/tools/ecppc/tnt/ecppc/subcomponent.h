/* tnt/ecppc/subcomponent.h
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
        void getDefinition(std::ostream& o, bool debug, bool externData, bool linenumbersEnabled) const;
        virtual void getScopevars(std::ostream& o, bool linenumbersEnabled) const;
    };
  }
}

#endif // TNT_ECPPC_SUBCOMPONENT_H

