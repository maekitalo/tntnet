////////////////////////////////////////////////////////////////////////
// makelang
//
#include <tnt/datachunks_creator.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <tnt/stringescaper.h>
#include <cxxtools/arg.h>
#include <stdexcept>

class makelang
{
    arg<const char*> compname;
    arg<const char*> arg_ofilename;

    std::string ifilename;
    std::string ofilename;

    tnt::datachunks_creator dc;

  public:
    makelang(int& argc, char* argv[])
      : compname(argc, argv, 'n', "component"),
        arg_ofilename(argc, argv, 'o', 0)
    {
      if (argv[1])
      {
        ifilename = argv[1];
        --argc;
      }

      if (arg_ofilename)
        ofilename = arg_ofilename.getValue();
      else
        ofilename = std::string(compname) + ".cpp";
    }

    void parse();
    void parse(std::istream& in);
    void generateCode();
    void generateCode(std::ostream& out);
};

void makelang::parse()
{
  if (!ifilename.empty())
  {
    std::ifstream in(ifilename.c_str());
    if (!in)
      throw std::runtime_error(std::string("can't open ") + std::string(ifilename));
    parse(in);
  }
  else
    parse(std::cin);
}

void makelang::parse(std::istream& in)
{
  std::string chunk;

  enum state_type
  {
    state_start,
    state_chunkesc,
    state_chunk
  };

  state_type state = state_start;
  char ch;
  while (in.get(ch))
  {
    switch(state)
    {
      case state_start:
        if (ch == '\t')
          state = state_chunk;
        break;

      case state_chunk:
        if (ch == '\n')
        {
          dc.push_back(chunk);
          chunk.clear();
          state = state_start;
        }
        else if (ch == '\\')
          state = state_chunkesc;
        else
          chunk += ch;
        break;

      case state_chunkesc:
        if (ch == 'n')
          chunk += '\n';
        else if (ch == 't')
          chunk += '\t';
        else
          chunk += ch;
        state = state_chunk;
        break;
    }
  }
}

void makelang::generateCode()
{
  std::ofstream out(ofilename.c_str());

  if (!out)
    throw std::runtime_error("can't open " + ofilename);

  generateCode(out);
}

void makelang::generateCode(std::ostream& out)
{
  out << "////////////////////////////////////////////////////////////////////////\n"
         "// " << ofilename << "\n"
         "// generated with makelang\n"
         "//\n\n"
         "const char* " << compname << "_data = \n"
         "\"";

  std::transform(
    dc.ptr(),
    dc.ptr() + dc.size(),
    std::ostream_iterator<const char*>(out),
    stringescaper());

  out << "\";\n"
      << "unsigned " << compname << "_datalen = " << dc.size() << ";\n";
}

int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
    makelang app(argc, argv);
    if (argc != 1)
    {
      std::cerr << "Aufruf: " << argv[0] << " {Optionen} Eingabedatei\n\n"
                << " -n name   Komponentenname (default: component)\n"
                << " -o datei  Ausgabedatei (default: komponentenname.cpp)\n";
      return 1;
    }

    app.parse();
    app.generateCode();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
