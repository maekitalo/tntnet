/*
 * Copyright (C) 2005 Tommi Maekitalo
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
      private:
        typedef std::map<std::string, std::string> inputfiles_type;
        typedef std::list<std::string> includes_type;

        std::string _componentname;
        std::string _extname;
        const char* _inputfile;

        inputfiles_type _inputFiles;

        std::string _ofile;
        std::string _odir;
        std::string _mimetype;
        std::string _mimedb;
        std::string _logCategory;
        bool _binary;
        bool _multibinary;
        bool _keepPath;
        bool _compress;
        bool _verbose;
        bool _generateDependencies;
        bool _generateCMakeDependencies;
        bool _disableLinenumbers;

        includes_type _includes;

        int runDependencies(bool cmake);
        int runGenerator();
        bool runParser(std::istream& in, tnt::ecpp::ParseHandler& handler, bool continueOnError);

      public:
        Ecppc(int& argc, char* argv[]);
        int run();
    };
  }
}

#endif // TNT_ECPPC_ECPPC_H
