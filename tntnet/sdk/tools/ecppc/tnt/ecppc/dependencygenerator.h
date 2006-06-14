/* dependencygenerator.h
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

#ifndef TNT_ECPP_DEPENDENCYGENERATOR_H
#define TNT_ECPP_DEPENDENCYGENERATOR_H

#include <tnt/ecpp/parsehandler.h>
#include <list>

namespace tnt
{
  namespace ecppc
  {

    ////////////////////////////////////////////////////////////////////////
    // Dependencygenerator
    //
    class Dependencygenerator : public tnt::ecpp::ParseHandler
    {
        std::string classname;
        std::string inputfile;
        typedef std::list<std::string> dependencies_type;
        dependencies_type dependencies;

      public:
        Dependencygenerator(const std::string& classname_, const std::string& inputfile_)
          : classname(classname_),
            inputfile(inputfile_)
        { }

        virtual void onInclude(const std::string& file);

        void getDependencies(std::ostream& out, bool useHeader) const;
    };

  }
}

#endif // TNT_ECPP_DEPENDENCYGENERATOR_H
