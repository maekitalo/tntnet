/* tnt/ecppc/variable.h
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

#ifndef TNT_ECPPC_VARIABLE_H
#define TNT_ECPPC_VARIABLE_H

#include <string>

namespace tnt
{
  namespace ecppc
  {
    class variable
    {
        std::string name;
        std::string type;
        std::string value;
        bool isvector;

        void getParamCodeVector(std::ostream& o) const;

      public:
        variable()  { }
        variable(const std::string& arg, const std::string& value_);

        void getParamCode(std::ostream& o) const;
        void getConfigInit(std::ostream& o, const std::string& classname) const;
        void getConfigDecl(std::ostream& o, const std::string& classname) const;
        void getConfigHDecl(std::ostream& o) const;
    };
  }
}

#endif // TNT_ECPPC_VARIABLE_H

