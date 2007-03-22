/* tnt/ecppc/variable.h
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


#ifndef TNT_ECPPC_VARIABLE_H
#define TNT_ECPPC_VARIABLE_H

#include <string>

namespace tnt
{
  namespace ecppc
  {
    class Variable
    {
        std::string name;
        std::string type;
        std::string value;
        bool isvector;

        void getParamCodeVector(std::ostream& o) const;

      public:
        Variable()  { }
        Variable(const std::string& arg, const std::string& value_);

        void getParamCode(std::ostream& o) const;
        void getConfigInit(std::ostream& o, const std::string& classname) const;
        void getConfigDecl(std::ostream& o, const std::string& classname) const;
        void getConfigHDecl(std::ostream& o) const;
    };
  }
}

#endif // TNT_ECPPC_VARIABLE_H

