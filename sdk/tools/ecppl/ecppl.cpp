/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "ecpplang.h"

#include <iostream>
#include <fstream>
#include <exception>
#include <cxxtools/arg.h>

#include "config.h"

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);
  try
  {
    cxxtools::Arg<bool> lang(argc, argv, 'l');
    cxxtools::Arg<bool> nolang(argc, argv, 'n');
    cxxtools::Arg<const char*> ofile(argc, argv, 'o');

    typedef std::list<std::string> includes_type;
    includes_type includes;

    while (true)
    {
      cxxtools::Arg<const char*> include(argc, argv, 'I', 0);
      if (!include.isSet())
        break;
      includes.push_back(include.getValue());
    }

    if (argc != 2)
    {
      std::cerr
        << PACKAGE_STRING "\n\n"
           "usage: " << argv[0] << " [options] ecpp-source\n\n"
           "  -o filename       outputfile\n"
           "  -n                extract nolang\n"
           "  -l                extract lang (default)\n"
           "  -I dir            include-directory\n"
        << std::endl;
      return 1;
    }


    std::ifstream in(argv[1]);

    Ecpplang generator;

    tnt::ecpp::Parser parser(generator, argv[1]);

    for (includes_type::const_iterator it = includes.begin();
         it != includes.end(); ++it)
      parser.addInclude(*it);

    generator.setLang(lang || !nolang);
    generator.setNoLang(nolang);

    parser.parse(in);

    if (ofile.isSet())
    {
      std::ofstream out(ofile);
      generator.print(out);
    }
    else
      generator.print(std::cout);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
