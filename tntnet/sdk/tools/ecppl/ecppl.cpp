////////////////////////////////////////////////////////////////////////
// main - ecpplang
//

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
    arg<bool> textformat(argc, argv, 't');

    if (argc != 2)
    {
      std::cerr
        << "Aufruf: " << argv[0] << " {Optionen} Quelldatei\n\n"
           "Generiert aus ecpp-Komponenten eine Tabulator-separierte Textdatei mit\n"
           "Texten, die zu übersetzen sind.\n\n"
           "  -t             Textformat\n"
           "  -o Dateiname   Ausgabedatei\n"
           "  -n             nolang\n"
           "  -l             lang (default)\n";
      return -1;
    }

    std::ifstream in(argv[1]);

    ecpplang generator;
    generator.setDebug(debug);
    generator.setSplitBar();
    generator.setLang(lang || !nolang);
    generator.setNoLang(nolang);
    generator.setTextformat(textformat);
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
