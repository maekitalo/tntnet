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
    const char* scopevar::getterMethod() const
    {
      switch (scope_container)
      {
        case ecpp::application_container:
          return "getApplicationScope";
        case ecpp::session_container:
          return "getSessionScope";
        case ecpp::request_container:
          return "getRequestScope";
      }
      return 0;
    }

    std::string scopevar::key() const
    {
      switch (scope)
      {
        case ecpp::global_scope:
          return '"' + var + '"';
        case ecpp::page_scope:
          return "getPageScopePrefix(getCompident()) + \":" + var + '"';
        case ecpp::component_scope:
          return "getComponentScopePrefix(getCompident()) + \":" + var + '"';
      }
      return 0;
    }

    void scopevar::get(std::ostream& out) const
    {
      out << "  // <%scope> " << type << ' ' << var;
      if (!init.empty())
        out << '(' << init << ')';
      out << "\n"
             "  typedef " << type << ' ' << var << "_type;\n"
             "  tnt::objectptr " << var << "_pointer = " << getterMethod() << "(request).get("
                << key() << ");\n"
             "  if (" << var << "_pointer == 0)\n"
             "    " << var << "_pointer = " << getterMethod() << "(request).putNew("
                    << key() << ", new tnt::objectTemplate<" << var << "_type>(" << init << "));\n"
             "  " << type << "& " << var << " = dynamic_cast<tnt::objectTemplate<"
               << var << "_type>&>(*" << var << "_pointer.getPtr()).getData();\n"
             "  // </%scope>\n";
    }

  }
}
