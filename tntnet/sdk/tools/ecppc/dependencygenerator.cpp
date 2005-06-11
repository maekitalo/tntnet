/* dependencygenerator.cpp
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

#include "tnt/ecppc/dependencygenerator.h"
#include <sstream>

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // dependencygenerator
    //
    void dependencygenerator::onInclude(const std::string& file)
    {
      dependencies.push_back(file);
    }

    std::string dependencygenerator::getDependencies() const
    {
      std::ostringstream deps;
      deps << classname << ".cpp " << classname << ".h: " << inputfile;
      for (dependencies_type::const_iterator it = dependencies.begin();
           it != dependencies.end(); ++it)
        deps << " \\\n  " << *it;
      return deps.str();
    }
  }
}
