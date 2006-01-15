/* ecppl.cpp
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

    if (argc != 2)
    {
      std::cerr
        << PACKAGE_STRING "\n\n"
           "ecppl-language-extractor\n\n"
           "usage: " << argv[0] << " [options] ecpp-source\n\n"
           "  -o filename        outputfile\n"
           "  -n                 nolang\n"
           "  -l                 lang (default)\n"
        << std::endl;
      return -1;
    }

    std::ifstream in(argv[1]);

    Ecpplang generator;

    tnt::ecpp::Parser parser(generator);
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
