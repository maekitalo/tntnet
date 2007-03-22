/* closecomponent.cpp
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


#include "tnt/ecppc/closecomponent.h"

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Closecomponent
    //
    void Closecomponent::getDefinition(std::ostream& code, bool externData) const
    {
      code << "unsigned " << getName() << "::endTag (tnt::HttpRequest& request, tnt::HttpReply& reply,\n"
           "  cxxtools::QueryParams& qparam)\n"
           "{\n";

      if (externData)
        code << "  tnt::DataChunks data(getData(request, rawData));\n";
      else
        code << "  tnt::DataChunks data(rawData);\n";

      Component::getBody(code);
      code << "}\n\n";
    }
  }
}
