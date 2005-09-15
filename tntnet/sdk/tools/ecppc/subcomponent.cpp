/* subcomponent.cpp
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
        code << "  const Component* dataComponent = main().getDataComponent(request);\n"
                "  ::use(dataComponent);\n\n";

      Component::getBody(code);
      code << "}\n\n";
    }
  }
}
