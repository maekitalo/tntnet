/* component.cpp
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include "tnt/ecppc/component.h"

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Component
    //
    void Component::getBody(std::ostream& body) const
    {
      getScopevars(body);

      body << "  // <%args>\n";

      getArgs(body);

      body << "  // </%args>\n\n";

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

    void Component::getScopevars(std::ostream& body) const
    {
      for (scopevars_type::const_iterator it = scopevars.begin();
           it != scopevars.end(); ++it)
      {
        it->get(body);
      }
    }

    void Component::getScopevars(std::ostream& body, ecpp::scope_type scope) const
    {
      for (scopevars_type::const_iterator it = scopevars.begin();
           it != scopevars.end(); ++it)
      {
        if (it->getScope() == scope)
          it->get(body);
      }
    }
  }
}
