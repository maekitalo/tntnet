/* dependencygenerator.h
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

#ifndef TNT_ECPP_DEPENDENCYGENERATOR_H
#define TNT_ECPP_DEPENDENCYGENERATOR_H

#include <tnt/ecpp/parsehandler.h>
#include <list>

namespace tnt
{
  namespace ecppc
  {

    ////////////////////////////////////////////////////////////////////////
    // dependencygenerator
    //
    class dependencygenerator : public tnt::ecpp::parseHandler
    {
        std::string classname;
        std::string inputfile;
        typedef std::list<std::string> dependencies_type;
        dependencies_type dependencies;

      public:
        dependencygenerator(const std::string& classname_, const std::string& inputfile_)
          : classname(classname_),
            inputfile(inputfile_)
        { }

        virtual void onInclude(const std::string& file);

        void getDependencies(std::ostream& out, bool useHeader) const;
    };

  }
}

#endif // TNT_ECPP_DEPENDENCYGENERATOR_H
