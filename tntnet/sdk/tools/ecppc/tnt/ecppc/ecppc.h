/* tnt/ecppc/ecppc.h
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


#ifndef TNT_ECPPC_ECPPC_H
#define TNT_ECPPC_ECPPC_H

#include <cxxtools/arg.h>
#include <exception>
#include <iosfwd>
#include <tnt/ecpp/parsehandler.h>
#include <set>
#include <list>

namespace tnt
{
  namespace ecppc
  {
    class Ecppc
    {
        std::string requestname;
        std::string extname;
        const char* inputfile;

        typedef std::set<std::string> inputfiles_type;
        inputfiles_type inputfiles;

        std::string ofile;
        std::string odir;
        cxxtools::Arg<std::string> mimetype;
        cxxtools::Arg<std::string> mimedb;
        cxxtools::Arg<bool> binary;
        cxxtools::Arg<bool> multibinary;
        cxxtools::Arg<bool> singleton;
        cxxtools::Arg<std::string> componentclass;
        cxxtools::Arg<std::string> baseclass;
        cxxtools::Arg<bool> compress;
        cxxtools::Arg<bool> verbose;
        cxxtools::Arg<bool> debug;
        cxxtools::Arg<bool> trace;
        cxxtools::Arg<bool> generateDependencies;
        cxxtools::Arg<bool> generateHeader;

        typedef std::list<std::string> includes_type;
        includes_type includes;

        int runDependencies();
        int runGenerator();
        void runParser(std::istream& in, tnt::ecpp::ParseHandler& handler);

      public:
        Ecppc(int& argc, char* argv[]);
        int run();
    };

    class Usage : public std::exception
    {
        std::string msg;
      public:
        Usage(const char* progname);
        ~Usage() throw()  { }

        const char* what() const throw()
        { return msg.c_str(); }
    };
  }
}

#endif // TNT_ECPPC_ECPPC_H

