/* ./ecppll.cpp
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

#include <tnt/ecppParser.h>
#include <tnt/stringescaper.h>

#include <cxxtools/arg.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <list>
#include <memory>

#include "config.h"

////////////////////////////////////////////////////////////////////////
// ecppll - Applikationsklasse - Basisklasse
//
class ecppll : public tnt::ecppParser
{
    int ret;

  protected:
    typedef std::list<std::string> data_type;
    data_type data;
    bool inLang;

    bool failOnWarn;

    void fail(int _ret = 1)  { ret = _ret; }
    void warn(int _ret = 1)  { if (failOnWarn) ret = _ret; };

    typedef std::list<std::pair<std::string, std::string> > replacetokens_type;

    replacetokens_type replacetokens;
    replacetokens_type::const_iterator next;

  public:
    ecppll()
      : ret(0),
        inLang(false),
        failOnWarn(false)
      { }

    virtual void readReplaceTokens(std::istream& in);
    virtual void processHtml(const std::string& html);
    virtual void tokenSplit(bool start);

    void setFailOnWarn(bool sw = true)
      { failOnWarn = sw; }

    void print(std::ostream& out, const std::string& compname);
    int getRet() const  { return ret; }
};

////////////////////////////////////////////////////////////////////////
// Funktionen zur Applikationsklasse
//
void ecppll::tokenSplit(bool start)
{
  inLang = start;
}

void ecppll::print(std::ostream& out, const std::string& compname)
{
  for (data_type::const_iterator it = data.begin();
       it != data.end(); ++it)
  {
    out << getSplitStartChar();
    for (data_type::const_iterator::value_type::const_iterator n = it->begin();
         n != it->end(); ++n)
    {
      if (*n == getSplitStartChar()
       || *n == getSplitEndChar())
        out << '\\';
      out << *n;
    }
    out << getSplitEndChar();
  }
}

void ecppll::readReplaceTokens(std::istream& in)
{
  enum state_type
  {
    state_key,
    state_token,
    state_esc
  };

  std::string key;
  std::string token;

  state_type state = state_key;
  char ch;
  unsigned curline = 1;
  while (in.get(ch))
  {
    if (ch == '\n')
      ++curline;

    switch (state)
    {
      case state_key:
        if (ch == '\t')
          state = state_token;
        else if (std::iscntrl(ch))
        {
          std::ostringstream msg;
          msg << "character expected in line " << curline;
          throw std::runtime_error(msg.str());
        }
        else
          key += ch;
        break;

      case state_token:
        if (ch == '\n')
        {
          replacetokens.push_back(std::make_pair(key, token));
          key.clear();
          token.clear();
          state = state_key;
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
    replacetokens.push_back(std::make_pair(key, token));

  next = replacetokens.begin();
}

void ecppll::processHtml(const std::string& html)
{
  if (inLang)
  {
    std::ostringstream msg;
    std::transform(
      html.begin(),
      html.end(),
      std::ostream_iterator<const char*>(msg),
      stringescaper(false));

    bool found = true;

    if (next == replacetokens.end()
     || next->first != msg.str())
    {
      // nächster Token passt nicht (oder kommt keiner mehr) - suche, ob wir ihn
      // noch finden
      replacetokens_type::const_iterator it;
      for (it = next;
           it != replacetokens.end() && it->first != msg.str();
           ++it)
        ;

      if (it == replacetokens.end())
      {
        std::cerr << "Warnung: Ersetzungstext \"" << html << "\" nicht gefunden ("
          << data.size() << ')'
          << std::endl;
        warn();
        found = false;
      }
      else
      {
        for (replacetokens_type::const_iterator it2 = next;
             it2 != it; ++it2)
          std::cerr << "Warnung: Ersetzungstext \"" << it2->first << "\" übersprungen ("
            << data.size() << ')'
            << std::endl;
        next = it;
        warn();
      }
    }

    if (found)
    {
      data.push_back(next->second);
      ++next;
    }
    else
      data.push_back(html);
  }
  else
  {
    data.push_back(html);
  }
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  std::ios::sync_with_stdio(false);

  try
  {
    cxxtools::arg<const char*> ofile(argc, argv, 'o');
    cxxtools::arg<const char*> compname(argc, argv, 'n', "component");
    cxxtools::arg<bool> fail_on_warn(argc, argv, 'F');
    cxxtools::arg<const char*> splitChars(argc, argv, "--split-chars");

    if (argc != 3)
    {
      std::cerr
        << PACKAGE_STRING "\n\n"
           "Aufruf " << argv[0] << " {Optionen} <ecpp-Komponente> <übersetzte Texte>\n\n"
           " -o outfile   Ausagedatei\n"
           " -n compname  Komponentenname\n"
           " -F           Warnungen als Fehler\n"
           " --split-chars zz setze alternative Teilungszeichen\n";
      return 1;
    }

    // open source
    std::ifstream ecpp(argv[1]);
    if (!ecpp)
    {
      std::cerr << "Fehler beim öffnen der Quelldatei \"" << argv[1] << '"'
        << std::endl;
      return 2;
    }

    // open translationfile
    std::ifstream txt(argv[2]);
    if (!ecpp)
    {
      std::cerr << "Fehler beim öffnen der Texte-Datei \"" << argv[2] << '"'
        << std::endl;
      return 3;
    }

    // create and initialize applicationobject
    ecppll app;

    app.setFailOnWarn(fail_on_warn);
    app.readReplaceTokens(txt);
    app.setSplitBar();

    if (splitChars.isSet())
    {
      if (splitChars.getValue()[0] == '\0'
       || splitChars.getValue()[1] == '\0'
       || splitChars.getValue()[2] != '\0')
        throw std::runtime_error("--split-chars needs exactly 2 characters");

      app.setSplitChars(splitChars.getValue()[0],
                        splitChars.getValue()[1]);
    }

    // parse
    app.parse(ecpp);

    // process
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
    return -1;
  }
}
