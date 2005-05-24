/* scopevar.cpp
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

#include "tnt/ecppc/scopevar.h"
#include <iostream>

namespace tnt
{
  namespace ecppc
  {
    scopevar::scopevar(const std::string& scope_, const std::string& type_,
      const std::string& var_, const std::string& init_)
      : scope(scope_),
        Scope(scope_),
        type(type_),
        var(var_),
        init(init_)
    {
      scope[0] = std::tolower(scope[0]);
      Scope[0] = std::toupper(Scope[0]);
    }

    void scopevar::get(std::ostream& out) const
    {
      out << "  // <%" << scope << "Scope> " << type << ' ' << var;
      if (!init.empty())
        out << '(' << init << ')';
      out << "\n"
             "  tnt::objectptr " << var << "_pointer = get" << Scope << "Scope(request).get(\"" << var << "\");\n"
             "  if (" << var << "_pointer == 0)\n"
             "    " << var << "_pointer = get" << Scope << "Scope(request).putNew(\"" << var << "\", new tnt::objectTemplate<" << type << " >(" << init << "));\n"
             "  " << type << "& " << var << " = dynamic_cast<tnt::objectTemplate<" << type << " >&>(*" << var << "_pointer.getPtr()).getData();\n"
             "  // </%" << scope << "Scope>\n";
    }

  }
}
