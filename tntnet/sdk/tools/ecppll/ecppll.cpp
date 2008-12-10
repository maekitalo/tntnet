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


#include <tnt/ecpp/parser.h>
#include <tnt/ecpp/parsehandler.h>
#include <tnt/stringescaper.h>
#include <tnt/datachunks_creator.h>
#include <tnt/filename.h>

#include <cxxtools/arg.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <list>
#include <memory>
#include <algorithm>

#include "config.h"

////////////////////////////////////////////////////////////////////////
// Ecppll - Applicationclass
//
class Ecppll : public tnt::ecpp::ParseHandler
{
    tnt::ecpp::Parser parser;
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
    explicit Ecppll(const std::string& fname)
      : parser(*this, fname),
        ret(0),
        inLang(false),
        failOnWarn(false)
      { }

    virtual void readReplaceTokens(std::istream& in);
    virtual void onHtml(const std::string& html);
    virtual void tokenSplit(bool start);

    void setFailOnWarn(bool sw = true)
      { failOnWarn = sw; }

    void print(std::ostream& out);
    int getRet() const  { return ret; }

    void addInclude(const std::string& dir)
      { parser.addInclude(dir); }

    void parse(std::istream& in)              { parser.parse(in); }
};

////////////////////////////////////////////////////////////////////////
// Funktionen zur Applikationsklasse
//
void Ecppll::tokenSplit(bool start)
{
  inLang = start;
}

void Ecppll::print(std::ostream& out)
{
  tnt::DatachunksCreator dc;
  std::copy(data.begin(), data.end(), std::back_inserter(dc));
  out.write(dc.ptr(), dc.size());
}

void Ecppll::readReplaceTokens(std::istream& in)
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

void Ecppll::onHtml(const std::string& html)
{
  if (inLang)
  {
    std::ostringstream msg;
    std::transform(
      html.begin(),
      html.end(),
      std::ostream_iterator<const char*>(msg),
      tnt::stringescaper(false));

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
        std::cerr << "warning: replacement-text \"" << html << "\" not found ("
          << data.size() << ')'
          << std::endl;
        warn();
        found = false;
      }
      else
      {
        for (replacetokens_type::const_iterator it2 = next;
             it2 != it; ++it2)
          std::cerr << "warning: replacement-text \"" << it2->first << "\" skipped ("
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
    cxxtools::Arg<const char*> ofile(argc, argv, 'o');
    cxxtools::Arg<bool> fail_on_warn(argc, argv, 'F');

    typedef std::list<std::string> includes_type;
    includes_type includes;

    while (true)
    {
      cxxtools::Arg<const char*> include(argc, argv, 'I', 0);
      if (!include.isSet())
        break;
      includes.push_back(include.getValue());
    }

    if (argc != 3)
    {
      std::cerr
        << PACKAGE_STRING "\n\n"
           "usage: " << argv[0] << " [options] ecpp-source translated-data\n\n"
           "  -o filename       outputfile\n"
           "  -F                fail on warning\n"
           "  -I dir            include-directory\n"
        << std::endl;
      return 1;
    }

    // open source
    std::ifstream ecpp(argv[1]);
    if (!ecpp)
    {
      std::cerr << "cannot open source-file \"" << argv[1] << '"'
        << std::endl;
      return 2;
    }

    // open translationfile
    std::ifstream txt(argv[2]);
    if (!ecpp)
    {
      std::cerr << "cannot open translation-file \"" << argv[2] << '"'
        << std::endl;
      return 3;
    }

    // create and initialize applicationobject
    Ecppll app(argv[1]);

    app.setFailOnWarn(fail_on_warn);
    app.readReplaceTokens(txt);

    for (includes_type::const_iterator it = includes.begin();
         it != includes.end(); ++it)
      app.addInclude(*it);

    // parse
    app.parse(ecpp);

    // process
    if (app.getRet() == 0)
    {
      if (ofile.isSet())
      {
        if (strcmp(ofile.getValue(), "-") == 0)
          app.print(std::cout);
        else
        {
          std::ofstream out(ofile);
          app.print(out);
        }
      }
      else
      {
        tnt::Filename f(argv[1]);
        f.setExt("tntdata");
        std::ofstream out(f.getFullPath().c_str());
        app.print(out);
      }
    }

    return app.getRet();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
