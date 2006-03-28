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
#include <cxxtools/log.h>

log_define("tntnet.body");

namespace tnt
{
  namespace ecppc
  {
    Bodypart::~Bodypart()
    { }

    void Bodypart::printLine(std::ostream& out) const
    {
      if (!curfile.empty())
        out << "#line " << (curline + 1) << " \"" << curfile << "\"\n";
      else
        log_warn("no filename to print in #line-directive");
    }


    void BodypartStatic::getBody(std::ostream& out) const
    {
      out << data;
    }

    unsigned BodypartCall::nextNumber = 0;

    void BodypartCall::call(std::ostream& out, const std::string& qparam) const
    {
      printLine(out);
      if (std::isalpha(comp[0]))
        callByIdent(out, qparam);
      else
        callByExpr(out, qparam);
    }

    void BodypartCall::callLocal(std::ostream& out, const std::string& qparam) const
    {
      if (haveEndCall)
        throw std::runtime_error("end-tag not allowed in call \"" + comp + '"');

      out << "  main()." << comp << "(request, reply, " << qparam;
      if (!cppargs.empty())
        out << ", " << cppargs;
      out << ");\n";
    }

    void BodypartCall::callByIdent(std::ostream& out, const std::string& qparam) const
    {
      tnt::Subcompident id(comp);

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
                        "tnt::Compident(std::string(), \"" + id.compname + "\")",
                        qparam);
          }
        }
        else
        {
          // case: comp.subcomp
          callByIdent(out,
                      "tnt::Subcompident(std::string(), \""
                                      + id.compname + "\", \""
                                      + id.subname + "\")",
                      qparam);
        }
      }
      else if (id.subname.empty())
      {
        // case: comp@lib
        callByIdent(out,
                    "tnt::Compident(\"" + id.libname + "\", "
                            "\"" + id.compname + "\")",
                    qparam);
      }
      else
      {
        // case: comp.subcomp@lib
          callByIdent(out,
                      "tnt::Subcompident(\"" + id.libname + "\", "
                                   "\"" + id.compname + "\", "
                                   "\"" + id.subname + "\")",
                      qparam);
      }
    }

    void BodypartCall::callByIdent(std::ostream& out, const std::string& ident, const std::string& qparam) const
    {
      if (!cppargs.empty())
        throw std::runtime_error("cppargs not allowed in call \"" + comp + '"');

      if (haveEndCall)
      {
        out << "  tnt::ComponentGuard _tnt_comp" << getNumber() << "(createComp(" << ident << "));\n"
               "  if (_tnt_comp" << getNumber() << "->call(request, reply, " << qparam << ") != DECLINED)\n"
               "  {\n";
      }
      else
        out << "  callComp(" << ident << ", request, reply, " << qparam << ");\n";
    }

    void BodypartCall::callByExpr(std::ostream& out, const std::string& qparam) const
    {
      if (!cppargs.empty())
        throw std::runtime_error("cppargs not allowed in call \"" + comp + '"');
      else if (haveEndCall)
        throw std::runtime_error("end-tag not allowed in call \"" + comp + '"');

      out << "  callComp(" << comp << ", request, reply, " << qparam << ");\n";
    }

    void BodypartCall::getBody(std::ostream& out) const
    {
      out << "  // <& " << comp << " ...\n";

      std::ostringstream o;
      o << "_tnt_cq" << getNumber();
      std::string queryParamName = o.str();

      if (args.empty())
      {
        if (pass_cgi.empty())
        {
          printLine(out);
          out << "  cxxtools::QueryParams " << queryParamName << "(qparam, false);\n";
          call(out, queryParamName);
        }
        else
        {
          call(out, pass_cgi);
        }
      }
      else
      {
        printLine(out);
        if (pass_cgi.empty())
          out << "  cxxtools::QueryParams " << queryParamName << "(qparam, false);\n";
        else
          out << "  cxxtools::QueryParams " << queryParamName << "(" << pass_cgi << ", true);\n";

        for (comp_args_type::const_iterator i = args.begin();
             i != args.end(); ++i)
        {
          printLine(out);
          out << "  " << queryParamName << ".add( \"" << i->first
              << "\", tnt::toString("
              << i->second
              << ", reply.out().getloc() ) );\n";
        }

        call(out, queryParamName);
      }

      out << "  // &>\n";
    }

    void BodypartEndCall::getBody(std::ostream& out) const
    {
      out << "  // </& " << bpc.getName() << " ...\n"
             "  }\n"
             "  _tnt_comp" << bpc.getNumber()
          << "->endTag(request, reply, _tnt_cq" << bpc.getNumber()
          << ");\n"
             "  // >\n";
    }

    void Body::addCall(unsigned line, const std::string& file,
                       const std::string& comp,
                       const comp_args_type& args,
                       const std::string& pass_cgi,
                       const std::string& cppargs)
    {
      BodypartCall* bpc = new BodypartCall(line, file,
          comp, args, pass_cgi, cppargs, subcomps);
      data.push_back(body_part_pointer(bpc));
      callStack.push(bpc);
    }

    void Body::addEndCall(unsigned line, const std::string& file,
        const std::string& comp)
    {
      while (!callStack.empty() && callStack.top()->getName() != comp)
        callStack.pop();
      if (callStack.empty())
        throw std::runtime_error("start tag for \"" + comp + "\" not found");

      BodypartEndCall* bpec = new BodypartEndCall(line, file, *callStack.top());
      callStack.pop();
      data.push_back(body_part_pointer(bpec));
    }
    void Body::getBody(std::ostream& out) const
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
