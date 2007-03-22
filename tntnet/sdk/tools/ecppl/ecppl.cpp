/* ecppl.cpp
 * Copyright (C) 2003-2005 Tommi Maekitalo
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
