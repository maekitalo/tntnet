/* ./ecppl.cpp
   Copyright (C) 2003 Tommi MÃ¤kitalo

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

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);
  try
  {
    arg<bool> debug(argc, argv, 'd');
    arg<bool> lang(argc, argv, 'l');
    arg<bool> nolang(argc, argv, 'n');
    arg<const char*> ofile(argc, argv, 'o');
    arg<const char*> splitChars(argc, argv, "--split-chars");

    if (argc != 2)
    {
      std::cerr
        << "Aufruf: " << argv[0] << " {Optionen} Quelldatei\n\n"
           "Generiert aus ecpp-Komponenten eine Tabulator-separierte Textdatei mit\n"
           "Texten, die zu übersetzen sind.\n\n"
           "  -o Dateiname   Ausgabedatei\n"
           "  -n             nolang\n"
           "  -l             lang (default)\n"
           "  --split-chars zz setze alternative Teilungszeichen\n"
        << std::endl;
      return -1;
    }

    std::ifstream in(argv[1]);

    ecpplang generator;
    generator.setDebug(debug);
    generator.setSplitBar();
    if (splitChars.isSet())
    {
      if (splitChars.getValue()[0] == '\0'
       || splitChars.getValue()[1] == '\0'
       || splitChars.getValue()[2] != '\0')
        throw std::runtime_error("--split-chars needs exactly 2 characters");

      generator.setSplitChars(splitChars.getValue()[0],
                              splitChars.getValue()[1]);
    }
    generator.setLang(lang || !nolang);
    generator.setNoLang(nolang);
    generator.parse(in);

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
