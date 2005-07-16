/* variable.cpp
   Copyright (C) 2003 Tommi Maekitalo

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

#include "tnt/ecppc/variable.h"
#include <sstream>

namespace tnt
{
  namespace ecppc
  {
    Variable::Variable(const std::string& arg, const std::string& value_)
    {
      // 'name' might be prefixed by a type
      // the variablename is the last word in 'name'
      // type is the rest before the variablename
      // examples:
      //   " int var "
      //   " var"
      //   " ns :: someclass  param"
      std::ostringstream a;

      std::string::size_type e = arg.size();
      while (e > 0 && std::isspace(arg.at(e - 1)))
        --e;

      // e points past the last character of our arg

      if (e > 1 && arg.at(e - 2) == '[' && arg.at(e-1) == ']')
      {
        isvector = true;
        e -= 2;
        while (e > 0 && std::isspace(arg.at(e - 1)))
          --e;
      }
      else
        isvector = false;

      std::string::size_type b = e;
      while (b > 0 && !std::isspace(arg.at(b - 1)))
        --b;
      // b points to the first character of our arg

      std::string::size_type t = b;
      while (t > 0 && std::isspace(arg.at(t - 1)))
        --t;
      // t points past the last character of the type

      name = std::string(arg.begin() + b, arg.begin() + e);
      type = std::string(arg.begin(), arg.begin() + t);
      value = value_;

    }

    void Variable::getParamCodeVector(std::ostream& o) const
    {
      if (type.empty())
      {
        o << "typedef std::vector<std::string> " << name << "_type;\n"
          << name << "_type " << name << "(qparam.begin(\"" << name
          << "\"), qparam.end());\n";
      }
      else
      {
        o << "typedef std::vector<" << type << "> " << name << "_type;\n"
          << name << "_type " << name << ";\n"
          << "std::transform(qparam.begin(\"" << name
          << "\"), qparam.end(), std::back_inserter(" << name
          << "), tnt::stringTo<" << type << ">);\n";
      }
    }

    void Variable::getParamCode(std::ostream& o) const
    {
      if (isvector)
        getParamCodeVector(o);
      else if (!type.empty())
      {
        // we have a type

        // print out type and name
        o << type << ' ' << name << " = ";

        if (value.empty())
        {
          // no default-value

          o << "tnt::stringTo<" << type << ">( qparam.param(\""
            << name << "\") );\n";
        }
        else
        {
          // with default-value
          o << "qparam.has(\"" << name << "\") ? tnt::stringToWithDefault<"
            << type << ">(qparam.param(\"" << name
            << "\"), " << value << ") : " << value << ";\n";
        }
      }
      else
      {
        // type defaults to std::string
        o << "std::string " << name 
          << " = qparam.param(\"" << name << '"';
        if (!value.empty())
          o << ", " << value;

        o << ");\n";
      }
    }

    void Variable::getConfigInit(std::ostream& o, const std::string& classname) const
    {
      if (!type.empty())
      {
        // we have a type

        o << "    if (cl.getConfig().hasValue(\"" << name << "\"))\n"
          << "      " << classname << "::" << name << " = tnt::stringTo<" << type
          << ">( cl.getConfig().getValue(\"" << name << "\") );\n";
      }
      else
      {
        // type defaults to std::string
        if (value.empty())
          o << "    " << classname << "::" << name 
            << " = cl.getConfig().getValue(\"" << name << "\");\n";
        else
          o << "    if (cl.getConfig().hasValue(\"" << name << "\"))\n"
            << "      " << classname << "::" << name << " = cl.getConfig().getValue(\"" << name << "\");\n";
      }
    }

    void Variable::getConfigDecl(std::ostream& o, const std::string& classname) const
    {
      std::string t = type.empty() ? "std::string" : type;

      o << t << ' ' << classname << "::" << name;
      if (!value.empty())
        o << " = " << value;
      o << ";\n";
    }

    void Variable::getConfigHDecl(std::ostream& o) const
    {
      std::string t = type.empty() ? "std::string" : type;

      o << "    static " << t << " " << name << ";\n";
    }
  }
}
