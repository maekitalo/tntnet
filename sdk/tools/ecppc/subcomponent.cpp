/*
 * Copyright (C) 2005 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "tnt/ecppc/subcomponent.h"

namespace tnt
{
  namespace ecppc
  {
    void Subcomponent::getHeader(std::ostream& o) const
    {
      o << "    class " << getName() << "_type : public tnt::EcppSubComponent\n"
           "    {\n"
           "        _component_& mainComp;\n"
           "        _component_& main()  { return mainComp; }\n\n"
           "      public:\n"
           "        " << getName() << "_type(_component_& m, const std::string& name)\n"
           "          : EcppSubComponent(m, name),\n"
           "            mainComp(m)\n"
           "          { }\n"
           "        unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam";
      for (cppargs_type::const_iterator j = _cppargs.begin(); j != _cppargs.end(); ++j)
      {
        o << ", " << j->first;
        if (!j->second.empty())
          o << '=' << j->second;
      }
      o << ");\n"
           "    };\n\n";
    }

    void Subcomponent::getDefinition(std::ostream& code, bool linenumbersEnabled) const
    {
      code << "unsigned _component_::" << getName()
           << "_type::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam";

      for (cppargs_type::const_iterator j = _cppargs.begin(); j != _cppargs.end(); ++j)
        code << ", " << j->first;

      code << ")\n"
              "{\n"
              "  log_trace(\"" << _outerclass->getName() << "::" << getName() << " \" << qparam.getUrl());\n"
              "  tnt::DataChunks _tntChunks(rawData);\n";

      Component::getBody(code, linenumbersEnabled);
      code << "}\n\n";
    }

    void Subcomponent::getScopevars(std::ostream& o, bool linenumbersEnabled) const
    {
      Component::getScopevars(o, linenumbersEnabled);
      _outerclass->getScopevars(o, ecpp::page_scope, linenumbersEnabled);
      _outerclass->getScopevars(o, ecpp::shared_scope, linenumbersEnabled);
    }
  }
}

