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
    void scopevar::get(std::ostream& out) const
    {
      std::string getterMethod =
          scope_container == ecpp::application_container ? "getApplicationScope"
        : scope_container == ecpp::session_container     ? "getSessionScope"
        :                                                  "getRequestScope";
      std::string key = scope == ecpp::global_scope ? ('"' + var + '"')
                      : scope == ecpp::page_scope   ? ("getPageScopePrefix(getCompident()) + \":" + var + '"')
                      : ("getComponentScopePrefix(getCompident()) + \":" + var + '"');

      out << "  // <%scope> " << type << ' ' << var;
      if (!init.empty())
        out << '(' << init << ')';
      out << "\n"
             "  typedef " << type << ' ' << var << "_type;\n"
             "  tnt::objectptr " << var << "_pointer = request." << getterMethod << "().get("
                << key << ");\n"
             "  if (" << var << "_pointer == 0)\n"
             "    " << var << "_pointer = request." << getterMethod << "().putNew("
                    << key << ", new tnt::objectTemplate<" << var << "_type>(" << init << "));\n"
             "  " << type << "& " << var << " = dynamic_cast<tnt::objectTemplate<"
               << var << "_type>&>(*" << var << "_pointer.getPtr()).getData();\n"
             "  // </%scope>\n";
    }

  }
}
