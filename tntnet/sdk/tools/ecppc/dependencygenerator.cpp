/* dependencygenerator.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include "tnt/ecppc/dependencygenerator.h"
#include <sstream>

namespace tnt
{
  namespace ecppc
  {
    ////////////////////////////////////////////////////////////////////////
    // Dependencygenerator
    //
    void Dependencygenerator::onInclude(const std::string& file)
    {
      dependencies.push_back(file);
    }

    void Dependencygenerator::getDependencies(std::ostream& out, bool useHeader) const
    {
      out << classname << ".cpp";
      if (useHeader)
        out << ' ' << classname << ".h";
      out << ": " << inputfile;
      for (dependencies_type::const_iterator it = dependencies.begin();
           it != dependencies.end(); ++it)
        out << " \\\n  " << *it;
      out << '\n';
    }
  }
}
