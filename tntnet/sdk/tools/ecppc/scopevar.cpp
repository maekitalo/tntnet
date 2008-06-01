/* scopevar.cpp
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


#include "tnt/ecppc/scopevar.h"
#include <tnt/stringescaper.h>
#include <iterator>
#include <iostream>

namespace tnt
{
  namespace ecppc
  {
    void Scopevar::get(std::ostream& out, bool linenumbersEnabled) const
    {
      std::string tag =
          scope_container == ecpp::application_container ? "application"
        : scope_container == ecpp::thread_container      ? "thread"
        : scope_container == ecpp::session_container     ? "session"
        : scope_container == ecpp::request_container     ? "request"
        :                                                  "param";

      std::string macro;
      if (scope_container == ecpp::param_container)
      {
        macro = "TNT_PARAM";
      }
      else
      {
        std::string container =
            scope_container == ecpp::application_container ? "APPLICATION_"
          : scope_container == ecpp::thread_container      ? "THREAD_"
          : scope_container == ecpp::session_container     ? "SESSION_"
          : scope_container == ecpp::request_container     ? "REQUEST_"
          :                                                  "PARAM_";
        std::string key = scope == ecpp::global_scope ? "GLOBAL_"
                        : scope == ecpp::page_scope   ? "PAGE_"
                        : scope_container != ecpp::param_container ? "COMPONENT_"
                        : std::string();
        macro = "TNT_" + container + key + "VAR";
      }

      if (linenumbersEnabled)
        out << "#line " << (myline + 1) << " \"" << myfile << "\"\n";

      out << "  " << macro << '(' << type << ", " << var
          << ", \"" << var << "\", (" << init << ")); " 
             "  // <%" << tag << "> " << type << ' ' << var;

      if (!init.empty())
      {
        out << '(';
        std::transform(
          init.begin(),
          init.end(),
          std::ostream_iterator<const char*>(out),
          stringescaper(false));
        out << ')';
      }

      out << '\n';
    }

  }
}
