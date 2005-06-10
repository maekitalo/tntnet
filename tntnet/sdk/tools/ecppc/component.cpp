/* component.cpp
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

#include "tnt/ecppc/component.h"

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // component
    //
    void component::getBody(std::ostream& body) const
    {
      
      body << "  // <%args>\n";

      getArgs(body);

      body << "  // </%args>\n\n";

      getScopevars(body);

      body << "  // <%cpp>\n";

      compbody.getBody(body);

      body << "  // <%/cpp>\n"
           << "  return HTTP_OK;\n";
    }

    void component::getArgs(std::ostream& body) const
    {
      for (variables_type::const_iterator it = args.begin();
           it != args.end(); ++it)
        it->getParamCode(body);
    }

    void component::getScopevars(std::ostream& body) const
    {
      for (scopevars_type::const_iterator it = scopevars.begin();
           it != scopevars.end(); ++it)
      {
        it->get(body);
      }
    }
  }
}
