/* component.cpp
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


#include "tnt/ecppc/component.h"

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Component
    //
    void Component::getBody(std::ostream& body, bool linenumbersEnabled) const
    {
      body << "  // <%args>\n";

      getArgs(body);

      body << "  // </%args>\n\n";

      getScopevars(body, linenumbersEnabled);

      body << "  // <%cpp>\n";

      compbody.getBody(body);

      body << "  // <%/cpp>\n"
           << "  return HTTP_OK;\n";
    }

    void Component::getArgs(std::ostream& body) const
    {
      for (variables_type::const_iterator it = args.begin();
           it != args.end(); ++it)
        it->getParamCode(body);
    }

    void Component::getScopevars(std::ostream& body, bool linenumbersEnabled) const
    {
      for (scopevars_type::const_iterator it = scopevars.begin();
           it != scopevars.end(); ++it)
      {
        it->get(body, linenumbersEnabled);
      }
    }

    void Component::getScopevars(std::ostream& body, ecpp::scope_type scope, bool linenumbersEnabled) const
    {
      for (scopevars_type::const_iterator it = scopevars.begin();
           it != scopevars.end(); ++it)
      {
        if (it->getScope() == scope)
          it->get(body, linenumbersEnabled);
      }
    }
  }
}
