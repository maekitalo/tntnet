/*
 * Copyright (C) 2003 Tommi Maekitalo
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


#include "tnt/ecppc/variable.h"
#include <sstream>
#include <cctype>

namespace tnt
{
  namespace ecppc
  {
    Variable::Variable(const std::string& arg, const std::string& value)
    {
      // 'name' might be prefixed by a type
      // the variablename is the last word in 'name'
      // type is the rest before the variablename
      // examples:
      //   " int var "
      //   " var"
      //   " ns :: someclass  param"
      std::string::size_type e = arg.size();
      while (e > 0 && std::isspace(arg.at(e - 1)))
        --e;

      // e points past the last character of our arg

      if (e > 1 && arg.at(e - 2) == '[' && arg.at(e-1) == ']')
      {
        _isVector = true;
        e -= 2;
        while (e > 0 && std::isspace(arg.at(e - 1)))
          --e;
      }
      else
        _isVector = false;

      std::string::size_type b = e;
      while (b > 0 && !std::isspace(arg.at(b - 1)))
        --b;
      // b points to the first character of our arg

      std::string::size_type t = b;
      while (t > 0 && std::isspace(arg.at(t - 1)))
        --t;
      // t points past the last character of the type

      _name = std::string(arg.begin() + b, arg.begin() + e);
      _type = std::string(arg.begin(), arg.begin() + t);
      _value = value;

    }

    void Variable::getParamCode(std::ostream& o, const std::string& qparam) const
    {
      std::string ltype = _type;
      if (ltype.empty())
        ltype = "std::string";

      if (_isVector)
      {
        o << "std::vector< " << ltype << " > " << _name << " = " << qparam << ".getvector< " << ltype << " >(\"" << _name << "\");\n";
      }
      else if (_value.empty())
      {
        o << ltype << ' ' << _name << " = " << qparam << ".get< " << ltype << " >(\"" << _name << "\");\n";
      }
      else
      {
        o << ltype << ' ' << _name << " = " << qparam << ".get< " << ltype << " >(\"" << _name << "\", (" << _value << "));\n";
      }
    }

    void Variable::getConfigInit(std::ostream& o) const
      { o << "  config.config.getMember(\"" << _name << "\", _component_::" << _name << ");\n"; }

    void Variable::getConfigDecl(std::ostream& o) const
    {
      std::string t = _type.empty() ? "std::string" : _type;

      o << t << " _component_::" << _name;
      if (!_value.empty())
        o << " = " << _value;
      o << ";\n";
    }

    void Variable::getConfigHDecl(std::ostream& o) const
    {
      std::string t = _type.empty() ? "std::string" : _type;

      o << "    static " << t << " " << _name << ";\n";
    }
  }
}
