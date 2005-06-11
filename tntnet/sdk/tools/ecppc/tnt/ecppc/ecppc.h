/* tnt/ecppc/ecppc.h
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

#ifndef TNT_ECPPC_ECPPC_H
#define TNT_ECPPC_ECPPC_H

#include <cxxtools/arg.h>
#include <exception>
#include <iosfwd>
#include <tnt/ecpp/parsehandler.h>

namespace tnt
{
  namespace ecppc
  {
    class ecppc
    {
        std::string requestname;
        const char* inputfile;
        std::string ns;
        std::string ofile;
        std::string odir;
        cxxtools::arg<std::string> mimetype;
        cxxtools::arg<bool> binary;
        cxxtools::arg<bool> singleton;
        cxxtools::arg<std::string> componentclass;
        cxxtools::arg<std::string> baseclass;
        cxxtools::arg<bool> htmlcompress;
        cxxtools::arg<bool> csscompress;
        cxxtools::arg<bool> jscompress;
        cxxtools::arg<bool> compress;
        cxxtools::arg<bool> externData;
        cxxtools::arg<bool> verbose;
        cxxtools::arg<bool> debug;
        cxxtools::arg<bool> splitBar;
        cxxtools::arg<const char*> splitChars;
        cxxtools::arg<bool> generateDependencies;

        int runDepencencies();
        int runGenerator();
        void runParser(std::istream& in, tnt::ecpp::parseHandler& handler);

      public:
        ecppc(int& argc, char* argv[]);
        int run();
    };

    class usage : public std::exception
    {
        std::string msg;
      public:
        usage(const char* progname);
        ~usage() throw()  { }

        const char* what() const throw()
        { return msg.c_str(); }
    };
  }
}

#endif // TNT_ECPPC_ECPPC_H

