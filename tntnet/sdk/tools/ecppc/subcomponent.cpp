/* subcomponent.cpp
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


#include "tnt/ecppc/subcomponent.h"

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Subcomponent
    //
    void Subcomponent::getHeader(std::ostream& o) const
    {
      o << "    class " << getName() << "_type : public tnt::EcppSubComponent\n"
           "    {\n"
           "        " << outerclass->getName() << "& mainComp;\n"
           "        " << outerclass->getName() << "& main()  { return mainComp; }\n\n"
           "      public:\n"
           "        " << getName() << "_type(" << outerclass->getName() << "& m, const std::string& name)\n"
           "          : EcppSubComponent(m, name),\n"
           "            mainComp(m)\n"
           "          { }\n"
           "        unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, cxxtools::QueryParams& qparam";
      for (cppargs_type::const_iterator j = cppargs.begin();
           j != cppargs.end(); ++j)
      {
        o << ", " << j->first;
        if (!j->second.empty())
          o << '=' << j->second;
      }
      o << ");\n"
           "    };\n\n";
    }

    void Subcomponent::getDefinition(std::ostream& code, bool debug, bool externData) const
    {
      code << "unsigned " << outerclass->getName() << "::" << getName()
           << "_type::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, cxxtools::QueryParams& qparam";
      for (cppargs_type::const_iterator j = cppargs.begin();
           j != cppargs.end(); ++j)
        code << ", " << j->first;
      code << ")\n"
              "{\n";

      if (debug)
        code << "  log_trace(\"" << outerclass->getName() << "::" << getName() << " \" << qparam.getUrl());\n";

      if (externData)
        code << "  tnt::DataChunks data(getData(request, qparam, rawData));\n";
      else
        code << "  tnt::DataChunks data(rawData);\n";

      Component::getBody(code);
      code << "}\n\n";
    }

    void Subcomponent::getScopevars(std::ostream& o) const
    {
      Component::getScopevars(o);
      outerclass->getScopevars(o, ecpp::page_scope);
      outerclass->getScopevars(o, ecpp::global_scope);
    }
  }
}
