/* body.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

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

#include "tnt/ecppc/body.h"
#include <tnt/ecpp.h>
#include <sstream>

namespace tnt
{
  namespace ecppc
  {
    void bodypart_static::getBody(std::ostream& out) const
    {
      out << data;
    }

    void bodypart_call::call(std::ostream& out, const std::string& qparam) const
    {
      if (std::isalpha(comp[0]))
        callByIdent(out, qparam);
      else
        callByExpr(out, qparam);
    }

    void bodypart_call::callLocal(std::ostream& out, const std::string& qparam) const
    {
      out << "    main()." << comp << "(request, reply, " << qparam;
      if (!cppargs.empty())
        out << ", " << cppargs;
      out << ");\n";
    }

    void bodypart_call::callByIdent(std::ostream& out, const std::string& qparam) const
    {
      tnt::subcompident id(comp);

      if (id.libname.empty())
      {
        if (id.subname.empty())
        {
          // case: comp or subcomp
          if (subcomps.find(id.compname) != subcomps.end())
          {
            // case: subcomp
            callLocal(out, qparam);
          }
          else
          {
            // case: comp
            callByIdent(out,
                        "compident(std::string(), \"" + id.compname + "\")",
                        qparam);
          }
        }
        else
        {
          // case: comp.subcomp
          callByIdent(out,
                      "subcompident(std::string(), \""
                                      + id.compname + "\", \""
                                      + id.subname + "\")",
                      qparam);
        }
      }
      else if (id.subname.empty())
      {
        // case: comp@lib
        callByIdent(out,
                    "compident(\"" + id.libname + "\", "
                            "\"" + id.compname + "\")",
                    qparam);
      }
      else
      {
        // case: comp.subcomp@lib
          callByIdent(out,
                      "subcompident(\"" + id.libname + "\", "
                                   "\"" + id.compname + "\", "
                                   "\"" + id.subname + "\")",
                      qparam);
      }
    }

    void bodypart_call::callByIdent(std::ostream& out, const std::string& ident,
                                    const std::string& qparam) const
    {
      if (!cppargs.empty())
        throw std::runtime_error("cppargs not allowed in call \"" + comp + '"');

      out << "    callComp(" << ident << ", request, reply, " << qparam << ");\n";
    }

    void bodypart_call::callByExpr(std::ostream& out, const std::string& qparam) const
    {
      if (!cppargs.empty())
        throw std::runtime_error("cppargs not allowed in call \"" + comp + '"');

      out << "    callComp(" << comp << ", request, reply, " << qparam << ");\n";
    }

    void bodypart_call::getBody(std::ostream& out) const
    {
      out << "  // <& " << comp << " ...\n";

      if (args.empty())
      {
        if (pass_cgi.empty())
        {
          out << "  {\n"
              << "    cxxtools::query_params cq(qparam, false);\n";
          call(out, "cq");
          out << "  }\n";
        }
        else
        {
          call(out, pass_cgi);
        }
      }
      else
      {
        out << "  {\n";
        if (pass_cgi.empty())
          out << "    cxxtools::query_params cq(qparam, false);\n";
        else
          out << "    cxxtools::query_params cq(" << pass_cgi << ");\n";

        for (comp_args_type::const_iterator i = args.begin();
             i != args.end(); ++i)
        {
          out << "    cq.add( \"" << i->first << "\", tnt::to_string";
          if (i->second[0] == '(' && i->second[i->second.size() - 1] == ')')
            out << i->second;
          else
            out << '(' << i->second << ')';
          out << " );\n";
        }

        call(out, "cq");
        out << "  }\n";
      }

      out << "  // &>\n";
    }

    void body::getBody(std::ostream& out) const
    {
      for (body_type::const_iterator it = data.begin();
           it != data.end(); ++it)
      {
        body_part_pointer p = *it;
        p->getBody(out);
      }
    }
  }
}
