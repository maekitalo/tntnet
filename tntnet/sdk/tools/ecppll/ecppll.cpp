////////////////////////////////////////////////////////////////////////
// ecppll - ecpp-language-linker
//
// ecpp-language-linker dient dem Zusammeführen von Texten
// aus ecpp-Komponetnen und übersetzten Texten.
//

#include <tnt/datachunks_creator.h>
#include <tnt/ecppParser.h>
#include <tnt/stringescaper.h>

#include <cxxtools/arg.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <list>

////////////////////////////////////////////////////////////////////////
// eppll - Applikationsklasse
//
class ecppll : public tnt::ecppParser
{
    //tnt::datachunks_creator data;

    typedef std::map<unsigned, std::string> replacetokens_type;
    typedef std::list<std::string> data_type;

    replacetokens_type replacetokens;
    data_type data;

    bool inLang;
    int ret;

  public:
    ecppll()
      : inLang(false),
        ret(0)
      { }

    void readReplaceTokens(std::istream& in);

    virtual void processHtml(const std::string& html);
    virtual void tokenSplit(bool start);
    void print(std::ostream& out, const std::string& compname);

    int getRet() const  { return ret; }
};

////////////////////////////////////////////////////////////////////////
// Funktionen zur Applikationsklasse
//
void ecppll::readReplaceTokens(std::istream& in)
{
  enum state_type
  {
    state_num0,
    state_num,
    state_token,
    state_esc
  };

  int max = -1;
  int n = 0;
  std::string token;

  state_type state = state_num0;
  char ch;
  unsigned curline = 1;
  while (in.get(ch))
  {
    if (ch == '\n')
      ++curline;

    switch (state)
    {
      case state_num0:
        if (std::isdigit(ch))
        {
          n = (ch - '0');
          state = state_num;
        }
        else if (!std::isspace(ch))
        {
          std::ostringstream msg;
          msg << "digit expected in line " << curline;
          throw std::runtime_error(msg.str());
        }
        break;

      case state_num:
        if (std::isdigit(ch))
          n = n * 10 + (ch - '0');
        else if (ch == '\t')
        {
          if (n <= max)
          {
            std::ostringstream msg;
            msg << n << " <= " << max << " in line " << curline;
            throw std::runtime_error(msg.str());
          }

          max = n;
          state = state_token;
        }
        else
        {
          std::ostringstream msg;
          msg << "parse error in line " << curline;
          throw std::runtime_error(msg.str());
        }
        break;

      case state_token:
        if (ch == '\n')
        {
          replacetokens[n] = token;
          token.clear();
          state = state_num0;
        }
        else if (ch == '\\')
          state = state_esc;
        else
          token += ch;
        break;

      case state_esc:
        switch (ch)
        {
          case 't': token += '\t'; break;
          case 'n': token += '\n'; break;
          case 'r': token += '\r'; break;
          case 'f': token += '\f'; break;
          default : token += ch;
        }
        state = state_token;
        break;
    }
  }

  if (state == state_token)
    replacetokens[n] = token;
}

void ecppll::processHtml(const std::string& html)
{
  replacetokens_type::const_iterator it;
  it = replacetokens.find(data.size());

  if (it == replacetokens.end())
  {
    if (inLang)
    {
      std::cerr << "Warnung: Token " << data.size() << " nicht gefunden"
        << std::endl;
    }
    data.push_back(html);
  }
  else
  {
    if (!inLang)
    {
      std::cerr << "Fehler: Token " << data.size() << " wird durch \""
        << it->second << "\" ersetzt" << std::endl;
      ret = 1;
    }
    data.push_back(it->second);
  }
}

void ecppll::tokenSplit(bool start)
{
  inLang = start;
}

void ecppll::print(std::ostream& out, const std::string& compname)
{
  for (data_type::const_iterator it = data.begin();
       it != data.end(); ++it)
    out << '«' << *it << '»';
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
    ecppll app;

    arg<const char*> ofile(argc, argv, 'o');
    arg<const char*> compname(argc, argv, 'n', "component");

    if (argc != 3)
    {
      std::cerr << "Aufruf " << argv[0] << " {Optionen} <ecpp-Komponente> <übersetzte Texte>\n\n"
                   " -o outfile   Ausagedatei\n"
                   " -n compname  Komponentenname\n";
      return 1;
    }

    std::ifstream ecpp(argv[1]);
    if (!ecpp)
    {
      std::cerr << "Fehler beim öffnen der ecpp-Datei \"" << argv[1] << '"'
        << std::endl;
      return 2;
    }

    std::ifstream txt(argv[2]);
    if (!ecpp)
    {
      std::cerr << "Fehler beim öffnen der Texte-Datei \"" << argv[2] << '"'
        << std::endl;
      return 3;
    }

    app.readReplaceTokens(txt);
    app.setSplitBar();
    app.parse(ecpp);

    if (ofile.isSet())
    {
      std::ofstream out(ofile);
      app.print(out, compname.getValue());
    }
    else
      app.print(std::cout, compname.getValue());

    return app.getRet();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
