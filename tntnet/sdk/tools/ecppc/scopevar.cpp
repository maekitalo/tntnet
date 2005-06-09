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
      std::string tag =
          scope_container == ecpp::application_container ? "application"
        : scope_container == ecpp::session_container     ? "session"
        :                                                  "request";
      std::string container =
          scope_container == ecpp::application_container ? "APPLICATION"
        : scope_container == ecpp::session_container     ? "SESSION"
        :                                                  "REQUEST";
      std::string key = scope == ecpp::global_scope ? "GLOBAL"
                      : scope == ecpp::page_scope   ? "PAGE"
                      : "COMPONENT";

      out << "  // <%" << tag << "> " << type << ' ' << var;
      if (!init.empty())
        out << '(' << init << ')';
      out << "\n"
             "  TNT_" << container << '_' << key << "_VAR(" << type << ", " << var
          << ", \"" << var << "\", (" << init << "));\n";
             "  // </%scope>\n";
    }

  }
}
